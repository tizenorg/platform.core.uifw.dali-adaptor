#ifndef __DALI_WAYLAND_SURFACE_MANAGER_H__
#define __DALI_WAYLAND_SURFACE_MANAGER_H__

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
class PerformanceInterface;

/**
 * Responsible for talking to Wayland compositor to control surfaces.
 *
 *
 *
 */
class WaylandSurfaceManager
{

public:

  /**
   * @brief Constructor
   */
  WaylandSurfaceManager();

  /**
   * @brief Destructor
   */
  ~WaylandSurfaceManager();

  /**
   * @brief Connect to Wayland server and setup internal data structures
   */
  void Initialise();

  /**
   * @brief create a surface for a window
   * @param[in] window window object
   */
  void CreateSurface( Dali::Wayland::Window& window );

  /**
   * @brief get the wayland surface
   * @return surface
   */
  WlSurface* GetSurface();

private: // change to private

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
   * @brief helper to get wayland interfaces
   */
  void GetWaylandInterfaces();

public:


  WlDisplay* mDisplay;        ///< Wayland display, handles all the data sent from and to the compositor
  WlShell* mShell;            ///< shell
  WlCompositor* mCompositor;  ///< compositor
  int mDisplayFileDescriptor; ///< File descriptor used by wayland client socket
  FileDescriptorMonitor* mFileDescriptorMonitor;  ///< File descriptor monitor
  WlXdgShell* mXdgShell;                ///< XDG Shell
  WlSurface* mSurface;                  ///< Wayland surface
  WlShellSurface* mShellSurface;        ///< Shell surface
  WlXdgShellSurface*  mXdgSurface;      ///< XDG Shell surface


};
} // Internal
} // Adaptor
} // Dali

#endif
