#ifndef __DALI_INTERNAL_UPDATE_RENDER_SYNCHRONIZATION_H__
#define __DALI_INTERNAL_UPDATE_RENDER_SYNCHRONIZATION_H__

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

// EXTERNAL INCLUDES
#include <stdint.h>
#include <dali/devel-api/common/mutex.h>

// INTERNAL INCLUDES
#include <base/interfaces/performance-interface.h>
#include <base/conditional-wait.h>
#include <trigger-event-interface.h>
#include <base/frame-time.h>
#include <base/render-thread.h>

namespace Dali
{

class RenderSurface;

namespace Integration
{

class Core;
class PlatformAbstraction;

} // namespace Integration

namespace Internal
{

namespace Adaptor
{

class AdaptorInternalServices;

/**
 * This object is used to synchronize the update, render and vsync threads.
 * The Core::GetMaximumUpdateCount() method determines how many frames may be prepared, ahead of the rendering.
 * For example if the maximum update count is 2, then Core::Update() for frame N+1 may be processed whilst frame N is being rendered.
 * However the Core::Update() for frame N+2 may not be called, until the Core::Render() method for frame N has returned.
 *
 */
class ThreadSynchronization
{
public:

  /**
   * Create an update/render synchronization object.
   * @param[in] adaptorInterfaces base adaptor interface
   * @param[in] numberOfVSyncsPerRender The number of frames per render
  */
  ThreadSynchronization( AdaptorInternalServices& adaptorInterfaces, unsigned int numberOfVSyncsPerRender );

  /**
   * Non virtual destructor. Not intended as base class.
   */
  ~ThreadSynchronization();

  /////////////////////////////////////////////////////////////////////////////////////////////////
  // Called by the Event Thread
  /////////////////////////////////////////////////////////////////////////////////////////////////

  /**
   * Initialises the ThreadSynchronisation class. The expectation is that this function will be
   * called when all threads are about to be set up and that Start will be called when
   * the the scene is actually prepared and ready to be displayed. This way our first update
   * will have the actual scene we require.
   *
   * @note Should only be called by the Event Thread.
   */
  void Initialise();

  /**
   * Starts running all threads. This waits until all threads are up and running.
   *
   * @pre A call to Initialise has been made.
   *
   * @note Should only be called by the Event Thread.
   */
  void Start();

  /**
   * Stop the threads.
   *
   * @note Should only be called by the Event Thread.
   */
  void Stop();

  /**
   * Pause the controller (and threads).
   *
   * @note Should only be called by the Event Thread.
   */
  void Pause();

  /**
   * Resume the controller (and threads).
   *
   * @note Should only be called by the Event Thread.
   */
  void Resume();

  /**
   * Wake update thread if sleeping. If the update thread is not sleeping
   * this becomes a no-op.
   * Called when an update is requested by Core.
   * i.e. when a batch of messages have been queued for the next update.
   *
   * @note Should only be called by the Event Thread.
   */
  void UpdateRequest();

  /**
   * Update once (even if paused)
   *
   * @note Should only be called by the Event Thread.
   */
  void UpdateOnce();

  /**
   * Resume the frame time predictor
   */
  void ResumeFrameTime();

  /**
   * Inform the render thread that there is a new surface, and that
   * it should replace the current surface.
   *
   * @param[in] newSurface The new surface for rendering.
   *
   * @note Should only be called by the Event Thread.
   */
  void ReplaceSurface( RenderSurface* newSurface );

  /**
   * Set the refresh rate for rendering
   *
   * @param[in] numberOfVSyncsPerRender The number of vsync frames per render
   *
   * @note Should only be called by the Event Thread.
   */
  void SetRenderRefreshRate( unsigned int numberOfVSyncsPerRender );

  /////////////////////////////////////////////////////////////////////////////////////////////////
  // Called by the Update Thread
  /////////////////////////////////////////////////////////////////////////////////////////////////

