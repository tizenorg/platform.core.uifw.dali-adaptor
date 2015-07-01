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

// EXTERNAL INCLUDES
#include <cstdio>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstring>
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
const int CREATED_THREAD_COUNT = 3;

///////////////////////////////////////////////////////////////////////////////////////////////////

#define ENABLE_LOG_IN_COLOR 1
#define ENABLE_VSYNC_COUNTER_LOGGING 1
#define ENABLE_UPDATE_COUNTER_LOGGING 1
#define ENABLE_VSYNC_THREAD_LOGGING 1
#define ENABLE_UPDATE_THREAD_LOGGING 1
#define ENABLE_RENDER_THREAD_LOGGING 1
#define ENABLE_EVENT_LOGGING

#ifdef ENABLE_LOG_IN_COLOR
#define COLOR_RED            "\033[31m"
#define COLOR_YELLOW         "\033[33m"
#define COLOR_BLUE           "\033[34m"
#define COLOR_LIGHT_RED      "\033[91m"
#define COLOR_LIGHT_YELLOW   "\033[93m"
#define COLOR_LIGHT_BLUE     "\033[94m"
#define COLOR_WHITE          "\033[97m"
#define COLOR_CLEAR          "\033[0m"
#else
#define COLOR_RED
#define COLOR_YELLOW
#define COLOR_BLUE
#define COLOR_LIGHT_RED
#define COLOR_LIGHT_YELLOW
#define COLOR_LIGHT_BLUE
#define COLOR_WHITE
#define COLOR_CLEAR
#endif

#define LOG_THREAD_SYNC(color,...)      std::cout << color; printf(__VA_ARGS__); std::cout << COLOR_CLEAR << std::endl;

#ifdef ENABLE_VSYNC_COUNTER_LOGGING
#define LOG_VSYNC_COUNTER_VSYNC(...)    LOG_THREAD_SYNC(COLOR_LIGHT_RED,__VA_ARGS__)
#define LOG_VSYNC_COUNTER_UPDATE(...)   LOG_THREAD_SYNC(COLOR_LIGHT_YELLOW,__VA_ARGS__)
#else
#define LOG_VSYNC_COUNTER_VSYNC(...)
#define LOG_VSYNC_COUNTER_UPDATE(...)
#endif

#ifdef ENABLE_UPDATE_COUNTER_LOGGING
#define LOG_UPDATE_COUNTER_UPDATE(...)  LOG_THREAD_SYNC(COLOR_YELLOW,__VA_ARGS__)
#define LOG_UPDATE_COUNTER_RENDER(...)  LOG_THREAD_SYNC(COLOR_LIGHT_BLUE,__VA_ARGS__)
#else
#define LOG_UPDATE_COUNTER_UPDATE(...)
#define LOG_UPDATE_COUNTER_RENDER(...)
#endif

#ifdef ENABLE_VSYNC_THREAD_LOGGING
#define LOG_VSYNC(...)             LOG_THREAD_SYNC(COLOR_RED,__VA_ARGS__)
#else
#define LOG_VSYNC(...)
#endif

#ifdef ENABLE_UPDATE_THREAD_LOGGING
#define LOG_UPDATE(...)            LOG_THREAD_SYNC(COLOR_YELLOW,__VA_ARGS__)
#else
#define LOG_UPDATE(...)
#endif

#ifdef ENABLE_RENDER_THREAD_LOGGING
#define LOG_RENDER(...)            LOG_THREAD_SYNC(COLOR_BLUE,__VA_ARGS__)
#else
#define LOG_RENDER(...)
#endif

#ifdef ENABLE_EVENT_LOGGING
#define LOG_EVENT(...)             LOG_THREAD_SYNC(COLOR_WHITE,__VA_ARGS__)
#else
#define LOG_EVENT(...)
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
} // unnamed namespace

