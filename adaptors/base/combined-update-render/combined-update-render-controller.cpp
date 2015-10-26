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
#include "combined-update-render-controller.h"

// EXTERNAL INCLUDES
#include <iostream>
#include <dali/integration-api/platform-abstraction.h>

// INTERNAL INCLUDES
#include <trigger-event-factory.h>
#include <base/combined-update-render/combined-update-render-controller-debug.h>
#include <base/environment-options.h>
#include <base/interfaces/adaptor-internal-services.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace
{
const unsigned int TOTAL_THREAD_COUNT = 2u;

const unsigned int TRUE = 1u;
const unsigned int FALSE = 0u;

const unsigned int MICROSECONDS_PER_SECOND( 1000000u );
const unsigned int MICROSECONDS_PER_MILLISECOND( 1000u );
const float        MICROSECONDS_TO_SECOND( 0.000001f );

const unsigned int TIME_PER_FRAME_IN_NANOSECONDS( 16666667u );
const unsigned int MILLISECONDS_PER_FRAME( 17u );
const float        FRAME_DELTA = 0.016666667;

const unsigned int MINIMUM_SLEEP_REQUESTS = 2;
} // unnamed namespace

///////////////////////////////////////////////////////////////////////////////////////////////////
// EVENT THREAD
///////////////////////////////////////////////////////////////////////////////////////////////////

CombinedUpdateRenderController::CombinedUpdateRenderController( AdaptorInternalServices& adaptorInterfaces, const EnvironmentOptions& environmentOptions )
: mFpsTracker( environmentOptions ),
  mUpdateStatusLogger( environmentOptions ),
  mRenderHelper( adaptorInterfaces ),
  mEventThreadWaitCondition(),
  mVSyncThreadWaitCondition(),
  mUpdateRenderThreadWaitCondition(),
  mAdaptorInterfaces( adaptorInterfaces ),
  mPerformanceInterface( adaptorInterfaces.GetPerformanceInterface() ),
  mCore( adaptorInterfaces.GetCore() ),
  mPlatformAbstraction( adaptorInterfaces.GetPlatformAbstractionInterface() ),
  mEnvironmentOptions( environmentOptions ),
  mNotificationTrigger( adaptorInterfaces.GetProcessCoreEventsTrigger() ),
  mSleepTrigger( NULL ),
  mVSyncMonitor( adaptorInterfaces.GetVSyncMonitorInterface() ),
  mVSyncThread( NULL ),
  mUpdateRenderThread( NULL ),
  mLastRenderTime( 0 ),
  mNumberOfVSyncsPerRender( 1 ),
  mTryToSleepCount( 0u ),
  mNumberOfThreadsStarted( 0 ),
  mVSyncAheadOfUpdate( 0 ),
  mVSyncThreadRunning( FALSE ),
  mVSyncThreadStop( FALSE ),
  mUpdateRenderThreadStop( FALSE ),
  mNewSurface( NULL ),
  mSurfaceReplaced( FALSE ),
  mPostRendering( FALSE )
{
  LOG_EVENT_TRACE;

  // Set the thread-synchronization interface on the render-surface
  RenderSurface* currentSurface = mAdaptorInterfaces.GetRenderSurfaceInterface();
  if( currentSurface )
  {
    currentSurface->SetThreadSynchronization( *this );
  }

  TriggerEventFactoryInterface& triggerFactory = mAdaptorInterfaces.GetTriggerEventFactoryInterface();
  mSleepTrigger = triggerFactory.CreateTriggerEvent( MakeCallback( this, &CombinedUpdateRenderController::ProcessSleepRequest ), TriggerEventInterface::KEEP_ALIVE_AFTER_TRIGGER );
}

CombinedUpdateRenderController::~CombinedUpdateRenderController()
{
  LOG_EVENT_TRACE;

  Stop();

  delete mSleepTrigger;
}

