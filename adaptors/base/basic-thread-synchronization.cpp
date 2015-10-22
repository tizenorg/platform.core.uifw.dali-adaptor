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
#include "basic-thread-synchronization.h"

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
const int TOTAL_THREAD_COUNT = 2;
const unsigned int TRUE = 1u;
const unsigned int FALSE = 0u;
} // unnamed namespace

BasicThreadSynchronization::BasicThreadSynchronization( AdaptorInternalServices& adaptorInterfaces, unsigned int numberOfVSyncsPerRender)
: ThreadSynchronizationBase( adaptorInterfaces ),
  mReplaceSurfaceRequest(),
  mUpdateRenderThreadWaitCondition(),
  mVSyncThreadWaitCondition(),
  mEventThreadWaitCondition(),
  mNumberOfVSyncsPerRender( numberOfVSyncsPerRender ),
  mTryToSleepCount( 0u ),
  mState( State::STOPPED ),
  mVSyncAheadOfUpdate( 0u ),
  mNumberOfThreadsStarted( 0u ),
  mUpdateRenderThreadResuming( FALSE ),
  mVSyncThreadRunning( FALSE ),
  mVSyncThreadStop( FALSE ),
  mRenderThreadPostRendering( FALSE ),
  mEventThreadSurfaceReplaced( FALSE ),
  mVSyncThreadInitialised( FALSE ),
  mRenderThreadSurfaceReplaced( FALSE )
{
}

BasicThreadSynchronization::~BasicThreadSynchronization()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EVENT THREAD
///////////////////////////////////////////////////////////////////////////////////////////////////

void BasicThreadSynchronization::Initialise()
{
  LOG_EVENT_TRACE;

  ConditionalWait::ScopedLock lock( mUpdateRenderThreadWaitCondition );
  if( mState == State::STOPPED )
  {
    LOG_EVENT( "INITIALISING" );
    mState = State::INITIALISING;
  }
}

void BasicThreadSynchronization::Start()
{
  LOG_EVENT_TRACE;

  bool start = false;
  {
    ConditionalWait::ScopedLock lock( mUpdateRenderThreadWaitCondition );
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
      ConditionalWait::ScopedLock lock( mUpdateRenderThreadWaitCondition  );
      mState = State::RUNNING;
    }
    mUpdateRenderThreadWaitCondition .Notify();
  }
}

void BasicThreadSynchronization::Stop()
{
  LOG_EVENT_TRACE;

  bool stop = false;
  {
    ConditionalWait::ScopedLock lock( mUpdateRenderThreadWaitCondition  );
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
    mUpdateRenderThreadWaitCondition .Notify();

    mFrameTime.Suspend();
  }
}

