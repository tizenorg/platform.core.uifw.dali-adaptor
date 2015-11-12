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
#include "pure-wayland-render-surface.h"

// EXTERNAL INCLUDES
#include <stdio.h>
#include <dali/integration-api/gl-abstraction.h>
#include <dali/integration-api/debug.h>
#include <cstring>
#include <unistd.h>

// INTERNAL INCLUDES
#include <base/separate-update-render/thread-synchronization.h>
#include <gl/egl-implementation.h>
#include <wl-types.h>
#include <trigger-event.h>
#include <base/interfaces/window-event-interface.h>
#include <pure-wayland-client.h>

#include <iostream>
#include <sys/time.h>
using namespace std;


namespace Dali
{

namespace Wayland
{

namespace
{

void ShellSurfacePing( void* data, struct wl_shell_surface* shell_surface, uint32_t serial)
{
  // The ping event comes from the compositor to checks if we're still alive.
  wl_shell_surface_pong(shell_surface, serial);
}

void ShellSurfaceConfigure( void* data, struct wl_shell_surface* shell_surface, uint32_t edges, int32_t width, int32_t height)
{
}

const struct wl_shell_surface_listener ShellSurfaceListener =
{
    ShellSurfacePing,
    ShellSurfaceConfigure,
};

void XdgShellPing( void* data, Dali::WlXdgShell* shell, uint32_t serial)
{
  // The ping event comes from the compositor to checks if we're still alive.
  xdg_shell_pong( shell, serial);
}

struct xdg_shell_listener XdgShellListerner=
{
    XdgShellPing,
};


}
RenderSurface::RenderSurface(Dali::PositionSize positionSize,
                                           Any surface,
                                           const std::string& name,
                                           bool isTransparent)
: mRenderNotification(NULL),
  mColorDepth(isTransparent ? COLOR_DEPTH_32 : COLOR_DEPTH_24)
{
  mWindow.mPosition = positionSize;
  mWindow.mTitle = name;
  mWaylandClient = new  Dali::Internal::Adaptor::WaylandClient;
  mWaylandClient->Initialise();
}

RenderSurface::~RenderSurface()
{
  delete mWaylandClient;
}



void RenderSurface::CreateSurface()
{

  // Create the surface
  // A Wayland surface is a rectangular area that is displayed on the screen.
  // It has a location, size and pixel contents.

  mWindow.mSurface = wl_compositor_create_surface( mWaylandClient->mCompositor );

  DALI_ASSERT_ALWAYS( mWindow.mSurface && "wl_compositor_create_surface failed" );

  // keep track of the surface id
  mWindow.mSurfaceId = wl_proxy_get_id( reinterpret_cast<struct wl_proxy *>(mWindow.mSurface ));

  // A surface in Wayland needs to be assigned a role.
  // This gives the surface the role of a shell surface.
  // It allows the surface to be treated like a toplevel, fullscreen or popup window.
  // which can be moved, resize or maximized and have associate metadata like title and class.

  // Try the xdg shell first. XDG is a designed to create a desktop shell with features like minimise etc.
  if( mWaylandClient->mXdgShell )
  {
    mWindow.mXdgSurface = xdg_shell_get_xdg_surface( mWaylandClient->mXdgShell, mWindow.mSurface );

    DALI_ASSERT_ALWAYS(mWindow.mXdgSurface&& "xdg_shell_get_xdg_surface failed" );
    if( mWindow.mTitle.length() )
    {
      xdg_surface_set_title(  mWindow.mXdgSurface, mWindow.mTitle.c_str() );
    }
    xdg_shell_add_listener( mWaylandClient->mXdgShell, &XdgShellListerner, 0);
  }
  else
  {
    // try the generic Wayland Shell
    mWindow.mShellSurface = wl_shell_get_shell_surface( mWaylandClient->mShell, mWindow.mSurface);
    DALI_ASSERT_ALWAYS(mWindow.mShellSurface && "wl_shell_get_shell_surface failed" );
    if( mWindow.mTitle.length() )
    {
       wl_shell_surface_set_title(  mWindow.mShellSurface, mWindow.mTitle.c_str() );
    }

    wl_shell_surface_set_toplevel( mWindow.mShellSurface);

    wl_shell_surface_add_listener( mWindow.mShellSurface , &ShellSurfaceListener, 0);
  }

  // send / receive all commands
  wl_display_roundtrip( mWaylandClient->mDisplay );
}


Window* RenderSurface::GetWindow()
{
  return &mWindow;
}


void RenderSurface::AssignWindowEventInterface( Dali::Internal::Adaptor::WindowEventInterface* eventInterface)
{
  mWaylandClient->AssignWindowEventInterface( eventInterface );
}

PositionSize RenderSurface::GetPositionSize() const
{
  return mWindow.mPosition;
}

void RenderSurface::InitializeEgl( EglInterface& egl )
{



  printf(" mWaylandClient init, Display = %p \n", mWaylandClient->mDisplay );

  Internal::Adaptor::EglImplementation& eglImpl = static_cast<Internal::Adaptor::EglImplementation&>( egl );

  if (!eglImpl.InitializeGles(static_cast<EGLNativeDisplayType>( mWaylandClient->mDisplay )))
  {
    DALI_LOG_ERROR("Failed to initialize GLES.");
  }

  eglImpl.ChooseConfig( true, mColorDepth );
}

void RenderSurface::CreateEglSurface( EglInterface& egl )
{
  Internal::Adaptor::EglImplementation& eglImpl = static_cast<Internal::Adaptor::EglImplementation&>( egl );

  // create the  surface
  CreateSurface();

  mEglWindow = wl_egl_window_create(mWindow.mSurface, mWindow.mPosition.width, mWindow.mPosition.height);

  eglImpl.CreateSurfaceWindow( (EGLNativeWindowType)mEglWindow, mColorDepth ); // reinterpret_cast does not compile

}

void RenderSurface::DestroyEglSurface( EglInterface& eglIf )
{
  Internal::Adaptor::EglImplementation& eglImpl = static_cast<Internal::Adaptor::EglImplementation&>( eglIf );
  eglImpl.DestroySurface();
  wl_egl_window_destroy( mEglWindow );
  mEglWindow = NULL;
}

bool RenderSurface::ReplaceEGLSurface( EglInterface& egl )
{
  return true;
}

void RenderSurface::MoveResize( Dali::PositionSize positionSize )
{
}

void RenderSurface::SetViewMode( ViewMode viewMode )
{
}

void RenderSurface::StartRender()
{
}

bool RenderSurface::PreRender( EglInterface&, Integration::GlAbstraction& )
{
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
void RenderSurface::PostRender( EglInterface& egl, Integration::GlAbstraction& glAbstraction, DisplayConnection* displayConnection, bool replacingSurface )
{
  if( !gFistSwapBuffer)
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

void RenderSurface::StopRender()
{
}

void RenderSurface::ReleaseLock()
{
}
void RenderSurface::SetThreadSynchronization( ThreadSynchronizationInterface& threadSynchronization )
{
  // required for logging Wayland Client time stamps
  Dali::Internal::Adaptor::ThreadSynchronization& threadSync( static_cast< Dali::Internal::Adaptor::ThreadSynchronization &>(threadSynchronization) );
  mWaylandClient->SetPerformanceInterface( threadSync.GetPerformanceInterface() );

}



} // namespace Wayland

} // namespace Dali
