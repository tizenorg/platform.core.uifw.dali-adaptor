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

//#define ENABLE_LOG_IN_COLOR 1
//#define ENABLE_VSYNC_COUNTER_LOGGING 1
//#define ENABLE_UPDATE_COUNTER_LOGGING 1
//#define ENABLE_VSYNC_THREAD_LOGGING 1
//#define ENABLE_UPDATE_THREAD_LOGGING 1
//#define ENABLE_RENDER_THREAD_LOGGING 1
//#define ENABLE_EVENT_LOGGING

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
  mMaximumUpdateCount( adaptorInterfaces.GetCore().GetMaximumUpdateCount()),
  mNumberOfVSyncsPerRender( numberOfVSyncsPerRender ),
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
  mStateMachine[ State::INITIALISING ][ Event::UPDATE_READY ] = &ThreadSynchronization::FirstUpdateReady;
  mStateMachine[ State::INITIALISING ][ Event::RENDER_READY ] = &ThreadSynchronization::FirstRenderReady;
  mStateMachine[ State::INITIALISING ][ Event::VSYNC_READY  ] = &ThreadSynchronization::FirstVSyncReady;
  mStateMachine[ State::INITIALISING ][ Event::STOP         ] = &ThreadSynchronization::Stop;

  mStateMachine[ State::RUNNING ][ Event::PAUSE           ] = &ThreadSynchronization::Pause;
  mStateMachine[ State::RUNNING ][ Event::UPDATE_READY    ] = &ThreadSynchronization::UpdateReady;
  mStateMachine[ State::RUNNING ][ Event::RENDER_READY    ] = &ThreadSynchronization::RenderReady;
  mStateMachine[ State::RUNNING ][ Event::VSYNC_READY     ] = &ThreadSynchronization::VSyncReady;
  mStateMachine[ State::RUNNING ][ Event::SLEEP           ] = &ThreadSynchronization::Sleep;
  mStateMachine[ State::RUNNING ][ Event::STOP            ] = &ThreadSynchronization::Stop;
  mStateMachine[ State::RUNNING ][ Event::REPLACE_SURFACE ] = &ThreadSynchronization::ReplaceSurface;

  mStateMachine[ State::SLEEPING ][ Event::PAUSE           ] = &ThreadSynchronization::Pause;
  mStateMachine[ State::SLEEPING ][ Event::UPDATE_READY    ] = &ThreadSynchronization::UpdateReadySleeping;
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
  mStateMachine[ State::REPLACING_SURFACE ][ Event::RENDER_READY     ] = &ThreadSynchronization::RenderReadyReplaceSurface;
  mStateMachine[ State::REPLACING_SURFACE ][ Event::VSYNC_READY      ] = &ThreadSynchronization::VSyncReady;
  mStateMachine[ State::REPLACING_SURFACE ][ Event::STOP             ] = &ThreadSynchronization::Stop;
  mStateMachine[ State::REPLACING_SURFACE ][ Event::SURFACE_REPLACED ] = &ThreadSynchronization::SurfaceReplaced;

  mStateMachine[ State::SURFACE_REPLACED ][ Event::RENDER_READY ] = &ThreadSynchronization::RenderReadySurfaceReplaced;
}

ThreadSynchronization::~ThreadSynchronization()
{
}

void ThreadSynchronization::Initialise()
{
  ProcessEvent( Event::INITIALISE );
}

void ThreadSynchronization::Start()
{
  ProcessEvent( Event::START );
}

void ThreadSynchronization::Stop()
{
  ProcessEvent( Event::STOP );
}

void ThreadSynchronization::Pause()
{
  ProcessEvent( Event::PAUSE );
}

void ThreadSynchronization::Resume()
{
  ProcessEvent( Event::RESUME );
}

void ThreadSynchronization::UpdateRequest()
{
  ProcessEvent( Event::UPDATE_REQUEST );
}

void ThreadSynchronization::UpdateOnce()
{
  ProcessEvent( Event::UPDATE_ONCE );
}

void ThreadSynchronization::ResumeFrameTime()
{
  mFrameTime.Resume();
}

void ThreadSynchronization::ReplaceSurface( RenderSurface* newSurface )
{
  EventData eventData;
  eventData.newSurface = newSurface;

  ThreadData data;
  data.eventData = &eventData;

  ProcessEvent( Event::REPLACE_SURFACE, &data );
}