void CombinedUpdateRenderController::Initialize()
{
  LOG_EVENT_TRACE;

  // Create VSync Thread
  DALI_ASSERT_ALWAYS( ! mVSyncThread );

  mVSyncThread = new pthread_t();
  int error = pthread_create( mVSyncThread, NULL, InternalVSyncThreadEntryFunc, this );
  DALI_ASSERT_ALWAYS( !error && "Return code from pthread_create() when creating VSyncThread" );

  // Create Update/Render Thread
  DALI_ASSERT_ALWAYS( ! mUpdateRenderThread );

  mUpdateRenderThread = new pthread_t();
  error = pthread_create( mUpdateRenderThread, NULL, InternalUpdateRenderThreadEntryFunc, this );
  DALI_ASSERT_ALWAYS( !error && "Return code from pthread_create() when creating UpdateRenderThread" );

  mRenderHelper.Start();
}

void CombinedUpdateRenderController::Start()
{
  LOG_EVENT_TRACE;

  DALI_ASSERT_ALWAYS( mVSyncThread && mUpdateRenderThread );

  mTryToSleepCount = 0;

  // Wait till all threads have initialised
  {
    ConditionalWait::ScopedLock lock( mEventThreadWaitCondition );
    while( mNumberOfThreadsStarted < TOTAL_THREAD_COUNT )
    {
      mEventThreadWaitCondition.Wait( lock );
    }
  }

  // Increment VSync Ahead Of Render count so we get one update/render straight away
  IncrementVSyncAheadOfRender();

  RunVSyncThread();
}

void CombinedUpdateRenderController::Pause()
{
  LOG_EVENT_TRACE;

  PauseVSyncThread();

  AddPerformanceMarker( PerformanceInterface::PAUSED );
}

void CombinedUpdateRenderController::Resume()
{
  LOG_EVENT_TRACE;

  mTryToSleepCount = 0;

  if( IsVSyncThreadPaused() )
  {
    LOG_EVENT( "Resuming" );;

    // Increment VSync Ahead Of Render count so we get one update/render straight away
    IncrementVSyncAheadOfRender();

    RunVSyncThread();

    AddPerformanceMarker( PerformanceInterface::RESUME );
  }
}

void CombinedUpdateRenderController::Stop()
{
  LOG_EVENT_TRACE;

  // Stop the VSync Thread
  StopVSyncThread();

  if( mVSyncThread )
  {
    LOG_EVENT( "Destroying VSyncThread" );

    // wait for the thread to finish
    pthread_join( *mVSyncThread, NULL );

    delete mVSyncThread;
    mVSyncThread = NULL;
  }

  mVSyncMonitor->Terminate();

  // Stop Rendering and the Update/Render Thread
  mRenderHelper.Stop();

  StopUpdateRenderThread();

  if( mUpdateRenderThread )
  {
    LOG_EVENT( "Destroying UpdateRenderThread" );

    // wait for the thread to finish
    pthread_join( *mUpdateRenderThread, NULL );

    delete mUpdateRenderThread;
    mUpdateRenderThread = NULL;
  }
}

void CombinedUpdateRenderController::RequestUpdate()
{
  LOG_EVENT_TRACE;

  mTryToSleepCount = 0;

  if( IsVSyncThreadPaused() )
  {
    // Increment VSync Ahead Of Render count so we get one update/render straight away
    IncrementVSyncAheadOfRender();

    RunVSyncThread();
  }
}

void CombinedUpdateRenderController::RequestUpdateOnce()
{
  if( IsVSyncThreadPaused() )
  {
    LOG_EVENT_TRACE;

    // Increment VSync Ahead Of Render count so we get one update/render
    IncrementVSyncAheadOfRender();
  }
}

void CombinedUpdateRenderController::ReplaceSurface( RenderSurface* newSurface )
{
  LOG_EVENT_TRACE;

  // Set the ThreadSyncronizationInterface on the new surface
  newSurface->SetThreadSynchronization( *this );

  // Ensure the current surface releases any locks to prevent deadlock.
  RenderSurface* currentSurface = mAdaptorInterfaces.GetRenderSurfaceInterface();
  if( currentSurface )
  {
    currentSurface->StopRender();
  }

  // Start replacing the surface. This will block until the update-render thread has replaced the surface.
  {
    ConditionalWait::ScopedLock lock( mEventThreadWaitCondition );
    mSurfaceReplaced = FALSE;
  }

  {
    ConditionalWait::ScopedLock lock( mUpdateRenderThreadWaitCondition );
    mNewSurface = newSurface;
  }
  mUpdateRenderThreadWaitCondition.Notify();

  {
    ConditionalWait::ScopedLock lock( mEventThreadWaitCondition );
    while( ! mSurfaceReplaced )
    {
      mEventThreadWaitCondition.Wait( lock );
    }
  }
}

