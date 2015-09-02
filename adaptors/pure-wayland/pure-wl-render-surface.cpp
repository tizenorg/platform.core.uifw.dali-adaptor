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
#include "wl-render-surface.h"
#include <stdio.h>
// EXTERNAL INCLUDES
#include <dali/integration-api/gl-abstraction.h>
#include <dali/integration-api/debug.h>

#include <cstring>
#include <uv.h>
#include <unistd.h>

// INTERNAL INCLUDES
#include <wl-types.h>
#include "pure-wayland-client.h"
#include <trigger-event.h>
#include <base/interfaces/window-event-interface.h>


namespace Dali
{

#if defined(DEBUG_ENABLED)
Debug::Filter* gRenderSurfaceLogFilter = Debug::Filter::New(Debug::Verbose, false, "LOG_WAYLAND_RENDER_SURFACE");
#endif

namespace Wayland
{

namespace
{

void ShellSurfacePing(void *data, struct wl_shell_surface *shell_surface, uint32_t serial)
{
  printf(" ShellSurfacePing \n");
  wl_shell_surface_pong(shell_surface, serial);
}

void ShellSurfaceConfigure(void *data, struct wl_shell_surface *shell_surface, uint32_t edges, int32_t width, int32_t height)
{
  printf(" ShellSurfaceConfigure %d, %d \n", width, height );
}

const struct wl_shell_surface_listener ShellSurfaceListener =
{
    ShellSurfacePing,
    ShellSurfaceConfigure,
};

}
WlRenderSurface::WlRenderSurface(Dali::PositionSize positionSize,
                                           Any surface,
                                           const std::string& name,
                                           bool isTransparent)
: mPosition(positionSize),
  mTitle(name),
  mRenderNotification(NULL),
  mColorDepth(isTransparent ? COLOR_DEPTH_32 : COLOR_DEPTH_24),
  mOwnSurface(false)
{

}

void WlRenderSurface::Init( Any surface )
{
  printf(" WlRenderSurface::Init \n");
  //@todo support surface being passed in
  mOwnSurface = true;

  mWaylandClient = new  Dali::Internal::Adaptor::WaylandClient;

  mWaylandClient->Initialise();
}

void WlRenderSurface::CreateSurface()
{
  printf("WlRenderSurface::CreateSurface \n");

  // Create the surface
  // A Wayland surface is a rectangular area that is displayed on the screen.
  // It has a location, size and pixel contents.
  mWindow.mSurface = wl_compositor_create_surface( mWaylandClient->mCompositor );

  DALI_ASSERT_ALWAYS(  mWindow.mSurface && "wl_compositor_create_surface failed" );

  // keep track of the surface id
  mWindow.mSurfaceId = wl_proxy_get_id( reinterpret_cast<struct wl_proxy *>(mWindow.mSurface ));

  // A surface in Wayland needs to be assigned a role.
  // This gives the surface the role of a shell surface.
  // It allows the surface to be treated like a toplevel, fullscreen or popup window.
  // which can be moved, resize or maximized and have associate metadata like title and class.
  mWindow.mShellSurface = wl_shell_get_shell_surface( mWaylandClient->mShell, mWindow.mSurface);

  DALI_ASSERT_ALWAYS(mWindow.mShellSurface && "wl_shell_get_shell_surface failed" );

  if( mTitle.length() )
  {
    wl_shell_surface_set_title(  mWindow.mShellSurface, mTitle.c_str() );
  }

   wl_shell_surface_set_toplevel( mWindow.mShellSurface);
 //  wl_shell_surface_set_fullscreen( mWindow.mShellSurface,   WL_SHELL_SURFACE_FULLSCREEN_METHOD_DEFAULT,
   //   0, NULL);

  wl_shell_surface_add_listener( mWindow.mShellSurface , &ShellSurfaceListener, 0);

  wl_display_roundtrip( mWaylandClient->mDisplay );
}

WlRenderSurface::~WlRenderSurface()
{

  delete mWaylandClient;

}

void WlRenderSurface::SetRenderNotification(TriggerEventInterface* renderNotification)
{
  mRenderNotification = renderNotification;
}

WlWindow* WlRenderSurface::GetWlWindow()
{
  return &mWindow;
}


void WlRenderSurface::AssignWindowEventInterface( Dali::Internal::Adaptor::WindowEventInterface* eventInterface)
{
  mWaylandClient->AssignWindowEventInterface( eventInterface );
}

WlWindow* WlRenderSurface::GetDrawable()
{
  return  &mWindow;
}

PositionSize WlRenderSurface::GetPositionSize() const
{
  return mPosition;
}

void WlRenderSurface::MoveResize( Dali::PositionSize positionSize )
{

}

void WlRenderSurface::SetViewMode( ViewMode viewMode )
{
}

unsigned int WlRenderSurface::GetSurfaceId( Any surface ) const
{
  // not used
  return 0;
}

} // namespace Wayland

} // namespace Dali
