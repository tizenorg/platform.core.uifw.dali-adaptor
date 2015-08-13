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
const unsigned int TRUE = 1u;
const unsigned int FALSE = 0u;
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
  mUpdateThreadResuming( FALSE ),
  mVSyncThreadRunning( FALSE ),
  mVSyncThreadStop( FALSE ),
  mRenderThreadStop( FALSE ),
  mRenderThreadReplacingSurface( FALSE ),
  mEventThreadSurfaceReplaced( FALSE ),
  mWaitingForFirstRenderAfterResume( FALSE ),
  mVSyncThreadInitialised( FALSE ),
  mRenderThreadInitialised( FALSE ),
  mRenderThreadSurfaceReplaced( FALSE )
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

      mUpdateThreadResuming = FALSE;

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
      mUpdateThreadResuming = TRUE;
    }
  }

  // Not atomic, but does not matter here as we just want to ensure we only resume if we're paused
  if( resume )
  {
    LOG_EVENT( "RESUMING" );

    {
      ConditionalWait::ScopedLock lock( mEventThreadWaitCondition );
      mWaitingForFirstRenderAfterResume = TRUE;
    }

    mFrameTime.Resume();

    // Start up Update thread again
    mUpdateThreadWaitCondition.Notify();

    // Can lock so we do not want to have a lock when calling this to avoid deadlocks
    AddPerformanceMarker( PerformanceInterface::RESUME);

    {
      ConditionalWait::ScopedLock lock( mEventThreadWaitCondition );
      while( mWaitingForFirstRenderAfterResume )
      {
        mEventThreadWaitCondition.Wait( lock );
      }
    }
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
    mEventThreadSurfaceReplaced = FALSE;
  }

  {
    ConditionalWait::ScopedLock lock( mRenderThreadWaitCondition );
    mReplaceSurfaceRequest.SetSurface( newSurface );
    mRenderThreadReplacingSurface = TRUE;
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
      StopAllThreads();
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

      if( IsUpdateThreadResuming() )
      {
        LOG_UPDATE( "Restarting VSyncThread" );

        {
          ConditionalWait::ScopedLock updateLock( mUpdateThreadWaitCondition );
          mUpdateThreadResuming = FALSE;
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
            break; // Break out of while loop
          }
          mUpdateThreadWaitCondition.Wait( updateLock );
        }
      }

      // Ensure we have had at least 1 V-Sync before we continue
      // Ensure we didn't stop while we were previously waiting
      {
        ConditionalWait::ScopedLock updateLock( mUpdateThreadWaitCondition );
        if( ( mState != State::STOPPED ) &&
            ( mVSyncAheadOfUpdate == 0 ) )
        {
          LOG_VSYNC_COUNTER_UPDATE( " vSyncAheadOfUpdate(%d) WAIT", mVSyncAheadOfUpdate );
          mUpdateThreadWaitCondition.Wait( updateLock );
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
    case State::REPLACING_SURFACE:
    {
      break;
    }
  }

  // Ensure we didn't stop while we were waiting
  if( IsUpdateThreadStopping() )
  {
    // Locks so we shouldn't have a scoped-lock when calling this
    StopAllThreads();
    return false; // Stop update-thread
  }

  // Just wait if we're replacing the surface as the render-thread is busy
  UpdateWaitIfReplacingSurface();

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

      mRenderThreadInitialised = TRUE;

      // Notify event thread that this thread is up and running, this locks so we should have a scoped-lock
      NotifyThreadInitialised();
    }
    else
    {
      if( mRenderThreadSurfaceReplaced )
      {
        mRenderThreadSurfaceReplaced = FALSE;
      }
      else
      {
        // decrement update-ahead-of-render
        ConditionalWait::ScopedLock renderLock( mRenderThreadWaitCondition );
        --mUpdateAheadOfRender;
      }
    }

    // Check if we've just resumed and notify event thread if we have.
    if( WaitingForFirstRenderAfterResume() )
    {
      {
        ConditionalWait::ScopedLock lock( mEventThreadWaitCondition );
        mWaitingForFirstRenderAfterResume = FALSE;
      }
      mEventThreadWaitCondition.Notify();
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
    mRenderThreadReplacingSurface = FALSE;
    mRenderThreadSurfaceReplaced = FALSE;
  }

  return IsRenderThreadRunning(); // Call to this function locks so should not be called if we have a scoped-lock
}