ThreadSynchronization::ThreadSynchronization( AdaptorInternalServices& adaptorInterfaces, unsigned int numberOfVSyncsPerRender)
: mStateMachine(),
  mMutex(),
  mFrameTime( adaptorInterfaces.GetPlatformAbstractionInterface() ),
  mNotificationTrigger( adaptorInterfaces.GetProcessCoreEventsTrigger() ),
  mPerformanceInterface( adaptorInterfaces.GetPerformanceInterface() ),
  mReplaceSurfaceRequest(),
  mUpdateThreadWaitCondition(),
  mRenderThreadWaitCondition(),
  mVSyncThreadWaitCondition(),
  mEventThreadWaitCondition(),
  mEventData(),
  mUpdateData(),
  mRenderData(),
  mVSyncData(),
  mMaximumUpdateCount( adaptorInterfaces.GetCore().GetMaximumUpdateCount()),
  mNumberOfVSyncsPerRender( numberOfVSyncsPerRender ),
  mTryToSleepCount( 0u ),
  mState( State::STOPPED ),
  mVSyncAheadOfUpdate( 0u ),
  mUpdateAheadOfRender( 0u ),
  mNumberOfThreadsStarted( 0u ),
  mSyncFrameNumber( 0u ),
  mSyncSeconds( 0u ),
  mSyncMicroseconds( 0u )
{
  // Ensure no processing is done for all states if not defined below
  memset( mStateMachine, 0, sizeof( mStateMachine[ 0 ][ 0 ] ) * State::TOTAL_STATES * Event::TOTAL_EVENTS );

  // State changes occur mainly in the event thread apart from:
  //   SLEEP (in the update thread) and SURFACE_REPLACED (in the render thread).

  mStateMachine[ State::STOPPED ][ Event::INITIALISE ] = &ThreadSynchronization::Initialise;

  mStateMachine[ State::INITIALISING ][ Event::START        ] = &ThreadSynchronization::Start;
  mStateMachine[ State::INITIALISING ][ Event::UPDATE_READY ] = &ThreadSynchronization::UpdateReadyForFirstFrame;
  mStateMachine[ State::INITIALISING ][ Event::RENDER_READY ] = &ThreadSynchronization::RenderReadyForFirstFrame;
  mStateMachine[ State::INITIALISING ][ Event::VSYNC_READY  ] = &ThreadSynchronization::VSyncReadyForFirstFrame;
  mStateMachine[ State::INITIALISING ][ Event::STOP         ] = &ThreadSynchronization::Stop;

  mStateMachine[ State::RUNNING ][ Event::PAUSE           ] = &ThreadSynchronization::Pause;
  mStateMachine[ State::RUNNING ][ Event::UPDATE_READY    ] = &ThreadSynchronization::UpdateReady;
  mStateMachine[ State::RUNNING ][ Event::RENDER_READY    ] = &ThreadSynchronization::RenderReady;
  mStateMachine[ State::RUNNING ][ Event::VSYNC_READY     ] = &ThreadSynchronization::VSyncReady;
  mStateMachine[ State::RUNNING ][ Event::STOP            ] = &ThreadSynchronization::Stop;
  mStateMachine[ State::RUNNING ][ Event::REPLACE_SURFACE ] = &ThreadSynchronization::ReplaceSurface;

  mStateMachine[ State::SLEEPING ][ Event::PAUSE           ] = &ThreadSynchronization::Pause;
  mStateMachine[ State::SLEEPING ][ Event::UPDATE_READY    ] = &ThreadSynchronization::UpdateReadyWhileSleeping;
  mStateMachine[ State::SLEEPING ][ Event::RENDER_READY    ] = &ThreadSynchronization::RenderReady;
  mStateMachine[ State::SLEEPING ][ Event::VSYNC_READY     ] = &ThreadSynchronization::VSyncReady;
  mStateMachine[ State::SLEEPING ][ Event::UPDATE_REQUEST  ] = &ThreadSynchronization::UpdateRequest;
  mStateMachine[ State::SLEEPING ][ Event::UPDATE_ONCE     ] = &ThreadSynchronization::UpdateRequest;
  mStateMachine[ State::SLEEPING ][ Event::STOP            ] = &ThreadSynchronization::Stop;
  mStateMachine[ State::SLEEPING ][ Event::REPLACE_SURFACE ] = &ThreadSynchronization::ReplaceSurface;

  mStateMachine[ State::PAUSED ][ Event::RESUME          ] = &ThreadSynchronization::Resume;
  mStateMachine[ State::PAUSED ][ Event::UPDATE_READY    ] = &ThreadSynchronization::UpdateReady;
  mStateMachine[ State::PAUSED ][ Event::RENDER_READY    ] = &ThreadSynchronization::RenderReady;
  mStateMachine[ State::PAUSED ][ Event::VSYNC_READY     ] = &ThreadSynchronization::VSyncReady;
  mStateMachine[ State::PAUSED ][ Event::UPDATE_ONCE     ] = &ThreadSynchronization::UpdateOnce;
  mStateMachine[ State::PAUSED ][ Event::STOP            ] = &ThreadSynchronization::Stop;
  mStateMachine[ State::PAUSED ][ Event::REPLACE_SURFACE ] = &ThreadSynchronization::ReplaceSurface;

  mStateMachine[ State::REPLACING_SURFACE ][ Event::PAUSE            ] = &ThreadSynchronization::Pause;
  mStateMachine[ State::REPLACING_SURFACE ][ Event::UPDATE_READY     ] = &ThreadSynchronization::UpdateReady;
  mStateMachine[ State::REPLACING_SURFACE ][ Event::RENDER_READY     ] = &ThreadSynchronization::RenderReadyDuringReplaceSurface;
  mStateMachine[ State::REPLACING_SURFACE ][ Event::VSYNC_READY      ] = &ThreadSynchronization::VSyncReady;
  mStateMachine[ State::REPLACING_SURFACE ][ Event::STOP             ] = &ThreadSynchronization::Stop;
  mStateMachine[ State::REPLACING_SURFACE ][ Event::SURFACE_REPLACED ] = &ThreadSynchronization::RenderInformSurfaceReplaced;

  mStateMachine[ State::SURFACE_REPLACED ][ Event::RENDER_READY ] = &ThreadSynchronization::RenderReadyAfterSurfaceReplaced;
}