bool ThreadSynchronization::UpdateReady( bool notifyEvent )
{
  UpdateData updateData;
  updateData.notifyEvent = notifyEvent;

  ThreadData data;
  data.updateData = &updateData;

  return ProcessEvent( Event::UPDATE_READY, &data );
}

void ThreadSynchronization::Sleep()
{
  ProcessEvent( Event::SLEEP );
}

bool ThreadSynchronization::RenderReady( RenderRequest*& requestPtr )
{
  RenderData renderData( requestPtr );
  ThreadData data;
  data.renderData = &renderData;

  return ProcessEvent( Event::RENDER_READY, &data );
}

void ThreadSynchronization::SurfaceReplaced()
{
  ProcessEvent( Event::SURFACE_REPLACED );
}

bool ThreadSynchronization::VSyncReady( bool validSync, unsigned int frameNumber, unsigned int seconds, unsigned int microseconds, unsigned int& numberOfVSyncsPerRender )
{
  VSyncData vSyncData( numberOfVSyncsPerRender );
  vSyncData.validSync = validSync;
  vSyncData.frameNumber = frameNumber;
  vSyncData.seconds = seconds;
  vSyncData.microseconds = microseconds;

  ThreadData data;
  data.vSyncData = &vSyncData;

  return ProcessEvent( Event::VSYNC_READY, &data );
}

uint64_t ThreadSynchronization::GetTimeMicroseconds()
{
  uint64_t currentTime(0);

  {
    Mutex::ScopedLock lock( mMutex );

    currentTime = mSyncSeconds;
    currentTime *= MICROSECONDS_PER_SECOND;
    currentTime += mSyncMicroseconds;
  }

  return currentTime;
}

void ThreadSynchronization::SetRenderRefreshRate( unsigned int numberOfVSyncsPerRender )
{
  mNumberOfVSyncsPerRender = numberOfVSyncsPerRender;
}

inline void ThreadSynchronization::AddPerformanceMarker( PerformanceInterface::MarkerType type )
{
  if( mPerformanceInterface )
  {
    mPerformanceInterface->AddMarker( type );
  }
}

