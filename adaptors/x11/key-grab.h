#ifndef __DALI_KEY_GRAB_H__
#define __DALI_KEY_GRAB_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali
{
class Window;

typedef int KEY;

namespace KeyGrab
{

/**
 * @brief Key grab mode.
 */
enum KeyGrabMode
{
  TOP_POSITION = 0,  ///< Grab a key only when on the top of the grabbing-window stack mode.
  SHARED             ///< Grab a key together with the other client window(s) mode.
};

/**
 * @brief Grabs the key specfied by @a key for @a window in @a grabMode.
 *
 * The key grab feature is designed for following example scenarios:
 * TV - A user might want to change the volume or channel of the background TV contents while focusing on the foregrund app.
 * Mobile - When a user presses Home key, the homescreen appears regardless of current foreground app.
 *
 * @note This function is only specified by Tizen.
 * @param[in] window The window to set
 * @param[in] dailKey The key code to grab (defined in key.h)
 * @param[in] grabMode The grab mode for the key
 * @return true if the grab succeed.
 */
DALI_IMPORT_API bool GrabKey( Window window, Dali::KEY daliKey, KeyGrabMode grabMode );

/**
 * @brief Ungrabs the key specfied by @a key for @a window.
 * @note This function is only specified by Tizen.
 * @param[in] window The window to set
 * @param[in] dailKey The key code to ungrab (defined in key.h)
 * @return true if the ungrab succeed.
 */
DALI_IMPORT_API bool UngrabKey( Window window, Dali::KEY daliKey );

} // namespace KeyGrab

} // namespace Dali

#endif // __DALI_KEY_GRAB_H__
