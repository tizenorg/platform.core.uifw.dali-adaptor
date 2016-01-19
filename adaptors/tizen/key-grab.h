#ifndef __DALI_KEY_GRAB_H__
#define __DALI_KEY_GRAB_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include "key.h"

namespace Dali
{
/**
 * @addtogroup dali_adaptor_framework
 * @{
 */

class Window;

/**
 * @brief Key grab functions.
 */
namespace KeyGrab
{

/**
 * @brief Grabs the key specfied by @a key for @a window only when @a window is the topmost window.
 *
 * This function can be used for following example scenarios:
 * - Mobile - Using volume up/down as zoom up/down in camera apps.
 *
 * @param[in] window The window to set
 * @param[in] dailKey The key code to grab (defined in key.h)
 * @return true if the grab succeeds.
 */
DALI_IMPORT_API bool GrabKeyTopmost( Window window, Dali::KEY daliKey );

/**
 * @brief Ungrabs the key specfied by @a key for @a window.
 *
 * @param[in] window The window to set
 * @param[in] dailKey The key code to ungrab (defined in key.h)
 * @return true if the ungrab succeeds.
 * @note If this function is called between key down and up events of a grabbed key,
 * an application doesn't receive the key up event.
 */
DALI_IMPORT_API bool UngrabKeyTopmost( Window window, Dali::KEY daliKey );

/**
 * @brief Key grab mode for platform-level APIs.
 */
enum KeyGrabMode
{
  TOPMOST = 0,             ///< Grab a key only when on the top of the grabbing-window stack mode.
  SHARED,                  ///< Grab a key together with the other client window(s) mode.
  OVERRIDE_EXCLUSIVE,      ///< Grab a key exclusively regardless of the grabbing-window's position on the window stack with the possibility of overriding the grab by the other client window mode.
  EXCLUSIVE                ///< Grab a key exclusively regardless of the grabbing-window's position on the window stack mode.
};

/**
 * @brief Grabs the key specfied by @a key for @a window in @a grabMode.
 *
 * @details This function can be used for following example scenarios:
 * - TV - A user might want to change the volume or channel of the background TV contents while focusing on the foregrund app.
 * - Mobile - When a user presses Home key, the homescreen appears regardless of current foreground app.
 * - Mobile - Using volume up/down as zoom up/down in camera apps.
 *
 * @param[in] window The window to set
 * @param[in] dailKey The key code to grab (defined in key.h)
 * @param[in] grabMode The grab mode for the key
 * @return true if the grab succeeds.
 */
DALI_IMPORT_API bool GrabKey( Window window, Dali::KEY daliKey, KeyGrabMode grabMode );

/**
 * @brief Ungrabs the key specfied by @a key for @a window.
 *
 * @param[in] window The window to set
 * @param[in] dailKey The key code to ungrab (defined in key.h)
 * @return true if the ungrab succeeds.
 * @note If this function is called between key down and up events of a grabbed key,
 * an application doesn't receive the key up event.
 */
DALI_IMPORT_API bool UngrabKey( Window window, Dali::KEY daliKey );

} // namespace KeyGrab

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_KEY_GRAB_H__
