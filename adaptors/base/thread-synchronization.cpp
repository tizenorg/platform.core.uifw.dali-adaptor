/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include "thread-synchronization.h"

// INTERNAL INCLUDES
#include <base/interfaces/adaptor-internal-services.h>
#include <base/thread-synchronization-debug.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace
{
const unsigned int TIME_PER_FRAME_IN_MICROSECONDS = 16667;
const int TOTAL_THREAD_COUNT = 3;
} // unnamed namespace

ThreadSynchronization::ThreadSynchronization( AdaptorInternalServices& adaptorInterfaces, unsigned int numberOfVSyncsPerRender)
: mFrameTime( adaptorInterfaces.GetPlatformAbstractionInterface() ),
  mNotificationTrigger( adaptorInterfaces.GetProcessCoreEventsTrigger() ),
  mPerformanceInterface( adaptorInterfaces.GetPerformanceInterface() ),
  mReplaceSurfaceRequest(),
  mUpdateThreadWaitCondition(),
  mRenderThreadWaitCondition(),
  mVSyncThreadWaitCondition(),
  mEventThreadWaitCondition(),
  mMaximumUpdateCount( adaptorInterfaces.GetCore().GetMaximumUpdateCount()),
  mNumberOfVSyncsPerRender( numberOfVSyncsPerRender ),
  mTryToSleepCount( 0u ),
  mState( State::STOPPED ),
  mVSyncAheadOfUpdate( 0u ),
  mUpdateAheadOfRender( 0u ),
  mNumberOfThreadsStarted( 0u ),
  mUpdateThreadResuming( false ),
  mVSyncThreadRunning( false ),
  mVSyncThreadStop( false ),
  mRenderThreadStop( false ),
  mRenderThreadReplacingSurface( false ),
  mEventThreadSurfaceReplaced( false ),
  mVSyncThreadInitialised( false ),
  mRenderThreadInitialised( false ),
  mRenderThreadSurfaceReplaced( false )
{
}

ThreadSynchronization::~ThreadSynchronization()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EVENT THREAD
///////////////////////////////////////////////////////////////////////////////////////////////////

void ThreadSynchronization::Initialise()
{
  LOG_EVENT( "%s", __FUNCTION__ );

  ConditionalWait::ScopedLock lock( mUpdateThreadWaitCondition );
  if( mState == State::STOPPED )
  {
    mState = State::INITIALISING;
  }
}

void ThreadSynchronization::Start()
{
  LOG_EVENT( "%s", __FUNCTION__ );

  bool start = false;
  {
    ConditionalWait::ScopedLock lock( mUpdateThreadWaitCondition );
    if( mState == State::INITIALISING )
    {
      start = true;
    }
  }

  // Not atomic, but does not matter here as we just want to ensure we only start from State::INITIALISING
  if( start )
  {
    mFrameTime.SetMinimumFrameTimeInterval( mNumberOfVSyncsPerRender * TIME_PER_FRAME_IN_MICROSECONDS );

    {
      ConditionalWait::ScopedLock lock( mEventThreadWaitCondition );
      while( mNumberOfThreadsStarted < TOTAL_THREAD_COUNT )
      {
        mEventThreadWaitCondition.Wait( lock );
      }
    }

    {
      ConditionalWait::ScopedLock lock( mUpdateThreadWaitCondition );
      mState = State::RUNNING;
    }
    mUpdateThreadWaitCondition.Notify();
  }
}

void ThreadSynchronization::Stop()
{
  LOG_EVENT( "%s", __FUNCTION__ );

  bool stop = false;
  {
    ConditionalWait::ScopedLock lock( mUpdateThreadWaitCondition );
    if( mState != State::STOPPED )
    {
      stop = true;
      mState = State::STOPPED;
    }
  }

  // Not atomic, but does not matter here as we just want to ensure we do not stop more than once
  if( stop )
  {
    // Notify update-thread so that it continues and sets up the other threads to stop as well
    mUpdateThreadWaitCondition.Notify();

    mFrameTime.Suspend();
  }
}