void ThreadSynchronization::RenderInformSurfaceReplaced()
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

    mVSyncThreadInitialised = TRUE;

    // Notify event thread that this thread is up and running, this locks so we should have a scoped-lock
    NotifyThreadInitialised();
  }
  else
  {
    // Increment v-sync-ahead-of-update count and inform update-thread
    {
      ConditionalWait::ScopedLock lock( mUpdateThreadWaitCondition );
      ++mVSyncAheadOfUpdate;
      LOG_VSYNC_COUNTER_VSYNC( " vSyncAheadOfUpdate(%d)", mVSyncAheadOfUpdate );
    }
    mUpdateThreadWaitCondition.Notify();
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

void ThreadSynchronization::UpdateInitialising()
{
  LOG_UPDATE_TRACE;

  // Notify event thread that this thread is up and running, locks so we shouldn't have a scoped-lock when calling this
  NotifyThreadInitialised();

  // Wait for first thread-sync point
  {
    ConditionalWait::ScopedLock updateLock( mUpdateThreadWaitCondition );

    while( mState == State::INITIALISING )
    {
      mUpdateThreadWaitCondition.Wait( updateLock );
    }
  }

  // Locks so we shouldn't have a scoped-lock when calling this
  RunVSyncThread();
}

void ThreadSynchronization::UpdateTryToSleep( bool runUpdate )
{
  LOG_UPDATE_TRACE;

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
        if( mState != State::STOPPED )
        {
          mState = State::SLEEPING;
        }
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

      ////////////////////////
      // WAKE UP
      ////////////////////////

      LOG_UPDATE( "Waking Up" );

      // Clear V-Sync-ahead-of-update-count
      {
        ConditionalWait::ScopedLock updateLock( mUpdateThreadWaitCondition );
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

void ThreadSynchronization::UpdateWaitIfReplacingSurface()
{
  bool replacingSurface = false;
  {
    ConditionalWait::ScopedLock updateLock( mUpdateThreadWaitCondition );
    replacingSurface = ( mState == State::REPLACING_SURFACE );
  }
  while( replacingSurface )
  {
    LOG_UPDATE_TRACE_FMT( "REPLACING SURFACE" );

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

    {
      ConditionalWait::ScopedLock updateLock( mUpdateThreadWaitCondition );
      mVSyncAheadOfUpdate = 0;
    }

    // Locks so should not be called while we have a scoped-lock
    RunVSyncThread();
  }
}

bool ThreadSynchronization::IsUpdateThreadResuming()
{
  ConditionalWait::ScopedLock updateLock( mUpdateThreadWaitCondition );
  return mUpdateThreadResuming;
}

bool ThreadSynchronization::IsUpdateThreadStopping()
{
  ConditionalWait::ScopedLock updateLock( mUpdateThreadWaitCondition );
  return ( mState == State::STOPPED );
}

bool ThreadSynchronization::MaximumUpdateAheadOfRenderReached()
{
  ConditionalWait::ScopedLock lock( mRenderThreadWaitCondition );
  return mUpdateAheadOfRender >= mMaximumUpdateCount;
}

void ThreadSynchronization::StopAllThreads()
{
  LOG_UPDATE_TRACE;

  // Lock so we shouldn't have a scoped-lock when calling these methods
  StopVSyncThread();
  StopRenderThread();
}

void ThreadSynchronization::RunVSyncThread()
{
  {
    ConditionalWait::ScopedLock lock( mVSyncThreadWaitCondition );
    mVSyncThreadRunning = TRUE;
  }
  mVSyncThreadWaitCondition.Notify();
}

void ThreadSynchronization::PauseVSyncThread()
{
  ConditionalWait::ScopedLock lock( mVSyncThreadWaitCondition );
  mVSyncThreadRunning = FALSE;
}

void ThreadSynchronization::StopVSyncThread()
{
  {
    ConditionalWait::ScopedLock lock( mVSyncThreadWaitCondition );
    mVSyncThreadStop = TRUE;
  }
  mVSyncThreadWaitCondition.Notify();
}

void ThreadSynchronization::StopRenderThread()
{
  {
    ConditionalWait::ScopedLock lock( mRenderThreadWaitCondition );
    mRenderThreadStop = TRUE;
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

bool ThreadSynchronization::WaitingForFirstRenderAfterResume()
{
  ConditionalWait::ScopedLock lock( mEventThreadWaitCondition );
  return mWaitingForFirstRenderAfterResume;
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
