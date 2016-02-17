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
#include "wayland-event-manager.h"


// EXTERNAL INCLUES
#include <stdio.h>
#include <cstring>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <base/interfaces/window-event-interface.h>


namespace Dali
{
namespace Internal
{
namespace Adaptor
{

namespace
{

const int NO_EVENTS_ALREADY_IN_QUEUE = 0;
WlDisplay* gDisplay = NULL;

/*void RegisterSeatListener( WaylandEventManager* client,
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

}*/
void RegistryGlobalCallback( void *data,
           struct wl_registry *wlRegistry,
           uint32_t name,
           const char *interface,
           uint32_t version)
{

  printf(" registry callback \n");

  WaylandEventManager* client = static_cast<   WaylandEventManager* >( data );

  if( strcmp( interface, wl_seat_interface.name) == 0  )
  {
    // register for seat callbacks and add a new seat to the input manager
    Dali::WlSeat* seatInterface = static_cast<Dali::WlSeat*>( wl_registry_bind( wlRegistry, name,  &wl_seat_interface, version ));

    mInputManager.AddSeatListener( seatInterface );

    // RegisterSeatListener( client, wlRegistry, name, interface, version);
  }
  else if ( strcmp( interface, wl_output_interface.name ) == 0)
  {
    // get the interface and add the listener
    Dali::WlOutput* output =  static_cast< Dali::WlOutput* >(wl_registry_bind(wlRegistry, name, &wl_output_interface, version));
    client->mCompositorOutput.AddListener( output );
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

WaylandEventManager::WaylandEventManager()
:mDisplay( NULL ),
 mDisplayFileDescriptor( 0 ),
 mFileDescriptorMonitor( NULL )
{
}
WaylandEventManager::~WaylandEventManager()
{
  if( mDisplay )
  {
    wl_display_disconnect( mDisplay );
  }

  delete mFileDescriptorMonitor;
}


void WaylandEventManager::ReadAndDispatchEvents()
{
  // Wayland client uses a single file descriptor to communicate with the compositor.
  // Because DALi can have multiple client threads ( event thread for input, render thread for rendering )
  // it has to use the Wayland client thread safe API to prevent a dead lock

  // prepare_read announces the calling thread's intention to read from the file descriptor
  // If there is already events queued up in the default queue, then dispatch those first
  while( wl_display_prepare_read( mDisplay ) != NO_EVENTS_ALREADY_IN_QUEUE )
  {
    // dispatch the event, e.g. a touch event or a clipboard event
    wl_display_dispatch_pending( mDisplay );
  }

  // At this point the default queue is empty.
  // We read data from the file descriptor in their respective queues
  // This is thread safe. No other threads will read from the fd and queue events during this operation.
  int ret = wl_display_read_events( mDisplay );

  if( ret == 0 )
  {
    // dispatch the events from the default queue
    wl_display_dispatch_pending( mDisplay );
  }
  else
  {
    DALI_LOG_ERROR("wl_display_read_events error");
  }

}

void WaylandEventManager::Initialise()
{
  if( mDisplay )
  {
    return;
  }
  printf(" connecting to display \n");
  mDisplay =  wl_display_connect( NULL );
  gDisplay = mDisplay;

  DALI_ASSERT_ALWAYS( mDisplay && "wl_display_connect failed");

  // get the file descriptor
  mDisplayFileDescriptor = wl_display_get_fd( mDisplay );

  // Get the interfaces for listening to input events / dpi / clipboard etc
  GetWaylandInterfaces();

  // Monitor the display file descriptor used to communicate with Wayland server
  InstallFileDescriptorMonitor();

}

void WaylandEventManager::AssignWindowEventInterface( WindowEventInterface* eventInterface)
{
  mInputManager.AssignWindowEventInterface( eventInterface );
}
void WaylandEventManager::GetWaylandInterfaces()
{
  struct pollfd pfd[2];

  // get and listen to the registry
  WlRegistry* registry = wl_display_get_registry( mDisplay );

  wl_registry_add_listener( registry, &RegistryListener, this);

  // adaptor wants the DPI instantly, so we have wait for the data
  while( !mCompositorOutput.DataReady() )
  {
    // This is the first and last time we use wl_display_roundtrip as its not thread safe
    // however at this point no other DALithreads have started
    wl_display_flush( mDisplay );
    wl_display_roundtrip( mDisplay );
  }
  wl_registry_destroy( registry );
}

void WaylandEventManager::GetDpi(unsigned int& dpiHorizontal, unsigned int& dpiVertical)
{
  mCompositorOutput.GetDpi( dpiHorizontal, dpiVertical );
}

void WaylandEventManager::InstallFileDescriptorMonitor()
{
  // create the callback that gets triggered when a read / write event occurs
  CallbackBase* callback =  MakeCallback( this, &WaylandEventManager::FileDescriptorCallback);

  // monitor read and write events
  int events = FileDescriptorMonitor::FD_READABLE;

  mFileDescriptorMonitor = new FileDescriptorMonitor( mDisplayFileDescriptor, callback, events );

}
void WaylandEventManager::FileDescriptorCallback( FileDescriptorMonitor::EventType eventTypeMask )
{
  if( eventTypeMask & FileDescriptorMonitor::FD_READABLE )
  {
    // read and dispatch events
    ReadAndDispatchEvents();
  }
}

}
}
}
