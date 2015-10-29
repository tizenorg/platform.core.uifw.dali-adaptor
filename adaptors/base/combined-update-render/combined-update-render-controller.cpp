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
#include <dali/integration-api/platform-abstraction.h>

// INTERNAL INCLUDES
#include <trigger-event-factory.h>
#include <base/combined-update-render/combined-update-render-controller-debug.h>
#include <base/environment-options.h>
#include <base/time-service.h>
#include <base/interfaces/adaptor-internal-services.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace
{
const unsigned int CREATED_THREAD_COUNT = 1u;

const int CONTINUOUS = -1;

const unsigned int TRUE = 1u;
const unsigned int FALSE = 0u;

const unsigned int MILLISECONDS_PER_SECOND( 1e+3 );

const unsigned int NANOSECONDS_PER_SECOND( 1e+9 );
const unsigned int NANOSECONDS_PER_MILLISECOND( 1e+6 );
const float        NANOSECONDS_TO_SECOND( 1e-9f );

const unsigned int TIME_PER_FRAME_IN_NANOSECONDS( 16666667u );
const unsigned int MILLISECONDS_PER_FRAME( 17u );
const float        FRAME_DELTA = 1.0f / 60.0f;

const unsigned int MINIMUM_SLEEP_REQUESTS = 2;
} // unnamed namespace

///////////////////////////////////////////////////////////////////////////////////////////////////
// EVENT THREAD
///////////////////////////////////////////////////////////////////////////////////////////////////

CombinedUpdateRenderController::CombinedUpdateRenderController( AdaptorInternalServices& adaptorInterfaces, const EnvironmentOptions& environmentOptions )
: mFpsTracker( environmentOptions ),
  mUpdateStatusLogger( environmentOptions ),
  mRenderHelper( adaptorInterfaces ),
  mEventThreadSemaphore(),
  mUpdateRenderThreadWaitCondition(),
  mAdaptorInterfaces( adaptorInterfaces ),
  mPerformanceInterface( adaptorInterfaces.GetPerformanceInterface() ),
  mCore( adaptorInterfaces.GetCore() ),
  mEnvironmentOptions( environmentOptions ),
  mNotificationTrigger( adaptorInterfaces.GetProcessCoreEventsTrigger() ),
  mSleepTrigger( NULL ),
  mUpdateRenderThread( NULL ),
  mLastFrameTime( 0u ),
  mNumberOfFramesPerRender( environmentOptions.GetRenderRefreshRate() ),
  mTryToSleepCount( 0u ),
  mRunning( FALSE ),
  mUpdateRenderRunCount( 0 ),
  mDestroyUpdateRenderThread( FALSE ),
  mNewSurface( NULL ),
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

  sem_init( &mEventThreadSemaphore, 0, 0 ); // Initialize to 0 so that it just waits if sem_post has not been called
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

  // Create Update/Render Thread
  DALI_ASSERT_ALWAYS( ! mUpdateRenderThread );

  mUpdateRenderThread = new pthread_t();
  int error = pthread_create( mUpdateRenderThread, NULL, InternalUpdateRenderThreadEntryFunc, this );
  DALI_ASSERT_ALWAYS( !error && "Return code from pthread_create() when creating UpdateRenderThread" );
}

void CombinedUpdateRenderController::Start()
{
  LOG_EVENT_TRACE;

  DALI_ASSERT_ALWAYS( !mRunning && mUpdateRenderThread );

  mTryToSleepCount = 0;

  // Wait till all created threads have initialised
  for( unsigned int i = 0; i < CREATED_THREAD_COUNT; ++i )
  {
    sem_wait( &mEventThreadSemaphore );
  }

  mRenderHelper.Start();

  mRunning = TRUE;

  LOG_EVENT( "Startup Complete, starting Update/Render Thread" )

  RunUpdateRenderThread( CONTINUOUS );
}

