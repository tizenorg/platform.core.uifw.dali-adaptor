/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// CLASS HEADER
#include "update-render-synchronization.h"

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <base/interfaces/adaptor-internal-services.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace
{
const unsigned int TIME_PER_FRAME_IN_MICROSECONDS = 16667;
const unsigned int MICROSECONDS_PER_SECOND( 1000000 );
const unsigned int INPUT_EVENT_UPDATE_PERIOD( MICROSECONDS_PER_SECOND / 90 ); // period between ecore x event updates

} // unnamed namespace

UpdateRenderSynchronization::UpdateRenderSynchronization( AdaptorInternalServices& adaptorInterfaces,
                                                          unsigned int numberOfVSyncsPerRender)
: mMaximumUpdateCount( adaptorInterfaces.GetCore().GetMaximumUpdateCount()),
  mNumberOfVSyncsPerRender( numberOfVSyncsPerRender ),
  mUpdateReadyCount( 0u ),
  mRunning( false ),
  mUpdateRequired( false ),
  mPaused( false ),
  mUpdateRequested( false ),
  mAllowUpdateWhilePaused( false ),
  mSleeping( false ),
  mSyncFrameNumber( 0u ),
  mSyncSeconds( 0u ),
  mSyncMicroseconds( 0u ),
  mFrameTime( adaptorInterfaces.GetPlatformAbstractionInterface() ),
  mNotificationTrigger( adaptorInterfaces.GetTriggerEventInterface() ),
  mPerformanceInterface( adaptorInterfaces.GetPerformanceInterface() ),
  mReplaceSurfaceRequest(),
  mReplaceSurfaceRequested( false )
{
}

UpdateRenderSynchronization::~UpdateRenderSynchronization()
{
}

void UpdateRenderSynchronization::Start()
{
  mFrameTime.SetMinimumFrameTimeInterval( mNumberOfVSyncsPerRender * TIME_PER_FRAME_IN_MICROSECONDS );
  mRunning = true;
}

void UpdateRenderSynchronization::Stop()
{
  mRunning = false;

  // Wake if sleeping
  UpdateRequested();

  // we may be paused so need to resume
  Resume();

  // Notify all condition variables, so if threads are waiting
  // they can break out, and check the running status.
  mUpdateFinishedCondition.Notify();
  mRenderFinishedCondition.Notify();
  mSleepCondition.Notify();
  mVSyncReceivedCondition.Notify();
  mRenderWaitForSurface.Notify();

  mFrameTime.Suspend();
}

void UpdateRenderSynchronization::Pause()
{
  mPaused = true;

  AddPerformanceMarker( PerformanceInterface::PAUSED );
  mFrameTime.Suspend();
}

void UpdateRenderSynchronization::ResumeFrameTime()
{
  mFrameTime.Resume();
}

void UpdateRenderSynchronization::Resume()
{
  mPaused = false;
  mSleeping = false;

  // Wake if sleeping
  mSleepCondition.Notify();
  // stay paused but notify the pause condition
  mPausedCondition.Notify();

  AddPerformanceMarker( PerformanceInterface::RESUME);
}

void UpdateRenderSynchronization::UpdateRequested()
{
  mUpdateRequested = true;

  // Wake if sleeping
  mSleepCondition.Notify();
  // if paused, do nothing
}

void UpdateRenderSynchronization::UpdateWhilePaused()
{
  mAllowUpdateWhilePaused = true;

  // Wake if sleeping
  mSleepCondition.Notify();
  // stay paused but notify the pause condition
  mPausedCondition.Notify();
}

bool UpdateRenderSynchronization::ReplaceSurface( RenderSurface* newSurface )
{
  bool result=false;

  UpdateRequested();
  UpdateWhilePaused();
  {
    mReplaceSurfaceRequest.SetSurface(newSurface);
    mReplaceSurfaceRequested = true;

    mRenderRequestFinishedCondition.Wait(); // wait unlocks the mutex on entry, and locks again on exit.

    mReplaceSurfaceRequested = false;
    result = mReplaceSurfaceRequest.GetReplaceCompleted();
  }

  return result;
}

