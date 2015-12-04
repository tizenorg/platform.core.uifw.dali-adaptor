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

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
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
const unsigned int INPUT_EVENT_UPDATE_PERIOD( MICROSECONDS_PER_SECOND / 90 );

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New( Debug::Concise, false, "DALI_THREAD_SYNC");
#endif

#define COLOR_RED            "\033[31;1m"
#define COLOR_BLUE           "\033[34;1m"
#define COLOR_PINK           "\033[35;1m"
#define COLOR_GREEN          "\033[32;1m"
#define COLOR_CLEAR          "\033[0m"

const char* COLOR_EVENT( COLOR_RED );
const char* COLOR_UPDATE( COLOR_PINK );
const char* COLOR_RENDER( COLOR_BLUE );
const char* COLOR_VSYNC( COLOR_GREEN );


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
  mAllowUpdateWhilePausedCount( 0 ),
  mVSyncSleep( false ),
  mSyncFrameNumber( 0u ),
  mSyncSeconds( 0u ),
  mSyncMicroseconds( 0u ),
  mFrameTime( adaptorInterfaces.GetPlatformAbstractionInterface() ),
  mNotificationTrigger( adaptorInterfaces.GetProcessCoreEventsTrigger() ),
  mPerformanceInterface( adaptorInterfaces.GetPerformanceInterface() ),
  mReplaceSurfaceRequest(),
  mReplaceSurfaceRequested( false ),
  mSurfaceLostRequested(false)
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

  DALI_LOG_INFO( gLogFilter, Debug::General, "ThreadSync E: %sNOTIFY_ALL%s\n", COLOR_EVENT, COLOR_CLEAR );
  // Notify all condition variables, so if threads are waiting
  // they can break out, and check the running status.
  mUpdateFinishedCondition.Notify();
  mRenderFinishedCondition.Notify();
  mVSyncSleepCondition.Notify();
  mVSyncReceivedCondition.Notify();
  mRenderRequestSleepCondition.Notify();

  mFrameTime.Suspend();
  // reset state variables
  mUpdateReadyCount = 0;
  mRunning = false;
  mUpdateRequired = false;
  mUpdateRequested = false;
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
  mVSyncSleep = false;

  DALI_LOG_INFO( gLogFilter, Debug::General, "ThreadSync E: %smPausedCondition NOTIFY%s\n", COLOR_EVENT, COLOR_CLEAR );
  DALI_LOG_INFO( gLogFilter, Debug::General, "ThreadSync E: %smVSyncSleepCondition NOTIFY%s\n", COLOR_EVENT, COLOR_CLEAR );
  mPausedCondition.Notify();
  mVSyncSleepCondition.Notify();

  AddPerformanceMarker( PerformanceInterface::RESUME);
}

void UpdateRenderSynchronization::UpdateRequested()
{
  mUpdateRequested = true;

  // Wake update thread if sleeping
  DALI_LOG_INFO( gLogFilter, Debug::General, "ThreadSync E: %smUpdateSleepCondition NOTIFY%s\n", COLOR_EVENT, COLOR_CLEAR );
  mUpdateSleepCondition.Notify();
}

void UpdateRenderSynchronization::UpdateWhilePaused()
{
  {
    Mutex::ScopedLock lock( mMutex );

    mAllowUpdateWhilePaused = true;
    mAllowUpdateWhilePausedCount = 3;
  }

  DALI_LOG_INFO( gLogFilter, Debug::General, "ThreadSync E: %smVsyncSleepCondition NOTIFY%s\n" , COLOR_EVENT, COLOR_CLEAR);
  // wake vsync if sleeping
  mVSyncSleepCondition.Notify();

  DALI_LOG_INFO( gLogFilter, Debug::General, "ThreadSync E: %smUpdateSleepCondition NOTIFY%s\n", COLOR_EVENT, COLOR_CLEAR );
  // Wake update if sleeping
  mUpdateSleepCondition.Notify();
  // stay paused but notify the pause condition
  DALI_LOG_INFO( gLogFilter, Debug::General, "ThreadSync E: %smPausedCondition NOTIFY%s\n", COLOR_EVENT, COLOR_CLEAR );
  mPausedCondition.Notify();
}

void UpdateRenderSynchronization::SurfaceLost()
{
  mSurfaceLostRequested = true;
  mReplaceSurfaceRequested = false;
  UpdateRequested(); // allow SURFACE_LOST request to be processed
  UpdateWhilePaused();
}

void UpdateRenderSynchronization::SurfaceLostCancel()
{
  mSurfaceLostRequested = false;
}