ThreadSynchronization::~ThreadSynchronization()
{
}

void ThreadSynchronization::Initialise()
{
  ProcessEvent( *this, Event::INITIALISE );
}

void ThreadSynchronization::Start()
{
  ProcessEvent( *this, Event::START );
}

void ThreadSynchronization::Stop()
{
  ProcessEvent( *this, Event::STOP );
}

void ThreadSynchronization::Pause()
{
  ProcessEvent( *this, Event::PAUSE );
}

void ThreadSynchronization::Resume()
{
  ProcessEvent( *this, Event::RESUME );
}

void ThreadSynchronization::UpdateRequest()
{
  ProcessEvent( *this, Event::UPDATE_REQUEST );
}

void ThreadSynchronization::UpdateOnce()
{
  ProcessEvent( *this, Event::UPDATE_ONCE );
}

void ThreadSynchronization::ResumeFrameTime()
{
  mFrameTime.Resume();
}

void ThreadSynchronization::ReplaceSurface( RenderSurface* newSurface )
{
  mEventData.newSurface = newSurface;

  ProcessEvent( *this, Event::REPLACE_SURFACE );
}

void ThreadSynchronization::SetRenderRefreshRate( unsigned int numberOfVSyncsPerRender )
{
  mNumberOfVSyncsPerRender = numberOfVSyncsPerRender;
}

