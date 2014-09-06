#ifndef __DALI_KEY_H__
#define __DALI_KEY_H__

/*
Copyright (c) 2000-2014 Samsung Electronics Co., Ltd All Rights Reserved

This file is part of Dali Adaptor

PROPRIETARY/CONFIDENTIAL

This software is the confidential and proprietary information of
SAMSUNG ELECTRONICS ("Confidential Information"). You shall not
disclose such Confidential Information and shall use it only in
accordance with the terms of the license agreement you entered
into with SAMSUNG ELECTRONICS.

SAMSUNG make no representations or warranties about the suitability
of the software, either express or implied, including but not limited
to the implied warranties of merchantability, fitness for a particular
purpose, or non-infringement. SAMSUNG shall not be liable for any
damages suffered by licensee as a result of using, modifying or
distributing this software or its derivatives.
*/

/**
 * @addtogroup CAPI_DALI_FRAMEWORK
 * @{
 */

// EXTERNAL INCLUDES

#include <dali/public-api/events/key-event.h>

namespace Dali DALI_IMPORT_API
{

/**
 * Mapping of keyboard and mouse button event keycodes to platform specific codes.
 * For tizen the X Server Keycode is used as reference, unless it's over ridden
 * in utilX.h in which case the values are based on utilX.h
 */

enum KEY
{
DALI_KEY_INVALID = -1,
DALI_KEY_ESCAPE = 9,        // hardcoded value to be removed when toolkit/demo starts using IsKey
DALI_KEY_BACK   = 166,      // hardcoded value to be removed when toolkit/demo starts using IsKey
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

};


/**
 * Check if a key event is for a specific DALI KEY.
 * @param keyEvent reference to a keyEvent structure
 * @param daliKey dali key enum
 * @return true if the key is matched, false if not
 */
bool IsKey( const KeyEvent& keyEvent, KEY daliKey);

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_KEY_H__
