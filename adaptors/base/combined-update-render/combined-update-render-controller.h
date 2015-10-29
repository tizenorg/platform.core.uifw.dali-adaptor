#ifndef __DALI_INTERNAL_COMBINED_UPDATE_RENDER_CONTROLLER_H__
#define __DALI_INTERNAL_COMBINED_UPDATE_RENDER_CONTROLLER_H__

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
#include <semaphore.h>
#include <stdint.h>
#include <dali/integration-api/core.h>
#include <dali/devel-api/threading/conditional-wait.h>

// INTERNAL INCLUDES
#include <integration-api/thread-synchronization-interface.h>
#include <base/interfaces/performance-interface.h>
#include <base/fps-tracker.h>
#include <base/render-helper.h>
#include <base/thread-controller-interface.h>
#include <base/update-status-logger.h>

namespace Dali
{

class RenderSurface;
class TriggerEventInterface;

namespace Internal
{

namespace Adaptor
{

class AdaptorInternalServices;
class EnvironmentOptions;

/**
 * @brief Like the Multi-Thread Controller, but the Update & Render happen on the same thread.
 *
 * Key Points:
 *  1. Two Threads:
 *    a. Main/Event Thread
 *    b. Update/Render Thread
 *  2. There is NO VSync thread:
 *    a. We retrieve the time before Update.
 *    b. Then retrieve the time after Render.
 *    c. We calculate the difference between these two times and if:
 *      i.  The difference is less than the default frame time, we sleep.
 *      ii. If it’s more or the same, then we continue.
 *  3. On the update/render thread, if we discover that we do not need to do any more updates, we use a trigger-event
 *     to inform the main/event thread. This is then processed as soon as the event thread is able to do so where it
 *     is easier to make a decision about whether we should stop the update/render thread or not (depending on any
 *     update requests etc.).
 *  4. The main thread is still blocked while the surface is being replaced.
 */
class CombinedUpdateRenderController : public ThreadControllerInterface,
                                       public ThreadSynchronizationInterface
{
public:

  /**
   * Constructor
   */
  CombinedUpdateRenderController( AdaptorInternalServices& adaptorInterfaces, const EnvironmentOptions& environmentOptions );

  /**
   * Non virtual destructor. Not intended as base class.
   */
  ~CombinedUpdateRenderController();

  /**
   * @copydoc ThreadControllerInterface::Initialize()
   */
  void Initialize();

  /**
   * @copydoc ThreadControllerInterface::Start()
   */
  void Start();

  /**
   * @copydoc ThreadControllerInterface::Pause()
   */
  void Pause();

  /**
   * @copydoc ThreadControllerInterface::Resume()
   */
  void Resume();

  /**
   * @copydoc ThreadControllerInterface::Stop()
   */
  void Stop();

  /**
   * @copydoc ThreadControllerInterface::RequestUpdate()
   */
  void RequestUpdate();

  /**
   * @copydoc ThreadControllerInterface::RequestUpdateOnce()
   */
  void RequestUpdateOnce();

  /**
   * @copydoc ThreadControllerInterface::ReplaceSurface()
   */
  void ReplaceSurface( RenderSurface* surface );

  /**
   * @copydoc ThreadControllerInterface::SetRenderRefreshRate()
   */
  void SetRenderRefreshRate( unsigned int numberOfFramesPerRender );

private:

  // Undefined copy constructor.
  CombinedUpdateRenderController( const CombinedUpdateRenderController& );

  // Undefined assignment operator.
  CombinedUpdateRenderController& operator=( const CombinedUpdateRenderController& );

  /////////////////////////////////////////////////////////////////////////////////////////////////
  // EventThread
  /////////////////////////////////////////////////////////////////////////////////////////////////

  /**
   * Runs the Update/Render Thread.
   * This will lock the mutex in mUpdateRenderThreadWaitCondition.
   *
   * @param[in]  numberOfCycles  The number of times the update/render cycle should run. If -1, then it will run continuously.
   */
  inline void RunUpdateRenderThread( int numberOfCycles );

  /**
   * Pauses the Update/Render Thread.
   * This will lock the mutex in mUpdateRenderThreadWaitCondition.
   */
  inline void PauseUpdateRenderThread();

  /**
   * Stops the Update/Render Thread.
   * This will lock the mutex in mUpdateRenderThreadWaitCondition.
   *
   * @note Should only be called in Stop as calling this will kill the update-thread.
   */
  inline void StopUpdateRenderThread();

  /**
   * Checks if the the Update/Render Thread is paused.
   * This will lock the mutex in mUpdateRenderThreadWaitCondition.
   *
   * @return true if paused, false otherwise
   */
  inline bool IsUpdateRenderThreadPaused();

  /**
   * Used as the callback for the sleep-trigger.
   *
   * Will sleep when enough requests are made without any requests.
   */
  void ProcessSleepRequest();

  /////////////////////////////////////////////////////////////////////////////////////////////////
  // UpdateRenderThread
  /////////////////////////////////////////////////////////////////////////////////////////////////

  /**
   * The Update/Render thread loop. This thread will be destroyed on exit from this function.
   */
  void UpdateRenderThread();

  /**
   * Called by the Update/Render Thread which ensures a wait if required.
   *
   * @return false, if the thread should stop.
   */
  bool UpdateRenderReady();

  /**
   * Checks to see if the surface needs to be replaced.
   * This will lock the mutex in mUpdateRenderThreadWaitCondition.
   *
   * @return Pointer to the new surface, NULL otherwise
   */
  RenderSurface* ShouldSurfaceBeReplaced();

  /**
   * Called by the Update/Render thread after a surface has been replaced.
   *
   * This will lock the mutex in mEventThreadWaitCondition
   */
  void SurfaceReplaced();

  /**
   * Helper for the thread calling the entry function
   * @param[in] This A pointer to the current object
   */
  static void* InternalUpdateRenderThreadEntryFunc( void* This )
  {
    ( static_cast<CombinedUpdateRenderController*>( This ) )->UpdateRenderThread();
    return NULL;
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////
  // ALL Threads
  /////////////////////////////////////////////////////////////////////////////////////////////////

  /**
   * Called by the update-render & v-sync threads when they up and running.
   *
   * This will lock the mutex in mEventThreadWaitCondition.
   */
  void NotifyThreadInitialised();

  /**
   * Helper to add a performance marker to the performance server (if it's active)
   * @param[in]  type  performance marker type
   */
  void AddPerformanceMarker( PerformanceInterface::MarkerType type );

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
  //// Called by the Render Thread if post-rendering is required
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

  FpsTracker                        mFpsTracker;                       ///< Object that tracks the FPS
  UpdateStatusLogger                mUpdateStatusLogger;               ///< Object that logs the update-status as required.

  RenderHelper                      mRenderHelper;                     ///< Helper class for EGL, pre & post rendering

  sem_t                             mEventThreadSemaphore;             ///< Used by the event thread to ensure all threads have been initialised, and when replacing the surface.

  ConditionalWait                   mUpdateRenderThreadWaitCondition;  ///< The wait condition for the update-render-thread.

  AdaptorInternalServices&          mAdaptorInterfaces;                ///< The adaptor internal interface
  PerformanceInterface*             mPerformanceInterface;             ///< The performance logging interface
  Integration::Core&                mCore;                             ///< Dali core reference
  const EnvironmentOptions&         mEnvironmentOptions;               ///< Environment options
  TriggerEventInterface&            mNotificationTrigger;              ///< Reference to notification event trigger
  TriggerEventInterface*            mSleepTrigger;                     ///< Used by the update-render thread to trigger the event thread when it no longer needs to do any updates

  pthread_t*                        mUpdateRenderThread;               ///< The Update/Render thread.

  float                             mDefaultFrameDelta;                ///< Default time delta between each frame (used for animations). Not protected by lock, but written to rarely so not worth adding a lock when reading.
  unsigned int                      mDefaultFrameDurationMilliseconds; ///< Default duration of a frame (used for predicting the time of the next frame). Not protected by lock, but written to rarely so not worth adding a lock when reading.
  unsigned int                      mDefaultFrameDurationNanoseconds;  ///< Default duration of a frame (used for sleeping if not enough time elapsed). Not protected by lock, but written to rarely so not worth adding a lock when reading.

  unsigned int                      mTryToSleepCount;                  ///< Count to ensure we don't go to sleep too early.
  unsigned int                      mRunning;                          ///< Read and set on the event-thread only to state whether we are running.

  //
  // NOTE: cannot use booleans as these are used from multiple threads, must use variable with machine word size for atomic read/write
  //

  volatile int                      mUpdateRenderRunCount;             ///< The number of times Update/Render cycle should run. If -1, then will run continuously (set by the event-thread, read by v-sync-thread).
  volatile unsigned int             mDestroyUpdateRenderThread;        ///< Whether the Update/Render thread be destroyed (set by the event-thread, read by the update-render-thread).

  RenderSurface* volatile           mNewSurface;                       ///< Will be set to the new-surface if requested (set by the event-thread, read & cleared by the update-render thread).

  volatile unsigned int             mPostRendering;                    ///< Whether post-rendering is taking place (set by the event & render threads, read by the render-thread).
};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_COMBINED_UPDATE_RENDER_CONTROLLER_H__