bool UpdateRenderSynchronization::NewSurface( RenderSurface* newSurface )
{
  bool result=false;

  UpdateRequested();
  UpdateWhilePaused();
  {
    mReplaceSurfaceRequest.SetSurface(newSurface);
    mReplaceSurfaceRequested = true;

    // Unblock the render thread if its waiting for new surface
    mRenderWaitForSurface.Notify();

    // Lock event thread until request has been processed by render
    mRenderRequestFinishedCondition.Wait();// wait unlocks the mutex on entry, and locks again on exit.

    mReplaceSurfaceRequested = false;
    result = mReplaceSurfaceRequest.GetReplaceCompleted();
  }

  return result;
}


void UpdateRenderSynchronization::UpdateReadyToRun()
{
  bool wokenFromPause( false );

  // atomic check first to avoid mutex lock in 99.99% of cases
  if( mPaused )
  {
    // wait while paused
    if( !mAllowUpdateWhilePaused )
    {
      mPausedCondition.Wait();

      wokenFromPause = true;
    }
  }

  if ( !wokenFromPause )
  {
    // Wait for the next Sync
    WaitSync();
  }

  AddPerformanceMarker( PerformanceInterface::UPDATE_START );
}

bool UpdateRenderSynchronization::UpdateSyncWithRender( bool notifyEvent, bool& renderNeedsUpdate )
{
  AddPerformanceMarker( PerformanceInterface::UPDATE_END );

  // Do the notifications first so the event thread can start processing them
  if( notifyEvent && mRunning )
  {
    // Tell the event-thread to wake up (if asleep) and send a notification event to Core
    mNotificationTrigger.Trigger();
  }

  // Another frame was prepared for rendering; increment counter
  ++mUpdateReadyCount;
  DALI_ASSERT_DEBUG( mUpdateReadyCount <= mMaximumUpdateCount );

  // Notify the render-thread that an update has completed
  mUpdateFinishedCondition.Notify();

  // The update-thread must wait until a frame has been rendered, when mMaximumUpdateCount is reached
  if( mRunning && ( mMaximumUpdateCount == mUpdateReadyCount ) )
  {
    mRenderFinishedCondition.Wait();
  }

  renderNeedsUpdate = mUpdateRequired;

  // Flag is used to during UpdateThread::Stop() to exit the update/render loops
  return mRunning;
}

void UpdateRenderSynchronization::UpdateWaitForAllRenderingToFinish()
{
  // Wait for all of the prepared frames to be rendered
  if( mRunning && ( 0u != mUpdateReadyCount ) && !mUpdateRequested )
  {
    mRenderFinishedCondition.Wait();
  }
}

bool UpdateRenderSynchronization::UpdateTryToSleep()
{
  if ( !mUpdateRequired && !mUpdateRequested )
  {
    // there's nothing to update in the scene, so wait for render to finish processing
    UpdateWaitForAllRenderingToFinish();
  }

  if( mRunning && !mUpdateRequired && !mUpdateRequested )
  {
    //
    // Going to sleep
    //

    // 1. put threads to sleep.
    mSleeping = true;

    // 2. inform frame time
    mFrameTime.Sleep();

    // 3. block thread and wait
    mSleepCondition.Wait();

    //
    // Woken up
    //
    mSleeping = false;

    // 1. inform frame timer
    mFrameTime.WakeUp();
  }

  mUpdateRequested = false;

  return mRunning;
}

bool UpdateRenderSynchronization::RenderWaitForNewSurface(RenderRequest*& requestPtr)
{
  // block here until new surface is available
  mRenderWaitForSurface.Wait();

  // write any new requests
  if( mReplaceSurfaceRequested )
  {
    requestPtr = &mReplaceSurfaceRequest;
  }
  mReplaceSurfaceRequested = false;
  return mRunning;
}

