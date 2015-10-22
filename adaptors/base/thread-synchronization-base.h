#ifndef __DALI_INTERNAL_THREAD_SYNCHRONIZATION_BASE_H__
#define __DALI_INTERNAL_THREAD_SYNCHRONIZATION_BASE_H__

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

// INTERNAL INCLUDES
#include <integration-api/thread-synchronization-interface.h>
#include <base/interfaces/performance-interface.h>
#include <base/interfaces/performance-interface.h>
#include <trigger-event-interface.h>
#include <base/frame-time.h>


namespace Dali
{

class RenderSurface;

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
class ThreadSynchronizationBase : public Dali::ThreadSynchronizationInterface
{
public:

  /**
   * Virtual destructor.
   */
  virtual ~ThreadSynchronizationBase();

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
  virtual void Initialise() = 0;

  /**
   * Starts running all threads. This waits until all threads are up and running.
   *
   * @pre A call to Initialise has been made.
   *
   * @note Should only be called by the Event Thread.
   */
  virtual void Start() = 0;

  /**
   * Stop the threads.
   *
   * @note Should only be called by the Event Thread.
   */
  virtual void Stop() = 0;

  /**
   * Pause the controller (and threads).
   *
   * @note Should only be called by the Event Thread.
   */
  virtual void Pause() = 0;

  /**
   * Resume the controller (and threads).
   *
   * @note Should only be called by the Event Thread.
   */
  virtual void Resume() = 0;

  /**
   * Wake update thread if sleeping. If the update thread is not sleeping
   * this becomes a no-op.
   * Called when an update is requested by Core.
   * i.e. when a batch of messages have been queued for the next update.
   *
   * @note Should only be called by the Event Thread.
   */
  virtual void UpdateRequest() = 0;

  /**
   * Update once (even if paused)
   *
   * @note Should only be called by the Event Thread.
   */
  virtual void UpdateOnce() = 0;

  /**
   * Inform the render thread that there is a new surface, and that
   * it should replace the current surface.
   *
   * @param[in] newSurface The new surface for rendering.
   *
   * @note Should only be called by the Event Thread.
   */
  virtual void ReplaceSurface( RenderSurface* newSurface ) = 0;

  /**
   * Set the refresh rate for rendering
   *
   * @param[in] numberOfVSyncsPerRender The number of vsync frames per render
   *
   * @note Should only be called by the Event Thread.
   */
  virtual void SetRenderRefreshRate( unsigned int numberOfVSyncsPerRender ) = 0;

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
  virtual bool VSyncReady( bool validSync, unsigned int frameNumber, unsigned int seconds, unsigned int microseconds, unsigned int& numberOfVSyncsPerRender ) = 0;

  /////////////////////////////////////////////////////////////////////////////////////////////////
  // Called by ALL Threads
  /////////////////////////////////////////////////////////////////////////////////////////////////

  /**
   * Helper to add a performance marker to the performance server (if it's active)
   * @param type performance marker type
   */
  void AddPerformanceMarker( PerformanceInterface::MarkerType type );

protected:

  /**
   * Create the synchronization object.
   */
  ThreadSynchronizationBase( AdaptorInternalServices& adaptorInterfaces );

protected:

  FrameTime mFrameTime;                               ///< Frame timer predicts next vsync time
  TriggerEventInterface& mNotificationTrigger;        ///< Reference to notification event trigger
  PerformanceInterface* mPerformanceInterface;        ///< The performance logging interface

}; // class ThreadSynchronization

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_THREAD_SYNCHRONIZATION_BASE_H__
