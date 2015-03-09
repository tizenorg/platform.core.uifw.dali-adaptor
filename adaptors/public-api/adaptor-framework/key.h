#ifndef __DALI_KEY_H__
#define __DALI_KEY_H__

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

// EXTERNAL INCLUDES

#include <dali/public-api/events/key-event.h>

namespace Dali DALI_IMPORT_API
{

/**
 * @brief Platform independent keyboard & button event codes.
 */
enum KEY
{
  DALI_KEY_INVALID,
  DALI_KEY_ESCAPE,
  DALI_KEY_BACK,
  DALI_KEY_CAMERA,
  DALI_KEY_CONFIG,
  DALI_KEY_POWER,
  DALI_KEY_PAUSE,
  DALI_KEY_CANCEL,
  DALI_KEY_PLAY_CD,
  DALI_KEY_STOP_CD,
  DALI_KEY_PAUSE_CD,
  DALI_KEY_NEXT_SONG,
  DALI_KEY_PREVIOUS_SONG,
  DALI_KEY_REWIND,
  DALI_KEY_FASTFORWARD,
  DALI_KEY_MEDIA,
  DALI_KEY_PLAY_PAUSE,
  DALI_KEY_MUTE,
  DALI_KEY_SEND,
  DALI_KEY_SELECT,
  DALI_KEY_END,
  DALI_KEY_MENU,
  DALI_KEY_HOME,
  DALI_KEY_HOMEPAGE,
  DALI_KEY_WEBPAGE,
  DALI_KEY_MAIL,
  DALI_KEY_SCREENSAVER,
  DALI_KEY_BRIGHTNESS_UP,
  DALI_KEY_BRIGHTNESS_DOWN,
  DALI_KEY_SOFT_KBD,
  DALI_KEY_QUICK_PANEL,
  DALI_KEY_TASK_SWITCH,
  DALI_KEY_APPS,
  DALI_KEY_SEARCH,
  DALI_KEY_VOICE,
  DALI_KEY_LANGUAGE,
  DALI_KEY_VOLUME_UP,
  DALI_KEY_VOLUME_DOWN,
  DALI_KEY_BACKSPACE,
  DALI_KEY_CURSOR_LEFT,
  DALI_KEY_CURSOR_RIGHT,
  DALI_KEY_CURSOR_UP,
  DALI_KEY_CURSOR_DOWN,
  DALI_KEY_TAB,
  DALI_KEY_SPACE,
  DALI_KEY_RETURN
};

/**
 * @brief Check if a key event is for a specific DALI KEY.
 *
 * @param keyEvent reference to a keyEvent structure
 * @param daliKey dali key enum
 * @return true if the key is matched, false if not
 */
bool IsKey( const KeyEvent& keyEvent, KEY daliKey);

} // namespace Dali

#endif // __DALI_KEY_H__