bool ThreadSynchronization::UpdateReady( bool notifyEvent, bool runUpdate, float& lastFrameDeltaSeconds, unsigned int& lastSyncTimeMilliseconds, unsigned int& nextSyncTimeMilliseconds )
{
  mUpdateData.notifyEvent = notifyEvent;
  mUpdateData.runUpdate = runUpdate;
  mUpdateData.lastFrameDeltaSeconds = &lastFrameDeltaSeconds;
  mUpdateData.lastSyncTimeMilliseconds = &lastSyncTimeMilliseconds;
  mUpdateData.nextSyncTimeMilliseconds = &nextSyncTimeMilliseconds;

  return ProcessEvent( *this, Event::UPDATE_READY );
}

bool ThreadSynchronization::RenderReady( RenderRequest*& requestPtr )
{
  mRenderData.requestPtr = &requestPtr;

  return ProcessEvent( *this, Event::RENDER_READY );
}

void ThreadSynchronization::RenderInformSurfaceReplaced()
{
  ProcessEvent( *this, Event::SURFACE_REPLACED );
}

bool ThreadSynchronization::VSyncReady( bool validSync, unsigned int frameNumber, unsigned int seconds, unsigned int microseconds, unsigned int& numberOfVSyncsPerRender )
{
  mVSyncData.validSync = validSync;
  mVSyncData.frameNumber = frameNumber;
  mVSyncData.seconds = seconds;
  mVSyncData.microseconds = microseconds;
  mVSyncData.numberOfVSyncsPerRender = &numberOfVSyncsPerRender;

  return ProcessEvent( *this, Event::VSYNC_READY );
}

