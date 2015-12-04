
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

// CLASS HEADER
#include "android-render-surface.h"

// EXTERNAL INCLUDES
#include <android/native_window.h>
#include <unistd.h>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/integration-api/gl-abstraction.h>
#include <adaptors/integration-api/framework.h>

#if ( DALI_GLES_VERSION == 20 )
#include <GLES2/gl2.h>
#else
#include <GLES3/gl3.h>
#endif

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
const unsigned int INPUT_EVENT_UPDATE_PERIOD( MICROSECONDS_PER_SECOND / 90 );
const int MILLISECONDS_PER_SECOND = 1000;
const int MICROSECONDS_PER_MILLISECOND = 1000;

} // unnamed namespace


AndroidRenderSurface::AndroidRenderSurface( Integration::Framework* framework,
                                            SurfaceType type,
                                            Dali::PositionSize positionSize,
                                            Any surface,
                                            Any display,
                                            const std::string& name,
                                            bool isTransparent)
: mFramework( framework ),
  mWindow(NULL),
  mType(type),
  mPosition(positionSize),
  mOwnSurface(false),
  mIsStopped(false),
  mColorDepth(isTransparent ? COLOR_DEPTH_32 : COLOR_DEPTH_24)
{
  // see if there is a surface in Any surface
  ANativeWindow* nativeWindow  = GetSurfaceWindow(surface);

  // if the surface is empty, create a new one.
  if ( nativeWindow == NULL )
  {
    // DALI_ASSERT_ALWAYS(0 && "Dont support creating renderables (yet)");
    // we own the surface about to created
    mOwnSurface = true;
    CreateRenderable();
  }
  else
  {
    UseExistingRenderable( nativeWindow );
  }
}

AndroidRenderSurface::~AndroidRenderSurface()
{
}

ANativeWindow* AndroidRenderSurface::GetWindow()
{
  mWindow = mFramework->GetWindow();
  return mWindow;
}

void AndroidRenderSurface::SurfaceCreated()
{
  CreateRenderable();
}

Dali::RenderSurface::SurfaceType AndroidRenderSurface::GetType()
{
  return mType;
}

ANativeWindow* AndroidRenderSurface::GetSurfaceWindow( Any surface ) const
{
  if (false == surface.Empty())
  {
    return AnyCast<ANativeWindow*>(surface);
  }

  return NULL;
}

Any AndroidRenderSurface::GetSurface()
{
  // return either the window or pixmap surface
  if ( mWindow )
  {
    // already an android type
    return Any( mWindow );
  }
  else
  {
    // TODO:
    return Any();
  }
}

Any AndroidRenderSurface::GetDisplay()
{
  // this getter is used by main thread so we need to return the main thread version of the display
  return Any();
}

PositionSize AndroidRenderSurface::GetPositionSize() const
{
  return mPosition;
}

void AndroidRenderSurface::CreateEglSurface( EglInterface& eglIf )
{
  EglImplementation& egl = static_cast<EglImplementation&>( eglIf );

  // create the EGL surface
  switch( mType )
  {
    case Dali::RenderSurface::WINDOW:
    {
      mWindow = mFramework->GetWindow();
      if (mWindow != NULL)
      {
        egl.CreateSurfaceWindow( (EGLNativeWindowType)mWindow, mColorDepth );
      }
      else
      {
        DALI_LOG_ERROR("CreateEglSurface(), don't create surface, window already destroyed");
      }

      break;
    }

    default:
    {
      DALI_ASSERT_ALWAYS( false && "EglImplementation::CreateSurface incorrect surface type" );
      break;
    }
  }
}

void AndroidRenderSurface::DestroyEglSurface( EglInterface& eglIf )
{
}

bool AndroidRenderSurface::ReplaceEGLSurface( EglInterface& eglIf )
{
  bool contextLost = false;
  EglImplementation& egl = static_cast<EglImplementation&>( eglIf );

  egl.MakeContextNull();

  // destroy previous context ( google advice when onStop() happens )
  egl.DestroyContext();

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

void AndroidRenderSurface::MoveResize( Dali::PositionSize positionSize )
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
    mPosition = positionSize;
  }
}