void CombinedUpdateRenderController::SetRenderRefreshRate( unsigned int numberOfVSyncsPerRender )
{
  LOG_EVENT_TRACE;

  // Not protected by lock, but written to rarely so not worth adding a lock when reading
  mNumberOfVSyncsPerRender = numberOfVSyncsPerRender;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EVENT THREAD
///////////////////////////////////////////////////////////////////////////////////////////////////

void CombinedUpdateRenderController::RunVSyncThread()
{
  {
    ConditionalWait::ScopedLock lock( mVSyncThreadWaitCondition );
    mVSyncThreadRunning = TRUE;
  }
  mVSyncThreadWaitCondition.Notify();
}

void CombinedUpdateRenderController::PauseVSyncThread()
{
  ConditionalWait::ScopedLock lock( mVSyncThreadWaitCondition );
  mVSyncThreadRunning = FALSE;
}

void CombinedUpdateRenderController::StopVSyncThread()
{
  {
    ConditionalWait::ScopedLock lock( mVSyncThreadWaitCondition );
    mVSyncThreadStop = TRUE;
  }
  mVSyncThreadWaitCondition.Notify();
}

bool CombinedUpdateRenderController::IsVSyncThreadPaused()
{
  ConditionalWait::ScopedLock lock( mVSyncThreadWaitCondition );
  return ! mVSyncThreadRunning;
}

void CombinedUpdateRenderController::StopUpdateRenderThread()
{
  {
    ConditionalWait::ScopedLock lock( mUpdateRenderThreadWaitCondition );
    mUpdateRenderThreadStop = TRUE;
  }
  mUpdateRenderThreadWaitCondition.Notify();
}

void CombinedUpdateRenderController::ProcessSleepRequest()
{
  LOG_EVENT_TRACE;

  if( ++mTryToSleepCount >= MINIMUM_SLEEP_REQUESTS )
  {
    LOG_EVENT( "Going to sleep" );

    PauseVSyncThread();
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EVENT THREAD & VSYNC THREAD
///////////////////////////////////////////////////////////////////////////////////////////////////
void CombinedUpdateRenderController::IncrementVSyncAheadOfRender()
{
  // Increment v-sync-ahead-of-update count and inform update-render-thread
  {
    ConditionalWait::ScopedLock lock( mUpdateRenderThreadWaitCondition );
    ++mVSyncAheadOfUpdate;
    LOG_VSYNC_COUNTER_VSYNC( " vSyncAheadOfUpdate(%d)", mVSyncAheadOfUpdate );
  }
  mUpdateRenderThreadWaitCondition.Notify();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// VSYNC THREAD
///////////////////////////////////////////////////////////////////////////////////////////////////

void CombinedUpdateRenderController::VSyncThread()
{
  // install a function for logging
  mEnvironmentOptions.InstallLogFunction();

  LOG_VSYNC( "THREAD CREATED" );

  NotifyThreadInitialised();

  while( VSyncReady() )
  {
    LOG_VSYNC_TRACE;

    // Hardware VSyncs available?
    if( mVSyncMonitor->UseHardware() )
    {
      // Yes..wait for N hardware VSync ticks

      unsigned int currentSequenceNumber( 0u );   // platform specific vsync sequence number (increments with each vsync)
      unsigned int currentSeconds( 0u );          // timestamp at latest sync
      unsigned int currentMicroseconds( 0u );     // timestamp at latest sync

      for( unsigned int i = 0; i < mNumberOfVSyncsPerRender; ++i )
      {
        // Ensure it's a valid sync before continuing
        while( ! mVSyncMonitor->DoSync( currentSequenceNumber, currentSeconds, currentMicroseconds ) )
        {
          // If we're about to stop, then just break out
          if( ! IsVSyncThreadRunning() )
          {
            break;
          }
        }
      }
    }
    else
    {
      // No..use software timer
      timespec sleepTime;
      sleepTime.tv_sec = 0;
      sleepTime.tv_nsec = mNumberOfVSyncsPerRender * TIME_PER_FRAME_IN_NANOSECONDS;
      nanosleep( &sleepTime, NULL );
    }

    AddPerformanceMarker( PerformanceInterface::VSYNC );
  }

  LOG_VSYNC( "THREAD DESTROYED" );

  // uninstall a function for logging
  mEnvironmentOptions.UnInstallLogFunction();
}

bool CombinedUpdateRenderController::VSyncReady()
{
  IncrementVSyncAheadOfRender();

  // Ensure event-thread has set us to run before continuing
  // Ensure we do not wait if we're supposed to stop
  {
    ConditionalWait::ScopedLock vSyncLock( mVSyncThreadWaitCondition );
    while( ! mVSyncThreadRunning &&
           ! mVSyncThreadStop )
    {
      LOG_VSYNC( "WAIT" );
      mVSyncThreadWaitCondition.Wait( vSyncLock );
    }
  }

  return IsVSyncThreadRunning();
}

bool CombinedUpdateRenderController::IsVSyncThreadRunning()
{
  ConditionalWait::ScopedLock lock( mVSyncThreadWaitCondition );
  return ! mVSyncThreadStop;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// UPDATE/RENDER THREAD
///////////////////////////////////////////////////////////////////////////////////////////////////

void CombinedUpdateRenderController::UpdateRenderThread()
{
  // Install a function for logging
  mEnvironmentOptions.InstallLogFunction();

  LOG_UPDATE_RENDER( "THREAD CREATED" );

  mRenderHelper.InitializeEgl();

  // tell core it has a context
  mCore.ContextCreated();

  NotifyThreadInitialised();

  Integration::UpdateStatus updateStatus;
  Dali::Integration::RenderStatus renderStatus;
  uint64_t currentTime = 0;
  unsigned int seconds = 0;
  unsigned int microSeconds = 0;
  unsigned int nextSyncTime = 0;
  float frameDelta = 0.0f;
  float absoluteTimeSinceLastRender = 0.0f;

  while( UpdateRenderReady() )
  {
    LOG_UPDATE_RENDER_TRACE;

    //////////////////////////////
    // REPLACE SURFACE
    //////////////////////////////
    RenderSurface* newSurface = ShouldSurfaceBeReplaced();
    if( newSurface )
    {
      LOG_UPDATE_RENDER_TRACE_FMT( "Replacing Surface" );
      mRenderHelper.ReplaceSurface( newSurface );
      SurfaceReplaced();
    }

    //////////////////////////////
    // CALCULATE TIMES
    //////////////////////////////

    mPlatformAbstraction.GetTimeMicroseconds( seconds, microSeconds );
    currentTime = ( seconds * MICROSECONDS_PER_SECOND ) + microSeconds;

    absoluteTimeSinceLastRender = ( currentTime - mLastRenderTime ) * MICROSECONDS_TO_SECOND;
    mLastRenderTime = currentTime;

    frameDelta = mNumberOfVSyncsPerRender * FRAME_DELTA;

    currentTime /= MICROSECONDS_PER_MILLISECOND;
    nextSyncTime = currentTime + mNumberOfVSyncsPerRender * MILLISECONDS_PER_FRAME;

    //////////////////////////////
    // UPDATE
    //////////////////////////////

    AddPerformanceMarker( PerformanceInterface::UPDATE_START );
    mCore.Update( frameDelta, currentTime, nextSyncTime, updateStatus );
    AddPerformanceMarker( PerformanceInterface::UPDATE_END );

    mFpsTracker.Track( absoluteTimeSinceLastRender );

    unsigned int keepUpdatingStatus = updateStatus.KeepUpdating();

    // Optional logging of update/render status
    mUpdateStatusLogger.Log( keepUpdatingStatus );

    // Tell the event-thread to wake up (if asleep) and send a notification event to Core if required
    if( updateStatus.NeedsNotification() )
    {
      mNotificationTrigger.Trigger();
    }

    // Trigger event thread to sleep if update not required
    if( Integration::KeepUpdating::NOT_REQUESTED == keepUpdatingStatus )
    {
      mSleepTrigger->Trigger();
    }

    //////////////////////////////
    // RENDER
    //////////////////////////////

    mRenderHelper.ConsumeEvents();
    mRenderHelper.PreRender();

    Integration::RenderStatus renderStatus;

    AddPerformanceMarker( PerformanceInterface::RENDER_START );
    mCore.Render( renderStatus );
    AddPerformanceMarker( PerformanceInterface::RENDER_END );

    if( renderStatus.HasRendered() )
    {
      mRenderHelper.PostRender();
    }
  }

  // Inform core of context destruction & shutdown EGL
  mCore.ContextDestroyed();
  mRenderHelper.ShutdownEgl();

  LOG_UPDATE_RENDER( "THREAD DESTROYED" );

  // Uninstall the logging function
  mEnvironmentOptions.UnInstallLogFunction();
}

bool CombinedUpdateRenderController::UpdateRenderReady()
{
  // Ensure we have had at least 1 V-Sync before we continue
  // Ensure we didn't stop while we were previously waiting
  {
    ConditionalWait::ScopedLock updateLock( mUpdateRenderThreadWaitCondition );
    while( ( mVSyncAheadOfUpdate == 0 ) &&
           ! mUpdateRenderThreadStop && // Ensure we don't wait if the update-render-thread is supposed to stop
           ! mNewSurface ) // Ensure we don't wait if we need to replace the surface
    {
      LOG_VSYNC_COUNTER_UPDATE_RENDER( " vSyncAheadOfUpdate(%d) WAIT", mVSyncAheadOfUpdate );
      mUpdateRenderThreadWaitCondition.Wait( updateLock );
    }
    mVSyncAheadOfUpdate = 0;
  }

  return IsUpdateRenderThreadRunning();
}

bool CombinedUpdateRenderController::IsUpdateRenderThreadRunning()
{
  ConditionalWait::ScopedLock lock( mUpdateRenderThreadWaitCondition );
  return ! mUpdateRenderThreadStop;
}

RenderSurface* CombinedUpdateRenderController::ShouldSurfaceBeReplaced()
{
  ConditionalWait::ScopedLock lock( mUpdateRenderThreadWaitCondition );

  RenderSurface* newSurface = mNewSurface;
  mNewSurface = NULL;

  return newSurface;
}

void CombinedUpdateRenderController::SurfaceReplaced()
{
  {
    ConditionalWait::ScopedLock lock( mEventThreadWaitCondition );
    mSurfaceReplaced = TRUE;
  }
  mEventThreadWaitCondition.Notify();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// ALL THREADS
///////////////////////////////////////////////////////////////////////////////////////////////////

void CombinedUpdateRenderController::NotifyThreadInitialised()
{
  {
    ConditionalWait::ScopedLock lock( mEventThreadWaitCondition );
    ++mNumberOfThreadsStarted;
  }
  mEventThreadWaitCondition.Notify();
}

void CombinedUpdateRenderController::AddPerformanceMarker( PerformanceInterface::MarkerType type )
{
  if( mPerformanceInterface )
  {
    mPerformanceInterface->AddMarker( type );
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// POST RENDERING: EVENT THREAD
/////////////////////////////////////////////////////////////////////////////////////////////////

void CombinedUpdateRenderController::PostRenderComplete()
{
  {
    ConditionalWait::ScopedLock lock( mUpdateRenderThreadWaitCondition );
    mPostRendering = FALSE;
  }
  mUpdateRenderThreadWaitCondition.Notify();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// POST RENDERING: RENDER THREAD
///////////////////////////////////////////////////////////////////////////////////////////////////

void CombinedUpdateRenderController::PostRenderStarted()
{
  ConditionalWait::ScopedLock lock( mUpdateRenderThreadWaitCondition );
  mPostRendering = TRUE;
}

void CombinedUpdateRenderController::PostRenderWaitForCompletion()
{
  ConditionalWait::ScopedLock lock( mUpdateRenderThreadWaitCondition );
  while( mPostRendering &&
         ! mNewSurface ) // We should NOT wait if we're replacing the surface
  {
    mUpdateRenderThreadWaitCondition.Wait( lock );
  }
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali