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
 *
 */
// CLASS HEADER
#include "wayland-client.h"

// EXTERNAL INCLUDES
#include <stdio.h>
#include <cstring>

// INTERNAL INCLUDES
#include <base/interfaces/window-event-interface.h>
#include <base/interfaces/performance-interface.h>
#include <input/input-listeners.h>
#include <input/seat.h>

namespace Dali
{
namespace Internal
{
namespace Adaptor
{

namespace
{

void RegisterSeatListener( WaylandClient* client,
                          struct wl_registry *wlRegistry,
                          uint32_t name,
                          const char *interface,
                          uint32_t version)
{
  Dali::WlSeat* seatInterface = static_cast<Dali::WlSeat*>( wl_registry_bind( wlRegistry, name,  &wl_seat_interface, version ));

  Seat* seat = new Seat( &client->mInputManager, seatInterface );

  client->mInputManager.AddSeat( seat );

  // listen to seat events
  wl_seat_add_listener( seatInterface, Wayland::GetSeatListener(), &client->mInputManager );

}
void RegistryGlobalCallback( void *data,
           struct wl_registry *wlRegistry,
           uint32_t name,
           const char *interface,
           uint32_t version)
{

  WaylandClient* client = static_cast<   WaylandClient* >( data );

  if (strcmp( interface, wl_compositor_interface.name) == 0)
  {
    client->mCompositor = static_cast<Dali::WlCompositor*>(wl_registry_bind( wlRegistry, name , &wl_compositor_interface, version ));
  }
  else if (strcmp(interface, wl_shell_interface.name) == 0)
  {
    client->mShell = static_cast<Dali::WlShell*>(wl_registry_bind( wlRegistry, name , &wl_shell_interface, version));
  }
  else if( strcmp( interface, wl_seat_interface.name) == 0  )
  {
    // register for seat callbacks and add a new seat to the input manager
    RegisterSeatListener( client, wlRegistry, name, interface, version);
  }
  else if ( strcmp( interface, xdg_shell_interface.name ) == 0)
  {
    client->mXdgShell  =  static_cast< struct xdg_shell* >(wl_registry_bind(wlRegistry, name, &xdg_shell_interface, version));
    // without this line Tizen 3 reports...
    // xdg_shell@7: error 0: Must call use_unstable_version first
    xdg_shell_use_unstable_version(client->mXdgShell, 5);
   }


}


void RegistryGlobalCallbackRemove(void *data, struct wl_registry *registry, uint32_t id)
{
  // occurs when a device is unplugged
}

const struct wl_registry_listener RegistryListener =
{
   RegistryGlobalCallback,
   RegistryGlobalCallbackRemove,
};

} //unnamed namespace

WaylandClient::WaylandClient()
:mDisplay( NULL ),
 mShell( NULL ),
 mCompositor( NULL ),
 mXdgShell( NULL )
{
}
WaylandClient::~WaylandClient()
{
  wl_shell_destroy( mShell );

  wl_compositor_destroy( mCompositor );

  wl_display_disconnect( mDisplay );
}

void WaylandClient::Initialise()
{
  if( mDisplay )
  {
    return;
  }
  // Connect to a Wayland socket on the Wayland server/
  // NULL = use default display aka "wayland-0"
  // returns a new display context object

  mDisplay =  wl_display_connect( NULL );

  DALI_ASSERT_ALWAYS( mDisplay && "wl_display_connect failed");

  // Monitor the display file descriptor used to communicate with Wayland server
  InstallFileDescriptorMonitor();

  // Get the interfaces to compositor / shell etc
  GetWaylandInterfaces();

}

void WaylandClient::AssignWindowEventInterface( WindowEventInterface* eventInterface)
{
  mInputManager.AssignWindowEventInterface( eventInterface );
}
void WaylandClient::GetWaylandInterfaces()
{
  // get and listen to the registry
  WlRegistry* registry = wl_display_get_registry( mDisplay );

  wl_registry_add_listener( registry, &RegistryListener, this);

  // send and receive, to trigger the registry listener callback
  wl_display_roundtrip( mDisplay );

  wl_registry_destroy( registry );
}

void WaylandClient::InstallFileDescriptorMonitor()
{
  // get the file descriptor
  mDisplayFileDescriptor = wl_display_get_fd( mDisplay );

  // create the callback that gets triggered when a read / write event occurs
  CallbackBase* callback =  MakeCallback( this, &WaylandClient::FileDescriptorCallback);

  // monitor read and write events
  int events = FileDescriptorMonitor::FD_READABLE;

  mFileDescriptorMonitor = new FileDescriptorMonitor( mDisplayFileDescriptor, callback, events );

}
void WaylandClient::FileDescriptorCallback( FileDescriptorMonitor::EventType eventTypeMask )
{
  if( eventTypeMask & FileDescriptorMonitor::FD_READABLE )
  {
    // read and dispatch
    wl_display_dispatch( mDisplay );
  }
}

void WaylandClient::SetPerformanceInterface( PerformanceInterface* performanceInterface)
{
}

}
}
}