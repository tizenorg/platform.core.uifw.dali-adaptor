#ifndef __DALI_INTERNAL_WINDOW_RENDER_SURFACE_H__
#define __DALI_INTERNAL_WINDOW_RENDER_SURFACE_H__

/*
Copyright (c) 2000-2012 Samsung Electronics Co., Ltd All Rights Reserved

This file is part of Dali Adaptor

PROPRIETARY/CONFIDENTIAL

This software is the confidential and proprietary information of
SAMSUNG ELECTRONICS ("Confidential Information"). You shall not
disclose such Confidential Information and shall use it only in
accordance with the terms of the license agreement you entered
into with SAMSUNG ELECTRONICS.

SAMSUNG make no representations or warranties about the suitability
of the software, either express or implied, including but not limited
to the implied warranties of merchantability, fitness for a particular
purpose, or non-infringement. SAMSUNG shall not be liable for any
damages suffered by licensee as a result of using, modifying or
distributing this software or its derivatives.
*/

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <render-surface-impl.h>
#include <gl/egl-implementation.h>

struct ANativeWindow;

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace Android
{

/**
 * Android implementation of render surface.
 * @todo change namespace to ECore_X11 as the class
 * is no longer pure X11.
 */
class AndroidRenderSurface : public Internal::Adaptor::RenderSurface
{
public:

  /**
    * Uses an Android view-surface to render to.
    * @param [in] type the type of surface passed in
    * @param [in] positionSize the position and size of the surface
    * @param [in] surface can be a X-window or X-pixmap (type must be unsigned int).
    * @param [in] display connection to X-server if the surface is a X window or pixmap (type must be void * to X display struct)
    * @param [in] name optional name of surface passed in
    * @param [in] isTransparent if it is true, surface has 32 bit color depth, otherwise, 24 bit
    */
  AndroidRenderSurface( SurfaceType type,
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

public: // from Dali::RenderSurface

  void SurfaceCreated();

  /**
   * @copydoc Dali::RenderSurface::GetType()
   */
  virtual Dali::RenderSurface::SurfaceType GetType();

  /**
   * @copydoc Dali::RenderSurface::GetSurface()
   */
  virtual Any GetSurface();

  /**
   * @copydoc Dali::RenderSurface::GetDisplay()
   */
  virtual Any GetDisplay();

  /**
   * @copydoc Dali::RenderSurface::GetPositionSize()
   */
  virtual PositionSize GetPositionSize() const;

public:  // from Internal::Adaptor::RenderSurface

  /**
   * @copydoc Dali::Internal::Adaptor::RenderSurface::InitializeEgl()
   */
  virtual void InitializeEgl( EglInterface& egl );

  /**
   * @copydoc Dali::Internal::Adaptor::RenderSurface::CreateEglSurface()
   */
  virtual void CreateEglSurface( EglInterface& eglIf );

  /**
   * @copydoc Dali::Internal::Adaptor::RenderSurface::DestroyEglSurface()
   */
  virtual void DestroyEglSurface( EglInterface& eglIf );

  /**
   * @copydoc Dali::Internal::Adaptor::RenderSurface::ReplaceEGLSurface()
   */
  virtual bool ReplaceEGLSurface( EglInterface& eglIf );

  /**
   * @copydoc Dali::Internal::Adaptor::RenderSurface::MoveResize()
   */
  virtual void MoveResize( Dali::PositionSize positionSize);

  /**
   * @copydoc Dali::Internal::Adaptor::RenderSurface::GetDpi()
   */
  virtual void GetDpi( unsigned int& dpiHorizontal, unsigned int& dpiVertical ) const;

  /**
   * @copydoc Dali::Internal::Adaptor::RenderSurface::PreRender()
   */
  virtual bool PreRender( EglInterface& egl, Integration::GlAbstraction& glAbstraction );

  /**
   * @copydoc Dali::Internal::Adaptor::RenderSurface::PostRender()
   */
  virtual void PostRender( EglInterface& egl, Integration::GlAbstraction& glAbstraction, unsigned int timeDelta, bool replacingSurface );

  /**
   * @copydoc Dali::Internal::Adaptor::RenderSurface::Map()
   */
  virtual void Map();

  /**
   * @copydoc Dali::Internal::Adaptor::RenderSurface::TransferDisplayOwner()
   */
  virtual void TransferDisplayOwner( Internal::Adaptor::RenderSurface& newSurface );

  /**
   * @copydoc Dali::Internal::Adaptor::RenderSurface::ConsumeEvents()
   */
  virtual void ConsumeEvents();

  /**
   * @copydoc Dali::Internal::Adaptor::RenderSurface::StopRender()
   */
  virtual void StopRender();

  /**
   * @copydoc Dali::Internal::Adaptor::RenderSurface::StartRender()
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

  void ReleaseLock();

  /*
   * Set the stereoscopic 3D view mode
   * @param[in] viewMode The new view mode
   */
  void SetViewMode( ViewMode viewMode );

  ANativeWindow*   mWindow;       ///< X-Window
  SurfaceType      mType;         ///< type of renderable
  PositionSize     mPosition;     ///< Position
  bool             mOwnSurface;   ///< Whether we own the surface (responsible for deleting it)
  bool             mOwnDisplay;   ///< Whether we own the display (responsible for deleting it)
  bool             mIsStopped;    ///< Render should be stopped
  std::string      mTitle;        ///< Title of window which shows from "xinfo -topvwins" command
  ColorDepth       mColorDepth;   ///< Color depth of surface (32 bit or 24 bit)
};

} // namespace x11

} // namespace Adaptor

} // namespace internal

} // namespace Dali

#endif // __DALI_X11_RENDER_SURFACE_H__
