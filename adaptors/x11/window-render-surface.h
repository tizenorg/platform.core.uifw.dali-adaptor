#ifndef __DALI_INTERNAL_ECORE_X_WINDOW_RENDER_SURFACE_H__
#define __DALI_INTERNAL_ECORE_X_WINDOW_RENDER_SURFACE_H__

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
#include <ecore-x-render-surface.h>

namespace Dali
{

namespace ECore
{

/**
 * @copydoc Dali::Internal::Adaptor::ECore::RenderSurface.
 * Window specialization.
 */
class WindowRenderSurface : public EcoreXRenderSurface
{
public:

  /**
    * Uses an X11 surface to render to.
    * @param [in] positionSize the position and size of the surface
    * @param [in] surface can be a X-window or X-pixmap (type must be unsigned int).
    * @param [in] name optional name of surface passed in
    * @param [in] isTransparent if it is true, surface has 32 bit color depth, otherwise, 24 bit
    */
  WindowRenderSurface( Dali::PositionSize positionSize,
                       Any surface,
                       const std::string& name,
                       bool isTransparent = false );

  /**
   * @copydoc Dali::Internal::Adaptor::ECore::EcoreXRenderSurface::~EcoreXRenderSurface
   */
  virtual ~WindowRenderSurface();

public: // API

  /**
   * @copydoc Dali::RenderSurface::GetDrawable()
   */
  virtual Ecore_X_Drawable GetDrawable();

  /**
   * Request to approve deiconify operation
   * If it is requested, it will send ECORE_X_ATOM_E_DEICONIFY_APPROVE event to window manager after rendering
   */
  void RequestToApproveDeiconify();

public: // from Dali::RenderSurface

  /**
   * @copydoc Dali::RenderSurface::GetSurface()
   */
  virtual Any GetSurface();

  /**
   * @copydoc Dali::RenderSurface::GetDrawable()
   */
  virtual Ecore_X_Window GetXWindow();

public:  // from Internal::Adaptor::RenderSurface

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
   * @copydoc Dali::Internal::Adaptor::RenderSurface::StartRender()
   */
  virtual void StartRender();

  /**
   * @copydoc Dali::Internal::Adaptor::RenderSurface::PreRender()
   */
  virtual bool PreRender( EglInterface& egl, Integration::GlAbstraction& glAbstraction );

  /**
   * @copydoc Dali::Internal::Adaptor::RenderSurface::PostRender()
   */
  virtual void PostRender( EglInterface& egl, Integration::GlAbstraction& glAbstraction, unsigned int timeDelta, bool replacingSurface );

  /**
   * @copydoc Dali::Internal::Adaptor::RenderSurface::StopRender()
   */
  virtual void StopRender();

  /**
   * @copydoc Dali::Internal::Adaptor::RenderSurface::SetViewMode()
   */
  void SetViewMode( ViewMode viewMode );

  /**
   * @copydoc Dali::Internal::Adaptor::RenderSurface::ReleaseLock()
   */
  virtual void ReleaseLock();

protected:

  /**
   * Create XWindow
   */
  virtual void CreateXRenderable();

  /**
   * @copydoc Dali::Internal::Adaptor::ECore::EcoreXRenderSurface::UseExistingRenderable
   */
  virtual void UseExistingRenderable( unsigned int surfaceId );

private: // Data

  Ecore_X_Window   mX11Window; ///< X-Window
  bool             mNeedToApproveDeiconify; ///< Whether need to send ECORE_X_ATOM_E_DEICONIFY_APPROVE event

}; // class WindowRenderSurface

} // namespace ECore

} // namespace Dali

#endif // __DALI_INTERNAL_ECORE_X_WINDOW_RENDER_SURFACE_H__