bool UpdateRenderSynchronization::RenderSyncWithUpdate(RenderRequest*& requestPtr)
{
  // Wait for update to produce a buffer, or for the mRunning state to change
  if( mRunning && ( 0u == mUpdateReadyCount ) )
  {
    mUpdateFinishedCondition.Wait();
  }

  if( mRunning )
  {
    AddPerformanceMarker( PerformanceInterface::RENDER_START );
  }

  // write any new requests
  if( mReplaceSurfaceRequested )
  {
    requestPtr = &mReplaceSurfaceRequest;
  }
  mReplaceSurfaceRequested = false;

  // Flag is used to during UpdateThread::Stop() to exit the update/render loops
  return mRunning;
}

void UpdateRenderSynchronization::RenderFinished( bool updateRequired, bool requestProcessed )
{
  {
    // Set the flag to say if update needs to run again.
    mUpdateRequired = updateRequired;

    // A frame has been rendered; decrement counter
    --mUpdateReadyCount;
    DALI_ASSERT_DEBUG( mUpdateReadyCount < mMaximumUpdateCount );
  }

  // Notify the update-thread that a render has completed
  mRenderFinishedCondition.Notify();

  if( requestProcessed )
  {
    // Notify the event thread that a request has completed
    mRenderRequestFinishedCondition.Notify();
  }

  AddPerformanceMarker( PerformanceInterface::RENDER_END );
}

void UpdateRenderSynchronization::WaitSync()
{
  // Block until the start of a new sync.
  if( mRunning )
  {
    mVSyncReceivedCondition.Wait();
  }

  // reset update while paused flag
  mAllowUpdateWhilePaused = false;
}

bool UpdateRenderSynchronization::VSyncNotifierSyncWithUpdateAndRender( bool validSync, unsigned int frameNumber, unsigned int seconds, unsigned int microseconds, unsigned int& numberOfVSyncsPerRender )
{
  // This may have changed since the last sync. Update VSyncNotifier's copy here if so.
  if( numberOfVSyncsPerRender != mNumberOfVSyncsPerRender )
  {
    numberOfVSyncsPerRender = mNumberOfVSyncsPerRender; // save it back
    mFrameTime.SetMinimumFrameTimeInterval( mNumberOfVSyncsPerRender * TIME_PER_FRAME_IN_MICROSECONDS );
  }

  if( validSync )
  {
    mFrameTime.SetSyncTime( frameNumber );
  }

  mSyncFrameNumber = frameNumber;
  mSyncSeconds = seconds;
  mSyncMicroseconds = microseconds;

  mVSyncReceivedCondition.Notify();

  AddPerformanceMarker( PerformanceInterface::VSYNC );

  if( mRunning && // sleep on condition variable WHILE still running
      !mAllowUpdateWhilePaused &&             // AND NOT allowing updates while paused
      ( mSleeping || mPaused ) )            // AND sleeping OR paused
  {
    mSleepCondition.Wait();
  }

  return mRunning;
}

unsigned int UpdateRenderSynchronization::GetFrameNumber() const
{
  return mSyncFrameNumber;
}

uint64_t UpdateRenderSynchronization::GetTimeMicroseconds()
{
  uint64_t currentTime(0);

  {
    currentTime = mSyncSeconds;
    currentTime *= MICROSECONDS_PER_SECOND;
    currentTime += mSyncMicroseconds;
  }

  return currentTime;
}

void UpdateRenderSynchronization::SetRenderRefreshRate( unsigned int numberOfVSyncsPerRender )
{
  mNumberOfVSyncsPerRender = numberOfVSyncsPerRender;
}

inline void UpdateRenderSynchronization::AddPerformanceMarker( PerformanceInterface::MarkerType type )
{
  if( mPerformanceInterface )
  {
    mPerformanceInterface->AddMarker( type );
  }
}

void UpdateRenderSynchronization::PredictNextSyncTime(
  float& lastFrameDeltaSeconds,
  unsigned int& lastSyncTimeMilliseconds,
  unsigned int& nextSyncTimeMilliseconds )
{
  mFrameTime.PredictNextSyncTime( lastFrameDeltaSeconds, lastSyncTimeMilliseconds, nextSyncTimeMilliseconds );
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
