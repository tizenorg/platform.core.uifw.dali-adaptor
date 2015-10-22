#ifndef __DALI_INTERNAL_RENDER_HELPER_H__
#define __DALI_INTERNAL_RENDER_HELPER_H__

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
#include <base/render-request.h>
#include <egl-interface.h>
#include <render-surface.h> // needed for Dali::RenderSurface

namespace Dali
{

class RenderSurface;
class DisplayConnection;

namespace Integration
{
class GlAbstraction;
}

namespace Internal
{
namespace Adaptor
{

class AdaptorInternalServices;
class EglFactoryInterface;

/**
 *
 */
class RenderHelper
{
public:

  /**
   *
   * @param[in] adaptorInterfaces base adaptor interface
   */
  RenderHelper( AdaptorInternalServices& adaptorInterfaces );

  /**
   * Destructor
   */
  ~RenderHelper();

  /////////////////////////////////////////////////////////////////////////////////////////////////
  // Called on the Event Thread
  /////////////////////////////////////////////////////////////////////////////////////////////////

  /**
   * Starts the render-thread
   */
  void Start();

  /**
   * Stops the render-thread
   */
  void Stop();

  /////////////////////////////////////////////////////////////////////////////////////////////////
  // Called on the Rendering Thread
  /////////////////////////////////////////////////////////////////////////////////////////////////

  /**
   * Consumes any pending events to avoid memory leaks
   */
  void ConsumeEvents();

  /**
   * Initializes EGL.
   * Called from render thread
   */
  void InitializeEgl();

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

private:

  // Undefined
  RenderHelper( const RenderHelper& RenderHelper );

  // Undefined
  RenderHelper& operator=( const RenderHelper& RenderHelper );

private: // Data

  Integration::GlAbstraction&   mGLES;                   ///< GL abstraction reference
  EglFactoryInterface*          mEglFactory;             ///< Factory class to create EGL implementation
  EglInterface*                 mEGL;                    ///< Interface to EGL implementation
  RenderSurface*                mSurface;                ///< Current surface
  Dali::DisplayConnection*      mDisplayConnection;      ///< Display connection
  bool                          mSurfaceReplaced;        ///< True when new surface has been initialzed.
};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_RENDER_HELPER_H__
