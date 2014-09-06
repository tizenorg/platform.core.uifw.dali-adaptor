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


// CLASS HEADER
#include "android-render-surface.h"

// EXTERNAL INCLUDES
#include "android-interface.h"
#include "nativeLogging.h"

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/integration-api/gl-abstraction.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace Android
{

namespace
{

const float MINIMUM_DIMENSION_CHANGE = 1.0f; ///< Minimum change for window to be considered to have moved
const unsigned int MICROSECONDS_PER_SECOND( 1000000 );
const unsigned int INPUT_EVENT_UPDATE_PERIOD( MICROSECONDS_PER_SECOND / 90 ); // period between ecore x event updates
const int MILLISECONDS_PER_SECOND = 1000;

} // unnamed namespace


RenderSurface::RenderSurface( SurfaceType type,
                              Dali::PositionSize positionSize,
                              boost::any surface,
                              boost::any display,
                              const std::string& name,
                              bool isTransparent)
: mType(type),
  mPosition(positionSize),
  mOwnSurface(false),
  mOwnDisplay(false),
  mTitle(name),
  mColorDepth(isTransparent ? COLOR_DEPTH_32 : COLOR_DEPTH_24),
  mRenderMode((type == PIXMAP) ? RENDER_SYNC : RENDER_DEFAULT),
  mIsStopped(false)
{
  // see if there is a surface in boost::any surface
  LOGD(" %s %d\n", __PRETTY_FUNCTION__, __LINE__);
  ANativeWindow* nativeWindow  = GetSurfaceWindow(surface);
  LOGD(" %s %d\n", __PRETTY_FUNCTION__, __LINE__);

  // if the surface is empty, create a new one.
  if ( nativeWindow == NULL )
  {
    LOGE(" %s %d\n", __PRETTY_FUNCTION__, __LINE__);
    // DALI_ASSERT_ALWAYS(0 && "Dont support creating renderables (yet)");
    // we own the surface about to created
    mOwnSurface = true;
    CreateRenderable();
  }
  else
  {
    LOGD(" %s %d\n", __PRETTY_FUNCTION__, __LINE__);
    UseExistingRenderable( nativeWindow );
  }

#ifdef DEBUG_ENABLED
  // prints out 'new RenderSurface, created display xx, used existing surface xx
  // we can not use LOG_INFO because the surface can be created before Dali Core is created.
  printf(  "new RenderSurface, %s surface %X(%X) \n",
             mOwnSurface?"created":"used existing",
             NULL, /*mX11Pixmap,*/ mWindow);
#endif
}

RenderSurface::~RenderSurface()
{
  // We don't own any of the surfaces
}

ANativeWindow* RenderSurface::GetWindow()
{
  // should find out why it isn't updated
  mWindow = androidcore_GetPrimaryWindow();
  return mWindow;
}

void RenderSurface::SurfaceCreated()
{
  CreateRenderable();
}

Dali::RenderSurface::SurfaceType RenderSurface::GetType()
{
  return mType;
}

ANativeWindow* RenderSurface::GetSurfaceWindow( boost::any surface ) const
{
  if (false == surface.empty())
  {
    return boost::any_cast<ANativeWindow*>(surface);
  }

  return NULL;
}

boost::any RenderSurface::GetSurface()
{
  // return either the window or pixmap surface
  if ( mWindow )
  {
    // already an android type
    return boost::any( mWindow );
  }
  else
  {
    // TODO:
    return boost::any();
  }
}

boost::any RenderSurface::GetDisplay()
{
  // this getter is used by main thread so we need to return the main thread version of the display
  return boost::any();
}

PositionSize RenderSurface::GetPositionSize() const
{
  return mPosition;
}

void RenderSurface::SetRenderMode(RenderMode mode)
{
  mRenderMode = mode;
}

Dali::RenderSurface::RenderMode RenderSurface::GetRenderMode() const
{
  return mRenderMode;
}

void RenderSurface::CreateEglSurface( EglInterface& eglIf )
{
  EglImplementation& egl = static_cast<EglImplementation&>( eglIf );
  egl.InitializeGles( EGL_DEFAULT_DISPLAY );

  // create the EGL surface
  switch( mType )
  {
    case Dali::RenderSurface::WINDOW:
    {
      mWindow = androidcore_GetPrimaryWindow();
      egl.CreateSurfaceWindow( mWindow, mColorDepth );
      break;
    }

    default:
    {
      DALI_ASSERT_ALWAYS( false && "EglImplementation::CreateSurface incorrect surface type" );
      break;
    }
  }
}

bool RenderSurface::ReplaceEGLSurface( EglInterface& eglIf )
{
  bool contextLost = false;
  EglImplementation& egl = static_cast<EglImplementation&>( eglIf );

  // a new surface for the new pixmap
  switch( mType )
  {
    case Dali::RenderSurface::WINDOW:
    {
      contextLost = egl.ReplaceSurfaceWindow( (EGLNativeWindowType)mWindow, reinterpret_cast< EGLNativeDisplayType >( EGL_DEFAULT_DISPLAY ) );
      break;
    }

    default:
    {
      DALI_ASSERT_ALWAYS( false && "EglImplementation::ReplaceSurface incorrect surface type" );
      break;
    }
  }

  return contextLost;
}

void RenderSurface::MoveResize( Dali::PositionSize positionSize )
{
  if ( mType != Dali::RenderSurface::WINDOW )
  {
    return;
  }

  if( (fabsf(positionSize.x - mPosition.x) > MINIMUM_DIMENSION_CHANGE)
       ||
      (fabsf(positionSize.y - mPosition.y) > MINIMUM_DIMENSION_CHANGE)
       ||
      (fabsf(positionSize.width - mPosition.width) > MINIMUM_DIMENSION_CHANGE)
       ||
      (fabsf(positionSize.height - mPosition.height) > MINIMUM_DIMENSION_CHANGE) )
  {
    // ecore_x_window_move_resize(mX11Window, positionSize.x, positionSize.y, positionSize.width, positionSize.height);
    mPosition = positionSize;
  }
}

void RenderSurface::GetDpi( unsigned int& dpiHorizontal, unsigned int& dpiVertical ) const
{
  // calculate DPI
  Vector2 androidDpi = android_getDpi();
  dpiHorizontal = int(androidDpi.x + 0.5f);  // rounding
  dpiVertical   = int(androidDpi.y + 0.5f);
}

void RenderSurface::Map()
{
  if (mType == Dali::RenderSurface::WINDOW)
  {
    //  ecore_x_window_show(mX11Window);
  }
}

void RenderSurface::TransferDisplayOwner( Internal::Adaptor::RenderSurface& newSurface )
{
  // if we don't own the display return
  if( mOwnDisplay == false )
  {
    return;
  }

  RenderSurface* other = dynamic_cast< RenderSurface* >( &newSurface );
  if( other )
  {
    // if both surfaces share the same display, and this surface owns it,
    // then transfer the ownership to the new surface
    /*
    if( other->mMainDisplay == mMainDisplay )
    {
      mOwnDisplay = false;
      other->mOwnDisplay = true;
    }
    */
  }
}

void RenderSurface::ConsumeEvents()
{
  // if the render surface has own display, check events
}

void RenderSurface::CreateRenderable()
{
  // create the x renderable
  if ( mType == Dali::RenderSurface::WINDOW )
  {
    CreateWindow();
  }
}

void RenderSurface::CreateWindow()
{
  mWindow = androidcore_GetPrimaryWindow();
  if (mWindow == NULL)
  {
    DALI_ASSERT_ALWAYS(0 && "Need a window");
  }

   // if width or height are zero, go full screen.
  if ( (mPosition.width == 0) || (mPosition.height == 0) )
  {
    // Default window size == screen size
    mPosition.x = 0;
    mPosition.y = 0;

    mPosition.width = ANativeWindow_getWidth(mWindow);
    mPosition.height = ANativeWindow_getHeight(mWindow);
  }
}

void RenderSurface::UseExistingRenderable( ANativeWindow* surfacePtr )
{
  if (mType == Dali::RenderSurface::WINDOW)
  {
    mWindow = surfacePtr;
    mPosition.x = 0;
    mPosition.y = 0;

    mPosition.width = ANativeWindow_getWidth(mWindow);
    mPosition.height = ANativeWindow_getHeight(mWindow);
  }
}

void RenderSurface::InitializeEgl( EglInterface& eglIf )
{
  EglImplementation& eglImpl = static_cast<EglImplementation&>( eglIf );
  eglImpl.InitializeGles( reinterpret_cast< EGLNativeDisplayType >( EGL_DEFAULT_DISPLAY ) );

  eglImpl.ChooseConfig(true, mColorDepth);
}

bool RenderSurface::PreRender( EglInterface& egl, Integration::GlAbstraction& glAbstraction )
{
  return mWindow != NULL;
}

bool RenderSurface::PostRender( EglInterface& egl, Integration::GlAbstraction& glAbstraction, unsigned int timeDelta )
{
  // flush gl instruction queue
  glAbstraction.Flush();
  EglImplementation& eglImpl = static_cast<EglImplementation&>( egl );
  eglImpl.SwapBuffers();

  return RenderSync( timeDelta );
}

void RenderSurface::StopRender()
{
  mIsStopped = true;
}

bool RenderSurface::RenderSync( unsigned int timeDelta )
{
  // ensure minimum delay between swap buffers matches the desired frame rate (default 60FPS)
  Dali::RenderSurface::RenderMode mode = mRenderMode;
  if( mode == Dali::RenderSurface::RENDER_DEFAULT )
  {
    mode = Dali::RenderSurface::RENDER_60FPS;
  }

  if( static_cast<int>(mode) > Dali::RenderSurface::RENDER_SYNC )
  {
    unsigned int syncPeriod( MICROSECONDS_PER_SECOND / static_cast< unsigned int >( mode ) - MILLISECONDS_PER_SECOND );
    if( timeDelta < syncPeriod )
    {
      usleep( syncPeriod - timeDelta );
    }
  }

  return mode == Dali::RenderSurface::RENDER_SYNC;
}

void RenderSurface::SurfaceLost()
{
  mWindow = NULL;
}


} // namespace X11

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
