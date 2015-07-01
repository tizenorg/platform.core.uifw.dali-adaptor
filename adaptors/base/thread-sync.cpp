#include <cstdio>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstring>

#include "conditional-wait.h"
#include "colors.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

enum TestConditionState
{
   STOP_WHILE_PAUSED,
   STOP_WHILE_RUNNING,
   STOP_WHILE_INITIALISING,
   STOP_WHILE_SLEEPING,
   REPLACE_WHILE_PAUSED,
   REPLACE_WHILE_RUNNING,
   REPLACE_WHILE_SLEEPING,
   UPDATE_SLOW,
   RENDER_SLOW,
   UPDATE_RENDER_SLOW,
   NORMAL
};
const TestConditionState TEST_CONDITION = NORMAL;

//#define ENABLE_VSYNC_COUNTER_LOGGING 1
#define ENABLE_UPDATE_COUNTER_LOGGING 1
//#define ENABLE_VSYNC_THREAD_LOGGING 1
//#define ENABLE_UPDATE_THREAD_LOGGING 1
//#define ENABLE_RENDER_THREAD_LOGGING 1
#define ENABLE_EVENT_LOGGING

#define LOG_THREAD_SYNC(color,...)      std::cout << color; printf(__VA_ARGS__); std::cout << Color::CLEAR << std::endl;

#ifdef ENABLE_VSYNC_COUNTER_LOGGING
#define LOG_VSYNC_COUNTER_VSYNC(...)    LOG_THREAD_SYNC(Color::LIGHT_RED,__VA_ARGS__)
#define LOG_VSYNC_COUNTER_UPDATE(...)   LOG_THREAD_SYNC(Color::LIGHT_YELLOW,__VA_ARGS__)
#else
#define LOG_VSYNC_COUNTER_VSYNC(...)
#define LOG_VSYNC_COUNTER_UPDATE(...)
#endif

#ifdef ENABLE_UPDATE_COUNTER_LOGGING
#define LOG_UPDATE_COUNTER_UPDATE(...)  LOG_THREAD_SYNC(Color::YELLOW,__VA_ARGS__)
#define LOG_UPDATE_COUNTER_RENDER(...)  LOG_THREAD_SYNC(Color::LIGHT_BLUE,__VA_ARGS__)
#else
#define LOG_UPDATE_COUNTER_UPDATE(...)
#define LOG_UPDATE_COUNTER_RENDER(...)
#endif

#ifdef ENABLE_VSYNC_THREAD_LOGGING
#define LOG_VSYNC(...)             LOG_THREAD_SYNC(Color::RED,__VA_ARGS__)
#else
#define LOG_VSYNC(...)
#endif

#ifdef ENABLE_UPDATE_THREAD_LOGGING
#define LOG_UPDATE(...)            LOG_THREAD_SYNC(Color::YELLOW,__VA_ARGS__)
#else
#define LOG_UPDATE(...)
#endif

#ifdef ENABLE_RENDER_THREAD_LOGGING
#define LOG_RENDER(...)            LOG_THREAD_SYNC(Color::BLUE,__VA_ARGS__)
#else
#define LOG_RENDER(...)
#endif

#ifdef ENABLE_EVENT_LOGGING
#define LOG_EVENT(...)             LOG_THREAD_SYNC(Color::WHITE,__VA_ARGS__)
#else
#define LOG_EVENT(...)
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace State
{
enum Type
{
  STOPPED,
  INITIALISING,
  RUNNING,
  PAUSED,
  SLEEPING,
  REPLACING_SURFACE,
  SURFACE_REPLACED,

  TOTAL_STATES
};
} // namespace State

namespace Event
{
enum Type
{
  INITIALISE,
  START,
  STOP,
  PAUSE,
  RESUME,
  SLEEP,
  UPDATE_REQUEST,
  UPDATE_ONCE,
  UPDATE_READY,
  RENDER_READY,
  VSYNC_READY,
  REPLACE_SURFACE,
  SURFACE_REPLACED,

  TOTAL_EVENTS
};
} // namespace Event

typedef void ( *ProcessEventFunction )();

