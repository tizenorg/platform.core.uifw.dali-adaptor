#ifndef __DALI_INTERNAL_BASIC_THREAD_SYNCHRONIZATION_H__
#define __DALI_INTERNAL_BASIC_THREAD_SYNCHRONIZATION_H__

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
#include <dali/devel-api/common/conditional-wait.h>

// INTERNAL INCLUDES
#include <base/thread-synchronization-base.h>
#include <base/render-thread.h>

namespace Dali
{

class RenderSurface;

namespace Internal
{

namespace Adaptor
{

class AdaptorInternalServices;

/**
 * This object is used to synchronize the event, a combined update-render and vsync threads.
 * This assumes that the Update and Render functions are called from the combined update-render thread.
 */
class BasicThreadSynchronization : public ThreadSynchronizationBase
{
public:

  /**
   * Create an update/render synchronization object.
   * @param[in] adaptorInterfaces base adaptor interface
   * @param[in] numberOfVSyncsPerRender The number of frames per render
  */
  BasicThreadSynchronization( AdaptorInternalServices& adaptorInterfaces, unsigned int numberOfVSyncsPerRender );

  /**
   * Non virtual destructor. Not intended as base class.
   */
  ~BasicThreadSynchronization();

  /////////////////////////////////////////////////////////////////////////////////////////////////
  // Called by the Event Thread
  /////////////////////////////////////////////////////////////////////////////////////////////////

  /**
   * @copydoc ThreadSynchronization::Initialise
   */
  void Initialise();

  /**
   * @copydoc ThreadSynchronization::Start
   */
  void Start();

  /**
   * @copydoc ThreadSynchronization::Stop
   */
  void Stop();

  /**
   * @copydoc ThreadSynchronization::Pause
   */
  void Pause();

  /**
   * @copydoc ThreadSynchronization::Resume
   */
  void Resume();

  /**
   * @copydoc ThreadSynchronization::UpdateRequest
   */
  void UpdateRequest();

  /**
   * @copydoc ThreadSynchronization::UpdateOnce
   */
  void UpdateOnce();

  /**
   * @copydoc ThreadSynchronization::ReplaceSurface
   */
  void ReplaceSurface( RenderSurface* newSurface );

  /**
   * @copydoc ThreadSynchronization::SetRenderRefreshRate
   */
  void SetRenderRefreshRate( unsigned int numberOfVSyncsPerRender );

  /////////////////////////////////////////////////////////////////////////////////////////////////
  // Called by the UpdateRender Thread
  /////////////////////////////////////////////////////////////////////////////////////////////////

  /**
   * Called by the combined Update & Render thread when it is ready to run the update and render.
   *
   * @param[in] notifyEvent Whether the event thread should be woken up.
   * @param[in] runUpdate Whether to run another update. If false, then the update-thread will attempt to sleep.
   * @param[out] lastFrameDeltaSeconds The delta, in seconds (with float precision), between the last two renders.
   * @param[out] lastSyncTimeMilliseconds The time, in milliseconds, of the last Sync.
   * @param[out] nextSyncTimeMilliseconds The estimated time, in milliseconds, at the next Sync.
   * @param[in] request Pointer to set if there are any requests. This does not need to be freed by the caller.
   * @return true if updating & rendering should continue, false if the update-render-thread should quit.
   *
   * @note Should only be called by the Update thread.
   */
  bool UpdateRenderReady( bool notifyEvent, bool runUpdate, float& lastFrameDeltaSeconds, unsigned int& lastSyncTimeMilliseconds, unsigned int& nextSyncTimeMilliseconds, RenderRequest*& requestPtr );

  /**
   * Called by the Render thread to inform the synchronization class that the surface has been replaced.
   *
   * @note Should only be called by the Render thread.
   */
  void UpdateRenderInformSurfaceReplaced();

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
   * @note The first call to this method should be BEFORE the actual VSync so a thread-sync point can be established (and startup time is not delayed).
   */
  bool VSyncReady( bool validSync, unsigned int frameNumber, unsigned int seconds, unsigned int microseconds, unsigned int& numberOfVSyncsPerRender );

  /////////////////////////////////////////////////////////////////////////////////////////////////
  // POST RENDERING
  /////////////////////////////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////////////////////////////
  //// Called by the Event Thread if post-rendering is required
  /////////////////////////////////////////////////////////////////////////////////////////////////

  /**
   * @copydoc ThreadSynchronizationInterface::PostRenderComplete()
   */
  void PostRenderComplete();

  /////////////////////////////////////////////////////////////////////////////////////////////////
  //// Called by the UpdateRender Thread if post-rendering is required
  /////////////////////////////////////////////////////////////////////////////////////////////////

  /**
   * @copydoc ThreadSynchronizationInterface::PostRenderStarted()
   */
  void PostRenderStarted();

  /**
   * @copydoc ThreadSynchronizationInterface::PostRenderStarted()
   */
  void PostRenderWaitForCompletion();

private:

  // Undefined copy constructor.
  BasicThreadSynchronization( const BasicThreadSynchronization& );