void ThreadSynchronization::Pause()
{
  LOG_EVENT( "%s", __FUNCTION__ );

  // Only pause if we're RUNNING or SLEEPING
  ConditionalWait::ScopedLock lock( mUpdateThreadWaitCondition );
  if( ( mState == State::RUNNING ) ||
      ( mState == State::SLEEPING ) )
  {
    mState = State::PAUSED;

    mUpdateThreadResuming = false;

    mFrameTime.Suspend();

    AddPerformanceMarker( PerformanceInterface::PAUSED );
  }
}

void ThreadSynchronization::Resume()
{
  LOG_EVENT( "%s", __FUNCTION__ );

  // Only resume if we're PAUSED
  bool resume = false;
  {
    ConditionalWait::ScopedLock lock( mUpdateThreadWaitCondition );
    if( mState == State::PAUSED )
    {
      resume = true;
      mState = State::RUNNING;
      mUpdateThreadResuming = true;
    }
  }

  // Not atomic, but does not matter here as we just want to ensure we only resume if we're paused
  if( resume )
  {
    // Start up Update thread again
    mUpdateThreadWaitCondition.Notify();

    AddPerformanceMarker( PerformanceInterface::RESUME);
  }
}

void ThreadSynchronization::UpdateRequest()
{
  LOG_EVENT( "%s", __FUNCTION__ );

  bool update = false;
  {
    ConditionalWait::ScopedLock lock( mUpdateThreadWaitCondition );
    if( mState == State::SLEEPING )
    {
      mState = State::RUNNING;
      update = true;
    }
    mTryToSleepCount = 0;
  }

  if( update )
  {
    mUpdateThreadWaitCondition.Notify();
  }
}

void ThreadSynchronization::UpdateOnce()
{
  LOG_EVENT( "%s", __FUNCTION__ );

  mUpdateThreadWaitCondition.Notify();
}

void ThreadSynchronization::ResumeFrameTime()
{
  mFrameTime.Resume();
}

void ThreadSynchronization::ReplaceSurface( RenderSurface* newSurface )
{
  LOG_EVENT( "%s: START", __FUNCTION__ );

  State::Type previousState( State::STOPPED );
  {
    ConditionalWait::ScopedLock lock( mUpdateThreadWaitCondition );
    previousState = mState;
    mState = State::REPLACING_SURFACE;
  }

  {
    ConditionalWait::ScopedLock lock( mRenderThreadWaitCondition );
    mReplaceSurfaceRequest.SetSurface( newSurface );
    mRenderThreadReplacingSurface = true;

    {
      // Ensures we have both render & event threads locked so that this is not changed in-between
      ConditionalWait::ScopedLock lock( mEventThreadWaitCondition );

      mEventThreadSurfaceReplaced = false;
    }
  }

  // Notify the RenderThread in case it's waiting
  mRenderThreadWaitCondition.Notify();

  {
    ConditionalWait::ScopedLock lock( mEventThreadWaitCondition );

    // Wait for RenderThread to replace the surface
    while( ! mEventThreadSurfaceReplaced )
    {
      mEventThreadWaitCondition.Wait( lock );
    }
  }

  {
    ConditionalWait::ScopedLock lock( mUpdateThreadWaitCondition );
    mState = previousState;
  }
  mUpdateThreadWaitCondition.Notify();

  LOG_EVENT( "%s: END", __FUNCTION__ );
}