void AndroidRenderSurface::GetDpi( unsigned int& dpiHorizontal, unsigned int& dpiVertical ) const
{
  // calculate DPI
  Vector2 androidDpi = mFramework->GetDpi();

  dpiHorizontal = int(androidDpi.x + 0.5f);  // rounding
  dpiVertical   = int(androidDpi.y + 0.5f);
}

void AndroidRenderSurface::TransferDisplayOwner( RenderSurface& newSurface )
{
  // Do nothing
}

void AndroidRenderSurface::ConsumeEvents()
{
  // Do nothing
}

void AndroidRenderSurface::CreateRenderable()
{
  // create the x renderable
  if ( mType == Dali::RenderSurface::WINDOW )
  {
    CreateWindow();
  }
}

void AndroidRenderSurface::CreateWindow()
{
  mWindow = mFramework->GetWindow();
  if( mWindow == NULL )
  {
    DALI_ASSERT_ALWAYS(0 && "Need a window");
  }

   // if width or height are zero, go full screen.
  if( (mPosition.width == 0) || (mPosition.height == 0) )
  {
    // Default window size == screen size
    mPosition.x = 0;
    mPosition.y = 0;

    mPosition.width = ANativeWindow_getWidth(mWindow);
    mPosition.height = ANativeWindow_getHeight(mWindow);
  }
}

void AndroidRenderSurface::UseExistingRenderable( ANativeWindow* surfacePtr )
{
}

void AndroidRenderSurface::InitializeEgl( EglInterface& eglIf )
{
  EglImplementation& eglImpl = static_cast<EglImplementation&>( eglIf );
  eglImpl.InitializeGles( reinterpret_cast< EGLNativeDisplayType >( EGL_DEFAULT_DISPLAY ) );

  eglImpl.ChooseConfig(true, mColorDepth);
}

bool AndroidRenderSurface::PreRender( EglInterface& egl, Integration::GlAbstraction& glAbstraction )
{
  return mWindow != NULL;
}

void AndroidRenderSurface::PostRender( EglInterface& egl, Integration::GlAbstraction& glAbstraction, Dali::DisplayConnection* displayConnection, unsigned int timeDelta, bool replacingSurface )
{
  // flush gl instruction queue
  EglImplementation& eglImpl = static_cast<EglImplementation&>( egl );

  // make sure we still have valid context
  if(eglImpl.GetContext())
  {
    eglImpl.SwapBuffers();
  }
  else
  {
    DALI_LOG_WARNING("PostRender() called on lost GL context!");
  }
  RenderSync( timeDelta );
}

void AndroidRenderSurface::StopRender()
{
  mIsStopped = true;
}

void AndroidRenderSurface::StartRender()
{
}

bool AndroidRenderSurface::RenderSync( unsigned int timeDelta )
{
  //@todo Remove this method. Vsync thread should handle syncing SwapBuffers to frame rate.

  // ensure minimum delay between swap buffers matches the desired frame rate (default 60FPS)
  int mode = 60;
  //if( static_cast<int>(mode) > Dali::RenderSurface::RENDER_SYNC )
  {
    unsigned int syncPeriod( MICROSECONDS_PER_SECOND / static_cast< unsigned int >( mode ) - MILLISECONDS_PER_SECOND );
    if( timeDelta < syncPeriod )
    {
      usleep( syncPeriod - timeDelta );
    }
  }

  return true; //mode == Dali::RenderSurface::RENDER_SYNC;
}

void AndroidRenderSurface::SurfaceLost()
{
  mWindow = NULL;
}

void AndroidRenderSurface::SetThreadSynchronization( ThreadSynchronizationInterface& /* threadSynchronization */ )
{
}

void AndroidRenderSurface::ReleaseLock()
{
}

void AndroidRenderSurface::SetViewMode( ViewMode viewMode )
{
}


} // namespace X11

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
