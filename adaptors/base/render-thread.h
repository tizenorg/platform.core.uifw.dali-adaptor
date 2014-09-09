#ifndef __DALI_INTERNAL_RENDER_THREAD_H__
#define __DALI_INTERNAL_RENDER_THREAD_H__

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

// EXTERNAL INCLUDES
#include <boost/thread.hpp>

// INTERNAL INCLUDES
#include <base/interfaces/egl-interface.h>
#include <render-surface-impl.h> // needed for Dali::Internal::Adaptor::RenderSurface


namespace Dali
{

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
class RenderSurface;
class UpdateRenderSynchronization;
class EglFactoryInterface;
class EnvironmentOptions;


class RenderRequest
{
public:
  enum Request
  {
    CHANGE_SURFACE, // Request to change surface
  };

  /**
   * Constructor.
   * @param[in] type The type of the request
   */
  RenderRequest( Request type );

  /**
   * @return the type of the request
   */
  Request GetType();

private:
  Request mRequestType;
};

class ChangeSurfaceRequest : public RenderRequest
{
public:

  /**
   * Constructor
   * @param[in] newSurface The new surface to use
   */
  ChangeSurfaceRequest(RenderSurface* newSurface);

  /**
   * @return the new surface
   */
  RenderSurface* GetSurface();

  /**
   * Called when the request has been completed to set the result.
   */
  void ReplaceCompleted();

  /**
   * @return true if the replace has completed.
   */
  bool GetReplaceCompleted();

private:
  RenderSurface* mNewSurface; ///< The new surface to use.
  bool mReplaceCompleted;      ///< Set to true when the replace has completed.
};


/**
 * The render-thread is responsible for calling Core::Render() after each update.
 */
class RenderThread
{
public:

  /**
   * Create the render-thread; this will not do anything until Start() is called.
   * @param[in] sync update-render synchronization object
   * @param[in] adaptorInterfaces base adaptor interface
   * @param[in] environmentOptions environment options

   */
  RenderThread( UpdateRenderSynchronization& sync,
                AdaptorInternalServices& adaptorInterfaces,
                const EnvironmentOptions& environmentOptions );

  /**
   * Virtual Destructor
   */
  virtual ~RenderThread();

public:

  /**
   * Starts the render-thread
   */
  void Start();

  /**
   * Stops the render-thread
   */
  void Stop();

  /**
   * Offscreen was posted to onscreen
   */
  void RenderSync();

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
   * Check if display has events
   * Called from render thread
   */
  void ConsumeEvents();

  /**
   * Check if main thread made any requests, e.g. ReplaceSurface
   * Called from render thread
   * @return True if there was a request and it was actioned, false otherwise.
   */
  void ProcessRequest(RenderRequest* request);

  /**
   * Changes the rendering surface
   * Used for replacing pixmaps due to resizing
   * Called from render thread
   * @param newSurface to use
   */
  void ChangeSurface( RenderSurface* newSurface );

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
   * @param[in] timeDelta Time since PostRender was last called in microseconds
   */
  void PostRender( unsigned int timeDelta );

private: // Data

  UpdateRenderSynchronization&  mUpdateRenderSync;       ///< Used to synchronize the update & render threads
  Dali::Integration::Core&      mCore;                   ///< Dali core reference
  Integration::GlAbstraction&   mGLES;                   ///< GL abstraction reference
  EglFactoryInterface*          mEglFactory;             ///< Factory class to create EGL implementation
  EglInterface*                 mEGL;                    ///< Interface to EGL implementation
  boost::thread*                mThread;                 ///< render thread
  bool                          mUsingPixmap;            ///< whether we're using a pixmap or a window
  RenderSurface*                mSurface;                ///< Current surface
  const EnvironmentOptions&     mEnvironmentOptions;     ///< Environment options
  bool                          mSurfaceReplaced;        ///< True when new surface has been initialzed.
};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_RENDER_THREAD_H__
