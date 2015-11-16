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
#include <errno.h>
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
const float        NANOSECONDS_TO_SECOND( 1e-9f );
const unsigned int NANOSECONDS_PER_SECOND( 1e+9 );
const unsigned int NANOSECONDS_PER_MILLISECOND( 1e+6 );

// The following values will get calculated at compile time
const float        DEFAULT_FRAME_DURATION_IN_SECONDS( 1.0f / 60.0f );
const unsigned int DEFAULT_FRAME_DURATION_IN_MILLISECONDS( DEFAULT_FRAME_DURATION_IN_SECONDS * MILLISECONDS_PER_SECOND );
const unsigned int DEFAULT_FRAME_DURATION_IN_NANOSECONDS( DEFAULT_FRAME_DURATION_IN_SECONDS * NANOSECONDS_PER_SECOND );

/**
 * Handles the use case when an update-request is received JUST before we process a sleep-request. If we did not have an update-request count then
 * there is a danger that, on the event-thread we could have:
 *  1) An update-request where we do nothing as Update/Render thread still running.
 *  2) Quickly followed by a sleep-request being handled where we pause the Update/Render Thread (even though we have an update to process).
 *
 * Using a counter means we increment the counter on an update-request, and decrement it on a sleep-request. This handles the above scenario because:
 *  1) MAIN THREAD:           Update Request: COUNTER = 1
 *  2) UPDATE/RENDER THREAD:  Do Update/Render, then no Updates required -> Sleep Trigger
 *  3) MAIN THREAD:           Update Request: COUNTER = 2
 *  4) MAIN THREAD:           Sleep Request:  COUNTER = 1 -> We do not sleep just yet
 *
 * Also ensures we preserve battery life by only doing ONE update when the above use case is not triggered.
 *  1) MAIN THREAD:           Update Request: COUNTER = 1
 *  2) UPDATE/RENDER THREAD:  Do Update/Render, then no Updates required -> Sleep Trigger
 *  3) MAIN THREAD:           Sleep Request:  COUNTER = 0 -> Go to sleep
 */
const unsigned int MAXIMUM_UPDATE_REQUESTS = 2;
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
  mDefaultFrameDelta( 0.0f ),
  mDefaultFrameDurationMilliseconds( 0u ),
  mDefaultFrameDurationNanoseconds( 0u ),
  mUpdateRequestCount( 0u ),
  mRunning( FALSE ),
  mUpdateRenderRunCount( 0 ),
  mDestroyUpdateRenderThread( FALSE ),
  mNewSurface( NULL ),
  mPostRendering( FALSE )
{
  LOG_EVENT_TRACE;

  // Initialise frame delta/duration variables first
  SetRenderRefreshRate( environmentOptions.GetRenderRefreshRate() );

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

  // Wait till all created threads have initialised
  for( unsigned int i = 0; i < CREATED_THREAD_COUNT; ++i )
  {
    sem_wait( &mEventThreadSemaphore );
  }

  mRenderHelper.Start();

  mRunning = TRUE;

  LOG_EVENT( "Startup Complete, starting Update/Render Thread" );

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

  if( !mRunning && IsUpdateRenderThreadPaused() )
  {
    LOG_EVENT( "Resuming" );

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

  // Increment the update-request count to the maximum
  if( mUpdateRequestCount < MAXIMUM_UPDATE_REQUESTS )
  {
    ++mUpdateRequestCount;
  }

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

  LOG_EVENT( "Starting to replace the surface, event-thread blocked" );

  // Start replacing the surface.
  {
    ConditionalWait::ScopedLock lock( mUpdateRenderThreadWaitCondition );
    mPostRendering = FALSE; // Clear the post-rendering flag as Update/Render thread will replace the surface now
    mNewSurface = newSurface;
    mUpdateRenderThreadWaitCondition.Notify( lock );
  }

  // Wait until the surface has been replaced
  sem_wait( &mEventThreadSemaphore );

  LOG_EVENT( "Surface replaced, event-thread continuing" );
}

void CombinedUpdateRenderController::SetRenderRefreshRate( unsigned int numberOfFramesPerRender )
{
  // Not protected by lock, but written to rarely so not worth adding a lock when reading
  mDefaultFrameDelta                  = numberOfFramesPerRender * DEFAULT_FRAME_DURATION_IN_SECONDS;
  mDefaultFrameDurationMilliseconds   = numberOfFramesPerRender * DEFAULT_FRAME_DURATION_IN_MILLISECONDS;
  mDefaultFrameDurationNanoseconds    = numberOfFramesPerRender * DEFAULT_FRAME_DURATION_IN_NANOSECONDS;

  LOG_EVENT( "mDefaultFrameDelta(%.6f), mDefaultFrameDurationMilliseconds(%lld), mDefaultFrameDurationNanoseconds(%lld)", mDefaultFrameDelta, mDefaultFrameDurationMilliseconds, mDefaultFrameDurationNanoseconds );
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

  // Decrement Update request count
  if( mUpdateRequestCount > 0 )
  {
    --mUpdateRequestCount;
  }

  // Only sleep if our update-request count is 0
  if( mUpdateRequestCount == 0 )
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
  uint64_t lastFrameTime;
  TimeService::GetNanoseconds( lastFrameTime );

  LOG_UPDATE_RENDER( "THREAD INITIALISED" );

  while( UpdateRenderReady() )
  {
    LOG_UPDATE_RENDER_TRACE;

    uint64_t currentFrameStartTime = 0;
    TimeService::GetNanoseconds( currentFrameStartTime );

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
    // UPDATE
    //////////////////////////////

    unsigned int currentTime = currentFrameStartTime / NANOSECONDS_PER_MILLISECOND;
    unsigned int nextFrameTime = currentTime + mDefaultFrameDurationMilliseconds;

    Integration::UpdateStatus updateStatus;

    AddPerformanceMarker( PerformanceInterface::UPDATE_START );
    mCore.Update( mDefaultFrameDelta, currentTime, nextFrameTime, updateStatus );
    AddPerformanceMarker( PerformanceInterface::UPDATE_END );

    unsigned int keepUpdatingStatus = updateStatus.KeepUpdating();

    // Tell the event-thread to wake up (if asleep) and send a notification event to Core if required
    if( updateStatus.NeedsNotification() )
    {
      mNotificationTrigger.Trigger();
    }

    // Optional logging of update/render status
    mUpdateStatusLogger.Log( keepUpdatingStatus );

    // Optional FPS Tracking
    if( mFpsTracker.Enabled() )
    {
      uint64_t timeSinceLastFrame = currentFrameStartTime - lastFrameTime;
      float absoluteTimeSinceLastRender = timeSinceLastFrame * NANOSECONDS_TO_SECOND;
      mFpsTracker.Track( absoluteTimeSinceLastRender );
      lastFrameTime = currentFrameStartTime; // Store frame start time
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

    // Trigger event thread to request Update/Render thread to sleep if update not required
    if( ( Integration::KeepUpdating::NOT_REQUESTED == keepUpdatingStatus ) &&
        ! renderStatus.NeedsUpdate() )
    {
      mSleepTrigger->Trigger();
    }

    //////////////////////////////
    // FRAME TIME
    //////////////////////////////

    // Sleep until at least the the default frame duration has elapsed. This will return immediately if the specified end-time has already passed.
    TimeService::SleepUntil( currentFrameStartTime + mDefaultFrameDurationNanoseconds );
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