bool UpdateRenderSynchronization::ReplaceSurface( RenderSurface* newSurface )
{
  bool result=false;
  if (mRunning)
  {
    UpdateRequested();
    UpdateWhilePaused();
    {
      ConditionalWait::ScopedLock lock( mRenderFinishedCondition );

      mReplaceSurfaceRequest.SetSurface(newSurface);
      mReplaceSurfaceRequested = true;
      mSurfaceLostRequested = false;

      // Unlock the render thread sleeping on requests
      DALI_LOG_INFO( gLogFilter, Debug::General, "ThreadSync E: %smRenderRequestSleepCondition NOTIFY%s\n", COLOR_EVENT, COLOR_CLEAR );
      mRenderRequestSleepCondition.Notify();

      // Lock event thread until request has been processeds
      DALI_LOG_INFO( gLogFilter, Debug::General, "ThreadSync E: %smRenderRequestFinishedCondition WAIT %s\n", COLOR_EVENT, COLOR_CLEAR );
      mRenderRequestFinishedCondition.Wait( lock );// wait unlocks the mutex on entry, and locks again on exit.

      mReplaceSurfaceRequested = false;
      result = mReplaceSurfaceRequest.GetReplaceCompleted();
    }
  }

  return result;
}



void UpdateRenderSynchronization::UpdateReadyToRun()
{
  bool wokenFromPause( false );

  // atomic check first to avoid mutex lock in 99.99% of cases
  if( mPaused )
  {
    ConditionalWait::ScopedLock lock( mPausedCondition );

    // wait while paused
    while( mPaused && !mAllowUpdateWhilePaused )
    {
      DALI_LOG_INFO( gLogFilter, Debug::General, "ThreadSync U: %smPausedCondition WAIT %s\n" ,
                     COLOR_UPDATE, COLOR_CLEAR);

      // this will automatically unlock mMutex
      mPausedCondition.Wait( lock );

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

  ConditionalWait::ScopedLock lock( mRenderFinishedCondition );

  // Another frame was prepared for rendering; increment counter
  ++mUpdateReadyCount;
  DALI_ASSERT_DEBUG( mUpdateReadyCount <= mMaximumUpdateCount );

  DALI_LOG_INFO( gLogFilter, Debug::General, "ThreadSync U: %smUpdateFinishedCondition NOTIFY%s\n", COLOR_UPDATE, COLOR_CLEAR );
  // Notify the render-thread that an update has completed
  mUpdateFinishedCondition.Notify();

  // The update-thread must wait until a frame has been rendered, when mMaximumUpdateCount is reached
  while( mRunning && ( mMaximumUpdateCount == mUpdateReadyCount ) )
  {
    // Wait will atomically add the thread to the set of threads waiting on
    // the condition variable mRenderFinishedCondition and unlock the mutex.
    DALI_LOG_INFO( gLogFilter, Debug::General, "ThreadSync U: %smRenderFinishedCondition WAIT%s\n", COLOR_UPDATE, COLOR_CLEAR );
    mRenderFinishedCondition.Wait( lock );
  }

  renderNeedsUpdate = mUpdateRequired;

  // Flag is used to during UpdateThread::Stop() to exit the update/render loops
  return mRunning;
}

void UpdateRenderSynchronization::UpdateWaitForAllRenderingToFinish()
{
  ConditionalWait::ScopedLock lock( mRenderFinishedCondition );

  // Wait for all of the prepared frames to be rendered
  while ( mRunning && ( 0u != mUpdateReadyCount ) && !mUpdateRequested )
  {
    // Wait will atomically add the thread to the set of threads waiting on
    // the condition variable mRenderFinishedCondition and unlock the mutex.
    DALI_LOG_INFO( gLogFilter, Debug::General, "ThreadSync U: %smRenderFinishedCondition WAIT%s\n", COLOR_UPDATE, COLOR_CLEAR );
    mRenderFinishedCondition.Wait( lock );
  }
}

bool UpdateRenderSynchronization::UpdateTryToSleep()
{
  if ( !mUpdateRequired && !mUpdateRequested )
  {
    // there's nothing to update in the scene, so wait for render to finish processing
    UpdateWaitForAllRenderingToFinish();
  }

  ConditionalWait::ScopedLock lock( mUpdateSleepCondition );

  while( mRunning && !mUpdateRequired && !mUpdateRequested )
  {
    //
    // Going to sleep
    //

    // 1. put VSync thread to sleep.
    mVSyncSleep = true;

    // 2. inform frame time
    mFrameTime.Sleep();

    // 3. block thread and wait for wakeup event
    DALI_LOG_INFO( gLogFilter, Debug::General, "ThreadSync U: %smUpdateFinishedCondition WAIT%s\n", COLOR_UPDATE, COLOR_CLEAR );
    mUpdateSleepCondition.Wait( lock );

    //
    // Woken up
    //

    // 1. inform frame timer
    mFrameTime.WakeUp();

    // 2. wake VSync thread.
    DALI_LOG_INFO( gLogFilter, Debug::General, "ThreadSync U: %smVSyncSleepCondition NOTIFY%s\n", COLOR_UPDATE, COLOR_CLEAR );
    mVSyncSleep = false;
    mVSyncSleepCondition.Notify();
  }

  mUpdateRequested = false;

  return mRunning;
}

bool UpdateRenderSynchronization::RenderSyncWithRequest(RenderRequest*& requestPtr)
{
  ConditionalWait::ScopedLock lock( mRenderRequestSleepCondition );

  // Wait for a replace surface request
  DALI_LOG_INFO( gLogFilter, Debug::General, "ThreadSync R: %smRenderRequestSleepCondition WAIT%s\n", COLOR_RENDER, COLOR_CLEAR );
  mRenderRequestSleepCondition.Wait(lock);

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
  ConditionalWait::ScopedLock lock( mUpdateFinishedCondition );

  // Wait for update to produce a buffer, or for the mRunning state to change
  while ( mRunning && ( 0u == mUpdateReadyCount ) )
  {
    // Wait will atomically add the thread to the set of threads waiting on
    // the condition variable mUpdateFinishedCondition and unlock the mutex.
    DALI_LOG_INFO( gLogFilter, Debug::General, "ThreadSync R: %smUpdateFinishedCondition WAIT%s\n", COLOR_RENDER, COLOR_CLEAR );
    mUpdateFinishedCondition.Wait( lock );
  }

  if( mRunning )
  {
    AddPerformanceMarker( PerformanceInterface::RENDER_START );
  }

  // write any new requests
  if( mReplaceSurfaceRequested )
  {
    requestPtr = &mReplaceSurfaceRequest;
    mReplaceSurfaceRequested = false;
    // receiving a new surface cancels any existing surface lost
    mSurfaceLostRequested = false;
  }
  else if (mSurfaceLostRequested)
  {
    requestPtr = &mSurfaceLostRequest;
    mSurfaceLostRequested = false;
  }


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
  DALI_LOG_INFO( gLogFilter, Debug::General, "ThreadSync R: %smRenderFinishedCondition NOTIFY%s\n", COLOR_RENDER, COLOR_CLEAR );
  mRenderFinishedCondition.Notify();

  if( requestProcessed )
  {
    // Notify the event thread that a request has completed
    DALI_LOG_INFO( gLogFilter, Debug::General, "ThreadSync R: %smRenderRequestFinishedCondition NOTIFY%s\n", COLOR_RENDER, COLOR_CLEAR );
    mRenderRequestFinishedCondition.Notify();
  }

  AddPerformanceMarker( PerformanceInterface::RENDER_END );
}

void UpdateRenderSynchronization::WaitSync()
{
  // Block until the start of a new sync.
  // If we're experiencing slowdown and are behind by more than a frame
  // then we should wait for the next frame

  unsigned int updateFrameNumber = mSyncFrameNumber;

  ConditionalWait::ScopedLock lock( mVSyncReceivedCondition );

  while ( mRunning && ( updateFrameNumber == mSyncFrameNumber ) )
  {
    // Wait will atomically add the thread to the set of threads waiting on
    // the condition variable mVSyncReceivedCondition and unlock the mutex.
    DALI_LOG_INFO( gLogFilter, Debug::General, "ThreadSync U: %smVSyncReceivedCondition WAIT%s\n", COLOR_UPDATE, COLOR_CLEAR );
    mVSyncReceivedCondition.Wait( lock );
  }

  // reset update while paused flag
  mAllowUpdateWhilePaused = false;
  mAllowUpdateWhilePausedCount = 0;
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

  ConditionalWait::ScopedLock lock( mVSyncSleepCondition );

  mSyncFrameNumber = frameNumber;
  mSyncSeconds = seconds;
  mSyncMicroseconds = microseconds;

  DALI_LOG_INFO( gLogFilter, Debug::General, "ThreadSync V: %smVSyncReceivedCondition NOTIFY%s\n", COLOR_VSYNC, COLOR_CLEAR );

  mVSyncReceivedCondition.Notify();

  AddPerformanceMarker( PerformanceInterface::VSYNC );

  // Count to clear flag if alive for more than 3 VSyncs
  // Temporary measure to avoid this flag keeping vsync thread alive
  // indefinitely

  if (mAllowUpdateWhilePaused) {
    mAllowUpdateWhilePausedCount--;
    if (mAllowUpdateWhilePausedCount <=0 ) {
      mAllowUpdateWhilePaused = false;
      mAllowUpdateWhilePausedCount = 0;
    }
  }

  while( mRunning && // sleep on condition variable WHILE still running
         !mAllowUpdateWhilePaused &&             // AND NOT allowing updates while paused
         ( mVSyncSleep || mPaused ) )            // AND sleeping OR paused
  {
    // Wait will atomically add the thread to the set of threads waiting on
    // the condition variable mVSyncSleepCondition and unlock the mutex.
    DALI_LOG_INFO( gLogFilter, Debug::General, "ThreadSync V: %smVSyncSleepCondition WAIT%s\n", COLOR_VSYNC, COLOR_CLEAR );
    mVSyncSleepCondition.Wait( lock );
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
    ConditionalWait::ScopedLock lock( mVSyncSleepCondition );

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
