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
