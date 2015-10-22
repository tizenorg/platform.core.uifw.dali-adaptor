#ifndef __DALI_INTERNAL_UPDATE_RENDER_THREAD_H__
#define __DALI_INTERNAL_UPDATE_RENDER_THREAD_H__

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
#include <pthread.h>

// INTERNAL INCLUDES
#include <base/fps-tracker.h>
#include <base/render-request.h>
#include <base/update-status-logger.h>
#include <egl-interface.h>
#include <render-surface.h> // needed for Dali::RenderSurface

namespace Dali
{

class RenderSurface;
class DisplayConnection;

namespace Integration
{
class GlAbstraction;
class Core;
}

namespace Internal
{
namespace Adaptor
{

class AdaptorInternalServices;
class BasicThreadSynchronization;
class EglFactoryInterface;
class EnvironmentOptions;

/**
 * The render-thread is responsible for calling Core::Render() after each update.
 */
class UpdateRenderThread
{
public:

  /**
   * Create the render-thread; this will not do anything until Start() is called.
   * @param[in] sync thread synchronization object
   * @param[in] adaptorInterfaces base adaptor interface
   * @param[in] environmentOptions environment options
   */
  UpdateRenderThread( BasicThreadSynchronization& sync,
                      AdaptorInternalServices& adaptorInterfaces,
                      const EnvironmentOptions& environmentOptions );

  /**
   * Destructor
   */
  ~UpdateRenderThread();

  /**
   * Starts the render-thread
   */
  void Start();

  /**
   * Stops the render-thread
   */
  void Stop();

private: // Render thread side helpers

  /**
   * This method is used by the Render thread for rendering the Core to the screen.
   * Called from render thread
   * @return true, if the thread finishes properly.
   */
  bool Run();

  /**
   * Initializes EGL.
   * Called from render thread
   */
  void InitializeEgl();

  /**
   * Check if main thread made any requests, e.g. ReplaceSurface
   * Called from render thread
   */
  void ProcessRequest( RenderRequest* request );

  /**
   * Replaces the rendering surface
   * Used for replacing pixmaps due to resizing
   * Called from render thread
   * @param newSurface to use
   */
  void ReplaceSurface( RenderSurface* newSurface );

  /**
   * Shuts down EGL.
   * Called from render thread
   */
  void ShutdownEgl();

  /**
   * Called before core renders the scene
   * Called from render thread
   * @return true if successful and Core::Render should be called.
   */
  bool PreRender();

  /**
   * Called after core has rendered the scene
   * Called from render thread
   */
  void PostRender();

  /**
   * Helper for the thread calling the entry function.
   * @param[in] This A pointer to the current UpdateRenderThread object
   */
  static inline void* InternalThreadEntryFunc( void* This )
  {
    ( static_cast<UpdateRenderThread*>( This ) )->Run();
    return NULL;
  }

private:

  // Undefined
  UpdateRenderThread( const UpdateRenderThread& UpdateRenderThread );

  // Undefined
  UpdateRenderThread& operator=( const UpdateRenderThread& UpdateRenderThread );

private: // Data

  BasicThreadSynchronization&   mThreadSynchronization;  ///< Used to synchronize the all threads
  Dali::Integration::Core&      mCore;                   ///< Dali core reference

  FpsTracker                    mFpsTracker;             ///< Object that tracks the FPS
  UpdateStatusLogger            mUpdateStatusLogger;     ///< Object that logs the update-status as required.

  Integration::GlAbstraction&   mGLES;                   ///< GL abstraction reference
  EglFactoryInterface*          mEglFactory;             ///< Factory class to create EGL implementation
  EglInterface*                 mEGL;                    ///< Interface to EGL implementation
  pthread_t*                    mThread;                 ///< render thread
  RenderSurface*                mSurface;                ///< Current surface
  Dali::DisplayConnection*      mDisplayConnection;      ///< Display connection
  const EnvironmentOptions&     mEnvironmentOptions;     ///< Environment options
  bool                          mSurfaceReplaced;        ///< True when new surface has been initialzed.
};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_UPDATE_RENDER_THREAD_H__
