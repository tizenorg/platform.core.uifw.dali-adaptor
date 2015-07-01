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

// Backwards support for older versions of GCC (< 4.7) - atomic store not supported in older versions, added for legacy building
#if __GNUC__ < 4 || \
  ( __GNUC__ == 4 && ( __GNUC_MINOR__ < 7 ) )
#define __atomic_fetch_and( variable, value, flags )              __sync_fetch_and_and( variable, value )
#define __atomic_store_n( variable, value, flags )                __sync_and_and_fetch( variable, 0 ) // We're always setting to 0 anyway
#define __atomic_add_fetch( variable, value, flags )              __sync_add_and_fetch( variable, value )
#endif

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
  LOG_EVENT_TRACE;

  ConditionalWait::ScopedLock lock( mUpdateThreadWaitCondition );
  if( mState == State::STOPPED )
  {
    LOG_EVENT( "INITIALISING" );
    mState = State::INITIALISING;
  }
}

void ThreadSynchronization::Start()
{
  LOG_EVENT_TRACE;

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
    LOG_EVENT( "STARTING" );
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
  LOG_EVENT_TRACE;

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
    LOG_EVENT( "STOPPING" );

    // Notify update-thread so that it continues and sets up the other threads to stop as well
    mUpdateThreadWaitCondition.Notify();

    mFrameTime.Suspend();
  }
}

void ThreadSynchronization::Pause()
{
  LOG_EVENT_TRACE;

  bool addPerformanceMarker = false;
  {
    // Only pause if we're RUNNING or SLEEPING
    ConditionalWait::ScopedLock lock( mUpdateThreadWaitCondition );
    if( ( mState == State::RUNNING ) ||
        ( mState == State::SLEEPING ) )
    {
      LOG_EVENT( "PAUSING" );

      mState = State::PAUSED;

      mUpdateThreadResuming = false;

      mFrameTime.Suspend();

      addPerformanceMarker = true;
    }
  }

  if( addPerformanceMarker )
  {
    // Can lock so we do not want to have a lock when calling this to avoid deadlocks
    AddPerformanceMarker( PerformanceInterface::PAUSED );
  }
}

void ThreadSynchronization::Resume()
{
  LOG_EVENT_TRACE;

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
    LOG_EVENT( "RESUMING" );

    mFrameTime.Resume();

    // Start up Update thread again
    mUpdateThreadWaitCondition.Notify();

    // Can lock so we do not want to have a lock when calling this to avoid deadlocks
    AddPerformanceMarker( PerformanceInterface::RESUME);
  }
}

void ThreadSynchronization::UpdateRequest()
{
  LOG_EVENT_TRACE;

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
    LOG_EVENT( "UPDATE REQUEST" );
    mUpdateThreadWaitCondition.Notify();
  }
}

void ThreadSynchronization::UpdateOnce()
{
  LOG_EVENT_TRACE;
  LOG_EVENT( "UPDATE ONCE" );

  mUpdateThreadWaitCondition.Notify();
}

void ThreadSynchronization::ReplaceSurface( RenderSurface* newSurface )
{
  LOG_EVENT_TRACE;

  State::Type previousState( State::STOPPED );
  {
    ConditionalWait::ScopedLock lock( mUpdateThreadWaitCondition );
    previousState = mState;
    mState = State::REPLACING_SURFACE;
  }

  {
    ConditionalWait::ScopedLock lock( mEventThreadWaitCondition );
    mEventThreadSurfaceReplaced = false;
  }

  {
    ConditionalWait::ScopedLock lock( mRenderThreadWaitCondition );
    mReplaceSurfaceRequest.SetSurface( newSurface );
    mRenderThreadReplacingSurface = true;
  }

  // Notify the RenderThread in case it's waiting
  mRenderThreadWaitCondition.Notify();

  {
    ConditionalWait::ScopedLock lock( mEventThreadWaitCondition );

    // Wait for RenderThread to replace the surface
    while( ! mEventThreadSurfaceReplaced )
    {
      LOG_EVENT( "Waiting for Surface to be Replaced" );

      mEventThreadWaitCondition.Wait( lock );
    }
  }

  {
    ConditionalWait::ScopedLock lock( mUpdateThreadWaitCondition );
    mState = previousState;
  }
  mUpdateThreadWaitCondition.Notify();
}