void CombinedUpdateRenderController::Pause()
{
  LOG_EVENT_TRACE;

  mRunning = FALSE;

  PauseUpdateRenderThread();

  AddPerformanceMarker( PerformanceInterface::PAUSED );
}

void CombinedUpdateRenderController::Resume()
{
  LOG_EVENT_TRACE;

  mTryToSleepCount = 0;

  if( !mRunning && IsUpdateRenderThreadPaused() )
  {
    LOG_EVENT( "Resuming" );;

    RunUpdateRenderThread( CONTINUOUS );

    AddPerformanceMarker( PerformanceInterface::RESUME );

    mRunning = TRUE;
  }
}

void CombinedUpdateRenderController::Stop()
{
  LOG_EVENT_TRACE;

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

  mRunning = FALSE;
}

void CombinedUpdateRenderController::RequestUpdate()
{
  LOG_EVENT_TRACE;

  mTryToSleepCount = 0;

  if( mRunning && IsUpdateRenderThreadPaused() )
  {
    LOG_EVENT( "Processing" );

    RunUpdateRenderThread( CONTINUOUS );
  }
}

void CombinedUpdateRenderController::RequestUpdateOnce()
{
  if( IsUpdateRenderThreadPaused() )
  {
    LOG_EVENT_TRACE;

    // Run Update/Render once
    RunUpdateRenderThread( 1 );
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
    ConditionalWait::ScopedLock lock( mUpdateRenderThreadWaitCondition );
    mNewSurface = newSurface;
    mUpdateRenderThreadWaitCondition.Notify( lock );
  }

  LOG_EVENT( "Starting to replace the surface, event-thread blocked" );

  // Wait until the surface has been replaced
  sem_wait( &mEventThreadSemaphore );

  LOG_EVENT( "Surface replaced, event-thread continuing" );
}

