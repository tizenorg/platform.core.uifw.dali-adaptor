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

// INTERNAL INCLUDES
#include <wl-types.h>
#include <trigger-event.h>

namespace Dali
{

#if defined(DEBUG_ENABLED)
Debug::Filter* gRenderSurfaceLogFilter = Debug::Filter::New(Debug::Verbose, false, "LOG_WAYLAND_RENDER_SURFACE");
#endif

namespace Wayland
{

namespace
{


void RegistryGlobalCallback( void *data,
           struct wl_registry *wlRegistry,
           uint32_t name,
           const char *interface,
           uint32_t version)
{

  WlWindow* window = static_cast<   WlWindow* >( data );

  printf("Wayland registry callback with %s \n",interface);

  if (strcmp( interface, wl_compositor_interface.name) == 0)
  {
    window->mCompositor = static_cast<Dali::WlCompositor*>(wl_registry_bind( wlRegistry, name , &wl_compositor_interface, version ));
  }
  else if (strcmp(interface, wl_shell_interface.name) == 0)
  {
    window->mShell = static_cast<Dali::WlShell*>(wl_registry_bind( wlRegistry, name , &wl_shell_interface, version));
  }
}

void RegistryGlobalCallbackRemove(void *data, struct wl_registry *registry, uint32_t id)
{
  // occurs when a device is unplugged
  printf("registry losing event %d\n", id);
}

const struct wl_registry_listener RegistryListener =
{
   RegistryGlobalCallback,
   RegistryGlobalCallbackRemove,
};

void ShellSurfacePing(void *data, struct wl_shell_surface *shell_surface, uint32_t serial)
{
  static int x = 0;
  x++;
  if(( x % 1000 ) == 0)
  {
    printf(" ShellSurfacePing \n");
  }
  wl_shell_surface_pong(shell_surface, serial);
}

void ShellSurfaceConfigure(void *data, struct wl_shell_surface *shell_surface, uint32_t edges, int32_t width, int32_t height)
{

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

  // we own the surface about to created
  // NULL = use default display aka "wayland-0"
  mWindow.mDisplay =  wl_display_connect( NULL );

  DALI_ASSERT_ALWAYS( mWindow.mDisplay && "wl_display_connect failed");

//  mWindow.mDisplayFileDescriptor = wl_display_get_fd( mWindow.mDisplay );

  mWindow.mRegistry = wl_display_get_registry( mWindow.mDisplay  );

  wl_registry_add_listener(  mWindow.mRegistry, &RegistryListener, &mWindow);

  // this will block
  printf(" WlRenderSurface::dispatch \n");

  wl_display_dispatch( mWindow.mDisplay);

  printf(" WlRenderSurface::roundtrip \n");
  wl_display_roundtrip( mWindow.mDisplay);
  printf(" WlRenderSurface::roundtrip finished \n");

}

void WlRenderSurface::CreateSurface()
{
  printf("WlRenderSurface::CreateSurface \n");
  mWindow.mSurface = wl_compositor_create_surface(mWindow.mCompositor);

  DALI_ASSERT_ALWAYS(  mWindow.mSurface && "wl_compositor_create_surface failed" );

  mWindow.mShellSurface = wl_shell_get_shell_surface(mWindow.mShell, mWindow.mSurface);

  DALI_ASSERT_ALWAYS(  mWindow.mShellSurface && "wl_shell_get_shell_surface failed" );

  wl_shell_surface_set_toplevel( mWindow.mShellSurface);
 // wl_shell_surface_set_fullscreen( mWindow.mShellSurface);

  wl_shell_surface_add_listener( mWindow.mShellSurface , &ShellSurfaceListener, 0);

  wl_display_dispatch( mWindow.mDisplay);
  wl_display_roundtrip( mWindow.mDisplay);
  printf(" CreateSurface end \n");
}

WlRenderSurface::~WlRenderSurface()
{
  wl_shell_destroy( mWindow.mShell );

  wl_compositor_destroy( mWindow.mCompositor );

  wl_display_disconnect( mWindow.mDisplay );
}

void WlRenderSurface::SetRenderNotification(TriggerEventInterface* renderNotification)
{
  mRenderNotification = renderNotification;
}

WlWindow* WlRenderSurface::GetWlWindow()
{
  return &mWindow;
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