void ThreadSynchronization::SetRenderRefreshRate( unsigned int numberOfVSyncsPerRender )
{
  LOG_EVENT_TRACE;
  LOG_EVENT( "SET RENDER REFRESH RATE" );

  mNumberOfVSyncsPerRender = numberOfVSyncsPerRender;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// UPDATE THREAD
///////////////////////////////////////////////////////////////////////////////////////////////////

bool ThreadSynchronization::UpdateReady( bool notifyEvent, bool runUpdate, float& lastFrameDeltaSeconds, unsigned int& lastSyncTimeMilliseconds, unsigned int& nextSyncTimeMilliseconds )
{
  LOG_UPDATE_TRACE;

  State::Type state = State::STOPPED;
  {
    ConditionalWait::ScopedLock updateLock( mUpdateThreadWaitCondition );
    state = mState;
  }

  switch( state )
  {
    case State::STOPPED:
    {
      LOG_UPDATE( "STOPPED" );

      // These methods lock so we should not have a scoped-lock when calling these
      StopVSyncThread();
      StopRenderThread();
      return false; // Stop update-thread
    }

    case State::INITIALISING:
    {
      LOG_UPDATE( "INITIALISING" );

      // Notify event thread that this thread is up and running, locks so we shouldn't have a scoped-lock when calling this
      NotifyThreadInitialised();

      {
        ConditionalWait::ScopedLock updateLock( mUpdateThreadWaitCondition );
        // Wait for first thread-sync point
        while( mState == State::INITIALISING )
        {
          mUpdateThreadWaitCondition.Wait( updateLock );
        }
      }

      // Locks so we shouldn't have a scoped-lock when calling this
      RunVSyncThread();
      break;
    }

    case State::PAUSED:
    {
      LOG_UPDATE( "PAUSED" );

      // Just pause the VSyncThread, locks so we shouldn't have a scoped-lock when calling this
      PauseVSyncThread();
    }
    // No break, fall through

    case State::RUNNING:
    {
      LOG_UPDATE( "RUNNING" );

      bool updateThreadResuming = false;
      {
        ConditionalWait::ScopedLock updateLock( mUpdateThreadWaitCondition );
        updateThreadResuming = mUpdateThreadResuming;
      }

      if( updateThreadResuming )
      {
        LOG_UPDATE( "Restarting VSyncThread" );

        {
          ConditionalWait::ScopedLock updateLock( mUpdateThreadWaitCondition );
          mUpdateThreadResuming = false;
        }

        // Restart the VSyncThread, locks so we shouldn't have a scoped-lock when calling this
        RunVSyncThread();
      }

      if( notifyEvent )
      {
        LOG_UPDATE( "Notify Event Thread" );

        // Do the notifications first so the event thread can start processing them
        // Tell the event-thread to wake up (if asleep) and send a notification event to Core
        mNotificationTrigger.Trigger();
      }

      // Inform render thread
      {
        ConditionalWait::ScopedLock lock( mRenderThreadWaitCondition );
        ++mUpdateAheadOfRender;
        LOG_UPDATE_COUNTER_UPDATE( "updateAheadOfRender(%d)", mUpdateAheadOfRender );
      }
      mRenderThreadWaitCondition.Notify();

      bool stopped = false;

      // Wait if we've reached the maximum-ahead-of-render count.
      while( MaximumUpdateAheadOfRenderReached() )
      {
        LOG_UPDATE( "Maximum Update Ahead of Render: WAIT" );

        mRenderThreadWaitCondition.Notify(); // Notify the render thread in case it was waiting

        {
          // Ensure we did not stop while we were waiting previously.
          ConditionalWait::ScopedLock updateLock( mUpdateThreadWaitCondition );
          if( mState == State::STOPPED )
          {
            stopped = true;
            break; // Break out of while loop
          }
          mUpdateThreadWaitCondition.Wait( updateLock );
        }
      }

      // Only continue if we didn't stop
      if( stopped )
      {
        break; // Break out of switch statement
      }

      // Ensure we have had at least 1 V-Sync before we continue
      // Ensure we didn't stop while we were previously waiting
      {
        ConditionalWait::ScopedLock updateLock( mUpdateThreadWaitCondition );
        int vSyncAheadOfUpdate = 0;
        if( ( mState != State::STOPPED ) && ( ( vSyncAheadOfUpdate = __atomic_fetch_and( &mVSyncAheadOfUpdate, 0, __ATOMIC_SEQ_CST ) ) == 0 ) )
        {
          LOG_VSYNC_COUNTER_UPDATE( " vSyncAheadOfUpdate(%d) WAIT", vSyncAheadOfUpdate );
          mUpdateThreadWaitCondition.Wait( updateLock );
          __atomic_store_n( &mVSyncAheadOfUpdate, 0, __ATOMIC_SEQ_CST ); // Reset to 0
        }
        else
        {
          LOG_VSYNC_COUNTER_UPDATE( " vSyncAheadOfUpdate(%d)", vSyncAheadOfUpdate );
        }
      }

      if( ! runUpdate )
      {
        LOG_UPDATE( "TryToSleep" );

        if( ++mTryToSleepCount >= 3 )
        {
          LOG_UPDATE( "Going to sleep" );

          // Locks so we shouldn't have a scoped-lock when calling this
          PauseVSyncThread();

          // Render thread will automatically wait as it relies on update-ahead-of-render count

          // Change the state
          {
            ConditionalWait::ScopedLock updateLock( mUpdateThreadWaitCondition );

            // Ensure we weren't stopped while we have been processing
            if( mState == State::STOPPED )
            {
              break; // Break out of switch statement
            }
            mState = State::SLEEPING;
          }

          // Inform FrameTime that we're going to sleep
          mFrameTime.Sleep();

          // Wait while we're SLEEPING
          {
            ConditionalWait::ScopedLock updateLock( mUpdateThreadWaitCondition );
            while( mState == State::SLEEPING )
            {
              mUpdateThreadWaitCondition.Wait( updateLock );
            }
          }

          LOG_UPDATE( "Waking Up" );

          // Clear V-Sync-ahead-of-update-count
          __atomic_store_n( &mVSyncAheadOfUpdate, 0, __ATOMIC_SEQ_CST );

          // Restart the v-sync-thread, locks so we shouldn't have a scoped-lock
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
  bool stopped = false;
  {
    ConditionalWait::ScopedLock updateLock( mUpdateThreadWaitCondition );
    stopped = ( mState == State::STOPPED );
  }
  if( stopped )
  {
    LOG_UPDATE( "STOPPED" );

    // Lock so we shouldn't have a scoped-lock when calling these methods
    StopVSyncThread();
    StopRenderThread();
    return false; // Stop update-thread
  }

  // Just wait if we're replacing the surface as the render-thread is busy
  bool replacingSurface = false;
  {
    ConditionalWait::ScopedLock updateLock( mUpdateThreadWaitCondition );
    replacingSurface = ( mState == State::REPLACING_SURFACE );
  }
  while( replacingSurface )
  {
    LOG_UPDATE( "REPLACING SURFACE" );

    // Locks so should not be called while we have a scoped-lock
    PauseVSyncThread();

    // One last check before we actually wait in case the state has changed since we checked earlier
    {
      ConditionalWait::ScopedLock updateLock( mUpdateThreadWaitCondition );
      replacingSurface = ( mState == State::REPLACING_SURFACE );
      if( replacingSurface )
      {
        mUpdateThreadWaitCondition.Wait( updateLock );
      }
    }

    __atomic_store_n( &mVSyncAheadOfUpdate, 0, __ATOMIC_SEQ_CST ); // Reset to 0

    // Locks so should not be called while we have a scoped-lock
    RunVSyncThread();
  }

  mFrameTime.PredictNextSyncTime( lastFrameDeltaSeconds, lastSyncTimeMilliseconds, nextSyncTimeMilliseconds );

  return true; // Keep update-thread running
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// RENDER THREAD
///////////////////////////////////////////////////////////////////////////////////////////////////

bool ThreadSynchronization::RenderReady( RenderRequest*& requestPtr )
{
  LOG_RENDER_TRACE;

  if( ! IsRenderThreadReplacingSurface() ) // Call to this function locks so should not be called if we have a scoped-lock
  {
    if( ! mRenderThreadInitialised )
    {
      LOG_RENDER( "Initialised" );

      mRenderThreadInitialised = true;

      // Notify event thread that this thread is up and running, this locks so we should have a scoped-lock
      NotifyThreadInitialised();
    }
    else
    {
      if( mRenderThreadSurfaceReplaced )
      {
        mRenderThreadSurfaceReplaced = false;
      }
      else
      {
        // decrement update-ahead-of-render
        ConditionalWait::ScopedLock renderLock( mRenderThreadWaitCondition );
        --mUpdateAheadOfRender;
      }
    }

    // Check if we've had an update, if we haven't then we just wait
    // Ensure we do not wait if we're supposed to stop
    {
      ConditionalWait::ScopedLock renderLock( mRenderThreadWaitCondition );
      if( mUpdateAheadOfRender <= 0 && ! mRenderThreadStop )
      {
        LOG_UPDATE_COUNTER_RENDER( "updateAheadOfRender(%d) WAIT", mUpdateAheadOfRender );
        mRenderThreadWaitCondition.Wait( renderLock );
      }
      else
      {
        LOG_UPDATE_COUNTER_RENDER( "updateAheadOfRender(%d)", mUpdateAheadOfRender );
      }
    }
  }
  else
  {
    LOG_RENDER( "Just Rendered, now Replacing surface" );

    // ... also decrement update-ahead-of-render
    ConditionalWait::ScopedLock renderLock( mRenderThreadWaitCondition );
    --mUpdateAheadOfRender;
  }

  // We may have been asked to replace the surface while we were waiting so check again here
  if( IsRenderThreadReplacingSurface() )
  {
    // Replacing surface
    LOG_RENDER( "REPLACE SURFACE" );

    ConditionalWait::ScopedLock renderLock( mRenderThreadWaitCondition );
    requestPtr = &mReplaceSurfaceRequest;
    mRenderThreadReplacingSurface = false;
    mRenderThreadSurfaceReplaced = false;
  }

  return IsRenderThreadRunning(); // Call to this function locks so should not be called if we have a scoped-lock
}

void ThreadSynchronization::RenderInformSurfaceReplaced()
{
  LOG_RENDER_TRACE;

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
  LOG_VSYNC_TRACE;

  {
    ConditionalWait::ScopedLock vSyncLock( mVSyncThreadWaitCondition );
    if( numberOfVSyncsPerRender != mNumberOfVSyncsPerRender )
    {
      numberOfVSyncsPerRender = mNumberOfVSyncsPerRender; // save it back
      mFrameTime.SetMinimumFrameTimeInterval( mNumberOfVSyncsPerRender * TIME_PER_FRAME_IN_MICROSECONDS );
    }

    if( validSync )
    {
      mFrameTime.SetSyncTime( frameNumber );
    }
  }

  if( ! mVSyncThreadInitialised )
  {
    LOG_VSYNC( "Initialised" );

    mVSyncThreadInitialised = true;

    // Notify event thread that this thread is up and running, this locks so we should have a scoped-lock
    NotifyThreadInitialised();
  }

  // Increment v-sync-ahead-of-update count and inform update-thread
  int vSyncAheadOfUpdate = __atomic_add_fetch( &mVSyncAheadOfUpdate, 1, __ATOMIC_SEQ_CST );
  (void)vSyncAheadOfUpdate;
  LOG_VSYNC_COUNTER_VSYNC( "vSyncAheadOfUpdate(%d)",  vSyncAheadOfUpdate );

  mUpdateThreadWaitCondition.Notify();

  // Ensure update-thread has set us to run before continuing
  // Ensure we do not wait if we're supposed to stop
  {
    ConditionalWait::ScopedLock vSyncLock( mVSyncThreadWaitCondition );
    while( ! mVSyncThreadRunning && ! mVSyncThreadStop )
    {
      LOG_VSYNC( "WAIT" );
      mVSyncThreadWaitCondition.Wait( vSyncLock );
    }
  }

  return IsVSyncThreadRunning(); // Call to this function locks so should not be called if we have a scoped-lock
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// ALL THREADS: Performance Marker
///////////////////////////////////////////////////////////////////////////////////////////////////

void ThreadSynchronization::AddPerformanceMarker( PerformanceInterface::MarkerType type )
{
  if( mPerformanceInterface )
  {
    mPerformanceInterface->AddMarker( type );
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// PRIVATE METHODS
//
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// Called by ALL Threads
///////////////////////////////////////////////////////////////////////////////////////////////////

void ThreadSynchronization::NotifyThreadInitialised()
{
  {
    ConditionalWait::ScopedLock lock( mEventThreadWaitCondition );
    ++mNumberOfThreadsStarted;
  }
  mEventThreadWaitCondition.Notify();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Called by Update Thread
///////////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////////
// Called by V-Sync Thread
///////////////////////////////////////////////////////////////////////////////////////////////////

bool ThreadSynchronization::IsVSyncThreadRunning()
{
  ConditionalWait::ScopedLock lock( mVSyncThreadWaitCondition );
  return ! mVSyncThreadStop;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Called by Render Thread
///////////////////////////////////////////////////////////////////////////////////////////////////

bool ThreadSynchronization::IsRenderThreadRunning()
{
  ConditionalWait::ScopedLock lock( mRenderThreadWaitCondition );
  return ! mRenderThreadStop;
}

bool ThreadSynchronization::IsRenderThreadReplacingSurface()
{
  ConditionalWait::ScopedLock lock( mRenderThreadWaitCondition );
  return mRenderThreadReplacingSurface;
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