void ThreadSynchronization::PredictNextSyncTime(
  float& lastFrameDeltaSeconds,
  unsigned int& lastSyncTimeMilliseconds,
  unsigned int& nextSyncTimeMilliseconds )
{
  mFrameTime.PredictNextSyncTime( lastFrameDeltaSeconds, lastSyncTimeMilliseconds, nextSyncTimeMilliseconds );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
///////////////////////////////////////////////////////////////////////////////////////////////////

bool ThreadSynchronization::ProcessEvent( int event, ThreadData* data  )
{
  int currentState = State::STOPPED;
  __atomic_load( &mState, &currentState, __ATOMIC_SEQ_CST );

  ProcessEventFunction func = mStateMachine[ currentState ][ event ];
  if( func )
  {
    (this->*func)( data );
  }

  return __atomic_load_n( &mState, __ATOMIC_SEQ_CST ) != State::STOPPED;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EVENT THREAD
///////////////////////////////////////////////////////////////////////////////////////////////////

void ThreadSynchronization::Initialise( ThreadData* /* data */ )
{
  LOG_EVENT( "%s", __FUNCTION__ );

  mState = State::INITIALISING;
}

void ThreadSynchronization::Start( ThreadData* /* data */ )
{
  LOG_EVENT( "%s", __FUNCTION__ );

  mFrameTime.SetMinimumFrameTimeInterval( mNumberOfVSyncsPerRender * TIME_PER_FRAME_IN_MICROSECONDS );

  while( __atomic_load_n( &mNumberOfThreadsStarted, __ATOMIC_SEQ_CST ) < CREATED_THREAD_COUNT )
  {
    mEventThreadWaitCondition.Wait();
  }

  __atomic_store_n( &mState, State::RUNNING, __ATOMIC_SEQ_CST );
  mVSyncThreadWaitCondition.Notify();
  mUpdateThreadWaitCondition.Notify();
}

void ThreadSynchronization::Pause( ThreadData* /* data */ )
{
  LOG_EVENT( "%s", __FUNCTION__ );

  __atomic_store_n( &mState, State::PAUSED, __ATOMIC_SEQ_CST );

  // TODO AddPerformanceMarker( PerformanceInterface::PAUSED );
  mFrameTime.Suspend();
}

void ThreadSynchronization::Resume( ThreadData* /* data */ )
{
  LOG_EVENT( "%s", __FUNCTION__ );

  __atomic_store_n( &mState, State::RUNNING, __ATOMIC_SEQ_CST );
  mVSyncThreadWaitCondition.Notify();

  // TODO AddPerformanceMarker( PerformanceInterface::RESUME);
}

void ThreadSynchronization::UpdateRequest( ThreadData* /* data */ )
{
  LOG_EVENT( "%s", __FUNCTION__ );

  __atomic_store_n( &mState, State::RUNNING, __ATOMIC_SEQ_CST );
  mVSyncThreadWaitCondition.Notify();
}

void ThreadSynchronization::UpdateOnce( ThreadData* /* data */ )
{
  LOG_EVENT( "%s", __FUNCTION__ );

  mUpdateThreadWaitCondition.Notify();
}

void ThreadSynchronization::Stop( ThreadData* /* data */ )
{
  LOG_EVENT( "%s", __FUNCTION__ );

  __atomic_store_n( &mState, State::STOPPED, __ATOMIC_SEQ_CST );

  // Notify all other threads so they continue
  mVSyncThreadWaitCondition.Notify();
  mUpdateThreadWaitCondition.Notify();
  mRenderThreadWaitCondition.Notify();

  mFrameTime.Suspend();
}

void ThreadSynchronization::ReplaceSurface( ThreadData* data )
{
  LOG_EVENT( "%s: START", __FUNCTION__ );

  if( data && data->eventData && data->eventData->newSurface )
  {
    Mutex::ScopedLock lock( mMutex );
    mReplaceSurfaceRequest.SetSurface( data->eventData->newSurface );
  }

  int previousState = __atomic_exchange_n( &mState, State::REPLACING_SURFACE, __ATOMIC_SEQ_CST );

  // Run the RenderThread in case it's waiting
  mRenderThreadWaitCondition.Notify();

  // Wait for RenderThread to replace the surface
  mEventThreadWaitCondition.Wait();

  __atomic_store_n( &mState, previousState, __ATOMIC_SEQ_CST );
  mVSyncThreadWaitCondition.Notify();

  LOG_EVENT( "%s: END", __FUNCTION__ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// UPDATE THREAD
///////////////////////////////////////////////////////////////////////////////////////////////////

void ThreadSynchronization::FirstUpdateReady( ThreadData* /* data */ )
{
  LOG_UPDATE( "%s", __FUNCTION__ );

  // Notify event thread that this thread is running
  __atomic_add_fetch( &mNumberOfThreadsStarted, 1, __ATOMIC_SEQ_CST );
  mEventThreadWaitCondition.Notify();

  // Wait for first thread-sync point
  if( __atomic_load_n( &mState, __ATOMIC_SEQ_CST ) == State::INITIALISING )
  {
    mUpdateThreadWaitCondition.Wait();
  }

  // TODO AddPerformanceMarker( PerformanceInterface::UPDATE_START );
}

void ThreadSynchronization::UpdateReady( ThreadData* data )
{
  LOG_UPDATE( "%s", __FUNCTION__ );

  // TODO AddPerformanceMarker( PerformanceInterface::UPDATE_END );

  if( data && data->updateData && data->updateData->notifyEvent )
  {
    // Do the notifications first so the event thread can start processing them
    // Tell the event-thread to wake up (if asleep) and send a notification event to Core
    mNotificationTrigger.Trigger();
  }

  int updateAheadOfRender = __atomic_add_fetch( &mUpdateAheadOfRender, 1, __ATOMIC_SEQ_CST );
  LOG_UPDATE_COUNTER_UPDATE( "%s: updateAheadOfRender(%d)", __FUNCTION__, updateAheadOfRender );
  mRenderThreadWaitCondition.Notify();

  if( updateAheadOfRender >= mMaximumUpdateCount )
  {
    while ( __atomic_load_n( &mUpdateAheadOfRender, __ATOMIC_SEQ_CST ) >= mMaximumUpdateCount )
    {
      mRenderThreadWaitCondition.Notify(); // Notify the render thread in case it was waiting
      mUpdateThreadWaitCondition.Wait();
    }
  }

  // Wait for VSync
  int vSyncAheadOfUpdate = 0;
  if( ( vSyncAheadOfUpdate = __atomic_fetch_and( &mVSyncAheadOfUpdate, 0, __ATOMIC_SEQ_CST ) ) == 0 )
  {
    LOG_VSYNC_COUNTER_UPDATE( "%s:  vSyncAheadOfUpdate(%d) WAIT", __FUNCTION__, vSyncAheadOfUpdate );
    mUpdateThreadWaitCondition.Wait();
    __atomic_store_n( &mVSyncAheadOfUpdate, 0, __ATOMIC_SEQ_CST ); // Reset to 0
  }
  else
  {
    LOG_VSYNC_COUNTER_UPDATE( "%s:  vSyncAheadOfUpdate(%d)", __FUNCTION__, vSyncAheadOfUpdate );
  }

  // TODO AddPerformanceMarker( PerformanceInterface::UPDATE_START );
}

void ThreadSynchronization::UpdateReadySleeping( ThreadData* data )
{
  LOG_UPDATE( "%s: START", __FUNCTION__ );

  // Inform frame time
  mFrameTime.Sleep();

  UpdateReady( data );

  //
  // Woken up
  //

  // Inform frame timer
  mFrameTime.WakeUp();

  LOG_UPDATE( "%s: END", __FUNCTION__ );
}

void ThreadSynchronization::Sleep( ThreadData* /* data */ )
{
  LOG_EVENT( "%s", __FUNCTION__ );
  __atomic_store_n( &mState, State::SLEEPING, __ATOMIC_SEQ_CST );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// RENDER THREAD
///////////////////////////////////////////////////////////////////////////////////////////////////

void ThreadSynchronization::FirstRenderReady( ThreadData* /* data */ )
{
  LOG_RENDER( "%s", __FUNCTION__ );

  // Notify event thread that this thread is running
  __atomic_add_fetch( &mNumberOfThreadsStarted, 1, __ATOMIC_SEQ_CST );
  mEventThreadWaitCondition.Notify();

  // Wait for first update to complete
  mRenderThreadWaitCondition.Wait();

  // TODO AddPerformanceMarker( PerformanceInterface::RENDER_START );
}

void ThreadSynchronization::RenderReady( ThreadData* data )
{
  LOG_RENDER( "%s", __FUNCTION__ );

  // TODO AddPerformanceMarker( PerformanceInterface::RENDER_END );

  int updateAheadOfRender = 0;
  if( ( updateAheadOfRender = __atomic_sub_fetch( &mUpdateAheadOfRender, 1, __ATOMIC_SEQ_CST ) ) == 0 )
  {
    LOG_UPDATE_COUNTER_RENDER( "%s: updateAheadOfRender(%d) WAIT", __FUNCTION__, updateAheadOfRender );
    mRenderThreadWaitCondition.Wait();
  }
  else
  {
    LOG_UPDATE_COUNTER_RENDER( "%s: updateAheadOfRender(%d)", __FUNCTION__, updateAheadOfRender );
  }

  if( __atomic_load_n( &mState, __ATOMIC_SEQ_CST ) == State::REPLACING_SURFACE )
  {
    if( data && data->renderData )
    {
      Mutex::ScopedLock lock( mMutex );
      data->renderData->requestPtr = &mReplaceSurfaceRequest;
    }
  }

  // TODO AddPerformanceMarker( PerformanceInterface::RENDER_START );
}

void ThreadSynchronization::RenderReadyReplaceSurface( ThreadData* data )
{
  LOG_RENDER( "%s", __FUNCTION__ );

  // TODO AddPerformanceMarker( PerformanceInterface::RENDER_END );

  // We're about to replace the surface so set replacing surface to true and return
  if( data && data->renderData )
  {
    Mutex::ScopedLock lock( mMutex );
    data->renderData->requestPtr = &mReplaceSurfaceRequest;
  }
}

void ThreadSynchronization::RenderReadySurfaceReplaced( ThreadData* data )
{
  LOG_RENDER( "%s: START", __FUNCTION__ );

  // Notify EventThread that the surface has been replaced
  mEventThreadWaitCondition.Notify();

  // Wait for next update before continuing
  mRenderThreadWaitCondition.Wait();

  if( __atomic_load_n( &mState, __ATOMIC_SEQ_CST ) == State::REPLACING_SURFACE )
  {
    if( data && data->renderData )
    {
      Mutex::ScopedLock lock( mMutex );
      data->renderData->requestPtr = &mReplaceSurfaceRequest;
    }
  }

  LOG_RENDER( "%s: END", __FUNCTION__ );
}

void ThreadSynchronization::SurfaceReplaced( ThreadData* data )
{
  LOG_EVENT( "%s", __FUNCTION__ );

  __atomic_store_n( &mState, State::SURFACE_REPLACED, __ATOMIC_SEQ_CST );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// V-SYNC THREAD
///////////////////////////////////////////////////////////////////////////////////////////////////

void ThreadSynchronization::FirstVSyncReady( ThreadData* data )
{
  LOG_VSYNC( "%s", __FUNCTION__ );

  if( data && data->vSyncData )
  {
    if( data->vSyncData->numberOfVSyncsPerRender != mNumberOfVSyncsPerRender )
    {
      data->vSyncData->numberOfVSyncsPerRender = mNumberOfVSyncsPerRender; // save it back
      mFrameTime.SetMinimumFrameTimeInterval( mNumberOfVSyncsPerRender * TIME_PER_FRAME_IN_MICROSECONDS );
    }

    if( data->vSyncData->validSync )
    {
      mFrameTime.SetSyncTime( data->vSyncData->frameNumber );
    }

    Mutex::ScopedLock lock( mMutex );

    mSyncFrameNumber = data->vSyncData->frameNumber;
    mSyncSeconds = data->vSyncData->seconds;
    mSyncMicroseconds = data->vSyncData->microseconds;
  }

  // Notify event thread that this thread is running
  __atomic_add_fetch( &mNumberOfThreadsStarted, 1, __ATOMIC_SEQ_CST );
  mEventThreadWaitCondition.Notify();

  // Wait for all threads to be ready
  if( __atomic_load_n( &mState, __ATOMIC_SEQ_CST ) == State::INITIALISING )
  {
    mVSyncThreadWaitCondition.Wait();
  }
}

void ThreadSynchronization::VSyncReady( ThreadData* data )
{
  LOG_VSYNC( "%s", __FUNCTION__ );

  if( data && data->vSyncData )
  {
    if( data->vSyncData->numberOfVSyncsPerRender != mNumberOfVSyncsPerRender )
    {
      data->vSyncData->numberOfVSyncsPerRender = mNumberOfVSyncsPerRender; // save it back
      mFrameTime.SetMinimumFrameTimeInterval( mNumberOfVSyncsPerRender * TIME_PER_FRAME_IN_MICROSECONDS );
    }

    if( data->vSyncData->validSync )
    {
      mFrameTime.SetSyncTime( data->vSyncData->frameNumber );
    }

    Mutex::ScopedLock lock( mMutex );

    mSyncFrameNumber = data->vSyncData->frameNumber;
    mSyncSeconds = data->vSyncData->seconds;
    mSyncMicroseconds = data->vSyncData->microseconds;
  }

  // TODO AddPerformanceMarker( PerformanceInterface::VSYNC );

  int vSyncAheadOfUpdate = __atomic_add_fetch( &mVSyncAheadOfUpdate, 1, __ATOMIC_SEQ_CST );
  (void)vSyncAheadOfUpdate;
  LOG_VSYNC_COUNTER_VSYNC( "%s: vSyncAheadOfUpdate(%d)\n",  __FUNCTION__, vSyncAheadOfUpdate );

  mUpdateThreadWaitCondition.Notify();

  int currentState = 0;
  __atomic_load( &mState, &currentState, __ATOMIC_SEQ_CST );
  while( ( currentState == State::PAUSED )   ||
         ( currentState == State::SLEEPING ) ||
         ( currentState == State::REPLACING_SURFACE ) )
  {
    mVSyncThreadWaitCondition.Wait();
    __atomic_load( &mState, &currentState, __ATOMIC_SEQ_CST );
  }
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
