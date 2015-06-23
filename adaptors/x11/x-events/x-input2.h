#ifndef __DALI_INTERNAL_X_INPUT_2_MANAGER_H__
#define __DALI_INTERNAL_X_INPUT_2_MANAGER_H__

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
#include <X11/extensions/XInput2.h>
#include <X11/extensions/XI2.h>
#include <dali/public-api/common/dali-vector.h>

// INTERNAL INCLUDES
#include <base/interfaces/window-event-interface.h>
#include "x-input2-device.h"

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

/**
 *
 * @brief Used to setup and process XInput2 events.
 *
 * For help debugging to list XInput devices in Linux type 'xinput -help'
 */
class XInput2
{

public:

  /**
   * Constructor
   */
  XInput2( XID window, Display* display, WindowEventInterface* eventInterface );

  /**
   * Destructor
   */
  ~XInput2();

  /**
   * @brief enumerates input devices using XIQueryDevice then sets up event filtering using XISelectEvents
   */
  void Initialize();

  /**
   * @brief get X the extension id
   * @return the Id
   */
  int GetExtensionId() const;

  /**
   * @brief process an XInput2 event
   * @param cookie X cookie
   */
  void ProcessEvent( XGenericEventCookie* cookie );


private:

  /**
   * @brief query x input devices
   */
  void QueryDevices();

  /**
   * @brief query multi-touch support
   */
  void QueryMultiTouchSupport();

  /**
   *
   */
  void SelectInputEvents();

  bool FilteredDevice( int deviceId ) const;

  void SelectEvents( int deviceId, const Dali::Vector< unsigned int >& filter );

  bool PreProcessEvent(  XIDeviceEvent *deviceEvent );

  void CreateKeyEvent( const XIDeviceEvent* deviceEvent, KeyEvent& keyEvent ) const;

  Dali::Vector< XInput2Device > mInputDeviceInfo; ///< Device info list


  WindowEventInterface* mEventInterface;
  Display* mDisplay;
  XID mWindow;
  int mXI2ExtensionId;
  bool mMultiTouchSupport:1;    // whether multi-touch is supported
};

} // namespace Adaptor
} // namespace Internal
} // namespace Dali

#endif
