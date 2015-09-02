#ifndef __DALI_PURE_WAYLAND_CLIENT_H__
#define __DALI_PURE_WAYLAND_CLIENT_H__

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
#include <wl-types.h>

// INTERNAL INCLUDES
#include <file-descriptor-monitor.h>
#include "pure-wayland-input.h"



namespace Dali
{

namespace Internal
{

namespace Adaptor
{

class WindowEventInterface;

/**
 * Client used to talk to Wayland server over a UNIX domain stream socket.
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
class WaylandClient
{

public:

  /**
   * @brief Constructor
   */
  WaylandClient();

  /**
   * @brief Destructor
   */
  ~WaylandClient();

  /**
   * @brief Connect to Wayland server and setup internal data structures
   */
  void Initialise();

  /**
   *
   */
  void AssignWindowEventInterface( WindowEventInterface* eventInterface);


public: // change to private

  /**
   * @brief
   */
  void InstallFileDescriptorMonitor();

  void FileDescriptorCallback( FileDescriptorMonitor::EventType eventTypeMask );

  void GetWaylandInterfaces();

  WaylandInput  mInputManager;
  WlDisplay* mDisplay;        ///< Wayland display, handles all the data sent from and to the compositor
  WlShell* mShell;            ///< shell
  WlCompositor* mCompositor;  ///< compositor
  WlXdgShell* mXdgShell;
  int mDisplayFileDescriptor;
  FileDescriptorMonitor* mFileDescriptorMonitor;

};
} // Internal
} // Adaptor
} // Dali

#endif