void ThreadSynchronization::SetRenderRefreshRate( unsigned int numberOfVSyncsPerRender )
{
  mNumberOfVSyncsPerRender = numberOfVSyncsPerRender;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// UPDATE THREAD
///////////////////////////////////////////////////////////////////////////////////////////////////

bool ThreadSynchronization::UpdateReady( bool notifyEvent, bool runUpdate, float& lastFrameDeltaSeconds, unsigned int& lastSyncTimeMilliseconds, unsigned int& nextSyncTimeMilliseconds )
{
  ConditionalWait::ScopedLock updateLock( mUpdateThreadWaitCondition );

  switch( mState )
  {
    case State::STOPPED:
    {
      StopVSyncThread();
      StopRenderThread();
      return false; // Stop update-thread
    }

    case State::INITIALISING:
    {
      LOG_UPDATE( "%s: INITIALISING", __FUNCTION__ );

      // Notify event thread that this thread is up and running
      NotifyThreadInitialised();

      // Wait for first thread-sync point
      while( mState == State::INITIALISING )
      {
        mUpdateThreadWaitCondition.Wait( updateLock );
      }

      RunVSyncThread();
      break;
    }

    case State::PAUSED:
    {
      LOG_UPDATE( "%s: PAUSED", __FUNCTION__ );

      // Just pause the VSyncThread
      PauseVSyncThread();
    }
    // No break, fall through

    case State::RUNNING:
    {
      LOG_UPDATE( "%s: RUNNING", __FUNCTION__ );

      AddPerformanceMarker( PerformanceInterface::UPDATE_END );

      if( mUpdateThreadResuming )
      {
        mUpdateThreadResuming = false;

        // Restart the VSyncThread
        RunVSyncThread();
      }

      if( notifyEvent )
      {
        // Do the notifications first so the event thread can start processing them
        // Tell the event-thread to wake up (if asleep) and send a notification event to Core
        mNotificationTrigger.Trigger();
      }

      // Inform render thread
      int updateAheadOfRender = 0;
      {
        ConditionalWait::ScopedLock lock( mRenderThreadWaitCondition );
        updateAheadOfRender = ++mUpdateAheadOfRender;
        LOG_UPDATE_COUNTER_UPDATE( "%s: updateAheadOfRender(%d)", __FUNCTION__, updateAheadOfRender );
        (void)updateAheadOfRender; // Allows building for release
      }
      mRenderThreadWaitCondition.Notify();

      // Wait if we've reached the maximum-ahead-of-render count.
      // Also ensure we did not stop while we were waiting.
      while( ( mState != State::STOPPED ) && MaximumUpdateAheadOfRenderReached() )
      {
        mRenderThreadWaitCondition.Notify(); // Notify the render thread in case it was waiting

        mUpdateThreadWaitCondition.Wait( updateLock );
      }

      // Ensure we have had at least 1 V-Sync before we continue
      // Also ensure we did not stop while we were waiting
      int vSyncAheadOfUpdate = 0;
      if( ( ( vSyncAheadOfUpdate = __atomic_fetch_and( &mVSyncAheadOfUpdate, 0, __ATOMIC_SEQ_CST ) ) == 0 ) &&
          ( mState != State::STOPPED ) )
      {
        LOG_VSYNC_COUNTER_UPDATE( "%s:  vSyncAheadOfUpdate(%d) WAIT", __FUNCTION__, vSyncAheadOfUpdate );
        mUpdateThreadWaitCondition.Wait( updateLock );
        __atomic_store_n( &mVSyncAheadOfUpdate, 0, __ATOMIC_SEQ_CST ); // Reset to 0
      }
      else
      {
        LOG_VSYNC_COUNTER_UPDATE( "%s:  vSyncAheadOfUpdate(%d)", __FUNCTION__, vSyncAheadOfUpdate );
      }

      if( ! runUpdate )
      {
        LOG_UPDATE( "TryToSleep" );

        if( ++mTryToSleepCount >= 3 )
        {
          LOG_UPDATE( "Going to sleep" );

          PauseVSyncThread();

          // Render thread will automatically wait as it relies on update-ahead-of-render count

          // Change the state, we already have a mutex-lock
          mState = State::SLEEPING;

          // Inform FrameTime that we're going to sleep
          mFrameTime.Sleep();

          // Wait while we're SLEEPING
          while( mState == State::SLEEPING )
          {
            mUpdateThreadWaitCondition.Wait( updateLock );
          }

          LOG_UPDATE( "Waking Up" );

          // Clear V-Sync-ahead-of-update-count
          __atomic_store_n( &mVSyncAheadOfUpdate, 0, __ATOMIC_SEQ_CST );

          // Restart the v-sync-thread
          RunVSyncThread();

          // Reset try-to-sleep count
          mTryToSleepCount = 0;

          // Inform frame timer that we've woken up
          mFrameTime.WakeUp();
        }
      }
      else
      {
        mTryToSleepCount = 0;
      }

      break;
    }

    case State::SLEEPING:
    case State::REPLACING_SURFACE:
    {
      break;
    }
  }

  // Ensure we didn't stop while we were waiting
  if( mState == State::STOPPED )
  {
    StopVSyncThread();
    StopRenderThread();
    return false; // Stop update-thread
  }

  // Just wait if we're replacing the surface as the render-thread is busy
  while( mState == State::REPLACING_SURFACE )
  {
    PauseVSyncThread();
    mUpdateThreadWaitCondition.Wait( updateLock );
    __atomic_store_n( &mVSyncAheadOfUpdate, 0, __ATOMIC_SEQ_CST ); // Reset to 0
    RunVSyncThread();
  }

  mFrameTime.PredictNextSyncTime( lastFrameDeltaSeconds, lastSyncTimeMilliseconds, nextSyncTimeMilliseconds );

  AddPerformanceMarker( PerformanceInterface::UPDATE_START );

  return true; // Keep update-thread running
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// RENDER THREAD
///////////////////////////////////////////////////////////////////////////////////////////////////

bool ThreadSynchronization::RenderReady( RenderRequest*& requestPtr )
{
  ConditionalWait::ScopedLock renderLock( mRenderThreadWaitCondition );

  LOG_RENDER( "%s", __FUNCTION__ );

  if( ! mRenderThreadReplacingSurface )
  {
    if( ! mRenderThreadInitialised )
    {
      mRenderThreadInitialised = true;

      // Notify event thread that this thread is up and running
      NotifyThreadInitialised();
    }
    else
    {
      // We've just rendered
      AddPerformanceMarker( PerformanceInterface::RENDER_END );

      if( mRenderThreadSurfaceReplaced )
      {
        mRenderThreadSurfaceReplaced = false;
      }
      else
      {
        // decrement update-ahead-of-render
        --mUpdateAheadOfRender;
      }
    }

    // Check if we've had an update, if we haven't then we just wait
    // Ensure we do not wait if we're supposed to stop
    if( mUpdateAheadOfRender <= 0 && ! mRenderThreadStop )
    {
      LOG_UPDATE_COUNTER_RENDER( "%s: updateAheadOfRender(%d) WAIT", __FUNCTION__, mUpdateAheadOfRender );
      mRenderThreadWaitCondition.Wait( renderLock );
    }
    else
    {
      LOG_UPDATE_COUNTER_RENDER( "%s: updateAheadOfRender(%d)", __FUNCTION__, mUpdateAheadOfRender );
    }
  }
  else
  {
    // We've just rendered
    AddPerformanceMarker( PerformanceInterface::RENDER_END );

    // ... also decrement update-ahead-of-render
    --mUpdateAheadOfRender;
  }

  // We may have been asked to replace the surface while we were sleeping so check again here
  if( mRenderThreadReplacingSurface )
  {
    // Replacing surface
    LOG_RENDER( "%s: REPLACE SURFACE", __FUNCTION__ );

    requestPtr = &mReplaceSurfaceRequest;
    mRenderThreadReplacingSurface = false;
    mRenderThreadSurfaceReplaced = false;
  }

  AddPerformanceMarker( PerformanceInterface::RENDER_START );

  return ! mRenderThreadStop;
}

void ThreadSynchronization::RenderInformSurfaceReplaced()
{
  LOG_RENDER( "%s", __FUNCTION__ );

  mRenderThreadSurfaceReplaced = true;
  {
    ConditionalWait::ScopedLock lock( mEventThreadWaitCondition );
    mEventThreadSurfaceReplaced = true;
  }
  mEventThreadWaitCondition.Notify();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// V-SYNC THREAD
///////////////////////////////////////////////////////////////////////////////////////////////////

bool ThreadSynchronization::VSyncReady( bool validSync, unsigned int frameNumber, unsigned int seconds, unsigned int microseconds, unsigned int& numberOfVSyncsPerRender )
{
  ConditionalWait::ScopedLock vSyncLock( mVSyncThreadWaitCondition );

  LOG_VSYNC( "%s", __FUNCTION__ );

  if( numberOfVSyncsPerRender != mNumberOfVSyncsPerRender )
  {
    numberOfVSyncsPerRender = mNumberOfVSyncsPerRender; // save it back
    mFrameTime.SetMinimumFrameTimeInterval( mNumberOfVSyncsPerRender * TIME_PER_FRAME_IN_MICROSECONDS );
  }

  if( validSync )
  {
    mFrameTime.SetSyncTime( frameNumber );
  }

  if( ! mVSyncThreadInitialised )
  {
    mVSyncThreadInitialised = true;

    // Notify event thread that this thread is up and running
    NotifyThreadInitialised();
  }

  AddPerformanceMarker( PerformanceInterface::VSYNC );

  // Increment v-sync-ahead-of-update count and inform update-thread
  int vSyncAheadOfUpdate = __atomic_add_fetch( &mVSyncAheadOfUpdate, 1, __ATOMIC_SEQ_CST );
  (void)vSyncAheadOfUpdate;
  LOG_VSYNC_COUNTER_VSYNC( "%s: vSyncAheadOfUpdate(%d)\n",  __FUNCTION__, vSyncAheadOfUpdate );

  mUpdateThreadWaitCondition.Notify();

  // Ensure update-thread has set us to run before continuing
  // Ensure we do not wait if we're supposed to stop
  while( ! mVSyncThreadRunning && ! mVSyncThreadStop )
  {
    LOG_VSYNC( "%s: WAIT", __FUNCTION__ );
    mVSyncThreadWaitCondition.Wait( vSyncLock );
  }

  return ! mVSyncThreadStop;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
///////////////////////////////////////////////////////////////////////////////////////////////////

inline void ThreadSynchronization::AddPerformanceMarker( PerformanceInterface::MarkerType type )
{
  if( mPerformanceInterface )
  {
    mPerformanceInterface->AddMarker( type );
  }
}

void ThreadSynchronization::NotifyThreadInitialised()
{
  {
    ConditionalWait::ScopedLock lock( mEventThreadWaitCondition );
    ++mNumberOfThreadsStarted;
  }
  mEventThreadWaitCondition.Notify();
}

bool ThreadSynchronization::MaximumUpdateAheadOfRenderReached()
{
  ConditionalWait::ScopedLock lock( mRenderThreadWaitCondition );
  return mUpdateAheadOfRender >= mMaximumUpdateCount;
}

void ThreadSynchronization::RunVSyncThread()
{
  {
    ConditionalWait::ScopedLock lock( mVSyncThreadWaitCondition );
    mVSyncThreadRunning = true;
  }
  mVSyncThreadWaitCondition.Notify();
}

void ThreadSynchronization::PauseVSyncThread()
{
  ConditionalWait::ScopedLock lock( mVSyncThreadWaitCondition );
  mVSyncThreadRunning = false;
}

void ThreadSynchronization::StopVSyncThread()
{
  {
    ConditionalWait::ScopedLock lock( mVSyncThreadWaitCondition );
    mVSyncThreadStop = true;
  }
  mVSyncThreadWaitCondition.Notify();
}

void ThreadSynchronization::StopRenderThread()
{
  {
    ConditionalWait::ScopedLock lock( mRenderThreadWaitCondition );
    mRenderThreadStop = true;
  }
  mRenderThreadWaitCondition.Notify();
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