inline void ThreadSynchronization::AddPerformanceMarker( PerformanceInterface::MarkerType type )
{
  if( mPerformanceInterface )
  {
    mPerformanceInterface->AddMarker( type );
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
///////////////////////////////////////////////////////////////////////////////////////////////////

bool ThreadSynchronization::ProcessEvent( ThreadSynchronization& sync, int event )
{
  int currentState = State::STOPPED;
  __atomic_load( &sync.mState, &currentState, __ATOMIC_SEQ_CST );

  ProcessEventFunction func = sync.mStateMachine[ currentState ][ event ];
  if( func )
  {
    func( sync );
  }

  // This ensures an atomic comparison, if the state was stopped, we set it to stopped otherwise it stays as it was
  int stopState = State::STOPPED;
  return ! __atomic_compare_exchange_n( &sync.mState, &stopState, stopState, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EVENT THREAD
///////////////////////////////////////////////////////////////////////////////////////////////////

void ThreadSynchronization::Initialise( ThreadSynchronization& sync )
{
  LOG_EVENT( "%s", __FUNCTION__ );

  // Just set the state, we have not started any threads at this point
  sync.mState = State::INITIALISING;
}

void ThreadSynchronization::Start( ThreadSynchronization& sync )
{
  LOG_EVENT( "%s", __FUNCTION__ );

  sync.mFrameTime.SetMinimumFrameTimeInterval( sync.mNumberOfVSyncsPerRender * TIME_PER_FRAME_IN_MICROSECONDS );

  while( __atomic_load_n( &sync.mNumberOfThreadsStarted, __ATOMIC_SEQ_CST ) < CREATED_THREAD_COUNT )
  {
    sync.mEventThreadWaitCondition.Wait();
  }

  __atomic_store_n( &sync.mState, State::RUNNING, __ATOMIC_SEQ_CST );
  sync.mVSyncThreadWaitCondition.Notify();
  sync.mUpdateThreadWaitCondition.Notify();
}

void ThreadSynchronization::Pause( ThreadSynchronization& sync )
{
  LOG_EVENT( "%s", __FUNCTION__ );

  __atomic_store_n( &sync.mState, State::PAUSED, __ATOMIC_SEQ_CST );

  // TODO AddPerformanceMarker( PerformanceInterface::PAUSED );
  sync.mFrameTime.Suspend();
}

void ThreadSynchronization::Resume( ThreadSynchronization& sync )
{
  LOG_EVENT( "%s", __FUNCTION__ );

  __atomic_store_n( &sync.mState, State::RUNNING, __ATOMIC_SEQ_CST );
  sync.mVSyncThreadWaitCondition.Notify();

  // TODO AddPerformanceMarker( PerformanceInterface::RESUME);
}

void ThreadSynchronization::UpdateRequest( ThreadSynchronization& sync )
{
  LOG_EVENT( "%s", __FUNCTION__ );

  __atomic_store_n( &sync.mState, State::RUNNING, __ATOMIC_SEQ_CST );
  sync.mVSyncThreadWaitCondition.Notify();
}

void ThreadSynchronization::UpdateOnce( ThreadSynchronization& sync )
{
  LOG_EVENT( "%s", __FUNCTION__ );

  sync.mUpdateThreadWaitCondition.Notify();
}

void ThreadSynchronization::Stop( ThreadSynchronization& sync )
{
  LOG_EVENT( "%s", __FUNCTION__ );

  __atomic_store_n( &sync.mState, State::STOPPED, __ATOMIC_SEQ_CST );

  // Notify all other threads so they continue
  sync.mVSyncThreadWaitCondition.Notify();
  sync.mUpdateThreadWaitCondition.Notify();
  sync.mRenderThreadWaitCondition.Notify();

  sync.mFrameTime.Suspend();
}

void ThreadSynchronization::ReplaceSurface( ThreadSynchronization& sync )
{
  LOG_EVENT( "%s: START", __FUNCTION__ );

  {
    Mutex::ScopedLock lock( sync.mMutex );
    sync.mReplaceSurfaceRequest.SetSurface( sync.mEventData.newSurface );
  }

  int previousState = __atomic_exchange_n( &sync.mState, State::REPLACING_SURFACE, __ATOMIC_SEQ_CST );

  // Run the RenderThread in case it's waiting
  sync.mRenderThreadWaitCondition.Notify();

  // Wait for RenderThread to replace the surface if we are still replacing the surface
  // This ensures an atomic comparison, if the state was REPLACING_SURFACE, we set it to REPLACING_SURFACE otherwise it stays as it was
  int replacingSurface = State::REPLACING_SURFACE;
  if( __atomic_compare_exchange_n( &sync.mState, &replacingSurface, replacingSurface, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST ) )
  {
    sync.mEventThreadWaitCondition.Wait();
  }

  __atomic_store_n( &sync.mState, previousState, __ATOMIC_SEQ_CST );
  sync.mVSyncThreadWaitCondition.Notify();

  LOG_EVENT( "%s: END", __FUNCTION__ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// UPDATE THREAD
///////////////////////////////////////////////////////////////////////////////////////////////////

void ThreadSynchronization::UpdateReadyForFirstFrame( ThreadSynchronization& sync )
{
  LOG_UPDATE( "%s", __FUNCTION__ );

  // Notify event thread that this thread is running
  __atomic_add_fetch( &sync.mNumberOfThreadsStarted, 1, __ATOMIC_SEQ_CST );
  sync.mEventThreadWaitCondition.Notify();

  // Wait for first thread-sync point
  // This ensures an atomic comparison, if the state was INITIALISING, we set it to INITIALISING otherwise it stays as it was
  int initialising = State::INITIALISING;
  if( __atomic_compare_exchange_n( &sync.mState, &initialising, initialising, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST ) )
  {
    sync.mUpdateThreadWaitCondition.Wait();
  }

  sync.mFrameTime.PredictNextSyncTime( *sync.mUpdateData.lastFrameDeltaSeconds, *sync.mUpdateData.lastSyncTimeMilliseconds, *sync.mUpdateData.nextSyncTimeMilliseconds );

  // TODO AddPerformanceMarker( PerformanceInterface::UPDATE_START );
}

void ThreadSynchronization::UpdateReady( ThreadSynchronization& sync )
{
  LOG_UPDATE( "%s", __FUNCTION__ );

  // TODO AddPerformanceMarker( PerformanceInterface::UPDATE_END );

  if( sync.mUpdateData.notifyEvent )
  {
    // Do the notifications first so the event thread can start processing them
    // Tell the event-thread to wake up (if asleep) and send a notification event to Core
    sync.mNotificationTrigger.Trigger();
  }

  // Inform render thread
  int updateAheadOfRender = __atomic_add_fetch( &sync.mUpdateAheadOfRender, 1, __ATOMIC_SEQ_CST );
  LOG_UPDATE_COUNTER_UPDATE( "%s: updateAheadOfRender(%d)", __FUNCTION__, updateAheadOfRender );
  sync.mRenderThreadWaitCondition.Notify();

  if( ! sync.mUpdateData.runUpdate )
  {
    LOG_UPDATE( "TryToSleep" );

    if( ++sync.mTryToSleepCount >= 3 )
    {
      LOG_UPDATE( "Going to sleep" );
      __atomic_store_n( &sync.mState, State::SLEEPING, __ATOMIC_SEQ_CST );
    }
  }
  else
  {
    sync.mTryToSleepCount = 0;
  }

  if( updateAheadOfRender >= sync.mMaximumUpdateCount )
  {
    while ( __atomic_load_n( &sync.mUpdateAheadOfRender, __ATOMIC_SEQ_CST ) >= sync.mMaximumUpdateCount )
    {
      sync.mRenderThreadWaitCondition.Notify(); // Notify the render thread in case it was waiting

      // Ensure we haven't JUST stopped before we wait
      // This ensures an atomic comparison, if the state was STOPPED, we set it to STOPPED otherwise it stays as it was
      int stopping = State::STOPPED;
      if( ! __atomic_compare_exchange_n( &sync.mState, &stopping, stopping, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST ) )
      {
        sync.mUpdateThreadWaitCondition.Wait();
      }
    }
  }

  // Wait for VSync
  int vSyncAheadOfUpdate = 0;
  if( ( vSyncAheadOfUpdate = __atomic_fetch_and( &sync.mVSyncAheadOfUpdate, 0, __ATOMIC_SEQ_CST ) ) == 0 )
  {
    LOG_VSYNC_COUNTER_UPDATE( "%s:  vSyncAheadOfUpdate(%d) WAIT", __FUNCTION__, vSyncAheadOfUpdate );

    // Ensure we haven't JUST stopped before we wait
    // This ensures an atomic comparison, if the state was STOPPED, we set it to STOPPED otherwise it stays as it was
    int stopping = State::STOPPED;
    if( ! __atomic_compare_exchange_n( &sync.mState, &stopping, stopping, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST ) )
    {
      sync.mUpdateThreadWaitCondition.Wait();
      __atomic_store_n( &sync.mVSyncAheadOfUpdate, 0, __ATOMIC_SEQ_CST ); // Reset to 0
    }
  }
  else
  {
    LOG_VSYNC_COUNTER_UPDATE( "%s:  vSyncAheadOfUpdate(%d)", __FUNCTION__, vSyncAheadOfUpdate );
  }

  sync.mFrameTime.PredictNextSyncTime( *sync.mUpdateData.lastFrameDeltaSeconds, *sync.mUpdateData.lastSyncTimeMilliseconds, *sync.mUpdateData.nextSyncTimeMilliseconds );

  // TODO AddPerformanceMarker( PerformanceInterface::UPDATE_START );
}

void ThreadSynchronization::UpdateReadyWhileSleeping( ThreadSynchronization& sync )
{
  LOG_UPDATE( "%s: START", __FUNCTION__ );

  // Inform frame time
  sync.mFrameTime.Sleep();

  UpdateReady( sync );

  //
  // Woken up
  //

  // Inform frame timer
  sync.mFrameTime.WakeUp();

  LOG_UPDATE( "%s: END", __FUNCTION__ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// RENDER THREAD
///////////////////////////////////////////////////////////////////////////////////////////////////

void ThreadSynchronization::RenderReadyForFirstFrame( ThreadSynchronization& sync )
{
  LOG_RENDER( "%s", __FUNCTION__ );

  // Notify event thread that this thread is running
  __atomic_add_fetch( &sync.mNumberOfThreadsStarted, 1, __ATOMIC_SEQ_CST );
  sync.mEventThreadWaitCondition.Notify();

  // Wait for first update to complete
  sync.mRenderThreadWaitCondition.Wait();

  // TODO AddPerformanceMarker( PerformanceInterface::RENDER_START );
}

void ThreadSynchronization::RenderReady( ThreadSynchronization& sync )
{
  LOG_RENDER( "%s", __FUNCTION__ );

  // TODO AddPerformanceMarker( PerformanceInterface::RENDER_END );

  int updateAheadOfRender = 0;
  if( ( updateAheadOfRender = __atomic_sub_fetch( &sync.mUpdateAheadOfRender, 1, __ATOMIC_SEQ_CST ) ) == 0 )
  {
    LOG_UPDATE_COUNTER_RENDER( "%s: updateAheadOfRender(%d) WAIT", __FUNCTION__, updateAheadOfRender );
    sync.mRenderThreadWaitCondition.Wait();
  }
  else
  {
    LOG_UPDATE_COUNTER_RENDER( "%s: updateAheadOfRender(%d)", __FUNCTION__, updateAheadOfRender );
  }

  // This ensures an atomic comparison, if the state was REPLACING_SURFACE, we set it to REPLACING_SURFACE otherwise it stays as it was
  int replaceSurface = State::REPLACING_SURFACE;
  if( __atomic_compare_exchange_n( &sync.mState, &replaceSurface, replaceSurface, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST ) )
  {
    Mutex::ScopedLock lock( sync.mMutex );
    *( sync.mRenderData.requestPtr ) = &sync.mReplaceSurfaceRequest;
  }

  // TODO AddPerformanceMarker( PerformanceInterface::RENDER_START );
}

void ThreadSynchronization::RenderReadyDuringReplaceSurface( ThreadSynchronization& sync )
{
  LOG_RENDER( "%s", __FUNCTION__ );

  // TODO AddPerformanceMarker( PerformanceInterface::RENDER_END );

  // We've just done a render so reduce update ahead of render count
  __atomic_sub_fetch( &sync.mUpdateAheadOfRender, 1, __ATOMIC_SEQ_CST );

  // We're about to replace the surface so set replacing surface to true and return
  {
    Mutex::ScopedLock lock( sync.mMutex );
    *( sync.mRenderData.requestPtr ) = &sync.mReplaceSurfaceRequest;
  }
}

void ThreadSynchronization::RenderReadyAfterSurfaceReplaced( ThreadSynchronization& sync )
{
  LOG_RENDER( "%s: START", __FUNCTION__ );

  // Notify EventThread that the surface has been replaced
  sync.mEventThreadWaitCondition.Notify();

  // Wait for next update before continuing
  sync.mRenderThreadWaitCondition.Wait();

  // This ensures an atomic comparison, if the state was REPLACING_SURFACE, we set it to REPLACING_SURFACE otherwise it stays as it was
  int replaceSurface = State::REPLACING_SURFACE;
  if( __atomic_compare_exchange_n( &sync.mState, &replaceSurface, replaceSurface, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST ) )
  {
    Mutex::ScopedLock lock( sync.mMutex );
    *( sync.mRenderData.requestPtr ) = &sync.mReplaceSurfaceRequest;
  }

  LOG_RENDER( "%s: END", __FUNCTION__ );
}

void ThreadSynchronization::RenderInformSurfaceReplaced( ThreadSynchronization& sync )
{
  LOG_EVENT( "%s", __FUNCTION__ );

  __atomic_store_n( &sync.mState, State::SURFACE_REPLACED, __ATOMIC_SEQ_CST );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// V-SYNC THREAD
///////////////////////////////////////////////////////////////////////////////////////////////////

void ThreadSynchronization::VSyncReadyForFirstFrame( ThreadSynchronization& sync )
{
  LOG_VSYNC( "%s", __FUNCTION__ );

  if( *( sync.mVSyncData.numberOfVSyncsPerRender ) != sync.mNumberOfVSyncsPerRender )
  {
    *( sync.mVSyncData.numberOfVSyncsPerRender ) = sync.mNumberOfVSyncsPerRender; // save it back
    sync.mFrameTime.SetMinimumFrameTimeInterval( sync.mNumberOfVSyncsPerRender * TIME_PER_FRAME_IN_MICROSECONDS );
  }

  if( sync.mVSyncData.validSync )
  {
    sync.mFrameTime.SetSyncTime( sync.mVSyncData.frameNumber );
  }

  {
    Mutex::ScopedLock lock( sync.mMutex );

    sync.mSyncFrameNumber = sync.mVSyncData.frameNumber;
    sync.mSyncSeconds = sync.mVSyncData.seconds;
    sync.mSyncMicroseconds = sync.mVSyncData.microseconds;
  }

  // Notify event thread that this thread is running
  __atomic_add_fetch( &sync.mNumberOfThreadsStarted, 1, __ATOMIC_SEQ_CST );
  sync.mEventThreadWaitCondition.Notify();

  // Wait for all threads to be ready
  // This ensures an atomic comparison, if the state was INITIALISING, we set it to INITIALISING otherwise it stays as it was
  int initialising = State::INITIALISING;
  if( __atomic_compare_exchange_n( &sync.mState, &initialising, initialising, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST ) )
  {
    sync.mVSyncThreadWaitCondition.Wait();
  }
}

void ThreadSynchronization::VSyncReady( ThreadSynchronization& sync )
{
  LOG_VSYNC( "%s", __FUNCTION__ );

  if( *( sync.mVSyncData.numberOfVSyncsPerRender ) != sync.mNumberOfVSyncsPerRender )
  {
    *( sync.mVSyncData.numberOfVSyncsPerRender ) = sync.mNumberOfVSyncsPerRender; // save it back
    sync.mFrameTime.SetMinimumFrameTimeInterval( sync.mNumberOfVSyncsPerRender * TIME_PER_FRAME_IN_MICROSECONDS );
  }

  if( sync.mVSyncData.validSync )
  {
    sync.mFrameTime.SetSyncTime( sync.mVSyncData.frameNumber );
  }

  {
    Mutex::ScopedLock lock( sync.mMutex );

    sync.mSyncFrameNumber = sync.mVSyncData.frameNumber;
    sync.mSyncSeconds = sync.mVSyncData.seconds;
    sync.mSyncMicroseconds = sync.mVSyncData.microseconds;
  }

  // TODO AddPerformanceMarker( PerformanceInterface::VSYNC );

  int vSyncAheadOfUpdate = __atomic_add_fetch( &sync.mVSyncAheadOfUpdate, 1, __ATOMIC_SEQ_CST );
  (void)vSyncAheadOfUpdate;
  LOG_VSYNC_COUNTER_VSYNC( "%s: vSyncAheadOfUpdate(%d)\n",  __FUNCTION__, vSyncAheadOfUpdate );

  sync.mUpdateThreadWaitCondition.Notify();

  int currentState = 0;
  __atomic_load( &sync.mState, &currentState, __ATOMIC_SEQ_CST );
  while( ( currentState == State::PAUSED )   ||
         ( currentState == State::SLEEPING ) ||
         ( currentState == State::REPLACING_SURFACE ) )
  {
    LOG_VSYNC( "%s: WAIT", __FUNCTION__ );
    sync.mVSyncThreadWaitCondition.Wait();
    __atomic_load( &sync.mState, &currentState, __ATOMIC_SEQ_CST );
  }
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
