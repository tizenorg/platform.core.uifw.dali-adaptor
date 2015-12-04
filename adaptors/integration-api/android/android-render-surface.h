#ifndef __DALI_INTERNAL_ANDROID_RENDER_SURFACE_H__
#define __DALI_INTERNAL_ANDROID_RENDER_SURFACE_H__

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
 */

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <render-surface.h>
#include <gl/egl-implementation.h>
#include <dali/devel-api/threading/mutex.h>
#include <base/display-connection.h>

struct ANativeWindow;

namespace Dali
{
namespace Integration
{
class Framework;
}

namespace Internal
{

namespace Adaptor
{

namespace Android
{

/**
 * Android implementation of render surface.
 */
class AndroidRenderSurface : public RenderSurface
{
public:

  /**
    * Uses an Android view-surface to render to.
    * @param[in] framework The application framework
    * @param[in] type the type of surface passed in
    * @param[in] positionSize the position and size of the surface
    * @param[in] surface can be a X-window or X-pixmap (type must be unsigned int).
    * @param[in] display connection to X-server if the surface is a X window or pixmap (type must be void * to X display struct)
    * @param[in] name optional name of surface passed in
    * @param[in] isTransparent if it is true, surface has 32 bit color depth, otherwise, 24 bit
    */
  AndroidRenderSurface( Integration::Framework* framework,
                        SurfaceType type,
                        Dali::PositionSize positionSize,
                        Any surface,
                        Any display,
                        const std::string& name,
                        bool isTransparent = false);

  /**
   * Destructor.
   * Will delete the display, if it has ownership.
   * Will delete the window/pixmap if it has owner ship
   */
  virtual ~AndroidRenderSurface();

public: // API

  /**
   * @return the android window handle
   */
  ANativeWindow* GetWindow();

public: // from Dali::AndroidRenderSurface

  void SurfaceCreated();

  /**
   * @copydoc Dali::AndroidRenderSurface::GetType()
   */
  virtual Dali::RenderSurface::SurfaceType GetType();

  /**
   * @copydoc Dali::AndroidRenderSurface::GetSurface()
   */
  virtual Any GetSurface();

  /**
   * @copydoc Dali::AndroidRenderSurface::GetDisplay()
   */
  virtual Any GetDisplay();

  /**
   * @copydoc Dali::AndroidRenderSurface::GetPositionSize()
   */
  virtual PositionSize GetPositionSize() const;

public:  // from Internal::Adaptor::AndroidRenderSurface

  /**
   * @copydoc Dali::Internal::Adaptor::AndroidRenderSurface::InitializeEgl()
   */
  virtual void InitializeEgl( EglInterface& egl );

  /**
   * @copydoc Dali::Internal::Adaptor::AndroidRenderSurface::CreateEglSurface()
   */
  virtual void CreateEglSurface( EglInterface& eglIf );

  /**
   * @copydoc Dali::Internal::Adaptor::AndroidRenderSurface::DestroyEglSurface()
   */
  virtual void DestroyEglSurface( EglInterface& eglIf );

  /**
   * @copydoc Dali::Internal::Adaptor::AndroidRenderSurface::ReplaceEGLSurface()
   */
  virtual bool ReplaceEGLSurface( EglInterface& eglIf );

  /**
   * @copydoc Dali::Internal::Adaptor::AndroidRenderSurface::MoveResize()
   */
  virtual void MoveResize( Dali::PositionSize positionSize);

  /**
   * @copydoc Dali::Internal::Adaptor::AndroidRenderSurface::GetDpi()
   */
  virtual void GetDpi( unsigned int& dpiHorizontal, unsigned int& dpiVertical ) const;

  /**
   * @copydoc Dali::Internal::Adaptor::AndroidRenderSurface::PreRender()
   */
  virtual bool PreRender( EglInterface& egl, Integration::GlAbstraction& glAbstraction );

  /**
   * @copydoc Dali::Internal::Adaptor::AndroidRenderSurface::PostRender()
   */
  virtual void PostRender( EglInterface& egl, Integration::GlAbstraction& glAbstraction, Dali::DisplayConnection* displayConnection, unsigned int timeDelta, bool replacingSurface );

  /**
   * @copydoc Dali::Internal::Adaptor::AndroidRenderSurface::TransferDisplayOwner()
   */
  virtual void TransferDisplayOwner( RenderSurface& newSurface );

  /**
   * @copydoc Dali::Internal::Adaptor::AndroidRenderSurface::ConsumeEvents()
   */
  virtual void ConsumeEvents();

  /**
   * @copydoc Dali::Internal::Adaptor::AndroidRenderSurface::StopRender()
   */
  virtual void StopRender();

  /**
   * @copydoc Dali::Internal::Adaptor::AndroidRenderSurface::StartRender()
   */
  virtual void StartRender();


private:
  enum SyncMode
  {
    SYNC_MODE_NONE,
    SYNC_MODE_WAIT
  };

  /**
   * Returns X event which received from x server
   * If other module handles with x event through same display,
   * event handling can be messed.
   * @param[out] event the x event if there is received event from x server
   * @return remain number of event, -1 if there is no event
   */
  // int GetXEvent( XEvent* event = NULL );

  /**
   * Sets the display, if display parameter is empty it opens a new display
   * @param display
   */
  void SetDisplay( Any display ); // TODO: Do we intend to have fully native android apps? If not, its safe to remove.

  /**
   * Get the surface id if the surface parameter is not empty
   * @param surface Any containing a surface id, or can be empty
   * @return surface id, or zero if surface is empty
   */
  ANativeWindow* GetSurfaceWindow( Any surface ) const;

  /**
   * Create Renderable
   */
  void CreateRenderable();

  /**
   * Create Window
   */
  void CreateWindow();

  /**
   * Perform render sync
   * @param[in] currentTime Current time in microseconds
   * @return true if the calling thread should wait for a RenderSync from Adaptor
   */
  bool RenderSync( unsigned int timeDelta );

  /**
   * Use an existing render surface
   * @param surfaceId the id of the surface
   */
  void UseExistingRenderable( ANativeWindow* surfacePtr );

  /**
   * Notify the render thread that this surface has been lost
   */
  void SurfaceLost();

  /**
   * Set the thread sync interface. Not used for android
   */
  void SetThreadSynchronization( ThreadSynchronizationInterface& );

  /**
   * Invoked when the compositor lock should be released. Not used for android
   */
  void ReleaseLock();

  /*
   * Set the stereoscopic 3D view mode
   * @param[in] viewMode The new view mode
   */
  void SetViewMode( ViewMode viewMode );

private: // Data
  Integration::Framework* mFramework; ///< Application framework
  ANativeWindow*   mWindow;       ///< X-Window
  SurfaceType      mType;         ///< type of renderable
  PositionSize     mPosition;     ///< Position
  bool             mOwnSurface;   ///< Whether we own the surface (responsible for deleting it)
  bool             mIsStopped;    ///< Render should be stopped
  ColorDepth       mColorDepth;   ///< Color depth of surface (32 bit or 24 bit)
};

} // namespace x11

} // namespace Adaptor

} // namespace internal

} // namespace Dali

#endif // __DALI_INTERNAL_ANDROID_RENDER_SURFACE_H__
