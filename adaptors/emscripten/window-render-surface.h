#ifndef __DALI_INTERNAL_EMSCRIPTEN_WINDOW_RENDER_SURFACE_H__
#define __DALI_INTERNAL_EMSCRIPTEN_WINDOW_RENDER_SURFACE_H__

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
namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace ECore
{

/**
 * @copydoc Dali::Internal::Adaptor::ECore::RenderSurface.
 * Window specialization.
 */
class WindowRenderSurface : public Internal::Adaptor::RenderSurface
{
public:

  /**
    * Uses an X11 surface to render to.
    * @param [in] positionSize the position and size of the surface
    * @param [in] surface can be a X-window or X-pixmap (type must be unsigned int).
    * @param [in] display connection to X-server if the surface is a X window or pixmap (type must be void * to X display struct)
    * @param [in] name optional name of surface passed in
    * @param [in] isTransparent if it is true, surface has 32 bit color depth, otherwise, 24 bit
    */
  WindowRenderSurface( Dali::PositionSize positionSize,
                       Any surface,
                       Any display,
                       const std::string& name,
                       bool isTransparent = false );

  /**
   * @copydoc Dali::Internal::Adaptor::ECore::RenderSurface::~RenderSurface
   */
  virtual ~WindowRenderSurface();

public: // from Dali::RenderSurface

  /**
   * @copydoc Dali::RenderSurface::GetType()
   */
  virtual Dali::RenderSurface::SurfaceType GetType();

  /**
   * @copydoc Dali::RenderSurface::GetSurface()
   */
  virtual Any GetSurface();

public:  // from Internal::Adaptor::RenderSurface

  /**
   * @copydoc Dali::Internal::Adaptor::RenderSurface::InitializeEgl()
   */
  virtual void InitializeEgl( EglInterface& egl );

  /**
   * @copydoc Dali::Internal::Adaptor::RenderSurface::CreateEglSurface()
   */
  virtual void CreateEglSurface( EglInterface& egl );

  /**
   * @copydoc Dali::Internal::Adaptor::RenderSurface::DestroyEglSurface()
   */
  virtual void DestroyEglSurface( EglInterface& egl );

  /**
   * @copydoc Dali::Internal::Adaptor::RenderSurface::ReplaceEGLSurface()
   */
  virtual bool ReplaceEGLSurface( EglInterface& egl );

  /**
   * @copydoc Dali::Internal::Adaptor::RenderSurface::MoveResize()
   */
  virtual void MoveResize( Dali::PositionSize positionSize);

  /**
   * @copydoc Dali::Internal::Adaptor::RenderSurface::Map()
   */
  virtual void Map();

  /**
   * @copydoc Dali::Internal::Adaptor::RenderSurface::PreRender()
   */
  virtual bool PreRender( EglInterface& egl, Integration::GlAbstraction& glAbstraction );

  /**
   * @copydoc Dali::Internal::Adaptor::RenderSurface::PostRender()
   */
  virtual void PostRender( EglInterface& egl, Integration::GlAbstraction& glAbstraction, unsigned int timeDelta, SyncMode syncMode );

  /**
   * @copydoc Dali::Internal::Adaptor::RenderSurface::SetViewMode()
   */
  void SetViewMode( ViewMode viewMode );

public:
#if !defined(EMSCRIPTEN)
  /**
   * @copydoc Dali::RenderSurface::GetDrawable()
   */
  virtual Ecore_X_Windo GetXWindow();
#endif
private: // Data


}; // class WindowRenderSurface

} // namespace ECore

} // namespace Adaptor

} // namespace internal

} // namespace Dali

#endif // __DALI_INTERNAL_EMSCRIPTEN_WINDOW_RENDER_SURFACE_H__
