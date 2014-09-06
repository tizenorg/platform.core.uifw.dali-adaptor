#ifndef __DALI_INTERNAL_ANDROID_RENDER_SURFACE_H__
#define __DALI_INTERNAL_ANDROID_RENDER_SURFACE_H__

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
#include <xf86drm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <boost/thread.hpp>

#include <dali/integration-api/core.h>
#include <dali/integration-api/platform-abstraction.h>

// INTERNAL INCLUDES
#include "vsync-notifier.h"
#include <base/interfaces/adaptor-internal-services.h>
#include <base/update-render-synchronization.h>
#include <base/environment-options.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace
{

const unsigned int MICROSECONDS_PER_SECOND( 100000u );
const unsigned int TIME_PER_FRAME_IN_MICROSECONDS( 16667u );

#if defined(DEBUG_ENABLED)
Integration::Log::Filter* gLogFilter = Integration::Log::Filter::New(Debug::Concise, false, "LOG_VSYNC_NOTIFIER");
#endif

} // unnamed namespace

VSyncNotifier::VSyncNotifier( UpdateRenderSynchronization& sync,
                              AdaptorInternalServices& adaptorInterfaces,
                              const EnvironmentOptions& environmentOptions )
: mUpdateRenderSync( sync ),
  mCore( adaptorInterfaces.GetCore() ),
  mPlatformAbstraction( adaptorInterfaces.GetPlatformAbstractionInterface() ),
  mVSyncMonitor( adaptorInterfaces.GetVSyncMonitorInterface() ),
  mThread( NULL ),
  mEnvironmentOptions( environmentOptions )
{
}

VSyncNotifier::~VSyncNotifier()
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "%s\n", __func__ );

  Stop();
}

void VSyncNotifier::Start()
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "%s\n", __func__ );

  if ( !mThread )
  {
    mVSyncMonitor->Initialize();

    mThread = new boost::thread( boost::bind( &VSyncNotifier::Run, this ) );
  }
}

void VSyncNotifier::Stop()
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "%s\n", __func__ );

  if( mThread )
  {
    // wait for the thread to finish
    mThread->join();

    delete mThread;
    mThread = NULL;
  }

  mVSyncMonitor->Terminate();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The following is executed inside the notifier thread !!!
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void VSyncNotifier::Run()
{
  // install a function for logging
  mEnvironmentOptions.InstallLogFunction();

  unsigned int frameNumber( 0u );             // frameCount, updated when the thread is paused
  unsigned int currentSequenceNumber( 0u );   // platform specific vsync sequence number (increments with each vsync)
  unsigned int currentSeconds( 0u );          // timestamp at latest vsync
  unsigned int currentMicroseconds( 0u );     // timestamp at latest vsync
  unsigned int seconds( 0u );
  unsigned int microseconds( 0u );

  bool running( true );
  while( running )
  {
    bool validSync( true );

    // Hardware VSyncs available?
    if( mVSyncMonitor->UseHardware() )
    {
      // Yes..wait for hardware VSync
      validSync = mVSyncMonitor->DoSync( currentSequenceNumber, currentSeconds, currentMicroseconds );
    }
    else
    {
      // No..use software timer
      mPlatformAbstraction.GetTimeMicroseconds( seconds, microseconds );

      unsigned int timeDelta( MICROSECONDS_PER_SECOND * (seconds - currentSeconds) );
      if( microseconds < currentMicroseconds)
      {
        timeDelta += (microseconds + MICROSECONDS_PER_SECOND) - currentMicroseconds;
      }
      else
      {
        timeDelta += microseconds - currentMicroseconds;
      }

      if( timeDelta < TIME_PER_FRAME_IN_MICROSECONDS )
      {
          usleep( TIME_PER_FRAME_IN_MICROSECONDS - timeDelta );
      }
      else
      {
        usleep( TIME_PER_FRAME_IN_MICROSECONDS );
      }
    }

    running = mUpdateRenderSync.VSyncNotifierSyncWithUpdateAndRender( validSync, ++frameNumber, currentSeconds, currentMicroseconds );
  }

  // uninstall a function for logging
  mEnvironmentOptions.UnInstallLogFunction();

}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali


// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <internal/common/render-surface-impl.h>
#include <internal/common/gl/egl-implementation.h>

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
class RenderSurface : public Internal::Adaptor::RenderSurface
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
  RenderSurface( SurfaceType type,
                 Dali::PositionSize positionSize,
                 boost::any surface,
                 boost::any display,
                 const std::string& name,
                 bool isTransparent = false);

  /**
   * Destructor.
   * Will delete the display, if it has ownership.
   * Will delete the window/pixmap if it has owner ship
   */
  virtual ~RenderSurface();

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
  virtual boost::any GetSurface();

  /**
   * @copydoc Dali::RenderSurface::GetDisplay()
   */
  virtual boost::any GetDisplay();

  /**
   * @copydoc Dali::RenderSurface::GetPositionSize()
   */
  virtual PositionSize GetPositionSize() const;

  /**
   * @copydoc Dali::RenderSurface::SetRenderMode()
   */
  virtual void SetRenderMode(RenderMode mode);

  /**
   * @copydoc Dali::RenderSurface::GetRenderMode()
   */
  virtual RenderMode GetRenderMode() const;

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
  virtual bool PostRender( EglInterface& egl, Integration::GlAbstraction& glAbstraction, unsigned int timeDelta );

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

private:

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
  void SetDisplay( boost::any display ); // TODO: Do we intend to have fully native android apps? If not, its safe to remove.

  /**
   * Get the surface id if the surface parameter is not empty
   * @param surface boost::any containing a surface id, or can be empty
   * @return surface id, or zero if surface is empty
   */
  ANativeWindow* GetSurfaceWindow( boost::any surface ) const;

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

private: // Data

  ANativeWindow*   mWindow;       ///< X-Window
  SurfaceType      mType;         ///< type of renderable
  PositionSize     mPosition;     ///< Position
  bool             mOwnSurface;   ///< Whether we own the surface (responsible for deleting it)
  bool             mOwnDisplay;   ///< Whether we own the display (responsible for deleting it)
  bool             mIsStopped;    ///< Render should be stopped
  std::string      mTitle;        ///< Title of window which shows from "xinfo -topvwins" command
  ColorDepth       mColorDepth;   ///< Color depth of surface (32 bit or 24 bit)
  RenderMode       mRenderMode;   ///< Render mode for pixmap surface type
};

} // namespace x11

} // namespace Adaptor

} // namespace internal

} // namespace Dali

#endif // __DALI_X11_RENDER_SURFACE_H__
