#ifndef __DALI_INTERNAL_UPDATE_RENDER_SYNCHRONIZATION_H__
#define __DALI_INTERNAL_UPDATE_RENDER_SYNCHRONIZATION_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <base/interfaces/performance-interface.h>
#include <base/frame-time.h>

// EXTERNAL INCLUDES
#include <stdint.h>
#include <boost/thread.hpp>
#include <boost/thread/condition_variable.hpp>
#include <stdint.h>

// INTERNAL INCLUDES
#include <base/interfaces/performance-interface.h>
#include <base/interfaces/adaptor-internal-services.h>

namespace Dali
{

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
 */
class UpdateRenderSynchronization
{
public:

  /**
   * Create an update/render synchronization object.
   * @param[in] adaptorInterfaces base adaptor interface
  */
  UpdateRenderSynchronization( AdaptorInternalServices& adaptorInterfaces );

  /**
   * Non virtual destructor. Not inteded as base class.
   */
  ~UpdateRenderSynchronization();

  /**
   * Start the threads
   */
  void Start();

  /**
   * Stop the threads
   */
  void Stop();

  /**
   * Pause the controller (and threads)
   */
  void Pause();

  /**
   * Resume the controller (and threads)
   */
  void Resume();

  /**
   * Resume the frame time predictor
   */
  void ResumeFrameTime();
  void AltSetRunning(bool running);
  void SetRenderRunning(bool running);

  /**
   * Wake update thread if sleeping. If the update thread is not sleeping
   * this becomes a noop.
   * Called when an update is requested by Core.
   * i.e. when a batch of messages have been queued for the next update.
   */
  void UpdateRequested();

  /**
   * Update once (even if paused)
   */
  void UpdateWhilePaused();

  /**
   * Called by Update thread before it runs the update. This is the point where we can pause
   */
  void UpdateReadyToRun();

  /**
   * Called after an update has completed, to inform render-thread a buffer is ready to render.
   * The function also waits for a free buffer to become available before returning.
   * @pre Called by update thread only.
   * @param[out] renderNeedsUpdate Whether the render task requires another update.
   * @return True if updating should continue, false if the update-thread should quit.
   */
  bool UpdateSyncWithRender( bool& renderNeedsUpdate );

  /**
   * Called by update thread to wait for all rendering to finish.
   * Used by update to check the status of the final render before pausing.
   * @pre Called by update thread only.
   */
  void UpdateWaitForAllRenderingToFinish();

  /**
   * Try block the update-thread when there's nothing to update.
   * @return True if updating should continue, false if the update-thread should quit.
   */
  bool UpdateTryToSleep();

  /**
   * Called by the render thread after it renders a frame.
   * Used to notify the update-thread that a frame has been rendered.
   * @pre Called by render thread only.
   * @param updateRequired Whether a further update is required.
   */
  void RenderFinished( bool updateRequired );

  /**
   * Called by the render-thread to wait for a buffer to read from and then render.
   * @pre Called by render thread only.
   * @return True if rendering should continue, false if the render-thread should quit.
   */
  bool RenderSyncWithUpdate();

  /**
   * Called by the render/update threads to wait for a VSync.
   */
  void WaitVSync();

  /**
   * Called by the VSync notifier thread so it can sleep if Update/Render threads are sleeping/paused
   * @return true if VSync monitoring/notifications should continue.
   */
  bool VSyncNotifierSyncWithUpdateAndRender( unsigned int frameNumber, unsigned int seconds, unsigned int microseconds );

  /**
   * Retrieves the last VSync frame number
   * @return The VSync frame number.
   */
  unsigned int GetFrameNumber() const;

  /**
   * Retrieves the time (in microseconds) at the last VSync
   * @return The VSync timestamp in microseconds.
   */
  uint64_t GetTimeMicroseconds();

  /**
   * Retrieves the current state of the threads, as in running or not.
   * @return True if the threads are set to be running.
   */
  int GetRunning() const
  {
    return mRunning;
  }

  int GetRenderRunning() const
  {
    return mRenderRunning;
  }

  /**
   * Helper to add a performance marker to the performance server (if its active)
   * @param type performance marker type
   */
  void AddPerformanceMarker( PerformanceMarker::MarkerType type );

private:

  // Undefined copy constructor.
  UpdateRenderSynchronization( const UpdateRenderSynchronization& );

  // Undefined assignment operator.
  UpdateRenderSynchronization& operator=( const UpdateRenderSynchronization& );

private:

  const unsigned int mMaximumUpdateCount;             ///< How many frames may be prepared, ahead of the rendering.
  volatile unsigned int mUpdateReadyCount;            ///< Incremented after each update, decremented after each render (protected by mMutex)
  // ARM CPUs perform aligned 32 bit read/writes atomically, so the following variables do not require mutex protection on modification
  volatile int mRunning;                              ///< Used during UpdateThread::Stop() to exit the update & render loops
  volatile int mRenderRunning;                        ///< Used during RenderThread::Stop to exit the render loop
  volatile int mUpdateRequired;                       ///< Used to inform the update thread, that render requires another update
  volatile int mPaused;                               ///< The paused flag
  volatile int mUpdateRequested;                      ///< An update has been requested
  volatile int mAllowUpdateWhilePaused;               ///< whether to allow (one) update while paused
  volatile int mVSyncSleep;                           ///< Set true when the VSync thread should sleep
  volatile unsigned int mVSyncFrameNumber;            ///< Frame number of latest VSync
  volatile unsigned int mVSyncSeconds;                ///< Timestamp (seconds) of latest VSync
  volatile unsigned int mVSyncMicroseconds;           ///< Timestamp (microseconds) of latest VSync

  boost::mutex mMutex;                                ///< This mutex must be locked before reading/writing mUpdateReadyCount
  boost::condition_variable mUpdateFinishedCondition; ///< The render thread waits for this condition
  boost::condition_variable mUpdateSleepCondition;    ///< The update thread waits for this condition when sleeping
  boost::condition_variable mRenderFinishedCondition; ///< The update thread waits for this condition
  boost::condition_variable mVSyncReceivedCondition;  ///< The render thread waits on this condition
  boost::condition_variable mVSyncSleepCondition;     ///< The vsync thread waits for this condition
  boost::condition_variable mPausedCondition;         ///< The controller waits for this condition while paused

  Dali::Integration::Core&            mCore;                ///< Dali core reference
  PerformanceInterface*               mPerformanceInterface;///< The performance logging interface

  bool mSkipNextVSync; ///< Is set on resumption so that we start an update straight away rather than wait for the Vsync

}; // class UpdateRenderSynchronization

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_UPDATE_RENDER_SYNCHRONIZATION_H__