void CombinedUpdateRenderController::SetRenderRefreshRate( unsigned int numberOfFramesPerRender )
{
  LOG_EVENT_TRACE;

  // Not protected by lock, but written to rarely so not worth adding a lock when reading
  mNumberOfFramesPerRender = numberOfFramesPerRender;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EVENT THREAD
///////////////////////////////////////////////////////////////////////////////////////////////////

void CombinedUpdateRenderController::RunUpdateRenderThread( int numberOfCycles )
{
  ConditionalWait::ScopedLock lock( mUpdateRenderThreadWaitCondition );
  mUpdateRenderRunCount = numberOfCycles;
  LOG_COUNTER_EVENT( "mUpdateRenderRunCount: %d", mUpdateRenderRunCount );
  mUpdateRenderThreadWaitCondition.Notify( lock );
}

void CombinedUpdateRenderController::PauseUpdateRenderThread()
{
  ConditionalWait::ScopedLock lock( mUpdateRenderThreadWaitCondition );
  mUpdateRenderRunCount = 0;
}

void CombinedUpdateRenderController::StopUpdateRenderThread()
{
  ConditionalWait::ScopedLock lock( mUpdateRenderThreadWaitCondition );
  mDestroyUpdateRenderThread = TRUE;
  mUpdateRenderThreadWaitCondition.Notify( lock );
}

bool CombinedUpdateRenderController::IsUpdateRenderThreadPaused()
{
  ConditionalWait::ScopedLock lock( mUpdateRenderThreadWaitCondition );
  return mUpdateRenderRunCount != CONTINUOUS; // Report paused if NOT continuously running
}

void CombinedUpdateRenderController::ProcessSleepRequest()
{
  LOG_EVENT_TRACE;

  if( ++mTryToSleepCount >= MINIMUM_SLEEP_REQUESTS )
  {
    LOG_EVENT( "Going to sleep" );

    PauseUpdateRenderThread();
  }
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

  // Update time
  TimeService::GetNanoseconds( mLastFrameTime );

  LOG_UPDATE_RENDER( "THREAD INITIALISED" );

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
    // CALCULATE FRAME TIMES
    //////////////////////////////

    uint64_t nanoseconds = 0;
    TimeService::GetNanoseconds( nanoseconds );

    unsigned int currentTime = nanoseconds / NANOSECONDS_PER_MILLISECOND;
    float frameDelta = mNumberOfFramesPerRender * FRAME_DELTA;
    unsigned int nextFrameTime = currentTime + mNumberOfFramesPerRender * MILLISECONDS_PER_FRAME;

    //////////////////////////////
    // UPDATE
    //////////////////////////////

    Integration::UpdateStatus updateStatus;

    AddPerformanceMarker( PerformanceInterface::UPDATE_START );
    mCore.Update( frameDelta, currentTime, nextFrameTime, updateStatus );
    AddPerformanceMarker( PerformanceInterface::UPDATE_END );

    unsigned int keepUpdatingStatus = updateStatus.KeepUpdating();

    // Optional logging of update/render status
    mUpdateStatusLogger.Log( keepUpdatingStatus );

    // Tell the event-thread to wake up (if asleep) and send a notification event to Core if required
    if( updateStatus.NeedsNotification() )
    {
      mNotificationTrigger.Trigger();
    }

    //////////////////////////////
    // FPS TRACKING
    //////////////////////////////

    if( mFpsTracker.Enabled() )
    {
      uint64_t timeSinceLastFrame = nanoseconds - mLastFrameTime;
      float absoluteTimeSinceLastRender = timeSinceLastFrame * NANOSECONDS_TO_SECOND;
      mFpsTracker.Track( absoluteTimeSinceLastRender );
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

    // Trigger event thread to sleep if update not required
    if( ( Integration::KeepUpdating::NOT_REQUESTED == keepUpdatingStatus ) &&
        ! renderStatus.NeedsUpdate() )
    {
      mSleepTrigger->Trigger();
    }

    //////////////////////////////
    // FRAME TIME
    //////////////////////////////

    mLastFrameTime = nanoseconds;
    TimeService::GetNanoseconds( nanoseconds );
    uint64_t frameTime = nanoseconds - mLastFrameTime;

    uint64_t defaultFrameTime = mNumberOfFramesPerRender * TIME_PER_FRAME_IN_NANOSECONDS;
    if( frameTime < defaultFrameTime )
    {
      timespec sleepTime;
      sleepTime.tv_sec = 0;
      sleepTime.tv_nsec = defaultFrameTime - frameTime;
      nanosleep( &sleepTime, NULL );
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
  ConditionalWait::ScopedLock updateLock( mUpdateRenderThreadWaitCondition );
  while( ! mUpdateRenderRunCount && // Should try to wait if event-thread has paused the Update/Render thread
         ! mDestroyUpdateRenderThread && // Ensure we don't wait if the update-render-thread is supposed to be destroyed
         ! mNewSurface ) // Ensure we don't wait if we need to replace the surface
  {
    mUpdateRenderThreadWaitCondition.Wait( updateLock );
  }

  LOG_COUNTER_UPDATE_RENDER( "mUpdateRenderRunCount: %d", mUpdateRenderRunCount );

  // If we've been asked to run Update/Render cycles a finite number of times then decrement so we wait after the
  // requested number of cycles
  if( mUpdateRenderRunCount > 0 )
  {
    --mUpdateRenderRunCount;
  }

  // Keep the update-render thread alive if this thread is NOT to be destroyed
  return ! mDestroyUpdateRenderThread;
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
  // Just increment the semaphore
  sem_post( &mEventThreadSemaphore );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// ALL THREADS
///////////////////////////////////////////////////////////////////////////////////////////////////

void CombinedUpdateRenderController::NotifyThreadInitialised()
{
  // Just increment the semaphore
  sem_post( &mEventThreadSemaphore );
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
  ConditionalWait::ScopedLock lock( mUpdateRenderThreadWaitCondition );
  mPostRendering = FALSE;
  mUpdateRenderThreadWaitCondition.Notify( lock );
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
