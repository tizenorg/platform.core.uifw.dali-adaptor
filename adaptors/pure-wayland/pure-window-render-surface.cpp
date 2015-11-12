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

// CLASS HEADER
#include "pure-window-render-surface.h"

// EXTERNAL INCLUDES
#include <dali/integration-api/gl-abstraction.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <wl-types.h>
#include <gl/egl-implementation.h>
#include <thread-synchronization-interface.h>
#include "pure-wayland-client.h"

#include <iostream>
#include <sys/time.h>
using namespace std;


namespace Dali
{

#if defined(DEBUG_ENABLED)
extern Debug::Filter* gRenderSurfaceLogFilter;
#endif

namespace Wayland
{

namespace
{
WlDisplay* gCurrentDisplay( NULL );

const int MINIMUM_DIMENSION_CHANGE( 1 ); ///< Minimum change for window to be considered to have moved

} // unnamed namespace

WindowRenderSurface::WindowRenderSurface( Dali::PositionSize positionSize,
                                          Any surface,
                                          const std::string& name,
                                          bool isTransparent)
: WlRenderSurface( positionSize, surface, name, isTransparent ),
  mNeedToApproveDeiconify(false)
{
  DALI_LOG_INFO( gRenderSurfaceLogFilter, Debug::Verbose, "Creating Window\n" );
  Init( surface );

  gCurrentDisplay = this->mWaylandClient->mDisplay;
}

WlDisplay* WindowRenderSurface::GetWaylandDisplay()
{
  return gCurrentDisplay;
}
WindowRenderSurface::~WindowRenderSurface()
{
  if( mOwnSurface )
  {
    // free window
  }
}

WlWindow* WindowRenderSurface::GetDrawable()
{
  // already an e-core type
  return &mWindow;
}

Any WindowRenderSurface::GetSurface()
{
  // already an e-core type
  return Any( &mWindow );
}

WlWindow* WindowRenderSurface::GetWlWindow()
{
  return &mWindow;
}

void WindowRenderSurface::RequestToApproveDeiconify()
{
  mNeedToApproveDeiconify = true;
}

void WindowRenderSurface::InitializeEgl( EglInterface& eglIf )
{
  DALI_LOG_TRACE_METHOD( gRenderSurfaceLogFilter );

  Internal::Adaptor::EglImplementation& eglImpl = static_cast<Internal::Adaptor::EglImplementation&>( eglIf );

  eglImpl.ChooseConfig(true, mColorDepth);
}

void WindowRenderSurface::CreateEglSurface( EglInterface& eglIf )
{
  DALI_LOG_TRACE_METHOD( gRenderSurfaceLogFilter );

  Internal::Adaptor::EglImplementation& eglImpl = static_cast<Internal::Adaptor::EglImplementation&>( eglIf );


  // create the  surface
  WlRenderSurface::CreateSurface();

  mEglWindow = wl_egl_window_create(mWindow.mSurface, mPosition.width, mPosition.height);

  eglImpl.CreateSurfaceWindow( (EGLNativeWindowType)mEglWindow, mColorDepth ); // reinterpret_cast does not compile
}

void WindowRenderSurface::DestroyEglSurface( EglInterface& eglIf )
{

  Internal::Adaptor::EglImplementation& eglImpl = static_cast<Internal::Adaptor::EglImplementation&>( eglIf );
  eglImpl.DestroySurface();
  wl_egl_window_destroy( mEglWindow );
  mEglWindow = NULL;
}

bool WindowRenderSurface::ReplaceEGLSurface( EglInterface& egl )
{
  // todo
  return true;
}

void WindowRenderSurface::MoveResize( Dali::PositionSize positionSize )
{
  bool needToMove = false;
  bool needToResize = false;

  // check moving
  if( (fabs(positionSize.x - mPosition.x) > MINIMUM_DIMENSION_CHANGE) ||
      (fabs(positionSize.y - mPosition.y) > MINIMUM_DIMENSION_CHANGE) )
  {
    needToMove = true;
  }

  // check resizing
  if( (fabs(positionSize.width - mPosition.width) > MINIMUM_DIMENSION_CHANGE) ||
      (fabs(positionSize.height - mPosition.height) > MINIMUM_DIMENSION_CHANGE) )
  {
    needToResize = true;
  }

  if(needToMove)
  {
   // ecore_wl_window_move(mWlWindow, positionSize.x, positionSize.y);
    mPosition = positionSize;
  }
  if (needToResize)
  {
   // ecore_wl_window_resize(mWlWindow, positionSize.width, positionSize.height, 0);
    mPosition = positionSize;
  }

}

void WindowRenderSurface::Map()
{
 // ecore_wl_window_show(mWlWindow);
}

void WindowRenderSurface::StartRender()
{
}

bool WindowRenderSurface::PreRender( EglInterface&, Integration::GlAbstraction& )
{
  // nothing to do for windows
  return true;
}

unsigned long long getMS()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  unsigned long long millisecondsSinceEpoch =
    (unsigned long long)(tv.tv_sec) * 1000 +
    (unsigned long long)(tv.tv_usec) / 1000;
  return millisecondsSinceEpoch;
}

bool gFirstSwapBuffer = false;
void WindowRenderSurface::PostRender( EglInterface& egl, Integration::GlAbstraction& glAbstraction, DisplayConnection* displayConnection, bool replacingSurface )
{
  if( !gFirstSwapBuffer)
  {
    cout << "(time-logger)Begin first SwapBuffers() call: " << getMS() << endl;
    DALI_LOG_ERROR("(time-logger)Begin first SwapBuffers() call: %llu  ", getMS());
  }

  Internal::Adaptor::EglImplementation& eglImpl = static_cast<Internal::Adaptor::EglImplementation&>( egl );
  eglImpl.SwapBuffers();

  if( !gFirstSwapBuffer)
  {
    cout << "(time-logger)End__ first SwapBuffers() call: " << getMS() << endl;
    DALI_LOG_ERROR("(time-logger)End__ first SwapBuffers() call: %llu  ", getMS());
    gFirstSwapBuffer = true;
  }
}

void WindowRenderSurface::StopRender()
{
}

void WindowRenderSurface::SetViewMode( ViewMode viewMode )
{
  //FIXME
}

void WindowRenderSurface::CreateWlRenderable()
{
}

void WindowRenderSurface::UseExistingRenderable( unsigned int surfaceId )
{
  return;
}

void WindowRenderSurface::ReleaseLock()
{
  // Nothing to do.
}
void WindowRenderSurface::SetThreadSynchronization( Dali::ThreadSynchronizationInterface& threadSynchronization )
{
  // Nothing to do
}

} // namespace ECore

} // namespace Dali