ProcessEventFunction STATE_MACHINE[ State::TOTAL_STATES ][ Event::TOTAL_EVENTS ] = { NULL, };

///////////////////////////////////////////////////////////////////////////////////////////////////

const int CREATED_THREAD_COUNT = 3;
const int MAX_UPDATE_AHEAD_OF_RENDER = 2;

volatile int gState = State::STOPPED;

ConditionalWait gUpdateThreadWaitCondition;
ConditionalWait gRenderThreadWaitCondition;
ConditionalWait gVSyncThreadWaitCondition;
ConditionalWait gEventThreadWaitCondition;

volatile int gVSyncAheadOfUpdate = 0;
volatile int gUpdateAheadOfRender = 0;

volatile int gNumberOfThreadsStarted = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
bool ProcessEvent( Event::Type event )
{
  int currentState = State::STOPPED;
  __atomic_load( &gState, &currentState, __ATOMIC_SEQ_CST );

  ProcessEventFunction func = STATE_MACHINE[ currentState ][ event ];
  if( func )
  {
    func();
  }

  return __atomic_load_n( &gState, __ATOMIC_SEQ_CST ) != State::STOPPED;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// UPDATE THREAD
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace UpdateThread
{
void FirstUpdateReady()
{
  LOG_UPDATE( "%s", __PRETTY_FUNCTION__ );

  // Notify event thread that this thread is running
  __atomic_add_fetch( &gNumberOfThreadsStarted, 1, __ATOMIC_SEQ_CST );
  gEventThreadWaitCondition.Notify();

  // Wait for first thread-sync point
  if( __atomic_load_n( &gState, __ATOMIC_SEQ_CST ) == State::INITIALISING )
  {
    gUpdateThreadWaitCondition.Wait();
  }
}

void UpdateReady()
{
  LOG_UPDATE( "%s", __PRETTY_FUNCTION__ );

  int updateAheadOfRender = __atomic_add_fetch( &gUpdateAheadOfRender, 1, __ATOMIC_SEQ_CST );
  LOG_UPDATE_COUNTER_UPDATE( "%s: updateAheadOfRender(%d)", __PRETTY_FUNCTION__, updateAheadOfRender );
  gRenderThreadWaitCondition.Notify();

  if( updateAheadOfRender >= MAX_UPDATE_AHEAD_OF_RENDER )
  {
    while ( __atomic_load_n( &gUpdateAheadOfRender, __ATOMIC_SEQ_CST ) >= MAX_UPDATE_AHEAD_OF_RENDER )
    {
      gUpdateThreadWaitCondition.Wait();
    }
  }

  // Wait for VSync
  int vSyncAheadOfUpdate = 0;
  if( ( vSyncAheadOfUpdate = __atomic_fetch_and( &gVSyncAheadOfUpdate, 0, __ATOMIC_SEQ_CST ) ) == 0 )
  {
    LOG_VSYNC_COUNTER_UPDATE( "%s:  vSyncAheadOfUpdate(%d) WAIT", __PRETTY_FUNCTION__, vSyncAheadOfUpdate );
    gUpdateThreadWaitCondition.Wait();
    __atomic_store_n( &gVSyncAheadOfUpdate, 0, __ATOMIC_SEQ_CST ); // Reset to 0
  }
  else
  {
    LOG_VSYNC_COUNTER_UPDATE( "%s:  vSyncAheadOfUpdate(%d)", __PRETTY_FUNCTION__, vSyncAheadOfUpdate );
  }
}

void Sleep()
{
  LOG_EVENT( "%s", __PRETTY_FUNCTION__ );
  __atomic_store_n( &gState, State::SLEEPING, __ATOMIC_SEQ_CST );
}

static void* Run( void* data )
{
  while ( ProcessEvent( Event::UPDATE_READY ) )
  {
    // UPDATE
    std::cout << Color::YELLOW << __PRETTY_FUNCTION__ << Color::CLEAR << std::endl;

    if( ( TEST_CONDITION == REPLACE_WHILE_SLEEPING ) ||
        ( TEST_CONDITION == STOP_WHILE_SLEEPING ) )
    {
      static int count = 0;
      static bool done = false;
      if ( ( ++count == 60 ) && !done )
      {
        done = true;
        ProcessEvent( Event::SLEEP );
      }
    }
    else if( TEST_CONDITION == UPDATE_SLOW )
    {
      static int count = 0;
      if( ++count == 60 )
      {
        sleep(1);
        count = 0;
      }
    }
    else if( TEST_CONDITION == UPDATE_RENDER_SLOW )
    {
      static int count = 0;
      static bool slow = false;
      if( !slow )
      {
        if( ++count == 62 )
        {
          sleep(1);
          slow = true;
        }
      }
      else
      {
        if( --count == 0 )
        {
          slow = false;
        }
        else if( count >= 60 )
        {
          sleep(1);
        }
      }
    }
  }
  return NULL;
}

} // namespace UpdateThread

///////////////////////////////////////////////////////////////////////////////////////////////////
// RENDER THREAD
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace RenderThread
{

bool gReplaceSurface = false;

void FirstRenderReady()
{
  LOG_RENDER( "%s", __PRETTY_FUNCTION__ );

  // Notify event thread that this thread is running
  __atomic_add_fetch( &gNumberOfThreadsStarted, 1, __ATOMIC_SEQ_CST );
  gEventThreadWaitCondition.Notify();

  // Wait for first update to complete
  gRenderThreadWaitCondition.Wait();
}

void RenderReady()
{
  LOG_RENDER( "%s", __PRETTY_FUNCTION__ );

  int updateAheadOfRender = 0;
  if( ( updateAheadOfRender = __atomic_sub_fetch( &gUpdateAheadOfRender, 1, __ATOMIC_SEQ_CST ) ) == 0 )
  {
    LOG_UPDATE_COUNTER_RENDER( "%s: updateAheadOfRender(%d) WAIT", __PRETTY_FUNCTION__, updateAheadOfRender );
    gRenderThreadWaitCondition.Wait();
  }
  else
  {
    LOG_UPDATE_COUNTER_RENDER( "%s: updateAheadOfRender(%d)", __PRETTY_FUNCTION__, updateAheadOfRender );
  }

  if( __atomic_load_n( &gState, __ATOMIC_SEQ_CST ) == State::REPLACING_SURFACE )
  {
    gReplaceSurface = true;
  }
}

void RenderReadyReplaceSurface()
{
  LOG_RENDER( "%s", __PRETTY_FUNCTION__ );

  // We're about to replace the surface so set replacing surface to true and return
  gReplaceSurface = true;
}

void RenderReadySurfaceReplaced()
{
  LOG_RENDER( "%s: START", __PRETTY_FUNCTION__ );

  // Notify EventThread that the surface has been replaced
  gEventThreadWaitCondition.Notify();

  // Wait for next update before continuing
  gRenderThreadWaitCondition.Wait();

  if( __atomic_load_n( &gState, __ATOMIC_SEQ_CST ) == State::REPLACING_SURFACE )
  {
    gReplaceSurface = true;
  }

  LOG_RENDER( "%s: END", __PRETTY_FUNCTION__ );
}

void SurfaceReplaced()
{
  LOG_EVENT( "%s", __PRETTY_FUNCTION__ );

  __atomic_store_n( &gState, State::SURFACE_REPLACED, __ATOMIC_SEQ_CST );
}

static void* Run( void* data )
{
  bool running = true;
  while ( running )
  {
    running = ProcessEvent( Event::RENDER_READY );

    // Check if we need to replace our surface
    if( gReplaceSurface )
    {
      sleep(2); // Pretend this is us replacing the surface

      gReplaceSurface = false;

      running = ProcessEvent( Event::SURFACE_REPLACED );
    }
    else
    {
      // RENDER
      if( running )
      {
        std::cout << Color::LIGHT_BLUE << __PRETTY_FUNCTION__ << Color::CLEAR << std::endl;
        if( TEST_CONDITION == RENDER_SLOW )
        {
          static int count = 0;
          if( ++count == 60 )
          {
            sleep(1);
            count = 0;
          }
        }
        else if( TEST_CONDITION == UPDATE_RENDER_SLOW )
        {
          static int count = 0;
          static bool slow = false;
          if( !slow )
          {
            if( ++count == 32 )
            {
              sleep(1);
              slow = true;
            }
          }
          else
          {
            if( --count == 0 )
            {
              slow = false;
            }
            else if( count >= 30 )
            {
              sleep(1);
            }
          }
        }
      }
    }
  }
  return NULL;
}

} // namespace RenderThread

///////////////////////////////////////////////////////////////////////////////////////////////////
// V-SYNC THREAD
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace VSyncThread
{

void FirstVSyncReady()
{
  LOG_VSYNC( "%s", __PRETTY_FUNCTION__ );

  // Notify event thread that this thread is running
  __atomic_add_fetch( &gNumberOfThreadsStarted, 1, __ATOMIC_SEQ_CST );
  gEventThreadWaitCondition.Notify();

  // Wait for all threads to be ready
  if( __atomic_load_n( &gState, __ATOMIC_SEQ_CST ) == State::INITIALISING )
  {
    gVSyncThreadWaitCondition.Wait();
  }
}

void VSyncReady()
{
  LOG_VSYNC( "%s", __PRETTY_FUNCTION__ );

  int vSyncAheadOfUpdate = __atomic_add_fetch( &gVSyncAheadOfUpdate, 1, __ATOMIC_SEQ_CST );
  LOG_VSYNC_COUNTER_VSYNC( "%s: vSyncAheadOfUpdate(%d)\n",  __PRETTY_FUNCTION__, vSyncAheadOfUpdate );
  gUpdateThreadWaitCondition.Notify();

  int currentState = 0;
  __atomic_load( &gState, &currentState, __ATOMIC_SEQ_CST );
  while( ( currentState == State::PAUSED )   ||
         ( currentState == State::SLEEPING ) ||
         ( currentState == State::REPLACING_SURFACE ) )
  {
    gVSyncThreadWaitCondition.Wait();
    __atomic_load( &gState, &currentState, __ATOMIC_SEQ_CST );
  }
}

static void* Run( void* data )
{
  while ( ProcessEvent( Event::VSYNC_READY ) )
  {
    usleep(16666);

    std::cout << Color::RED << __PRETTY_FUNCTION__ << Color::CLEAR << std::endl;
  }
  return NULL;
}

} // namespace VSyncThread

///////////////////////////////////////////////////////////////////////////////////////////////////
// EVENT THREAD
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace EventThread
{
void Initialise()
{
  LOG_EVENT( "%s", __PRETTY_FUNCTION__ );

  gState = State::INITIALISING;
}

void Start()
{
  LOG_EVENT( "%s", __PRETTY_FUNCTION__ );

  while( __atomic_load_n( &gNumberOfThreadsStarted, __ATOMIC_SEQ_CST ) < CREATED_THREAD_COUNT )
  {
    gEventThreadWaitCondition.Wait();
  }

  __atomic_store_n( &gState, State::RUNNING, __ATOMIC_SEQ_CST );
  gVSyncThreadWaitCondition.Notify();
  gUpdateThreadWaitCondition.Notify();
}

void Pause()
{
  LOG_EVENT( "%s", __PRETTY_FUNCTION__ );

  __atomic_store_n( &gState, State::PAUSED, __ATOMIC_SEQ_CST );
}

void Resume()
{
  LOG_EVENT( "%s", __PRETTY_FUNCTION__ );

  __atomic_store_n( &gState, State::RUNNING, __ATOMIC_SEQ_CST );
  gVSyncThreadWaitCondition.Notify();
}

void UpdateRequest()
{
  LOG_EVENT( "%s", __PRETTY_FUNCTION__ );

  __atomic_store_n( &gState, State::RUNNING, __ATOMIC_SEQ_CST );
  gVSyncThreadWaitCondition.Notify();
}

void UpdateOnce()
{
  LOG_EVENT( "%s", __PRETTY_FUNCTION__ );

  gUpdateThreadWaitCondition.Notify();
}

void Stop()
{
  LOG_EVENT( "%s", __PRETTY_FUNCTION__ );

  __atomic_store_n( &gState, State::STOPPED, __ATOMIC_SEQ_CST );

  // Notify all other threads so they continue
  gVSyncThreadWaitCondition.Notify();
  gUpdateThreadWaitCondition.Notify();
  gRenderThreadWaitCondition.Notify();
}

void ReplaceSurface()
{
  LOG_EVENT( "%s: START", __PRETTY_FUNCTION__ );

  // Lock Mutex
  // Set new surface
  // Unlock Mutex

  int previousState = __atomic_exchange_n( &gState, State::REPLACING_SURFACE, __ATOMIC_SEQ_CST );

  // Run the RenderThread in case it's waiting
  gRenderThreadWaitCondition.Notify();

  // Wait for RenderThread to replace the surface
  gEventThreadWaitCondition.Wait();

  __atomic_store_n( &gState, previousState, __ATOMIC_SEQ_CST );
  gVSyncThreadWaitCondition.Notify();

  LOG_EVENT( "%s: END", __PRETTY_FUNCTION__ );
}

} // namespace EventThread

///////////////////////////////////////////////////////////////////////////////////////////////////

void SetupStateMachine()
{
  STATE_MACHINE[ State::STOPPED ][ Event::INITIALISE ] = EventThread::Initialise;

  STATE_MACHINE[ State::INITIALISING ][ Event::START        ] = EventThread::Start;
  STATE_MACHINE[ State::INITIALISING ][ Event::UPDATE_READY ] = UpdateThread::FirstUpdateReady;
  STATE_MACHINE[ State::INITIALISING ][ Event::RENDER_READY ] = RenderThread::FirstRenderReady;
  STATE_MACHINE[ State::INITIALISING ][ Event::VSYNC_READY  ] = VSyncThread::FirstVSyncReady;
  STATE_MACHINE[ State::INITIALISING ][ Event::STOP         ] = EventThread::Stop;

  STATE_MACHINE[ State::RUNNING ][ Event::PAUSE           ] = EventThread::Pause;
  STATE_MACHINE[ State::RUNNING ][ Event::UPDATE_READY    ] = UpdateThread::UpdateReady;
  STATE_MACHINE[ State::RUNNING ][ Event::RENDER_READY    ] = RenderThread::RenderReady;
  STATE_MACHINE[ State::RUNNING ][ Event::VSYNC_READY     ] = VSyncThread::VSyncReady;
  STATE_MACHINE[ State::RUNNING ][ Event::SLEEP           ] = UpdateThread::Sleep;
  STATE_MACHINE[ State::RUNNING ][ Event::STOP            ] = EventThread::Stop;
  STATE_MACHINE[ State::RUNNING ][ Event::REPLACE_SURFACE ] = EventThread::ReplaceSurface;

  STATE_MACHINE[ State::SLEEPING ][ Event::PAUSE           ] = EventThread::Pause;
  STATE_MACHINE[ State::SLEEPING ][ Event::UPDATE_READY    ] = UpdateThread::UpdateReady;
  STATE_MACHINE[ State::SLEEPING ][ Event::RENDER_READY    ] = RenderThread::RenderReady;
  STATE_MACHINE[ State::SLEEPING ][ Event::VSYNC_READY     ] = VSyncThread::VSyncReady;
  STATE_MACHINE[ State::SLEEPING ][ Event::UPDATE_REQUEST  ] = EventThread::UpdateRequest;
  STATE_MACHINE[ State::SLEEPING ][ Event::UPDATE_ONCE     ] = EventThread::UpdateRequest;
  STATE_MACHINE[ State::SLEEPING ][ Event::STOP            ] = EventThread::Stop;
  STATE_MACHINE[ State::SLEEPING ][ Event::REPLACE_SURFACE ] = EventThread::ReplaceSurface;

  STATE_MACHINE[ State::PAUSED ][ Event::RESUME          ] = EventThread::Resume;
  STATE_MACHINE[ State::PAUSED ][ Event::UPDATE_READY    ] = UpdateThread::UpdateReady;
  STATE_MACHINE[ State::PAUSED ][ Event::RENDER_READY    ] = RenderThread::RenderReady;
  STATE_MACHINE[ State::PAUSED ][ Event::VSYNC_READY     ] = VSyncThread::VSyncReady;
  STATE_MACHINE[ State::PAUSED ][ Event::UPDATE_ONCE     ] = EventThread::UpdateOnce;
  STATE_MACHINE[ State::PAUSED ][ Event::STOP            ] = EventThread::Stop;
  STATE_MACHINE[ State::PAUSED ][ Event::REPLACE_SURFACE ] = EventThread::ReplaceSurface;

  STATE_MACHINE[ State::REPLACING_SURFACE ][ Event::PAUSE            ] = EventThread::Pause;
  STATE_MACHINE[ State::REPLACING_SURFACE ][ Event::UPDATE_READY     ] = UpdateThread::UpdateReady;
  STATE_MACHINE[ State::REPLACING_SURFACE ][ Event::RENDER_READY     ] = RenderThread::RenderReadyReplaceSurface;
  STATE_MACHINE[ State::REPLACING_SURFACE ][ Event::VSYNC_READY      ] = VSyncThread::VSyncReady;
  STATE_MACHINE[ State::REPLACING_SURFACE ][ Event::STOP             ] = EventThread::Stop;
  STATE_MACHINE[ State::REPLACING_SURFACE ][ Event::SURFACE_REPLACED ] = RenderThread::SurfaceReplaced;

  STATE_MACHINE[ State::SURFACE_REPLACED ][ Event::RENDER_READY ] = RenderThread::RenderReadySurfaceReplaced;
}

int main()
{
  gState = State::STOPPED;

  SetupStateMachine();

  ProcessEvent( Event::INITIALISE );

  pthread_t updateThread;
  pthread_create( &updateThread, NULL, UpdateThread::Run, NULL );

  pthread_t renderThread;
  pthread_create( &renderThread, NULL, RenderThread::Run, NULL );

  pthread_t vSyncThread;
  pthread_create( &vSyncThread, NULL, VSyncThread::Run, NULL );

  if( TEST_CONDITION == STOP_WHILE_INITIALISING )
  {
    ProcessEvent( Event::STOP );
  }
  else
  {
    ProcessEvent( Event::START );

    int running = true;
    while ( running )
    {
      switch( TEST_CONDITION )
      {
        case STOP_WHILE_PAUSED:
        {
          sleep(2);
          running = ProcessEvent( Event::PAUSE );
          sleep(1);
          running = ProcessEvent( Event::STOP );
          break;
        }

        case STOP_WHILE_RUNNING:
        case STOP_WHILE_SLEEPING:
        {
          sleep(5);
          running = ProcessEvent( Event::STOP );
          break;
        }

        case STOP_WHILE_INITIALISING:
        {
          break;
        }
        case REPLACE_WHILE_PAUSED:
        {
          sleep(1);
          running = ProcessEvent( Event::PAUSE );
          sleep(1);
          running = ProcessEvent( Event::REPLACE_SURFACE );
          static int count = 0;
          if( ++count == 5 )
          {
            sleep(1);
            running = ProcessEvent( Event::UPDATE_ONCE );
          }
          else if( count == 7 )
          {
            sleep(1);
            running = ProcessEvent( Event::RESUME );
            count = 0;
          }
          break;
        }

        case REPLACE_WHILE_RUNNING:
        {
          sleep(5);
          running = ProcessEvent( Event::REPLACE_SURFACE );
          break;
        }

        case REPLACE_WHILE_SLEEPING:
        {
          sleep(5);
          running = ProcessEvent( Event::REPLACE_SURFACE );
          static int count = 0;
          if( ++count == 5 )
          {
            running = ProcessEvent( Event::UPDATE_REQUEST );
          }
          break;
        }

        case UPDATE_SLOW:
        case RENDER_SLOW:
        case UPDATE_RENDER_SLOW:
        case NORMAL:
        {
          sleep(5);
          break;
        }
      }
    }
  }

  // Wait for all threads to stop
  pthread_join( vSyncThread,  NULL );
  pthread_join( updateThread, NULL );
  pthread_join( renderThread, NULL );

  std::cout << "All Threads Stopped" << std::endl;

  return 0;
}