  // Undefined assignment operator.
  BasicThreadSynchronization& operator=( const BasicThreadSynchronization& );

  /////////////////////////////////////////////////////////////////////////////////////////////////
  // Called by ALL Threads
  /////////////////////////////////////////////////////////////////////////////////////////////////

  /**
   * Called by the update, render & v-sync thread when they up and running.
   * This will lock the mutex in mEventThreadWaitCondition.
   */
  inline void NotifyThreadInitialised();

  /////////////////////////////////////////////////////////////////////////////////////////////////
  // Called by Update Thread
  /////////////////////////////////////////////////////////////////////////////////////////////////

  /**
   * Called by the update-thread when the state is State::INITIALISING.
   * Calls methods that lock and locks itself so should NOT be called while a scoped-lock is held.
   */
  void UpdateInitialising();

  /**
   * Called by the update thread to attempt to sleep.
   * @param[in] runUpdate Whether to run another update. If false, then the update-thread will attempt to sleep.
   */
  void UpdateTryToSleep( bool runUpdate );

  /**
   * Called by the update thread to check if we're just resuming.
   * This will lock the mutex in mUpdateRenderThreadWaitCondition.
   */
  inline bool IsUpdateRenderThreadResuming();

  /**
   * Called by the update thread to check if the update thread should be running.
   * This will lock the mutex in mUpdateRenderThreadWaitCondition.
   *
   * @return True if we're stopping, false otherwise.
   */
  inline bool IsUpdateRenderThreadStopping();

  /**
   * Called by the update-render thread to check if we are supposed to be replacing the surface.
   * This will lock the mutex in mUpdateRenderThreadWaitCondition.
   *
   * @return True if we're replacing the surface, false otherwise.
   */
  inline bool IsUpdateRenderReplacingSurface();

  /**
   * Runs the V-Sync Thread.
   * This will lock the mutex in mVSyncThreadWaitCondition.
   */
  inline void RunVSyncThread();

  /**
   * Pauses the V-Sync Thread.
   * This will lock the mutex in mVSyncThreadWaitCondition.
   */
  inline void PauseVSyncThread();

  /**
   * Stops the V-Sync Thread.
   * This will lock the mutex in mVSyncThreadWaitCondition.
   */
  inline void StopVSyncThread();

  /////////////////////////////////////////////////////////////////////////////////////////////////
  // Called by V-Sync Thread
  /////////////////////////////////////////////////////////////////////////////////////////////////

  /**
   * Checks if the V-Sync thread should be running.
   * This will lock the mutex in mVSyncThreadWaitCondition.
   *
   * @return true if running, false otherwise.
   */
  inline bool IsVSyncThreadRunning();

private:

  struct State
  {
    enum Type
    {
      STOPPED,
      INITIALISING,
      RUNNING,
      PAUSED,
      SLEEPING,
      REPLACING_SURFACE
    };
  };

  ReplaceSurfaceRequest mReplaceSurfaceRequest;       ///< Holder for a replace surface request

  ConditionalWait mUpdateRenderThreadWaitCondition;   ///< The wait condition for the update-render-thread.
  ConditionalWait mVSyncThreadWaitCondition;          ///< The wait condition for the v-sync-thread.
  ConditionalWait mEventThreadWaitCondition;          ///< The wait condition for the event-thread.

  unsigned int mNumberOfVSyncsPerRender;              ///< How many frames for each update/render cycle.

  unsigned int mTryToSleepCount;                      ///< Count to ensure we don't go to sleep too early

  volatile State::Type mState;                        ///< The current state of synchronisation (set & read by both the event & update threads).

  volatile int mVSyncAheadOfUpdate;                   ///< The number of frames vsync is ahead of update (set & read by both the v-sync & update threads).
  volatile int mNumberOfThreadsStarted;               ///< The number of threads that are initialised and running (set by update, v-sync & render threads, read by event-thread).

  //
  // NOTE: cannot use booleans as these are used from multiple threads, must use variable with machine word size for atomic read/write
  //

  volatile unsigned int mUpdateRenderThreadResuming;  ///< Whether the update-thread is resuming.

  volatile unsigned int mVSyncThreadRunning;          ///< Whether the V-Sync thread is running (set by the update-thread, read by v-sync-thread).
  volatile unsigned int mVSyncThreadStop;             ///< Whether the V-Sync thread should be stopped (set by the update-thread, read by the v-sync-thread).

  volatile unsigned int mRenderThreadPostRendering;   ///< Whether post-rendering is taking place (set by the event & render threads, read by the render-thread).

  volatile unsigned int mEventThreadSurfaceReplaced;  ///< Checked by the event-thread & set by the render-thread when the surface has been replaced (set by the event & render threads, read by the event-thread).

  unsigned int mVSyncThreadInitialised;               ///< Whether the V-Sync thread has been initialised (only used by v-sync-thread).
  unsigned int mRenderThreadSurfaceReplaced;          ///< Whether the render-thread has replaced the surface (only used by render-thread).
};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_BASIC_THREAD_SYNCHRONIZATION_H__
