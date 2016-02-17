#ifndef __DALI_WAYLAND_EVENT_MANAGER_H__
#define __DALI_WAYLAND_EVENT_MANAGER_H__

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

// EXTERNAL INCLUDES
#include <wayland-extension/xdg-shell-client-protocol.h>   // from wayland

// INTERNAL INCLUDES
#include <wl-types.h>
#include <file-descriptor-monitor.h>
#include "input-manager.h"
#include "wayland-window.h"
#include "compositor-output-region/compositor-output.h"

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

class WindowEventInterface;

/**
 * Client used to talk to Wayland server over a UNIX domain stream socket.
 * This client runs on DALi's main thread and is used for listening to events such
 * as input, DPI, clipboard and IMF. No wayland shell / surface operations take place.
 *
 *
 * Brief overview of Wayland:
 *
 * Transport mechanism = Socket
 * Display = handles all the data sent from and to the compositor
 * Display has a file descriptor that can be monitored for read / write events
 *
 * wl client function calls will place messages in a queue
 * Calling wl_display_flush() will flush the messages to  the server
 *
 * Incoming data is handled in two steps: queueing and dispatching.
 * In the queue step, the data coming from the display fd is interpreted and
 * added to a queue. On the dispatch step, the handler for the incoming event is called.
 *
 * default queue is dispatched by calling wl_display_dispatch().
 *
 * The compositor sends out the frame event every time it draws a frame.
 * wl_display_frame_callback() to schedule a callback per frame.
 *
 *
 * wl_display_dispatch(). This will dispatch any events queued on the default queue and
 * attempt to read from the display fd if it's empty.
 * Events read are then queued on the appropriate queues according to the proxy assignment.
 *
 *
 */
class WaylandEventManager
{

public:

  /**
   * @brief Constructor
   */
  WaylandEventManager();

  /**
   * @brief Destructor
   */
  ~WaylandEventManager();

  /**
   * @brief Connect to Wayland server and setup internal data structures
   * @note no other wayland client threads should be running at this point
   */
  void Initialise();

  /**
   * @brief Assign window event interface.
   * @param[in] eventInterface window event interface
   */
  void AssignWindowEventInterface( WindowEventInterface* eventInterface);

  /**
   * @brief Get DPI
   * @param[out] dpiHorizontal set to the horizontal dpi
   * @param[out] dpiVertical set to the vertical dpi
   */
  void GetDpi(unsigned int& dpiHorizontal, unsigned int& dpiVertical);

  /**
   * @brief get the display ( communication object ) to the compositor
   */
  static WlDisplay* GetDisplay();

private:

  /**
   * @brief Install file descriptor monitor
   */
  void InstallFileDescriptorMonitor();

  /**
   * @brief File descriptor callback function, triggered when wayland compositor
   * sends an event to the client (us)
   * @param[in] eventTypeMask
   */
  void FileDescriptorCallback( FileDescriptorMonitor::EventType eventTypeMask );

  /**
   * @brief Reads and dispatches any events from the Wayland compositor
   * We have a file descriptor monitor active to decide when to call this function
   */
  void ReadAndDispatchEvents();

  /**
   * @brief helper to get wayland interfaces
   */
  void GetWaylandInterfaces();

public:


  InputManager  mInputManager;
  CompositorOutput mCompositorOutput;     ///< handles monitor information and DPI
  WlDisplay* mDisplay;                    ///< Wayland display, handles data sent from and to the compositor
  int mDisplayFileDescriptor;             ///< File descriptor used by wayland client socket
  FileDescriptorMonitor* mFileDescriptorMonitor;  ///< File descriptor monitor



};
} // Internal
} // Adaptor
} // Dali

#endif