void BasicThreadSynchronization::Pause()
{
  LOG_EVENT_TRACE;

  bool addPerformanceMarker = false;
  {
    // Only pause if we're RUNNING or SLEEPING
    ConditionalWait::ScopedLock lock( mUpdateRenderThreadWaitCondition  );
    if( ( mState == State::RUNNING ) ||
        ( mState == State::SLEEPING ) )
    {
      LOG_EVENT( "PAUSING" );

      mState = State::PAUSED;

      mUpdateRenderThreadResuming = FALSE;

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

void BasicThreadSynchronization::Resume()
{
  LOG_EVENT_TRACE;

  // Only resume if we're PAUSED
  bool resume = false;
  {
    ConditionalWait::ScopedLock lock( mUpdateRenderThreadWaitCondition  );
    if( mState == State::PAUSED )
    {
      resume = true;
      mState = State::RUNNING;
      mUpdateRenderThreadResuming = TRUE;
    }
  }

  // Not atomic, but does not matter here as we just want to ensure we only resume if we're paused
  if( resume )
  {
    LOG_EVENT( "RESUMING" );

    mFrameTime.Resume();

    // Start up Update thread again
    mUpdateRenderThreadWaitCondition .Notify();

    // Can lock so we do not want to have a lock when calling this to avoid deadlocks
    AddPerformanceMarker( PerformanceInterface::RESUME);
  }
}

void BasicThreadSynchronization::UpdateRequest()
{
  LOG_EVENT_TRACE;

  bool update = false;
  {
    ConditionalWait::ScopedLock lock( mUpdateRenderThreadWaitCondition  );
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
    mUpdateRenderThreadWaitCondition .Notify();
  }
}

void BasicThreadSynchronization::UpdateOnce()
{
  LOG_EVENT_TRACE;
  LOG_EVENT( "UPDATE ONCE" );

  // If we're sleeping then change state to running as this will also wake up the v-sync-thread
  {
    ConditionalWait::ScopedLock lock( mUpdateRenderThreadWaitCondition  );
    if( mState == State::SLEEPING )
    {
      mState = State::RUNNING;
    }
  }

  mUpdateRenderThreadWaitCondition .Notify();
}

void BasicThreadSynchronization::ReplaceSurface( RenderSurface* newSurface )
{
  LOG_EVENT_TRACE;

  {
    ConditionalWait::ScopedLock lock( mEventThreadWaitCondition );
    mEventThreadSurfaceReplaced = FALSE;
  }

  State::Type previousState( State::STOPPED );
  {
    ConditionalWait::ScopedLock lock( mUpdateRenderThreadWaitCondition );
    previousState = mState;
    mReplaceSurfaceRequest.SetSurface( newSurface );
    mState = State::REPLACING_SURFACE;
  }

  // Notify the UpdateRenderThread in case it's waiting
  mUpdateRenderThreadWaitCondition.Notify();

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
    ConditionalWait::ScopedLock lock( mUpdateRenderThreadWaitCondition );
    mState = previousState;
  }
  mUpdateRenderThreadWaitCondition.Notify();
}

void BasicThreadSynchronization::SetRenderRefreshRate( unsigned int numberOfVSyncsPerRender )
{
  LOG_EVENT_TRACE;
  LOG_EVENT( "SET RENDER REFRESH RATE" );

  mNumberOfVSyncsPerRender = numberOfVSyncsPerRender;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// UPDATE THREAD
///////////////////////////////////////////////////////////////////////////////////////////////////

bool BasicThreadSynchronization::UpdateRenderReady( bool notifyEvent, bool runUpdate, float& lastFrameDeltaSeconds, unsigned int& lastSyncTimeMilliseconds, unsigned int& nextSyncTimeMilliseconds, RenderRequest*& requestPtr )
{
  LOG_UPDATE_TRACE;

  State::Type state = State::STOPPED;
  {
    ConditionalWait::ScopedLock updateLock( mUpdateRenderThreadWaitCondition );
    state = mState;
  }

  switch( state )
  {
    case State::STOPPED:
    {
      StopVSyncThread();
      return false; // Stop update-thread
    }

    case State::INITIALISING:
    {
      UpdateInitialising();
      break;
    }

    case State::PAUSED:
    {
      LOG_UPDATE_TRACE_FMT( "PAUSED" );

      // Just pause the VSyncThread, locks so we shouldn't have a scoped-lock when calling this
      PauseVSyncThread();
    }
    // No break, fall through

    case State::RUNNING:
    {
      LOG_UPDATE_TRACE_FMT( "RUNNING" );

      if( IsUpdateRenderThreadResuming() )
      {
        LOG_UPDATE( "Restarting VSyncThread" );

        {
          ConditionalWait::ScopedLock updateLock( mUpdateRenderThreadWaitCondition );
          mUpdateRenderThreadResuming = FALSE;
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

      // Ensure we have had at least 1 V-Sync before we continue
      // Ensure we didn't stop while we were previously waiting
      {
        ConditionalWait::ScopedLock updateLock( mUpdateRenderThreadWaitCondition );
        if( ( mState != State::STOPPED ) &&
            ( mVSyncAheadOfUpdate == 0 ) &&
            ( !mUpdateRenderThreadResuming ) ) // Ensure we don't wait if the update-thread is JUST resuming
        {
          LOG_VSYNC_COUNTER_UPDATE( " vSyncAheadOfUpdate(%d) WAIT", mVSyncAheadOfUpdate );
          mUpdateRenderThreadWaitCondition.Wait( updateLock );
        }
        else
        {
          LOG_VSYNC_COUNTER_UPDATE( " vSyncAheadOfUpdate(%d)", mVSyncAheadOfUpdate );
        }
        mVSyncAheadOfUpdate = 0;
      }

      // Try to sleep if we do not require any more updates
      UpdateTryToSleep( runUpdate );

      break;
    }

    case State::SLEEPING:
    {
      DALI_ASSERT_ALWAYS( false && "Should never reach here" );
      break;
    }

    case State::REPLACING_SURFACE:
    {
      break;
    }
  }

  // Ensure we didn't stop while we were waiting
  if( IsUpdateRenderThreadStopping() )
  {
    // Locks so we shouldn't have a scoped-lock when calling this
    StopVSyncThread();
    return false; // Stop update-thread
  }

  // Ensure we haven't been asked to replace the surface while we were waiting
  if( IsUpdateRenderReplacingSurface() )
  {
    // Replacing surface
    LOG_UPDATE( "REPLACE SURFACE" );

    ConditionalWait::ScopedLock updateLock( mUpdateRenderThreadWaitCondition );
    requestPtr = &mReplaceSurfaceRequest;
    mRenderThreadSurfaceReplaced = FALSE;
  }

  mFrameTime.PredictNextSyncTime( lastFrameDeltaSeconds, lastSyncTimeMilliseconds, nextSyncTimeMilliseconds );

  return true; // Keep update-thread running
}

void BasicThreadSynchronization::UpdateRenderInformSurfaceReplaced()
{
  LOG_RENDER_TRACE;

  mRenderThreadSurfaceReplaced = TRUE;
  {
    ConditionalWait::ScopedLock lock( mEventThreadWaitCondition );
    mEventThreadSurfaceReplaced = TRUE;
  }
  mEventThreadWaitCondition.Notify();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// V-SYNC THREAD
///////////////////////////////////////////////////////////////////////////////////////////////////

bool BasicThreadSynchronization::VSyncReady( bool validSync, unsigned int frameNumber, unsigned int seconds, unsigned int microseconds, unsigned int& numberOfVSyncsPerRender )
{
  LOG_VSYNC_TRACE;

  // Ensure we do not process an invalid v-sync
  if( validSync )
  {
    bool minimumFrameTimeIntervalChanged = false;
    {
      ConditionalWait::ScopedLock vSyncLock( mVSyncThreadWaitCondition );
      if( numberOfVSyncsPerRender != mNumberOfVSyncsPerRender )
      {
        numberOfVSyncsPerRender = mNumberOfVSyncsPerRender; // save it back
        minimumFrameTimeIntervalChanged = true;
      }
    }

    if( minimumFrameTimeIntervalChanged )
    {
      mFrameTime.SetMinimumFrameTimeInterval( mNumberOfVSyncsPerRender * TIME_PER_FRAME_IN_MICROSECONDS );
    }

    mFrameTime.SetSyncTime( frameNumber );

    if( ! mVSyncThreadInitialised )
    {
      LOG_VSYNC( "Initialised" );

      mVSyncThreadInitialised = TRUE;

      // Notify event thread that this thread is up and running, this locks so we should have a scoped-lock
      NotifyThreadInitialised();
    }
    else
    {
      // Increment v-sync-ahead-of-update count and inform update-thread
      {
        ConditionalWait::ScopedLock lock( mUpdateRenderThreadWaitCondition );
        ++mVSyncAheadOfUpdate;
        LOG_VSYNC_COUNTER_VSYNC( " vSyncAheadOfUpdate(%d)", mVSyncAheadOfUpdate );
      }
      mUpdateRenderThreadWaitCondition.Notify();
    }

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
  }
  else
  {
    LOG_VSYNC( "INVALID SYNC" );

    // Later we still check if the v-sync thread is supposed to keep running so we can still stop the thread if we are supposed to
  }

  return IsVSyncThreadRunning(); // Call to this function locks so should not be called if we have a scoped-lock
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// POST RENDERING: EVENT THREAD
/////////////////////////////////////////////////////////////////////////////////////////////////

void BasicThreadSynchronization::PostRenderComplete()
{
  LOG_EVENT_TRACE;

  {
    ConditionalWait::ScopedLock lock( mUpdateRenderThreadWaitCondition );
    mRenderThreadPostRendering = FALSE;
  }
  mUpdateRenderThreadWaitCondition.Notify();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// POST RENDERING: RENDER THREAD
///////////////////////////////////////////////////////////////////////////////////////////////////

void BasicThreadSynchronization::PostRenderStarted()
{
  LOG_RENDER_TRACE;

  ConditionalWait::ScopedLock lock( mUpdateRenderThreadWaitCondition );
  mRenderThreadPostRendering = TRUE;
}

void BasicThreadSynchronization::PostRenderWaitForCompletion()
{
  LOG_RENDER_TRACE;

  ConditionalWait::ScopedLock lock( mUpdateRenderThreadWaitCondition );
  while( mRenderThreadPostRendering &&
         mState != State::REPLACING_SURFACE ) // We should NOT wait if we're replacing the surface
  {
    LOG_RENDER( "WAIT" );
    mUpdateRenderThreadWaitCondition.Wait( lock );
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

void BasicThreadSynchronization::NotifyThreadInitialised()
{
  {
    ConditionalWait::ScopedLock lock( mEventThreadWaitCondition );
    ++mNumberOfThreadsStarted;
  }
  mEventThreadWaitCondition.Notify();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Called by Update-Render Thread
///////////////////////////////////////////////////////////////////////////////////////////////////

void BasicThreadSynchronization::UpdateInitialising()
{
  LOG_UPDATE_TRACE;

  // Notify event thread that this thread is up and running, locks so we shouldn't have a scoped-lock when calling this
  NotifyThreadInitialised();

  // Wait for first thread-sync point
  {
    ConditionalWait::ScopedLock updateLock( mUpdateRenderThreadWaitCondition );

    while( mState == State::INITIALISING )
    {
      mUpdateRenderThreadWaitCondition.Wait( updateLock );
    }
  }

  // Locks so we shouldn't have a scoped-lock when calling this
  RunVSyncThread();
}

void BasicThreadSynchronization::UpdateTryToSleep( bool runUpdate )
{
  LOG_UPDATE_TRACE;

  if( ! runUpdate &&
      ! IsUpdateRenderThreadResuming() ) // Locks so we shouldn't have a lock, we shouldn't try to sleep if we're JUST resuming
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
        ConditionalWait::ScopedLock updateLock( mUpdateRenderThreadWaitCondition );

        // Ensure we weren't stopped while we have been processing
        if( mState != State::STOPPED )
        {
          mState = State::SLEEPING;
        }
      }

      // Inform FrameTime that we're going to sleep
      mFrameTime.Sleep();

      // Wait while we're SLEEPING
      {
        ConditionalWait::ScopedLock updateLock( mUpdateRenderThreadWaitCondition );
        while( mState == State::SLEEPING )
        {
          mUpdateRenderThreadWaitCondition.Wait( updateLock );
        }
      }

      ////////////////////////
      // WAKE UP
      ////////////////////////

      LOG_UPDATE( "Waking Up" );

      // Clear V-Sync-ahead-of-update-count
      {
        ConditionalWait::ScopedLock updateLock( mUpdateRenderThreadWaitCondition );
        mVSyncAheadOfUpdate = 0;
      }

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
}

bool BasicThreadSynchronization::IsUpdateRenderThreadResuming()
{
  ConditionalWait::ScopedLock updateLock( mUpdateRenderThreadWaitCondition );
  return mUpdateRenderThreadResuming;
}

bool BasicThreadSynchronization::IsUpdateRenderThreadStopping()
{
  ConditionalWait::ScopedLock updateLock( mUpdateRenderThreadWaitCondition );
  return ( mState == State::STOPPED );
}

bool BasicThreadSynchronization::IsUpdateRenderReplacingSurface()
{
  ConditionalWait::ScopedLock updateLock( mUpdateRenderThreadWaitCondition );
  return ( mState == State::REPLACING_SURFACE );
}

void BasicThreadSynchronization::RunVSyncThread()
{
  {
    ConditionalWait::ScopedLock lock( mVSyncThreadWaitCondition );
    mVSyncThreadRunning = TRUE;
  }
  mVSyncThreadWaitCondition.Notify();
}

void BasicThreadSynchronization::PauseVSyncThread()
{
  ConditionalWait::ScopedLock lock( mVSyncThreadWaitCondition );
  mVSyncThreadRunning = FALSE;
}

void BasicThreadSynchronization::StopVSyncThread()
{
  {
    ConditionalWait::ScopedLock lock( mVSyncThreadWaitCondition );
    mVSyncThreadStop = TRUE;
  }
  mVSyncThreadWaitCondition.Notify();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Called by V-Sync Thread
///////////////////////////////////////////////////////////////////////////////////////////////////

bool BasicThreadSynchronization::IsVSyncThreadRunning()
{
  ConditionalWait::ScopedLock lock( mVSyncThreadWaitCondition );
  return ! mVSyncThreadStop;
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
