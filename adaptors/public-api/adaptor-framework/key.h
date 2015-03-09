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
  DALI_KEY_INVALID          = -1,
  DALI_KEY_ESCAPE           =  0,
  DALI_KEY_BACK             =  1,
  DALI_KEY_CAMERA           =  2,
  DALI_KEY_CONFIG           =  3,
  DALI_KEY_POWER            =  4,
  DALI_KEY_PAUSE            =  5,
  DALI_KEY_CANCEL           =  6,
  DALI_KEY_PLAY_CD          =  7,
  DALI_KEY_STOP_CD          =  8,
  DALI_KEY_PAUSE_CD         =  9,
  DALI_KEY_NEXT_SONG        = 10,
  DALI_KEY_PREVIOUS_SONG    = 11,
  DALI_KEY_REWIND           = 12,
  DALI_KEY_FASTFORWARD      = 13,
  DALI_KEY_MEDIA            = 14,
  DALI_KEY_PLAY_PAUSE       = 15,
  DALI_KEY_MUTE             = 16,
  DALI_KEY_SEND             = 17,
  DALI_KEY_SELECT           = 18,
  DALI_KEY_END              = DALI_KEY_BACK,
  DALI_KEY_MENU             = DALI_KEY_SEND,
  DALI_KEY_HOME             = DALI_KEY_SELECT,
  DALI_KEY_HOMEPAGE         = 19,
  DALI_KEY_WEBPAGE          = 20,
  DALI_KEY_MAIL             = 21,
  DALI_KEY_SCREENSAVER      = 22,
  DALI_KEY_BRIGHTNESS_UP    = 23,
  DALI_KEY_BRIGHTNESS_DOWN  = 24,
  DALI_KEY_SOFT_KBD         = 25,
  DALI_KEY_QUICK_PANEL      = 26,
  DALI_KEY_TASK_SWITCH      = 27,
  DALI_KEY_APPS             = 28,
  DALI_KEY_SEARCH           = 29,
  DALI_KEY_VOICE            = 30,
  DALI_KEY_LANGUAGE         = 31,
  DALI_KEY_VOLUME_UP        = 32,
  DALI_KEY_VOLUME_DOWN      = 33,
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