  /**
   * Called by Update thread when it is ready to run the update.
   *
   * @param[in] notifyEvent Whether the event thread should be woken up.
   * @param[in] runUpdate Whether to run another update. If false, then the update-thread will attempt to sleep.
   * @param[out] lastFrameDeltaSeconds The delta, in seconds (with float precision), between the last two renders.
   * @param[out] lastSyncTimeMilliseconds The time, in milliseconds, of the last Sync.
   * @param[out] nextSyncTimeMilliseconds The estimated time, in milliseconds, at the next Sync.
   * @return true if updating should continue, false if the update-thread should quit.
   *
   * @note Should only be called by the Update thread.
   */
  bool UpdateReady( bool notifyEvent, bool runUpdate, float& lastFrameDeltaSeconds, unsigned int& lastSyncTimeMilliseconds, unsigned int& nextSyncTimeMilliseconds );

  /////////////////////////////////////////////////////////////////////////////////////////////////
  // Called by the Render Thread
  /////////////////////////////////////////////////////////////////////////////////////////////////

  /**
   * Called by the Render thread when it is ready to render.
   *
   * @param[in] request Pointer to set if there are any requests. This does not need to be freed by the caller.
   *
   * @note Should only be called by the Render thread.
   * @note If there is a request, then the Render thread should NOT perform a Render and only process the request
   */
  bool RenderReady( RenderRequest*& request );

  /**
   * Called by the Render thread to inform the synchronization class that the surface has been replaced.
   *
   * @note Should only be called by the Render thread.
   */
  void RenderInformSurfaceReplaced();

  /////////////////////////////////////////////////////////////////////////////////////////////////
  // Called by the V-Sync Thread
  /////////////////////////////////////////////////////////////////////////////////////////////////

  /**
   * Called by the VSync notifier thread so it can sleep if Update/Render threads are sleeping/paused
   *
   * @param[in] validSync True if the sync was valid (@see VSyncMonitor::DoSync)
   * @param[in] frameNumber The current frame number
   * @param[in] seconds The current time
   * @param[in] microseconds The current time
   * @param[out] numberOfVSyncsPerRender The number of frames per render.
   * @return true if VSync monitoring/notifications should continue.
   *
   * @note Should only be called by the VSync thread.
   */
  bool VSyncReady( bool validSync, unsigned int frameNumber, unsigned int seconds, unsigned int microseconds, unsigned int& numberOfVSyncsPerRender );

private:

  // Undefined copy constructor.
  ThreadSynchronization( const ThreadSynchronization& );

  // Undefined assignment operator.
  ThreadSynchronization& operator=( const ThreadSynchronization& );

  /**
   * Helper to add a performance marker to the performance server (if its active)
   * @param type performance marker type
   */
  void AddPerformanceMarker( PerformanceInterface::MarkerType type );

  struct State
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
  };

  struct Event
  {
    enum Type
    {
      INITIALISE,
      START,
      STOP,
      PAUSE,
      RESUME,
      UPDATE_REQUEST,
      UPDATE_ONCE,
      UPDATE_READY,
      RENDER_READY,
      VSYNC_READY,
      REPLACE_SURFACE,
      SURFACE_REPLACED,

      TOTAL_EVENTS
    };
  };

  struct EventData
  {
    EventData() : newSurface( NULL ) { }
    RenderSurface* newSurface;
  };

  struct UpdateData
  {
    UpdateData()
    : notifyEvent( false ),
      runUpdate( true ),
      lastFrameDeltaSeconds( NULL ),
      lastSyncTimeMilliseconds( NULL ),
      nextSyncTimeMilliseconds( NULL )
    {
    }

    bool notifyEvent;
    bool runUpdate;
    float* lastFrameDeltaSeconds;
    unsigned int* lastSyncTimeMilliseconds;
    unsigned int* nextSyncTimeMilliseconds;
  };

  struct RenderData
  {
    RenderData() : requestPtr( NULL ) { }
    RenderRequest** requestPtr;
  };

  struct VSyncData
  {
    VSyncData()
    : validSync( false ),
      frameNumber( 0 ),
      seconds( 0 ),
      microseconds( 0 ),
      numberOfVSyncsPerRender( NULL )
    {
    }

