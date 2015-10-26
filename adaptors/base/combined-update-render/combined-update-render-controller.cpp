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
#include <base/interfaces/adaptor-internal-services.h>
#include <base/environment-options.h>


namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace
{
#if defined(DEBUG_ENABLED)
Integration::Log::Filter* gLogFilter = Integration::Log::Filter::New( Debug::NoLogging, false, "LOG_THREAD_SYNC" );
#endif

const unsigned int TOTAL_THREAD_COUNT = 2u;
const unsigned int TIME_PER_FRAME_IN_NANOSECONDS( 16666667u );
const unsigned int TRUE = 1u;
const unsigned int FALSE = 0u;
const unsigned int MICROSECONDS_PER_SECOND( 1000000u );
const unsigned int MICROSECONDS_PER_MILLISECOND( 1000u );
const float        MICROSECONDS_TO_SECOND( 0.000001f );

const unsigned int MILLISECONDS_PER_FRAME( 17u );
const float FRAME_DELTA = 0.016666667;
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
  mVSyncMonitor( adaptorInterfaces.GetVSyncMonitorInterface() ),
  mVSyncThread( NULL ),
  mUpdateRenderThread( NULL ),
  mLastRenderTime( 0 ),
  mNumberOfVSyncsPerRender( 1 ),
  mNumberOfThreadsStarted( 0 ),
  mVSyncAheadOfUpdate( 0 ),
  mVSyncThreadRunning( FALSE ),
  mVSyncThreadStop( FALSE ),
  mUpdateRenderThreadStop( FALSE ),
  mNewSurface( NULL ),
  mSurfaceReplaced( FALSE ),
  mPostRendering( FALSE )
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "%s()\n", __FUNCTION__ );

  // Set the thread-synchronization interface on the render-surface
  RenderSurface* currentSurface = mAdaptorInterfaces.GetRenderSurfaceInterface();
  if( currentSurface )
  {
    currentSurface->SetThreadSynchronization( *this );
  }
}

CombinedUpdateRenderController::~CombinedUpdateRenderController()
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "%s()\n", __FUNCTION__ );

  Stop();
}

void CombinedUpdateRenderController::Initialize()
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "%s()\n", __FUNCTION__ );

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
  DALI_LOG_INFO( gLogFilter, Debug::General, "%s()\n", __FUNCTION__ );

  DALI_ASSERT_ALWAYS( mVSyncThread && mUpdateRenderThread );

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
  PauseVSyncThread();

  AddPerformanceMarker( PerformanceInterface::PAUSED );
}

void CombinedUpdateRenderController::Resume()
{
  if( IsVSyncThreadPaused() )
  {
    // Increment VSync Ahead Of Render count so we get one update/render straight away
    IncrementVSyncAheadOfRender();

    RunVSyncThread();

    AddPerformanceMarker( PerformanceInterface::RESUME );
  }
}

void CombinedUpdateRenderController::Stop()
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "%s()\n", __FUNCTION__ );

  // Stop the VSync Thread
  StopVSyncThread();

  if( mVSyncThread )
  {
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
    // wait for the thread to finish
    pthread_join( *mUpdateRenderThread, NULL );

    delete mUpdateRenderThread;
    mUpdateRenderThread = NULL;
  }
}

void CombinedUpdateRenderController::RequestUpdate()
{
  if( IsVSyncThreadPaused() )
  {
    DALI_LOG_INFO( gLogFilter, Debug::General, "%s()\n", __FUNCTION__ );

    // Increment VSync Ahead Of Render count so we get one update/render straight away
    IncrementVSyncAheadOfRender();

    RunVSyncThread();
  }
}

void CombinedUpdateRenderController::RequestUpdateOnce()
{
  if( IsVSyncThreadPaused() )
  {
    DALI_LOG_INFO( gLogFilter, Debug::General, "%s()\n", __FUNCTION__ );

    // Increment VSync Ahead Of Render count so we get one update/render
    IncrementVSyncAheadOfRender();
  }
}

void CombinedUpdateRenderController::ReplaceSurface( RenderSurface* newSurface )
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "%s()\n", __FUNCTION__ );

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
  DALI_LOG_INFO( gLogFilter, Debug::General, "%s()\n", __FUNCTION__ );

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

///////////////////////////////////////////////////////////////////////////////////////////////////
// EVENT THREAD & VSYNC THREAD
///////////////////////////////////////////////////////////////////////////////////////////////////
void CombinedUpdateRenderController::IncrementVSyncAheadOfRender()
{
  // Increment v-sync-ahead-of-update count and inform update-render-thread
  {
    ConditionalWait::ScopedLock lock( mUpdateRenderThreadWaitCondition );
    ++mVSyncAheadOfUpdate;
    //LOG_VSYNC_COUNTER_VSYNC( " vSyncAheadOfUpdate(%d)", mVSyncAheadOfUpdate );
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

  NotifyThreadInitialised();

  while( VSyncReady() )
  {
    DALI_LOG_INFO( gLogFilter, Debug::General, "%s()\n", __FUNCTION__ );

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

  // uninstall a function for logging
  mEnvironmentOptions.UnInstallLogFunction();
}

bool CombinedUpdateRenderController::VSyncReady()
{
  IncrementVSyncAheadOfRender();

  // Ensure update-thread has set us to run before continuing
  // Ensure we do not wait if we're supposed to stop
  {
    ConditionalWait::ScopedLock vSyncLock( mVSyncThreadWaitCondition );
    while( ! mVSyncThreadRunning && ! mVSyncThreadStop )
    {
      //LOG_VSYNC( "WAIT" );
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
    DALI_LOG_INFO( gLogFilter, Debug::General, "%s()\n", __FUNCTION__ );

    //////////////////////////////
    // REPLACE SURFACE
    //////////////////////////////
    RenderSurface* newSurface = ShouldSurfaceBeReplaced();
    if( newSurface )
    {
      mRenderHelper.ReplaceSurface( newSurface );
      SurfaceReplaced();
    }

    //////////////////////////////
    // CALCULATE TIMES
    //////////////////////////////

    mPlatformAbstraction.GetTimeMicroseconds( seconds, microSeconds );
    currentTime = ( seconds * MICROSECONDS_PER_SECOND ) + microSeconds;

    absoluteTimeSinceLastRender = currentTime - mLastRenderTime * MICROSECONDS_TO_SECOND;
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

    // TODO: Stop rendering if update not required

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
      //LOG_VSYNC_COUNTER_UPDATE( " vSyncAheadOfUpdate(%d) WAIT", mVSyncAheadOfUpdate );
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
