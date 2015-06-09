#ifndef __DALI_INTERNAL_X_EVENT_MANAGER_H__
#define __DALI_INTERNAL_X_EVENT_MANAGER_H__

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
#include <X11/X.h>
#include <X11/Xlib.h>
#include <file-descriptor-monitor.h>

// INTERNAL INCLUDES
#include <base/interfaces/window-event-interface.h>
#include "x-input2.h"

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

/**
 *
 * @brief Used to handle X events.
 * The code is mainloop agnostic, so the monitoring of the X event file descriptor
 * for X events is external to this class.
 *
 *
 */
class XEventManager
{
  public:

  /**
   * Constructor
   */
  XEventManager( XID window, Display* display, WindowEventInterface* eventInterface );

  /**
   * Destructor
   */
  ~XEventManager();

  /**
   * @brief enumerates input devices using XIQueryDevice then sets up event filtering using XISelectEvents
   * @param  eventInterface used to receive events
   * @return true on success, false on failure.
   */
  void Initialize();


  //void ProcessEvent( XGenericEventCookie* cookie );

  private:

  /**
   * @brief Should be called when the Event file descriptor signals data is available
   */
  void XEventReceived();

  XInput2 mXInput2;
  FileDescriptorMonitor* mFileDescriptorMonitor;
  Display* mDisplay;
  XID mWindow;
  bool mInitialized:1;    // whether multi-touch is supported


};

} // namespace Adaptor
} // namespace Internal
} // namespace Dali

#endif // __DALI_INTERNAL_X_EVENT_MANAGER_H__