    bool          validSync;
    unsigned int  frameNumber;
    unsigned int  seconds;
    unsigned int  microseconds;
    unsigned int* numberOfVSyncsPerRender;
  };

  typedef void ( *ProcessEventFunction )( ThreadSynchronization& );

  // Event Processing
  static bool ProcessEvent( ThreadSynchronization& sync, int event );

  // Event Thread
  static void Initialise( ThreadSynchronization& sync );
  static void Start( ThreadSynchronization& sync );
  static void Pause( ThreadSynchronization& sync );
  static void Resume( ThreadSynchronization& sync );
  static void UpdateRequest( ThreadSynchronization& sync );
  static void UpdateOnce( ThreadSynchronization& sync );
  static void Stop( ThreadSynchronization& sync );
  static void ReplaceSurface( ThreadSynchronization& sync );

  // Update Thread
  static void UpdateReadyForFirstFrame( ThreadSynchronization& sync );
  static void UpdateReady( ThreadSynchronization& sync );
  static void UpdateReadyWhileSleeping( ThreadSynchronization& sync );
  static void UpdateTryToSleep( ThreadSynchronization& sync );

  // Render Thread
  static void RenderReadyForFirstFrame( ThreadSynchronization& sync );
  static void RenderReady( ThreadSynchronization& sync );
  static void RenderReadyDuringReplaceSurface( ThreadSynchronization& sync );
  static void RenderReadyAfterSurfaceReplaced( ThreadSynchronization& sync );
  static void RenderInformSurfaceReplaced( ThreadSynchronization& sync );

  // VSync Thread
  static void VSyncReadyForFirstFrame( ThreadSynchronization& sync );
  static void VSyncReady( ThreadSynchronization& sync );

private:
  /// State machine table
  ProcessEventFunction mStateMachine[ State::TOTAL_STATES ][ Event::TOTAL_EVENTS ];

  Mutex mMutex;                                       ///< This mutex must be locked before reading/writing mUpdateReadyCount
  FrameTime mFrameTime;                               ///< Frame timer predicts next vsync time
  TriggerEventInterface& mNotificationTrigger;        ///< Reference to notification event trigger
  PerformanceInterface* mPerformanceInterface;        ///< The performance logging interface
  ReplaceSurfaceRequest mReplaceSurfaceRequest;       ///< Holder for a replace surface request

  ConditionalWait mUpdateThreadWaitCondition;         ///< The wait condition for the update-thread.
  ConditionalWait mRenderThreadWaitCondition;         ///< The wait condition for the render-thread.
  ConditionalWait mVSyncThreadWaitCondition;          ///< The wait condition for the v-sync-thread.
  ConditionalWait mEventThreadWaitCondition;          ///< The wait condition for the event-thread.

  EventData  mEventData;                              ///< Data used by the event-thread.
  UpdateData mUpdateData;                             ///< Data used by the update-thread.
  RenderData mRenderData;                             ///< Data used by the render-thread.
  VSyncData  mVSyncData;                              ///< Data used by the v-sync-thread.

  const int mMaximumUpdateCount;                      ///< How many frames may be prepared, ahead of the rendering.
  unsigned int mNumberOfVSyncsPerRender;              ///< How many frames for each update/render cycle.

  unsigned int mTryToSleepCount;                      ///< Count to ensure we don't go to sleep too early

  volatile int mState;                                ///< The current state of synchronisation.
  volatile int mVSyncAheadOfUpdate;                   ///< The number of frames vsync is ahead of update.
  volatile int mUpdateAheadOfRender;                  ///< The number of frames update is ahead of render.
  volatile int mNumberOfThreadsStarted;               ///< The number of threads that are initialised and running.

  volatile unsigned int mSyncFrameNumber;             ///< Frame number of latest Sync
  volatile unsigned int mSyncSeconds;                 ///< Timestamp (seconds) of latest Sync
  volatile unsigned int mSyncMicroseconds;            ///< Timestamp (microseconds) of latest Sync
}; // class ThreadSynchronization

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_UPDATE_RENDER_SYNCHRONIZATION_H__
