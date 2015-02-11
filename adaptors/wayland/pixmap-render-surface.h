#ifndef __DALI_INTERNAL_ECORE_X_PIXMAP_RENDER_SURFACE_H__
#define __DALI_INTERNAL_ECORE_X_PIXMAP_RENDER_SURFACE_H__

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
#include <ecore-wl-render-surface.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{
class TriggerEvent;

namespace ECore
{

/**
 * Ecore X11 implementation of render surface.
 */
class PixmapRenderSurface : public EcoreWlRenderSurface
{
public:

  /**
    * Uses an Wayland surface to render to.
    * @param [in] positionSize the position and size of the surface
    * @param [in] surface can be a Wayland-window (type must be unsigned int).
    * @param [in] name optional name of surface passed in
    * @param [in] isTransparent if it is true, surface has 32 bit color depth, otherwise, 24 bit
    */
  PixmapRenderSurface( Dali::PositionSize positionSize,
                       Any surface,
                       const std::string& name,
                       bool isTransparent = false);

  /**
   * @copydoc Dali::Internal::Adaptor::ECore::RenderSurface::~RenderSurface
   */
  virtual ~PixmapRenderSurface();

public: // API

public: // from Dali::RenderSurface

  /**
   * @copydoc Dali::RenderSurface::GetSurface()
   */
  virtual Any GetSurface();

public: // from Internal::Adaptor::RenderSurface

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

private:
  enum SyncMode
  {
    SYNC_MODE_NONE,
    SYNC_MODE_WAIT
  };

  void SetSyncMode( SyncMode syncMode );

  /**
   * If sync mode is WAIT, then acquire a lock. This prevents render thread from
   * continuing until the pixmap has been drawn by the compositor.
   * It must be released for rendering to continue.
   * @param[in] syncMode The current sync mode
   */
  void AcquireLock( SyncMode syncMode );

  /**
   * Release any locks.
   */
  void ReleaseLock();

  /**
   * Create XPixmap
   */
  virtual void CreateWlRenderable();

  /**
   * @copydoc Dali::Internal::Adaptor::ECore::RenderSurface::UseExistingRenderable
   */
  virtual void UseExistingRenderable( unsigned int surfaceId );

private: // Data

};

} // namespace ECore

} // namespace Adaptor

} // namespace internal

} // namespace Dali

#endif // __DALI_INTERNAL_ECORE_X_PIXMAP_RENDER_SURFACE_H__
