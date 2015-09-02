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

bool gDisplayReady = false;
WlDisplay* gDisplay = NULL;

void IdleCallback( uv_idle_t* handle )
{
  int ret = wl_display_flush(gDisplay);
  printf(" wl_display_flush = %d \n", ret);


  ret = wl_display_dispatch_pending(gDisplay);

  printf(" wl_display_dispatch_pending = %d \n", ret);

}

void PollCabllack(uv_poll_t* handle, int status, int events)
{
  printf(" got a poll callback with status: %d, events %d \n",status,events);
   if( handle->data )
   {

     WlRenderSurface* surface = static_cast<WlRenderSurface* >(handle->data);
        // run the function

     if(  events &  UV_READABLE )
     {
       surface->DisplayFileDescriptorRead();
     }
     if(  events &  UV_READABLE )
     {
        surface->DisplayFileDescriptorWrite();
     }
   }
}

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

void SyncCallback(void *data, struct wl_callback *callback, uint32_t serial)
{


 // WlRenderSurface* surface = static_cast<WlRenderSurface* >(data);
  gDisplayReady = true;
  //surface->mDisplayReady = true;
  //surface->mTemp = "hello";

  printf(" wayland sync callback, display ready == true\n");

 // wl_callback_destroy(callback);

}
const struct wl_callback_listener InitSyncListener =
{
   SyncCallback
};


void ShellSurfacePing(void *data, struct wl_shell_surface *shell_surface, uint32_t serial)
{

    printf(" ShellSurfacePing \n");

  wl_shell_surface_pong(shell_surface, serial);
}

void ShellSurfaceConfigure(void *data, struct wl_shell_surface *shell_surface, uint32_t edges, int32_t width, int32_t height)
{
  printf(" ShellSurfaceConfigure \n");
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
  mOwnSurface(false),
  mDisplayReady( false )
{
  gDisplayReady = false;
}

void WlRenderSurface::Init( Any surface )
{
  printf(" WlRenderSurface::Init \n");
  //@todo support surface being passed in
  mOwnSurface = true;

  // we own the surface about to created
  // NULL = use default display aka "wayland-0"
  mWindow.mDisplay =  wl_display_connect( NULL );

  gDisplay = mWindow.mDisplay;


  DALI_ASSERT_ALWAYS( mWindow.mDisplay && "wl_display_connect failed");

  mWindow.mDisplayFileDescriptor = wl_display_get_fd( mWindow.mDisplay );

  InstallFileDescriptorMonitor();

  mWindow.mRegistry = wl_display_get_registry( mWindow.mDisplay  );

  wl_registry_add_listener(  mWindow.mRegistry, &RegistryListener, &mWindow);

  struct wl_callback *callback;

  callback = wl_display_sync( mWindow.mDisplay );

  wl_callback_add_listener(callback, &InitSyncListener, mWindow.mDisplay);

  // this will block
  printf(" WlRenderSurface::dispatch \n");

 // Wait for display to be ready..

  sleep(1);
  while (gDisplayReady == false)
  {
    printf(" display not ready, calling dispatch \n");
    wl_display_dispatch( mWindow.mDisplay);
  }
  sleep(1);

  printf(" display NOW READY\n");

 // printf(" WlRenderSurface::roundtrip \n");
 // wl_display_roundtrip( mWindow.mDisplay);
//  printf(" WlRenderSurface::roundtrip finished \n");




}

// The user should therefore always be prepared to handle EAGAIN or equivalent when it attempts to read from or write to the fd.

void WlRenderSurface::InstallFileDescriptorMonitor()
{
  printf(" installing file descriptor monitor on FD %d  \n",   mWindow.mDisplayFileDescriptor);
  // mWindow.mDisplayFileDescriptor
  mDisplayPollHandle = new uv_poll_t;

  uv_poll_init( uv_default_loop(), mDisplayPollHandle,  mWindow.mDisplayFileDescriptor);

  mDisplayPollHandle->data = this;

  uv_poll_start( mDisplayPollHandle, UV_READABLE | UV_WRITABLE, PollCabllack);

  ////////////////// Install idle callback

  mIdleCallbackHandle = new uv_idle_t;

  uv_idle_init( uv_default_loop() , mIdleCallbackHandle );

  uv_idle_start( mIdleCallbackHandle, IdleCallback);

}

void WlRenderSurface::DisplayFileDescriptorRead()
{
  wl_display_dispatch( mWindow.mDisplay);
}
void WlRenderSurface::DisplayFileDescriptorWrite()
{
  wl_display_flush( mWindow.mDisplay);
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


  //callback = wl_display_sync(_ecore_wl_disp->wl.display);
   //      wl_callback_add_listener(callback, &_ecore_wl_init_sync_listener,
     //                             _ecore_wl_disp);

 // wl_display_dispatch( mWindow.mDisplay);


 // wl_display_roundtrip( mWindow.mDisplay);



 // printf(" Round trip finished \n");
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
