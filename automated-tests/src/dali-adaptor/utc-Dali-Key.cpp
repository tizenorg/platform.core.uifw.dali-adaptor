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
#include <map>
#include <string.h>
#include <iostream>

// CLASS HEADER
#include <stdlib.h>
#include <iostream>
#include <dali.h>
#include <dali-test-suite-utils.h>

using namespace Dali;

void utc_dali_adaptor_key_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_adaptor_key_cleanup(void)
{
  test_return_value = TET_PASS;
}

// Copied from key-impl.cpp
struct KeyLookup
{
  const char* keyName;      ///< X string representation
  const int   daliKeyCode;  ///< Dali Enum Representation
  const bool  deviceButton; ///< Whether the key is from a button on the device
};

// Taken from utilX.h
#define KEY_VOLUMEUP		"XF86AudioRaiseVolume"	/**< this macro means the XKeySym (XServer Key Symbol) corresponds to 'Volume Up' key */
#define KEY_VOLUMEDOWN		"XF86AudioLowerVolume"	/**< this macro means the XKeySym (XServer Key Symbol) corresponds to 'Volume Down' key */

#define KEY_CAMERA		"XF86WebCam"	/**< this macro means the XKeySym (XServer Key Symbol) corresponds to 'Half-Press of Camera' key */
#define KEY_CONFIG		"XF86Pictures"	/**< this macro means the XKeySym (XServer Key Symbol) corresponds to 'Full-Press of Camera' key */

#define KEY_POWER		"XF86PowerOff"	/**< this macro means the XKeySym (XServer Key Symbol) corresponds to 'Power' key */
#define KEY_PAUSE		"XF86Standby"	/**< this macro means the XKeySym (XServer Key Symbol) corresponds to 'Pause' key */
#define KEY_CANCEL              "Cancel"        /**< this macro means the XKeySym (XServer Key Symbol) corresponds to 'Cancel' key */

// Earjack/BT Headset/Multimedia keys
#define KEY_PLAYCD		"XF86AudioPlay"	/**< this macro means the XKeySym (XServer Key Symbol) corresponds to 'Play Audio' key */
#define KEY_STOPCD		"XF86AudioStop"	/**< this macro means the XKeySym (XServer Key Symbol) corresponds to 'Stop Audio' key */
#define KEY_PAUSECD		"XF86AudioPause"	/**< this macro means the XKeySym (XServer Key Symbol) corresponds to 'Pause Audio' key */
#define KEY_NEXTSONG		"XF86AudioNext"	/**< this macro means the XKeySym (XServer Key Symbol) corresponds to 'Next Song' key */
#define KEY_PREVIOUSSONG	"XF86AudioPrev"	/**< this macro means the XKeySym (XServer Key Symbol) corresponds to 'Previous Song' key */
#define KEY_REWIND		"XF86AudioRewind"	/**< this macro means the XKeySym (XServer Key Symbol) corresponds to 'Rewind Song' key */
#define KEY_FASTFORWARD		"XF86AudioForward"	/**< this macro means the XKeySym (XServer Key Symbol) corresponds to 'Forward Song' key */
#define KEY_MEDIA		"XF86AudioMedia"	/**< this macro means the XKeySym (XServer Key Symbol) corresponds to 'Media' key */
#define KEY_PLAYPAUSE		"XF86AudioPlayPause"	/**< this macro means the XKeySym (XServer Key Symbol) corresponds to 'PlayPause' key */
#define KEY_MUTE			"XF86AudioMute"	/**< this macro means the XKeySym (XServer Key Symbol) corresponds to 'Mute' key */

// 3-Touch key
#define KEY_SEND		"XF86Send"	/**< this macro means the XKeySym (XServer Key Symbol) corresponds to 'Send' key */
#define KEY_SELECT		"XF86Phone"	/**< this macro means the XKeySym (XServer Key Symbol) corresponds to 'Home' key */
#define KEY_END			"XF86Stop"	/**< this macro means the XKeySym (XServer Key Symbol) corresponds to 'End' key */

// Renamed 3-Touch key
#define KEY_MENU		"XF86Send"	/**< this macro means the XKeySym (XServer Key Symbol) corresponds to 'Send' key */
#define KEY_HOME		"XF86Phone"	/**< this macro means the XKeySym (XServer Key Symbol) corresponds to 'Home' key */
#define KEY_BACK		"XF86Stop"	/**< this macro means the XKeySym (XServer Key Symbol) corresponds to 'End' key */

//Other functions keys
#define KEY_HOMEPAGE		"XF86HomePage"	/**< this macro means the XKeySym (XServer Key Symbol) corresponds to 'HomePage' key */
#define KEY_WEBPAGE		"XF86WWW"	/**< this macro means the XKeySym (XServer Key Symbol) corresponds to 'WWW' key */
#define KEY_MAIL			"XF86Mail"	/**< this macro means the XKeySym (XServer Key Symbol) corresponds to 'Mail' key */
#define KEY_SCREENSAVER	"XF86ScreenSaver"	/**< this macro means the XKeySym (XServer Key Symbol) corresponds to 'ScreenSaver' key */
#define KEY_BRIGHTNESSUP 	"XF86MonBrightnessUp"	/**< this macro means the XKeySym (XServer Key Symbol) corresponds to 'BrightnessUp' key */
#define KEY_BRIGHTNESSDOWN	"XF86MonBrightnessDown"	/**< this macro means the XKeySym (XServer Key Symbol) corresponds to 'BrightnessDown' key */
#define KEY_SOFTKBD			"XF86MenuKB"	/**< this macro means the XKeySym (XServer Key Symbol) corresponds to 'Soft keyboard(toggle)' key */
#define KEY_QUICKPANEL		"XF86Tools"	/**< this macro means the XKeySym (XServer Key Symbol) corresponds to 'Quick panel(toggle)' key */
#define KEY_TASKSWITCH		"XF86TaskPane"	/**< this macro means the XKeySym (XServer Key Symbol) corresponds to 'Task switcher(toggle)' key */
#define KEY_APPS		"XF86Launch0"	/**< this macro means the XKeySym (XServer Key Symbol) corresponds to 'Apptray(toggle)' key */
#define KEY_SEARCH		"XF86Search"		/**< this macro means the XKeySym (XServer Key Symbol) corresponds to 'Search(toggle)' key */
#define KEY_VOICE		"XF86Launch2"		/**< this macro means the XKeySym (XServer Key Symbol) corresponds to 'Voice(toggle)' key */
#define KEY_LANGUAGE		"Hangul"		/**< this macro means the XKeySym (XServer Key Symbol) corresponds to 'Language(toggle)' key */

// Taken from key-impl.cpp
KeyLookup KeyLookupTable[]=
{
  // KeyName                DALi Equivalent           true if device key
  { "Escape",               DALI_KEY_ESCAPE,          false },
  { "Menu",                 DALI_KEY_MENU,            false },
  { KEY_CAMERA,             DALI_KEY_CAMERA,          false },
  { KEY_CONFIG,             DALI_KEY_CONFIG,          false },
  { KEY_POWER,              DALI_KEY_POWER,           true  },
  { KEY_PAUSE,              DALI_KEY_PAUSE,           false },
  { KEY_CANCEL,             DALI_KEY_CANCEL,          false },
  { KEY_PLAYCD,             DALI_KEY_PLAY_CD,         false },
  { KEY_STOPCD,             DALI_KEY_STOP_CD,         false },
  { KEY_PAUSECD,            DALI_KEY_PAUSE_CD,        false },
  { KEY_NEXTSONG,           DALI_KEY_NEXT_SONG,       false },
  { KEY_PREVIOUSSONG,       DALI_KEY_PREVIOUS_SONG,   false },
  { KEY_REWIND,             DALI_KEY_REWIND,          false },
  { KEY_FASTFORWARD,        DALI_KEY_FASTFORWARD,     false },
  { KEY_MEDIA,              DALI_KEY_MEDIA,           false },
  { KEY_PLAYPAUSE,          DALI_KEY_PLAY_PAUSE,      false },
  { KEY_MUTE,               DALI_KEY_MUTE,            false },
  { KEY_SEND,               DALI_KEY_SEND,            true  },
  { KEY_SELECT,             DALI_KEY_SELECT,          true  },
  { KEY_END,                DALI_KEY_END,             true  },
  { KEY_MENU,               DALI_KEY_MENU,            true  },
  { KEY_HOME,               DALI_KEY_HOME,            true  },
  { KEY_BACK,               DALI_KEY_BACK,            true  },
  { KEY_HOMEPAGE,           DALI_KEY_HOMEPAGE,        false },
  { KEY_WEBPAGE,            DALI_KEY_WEBPAGE,         false },
  { KEY_MAIL,               DALI_KEY_MAIL,            false },
  { KEY_SCREENSAVER,        DALI_KEY_SCREENSAVER,     false },
  { KEY_BRIGHTNESSUP,       DALI_KEY_BRIGHTNESS_UP,   false },
  { KEY_BRIGHTNESSDOWN,     DALI_KEY_BRIGHTNESS_DOWN, false },
  { KEY_SOFTKBD,            DALI_KEY_SOFT_KBD,        false },
  { KEY_QUICKPANEL,         DALI_KEY_QUICK_PANEL,     false },
  { KEY_TASKSWITCH,         DALI_KEY_TASK_SWITCH,     false },
  { KEY_APPS,               DALI_KEY_APPS,            false },
  { KEY_SEARCH,             DALI_KEY_SEARCH,          false },
  { KEY_VOICE,              DALI_KEY_VOICE,           false },
  { KEY_LANGUAGE,           DALI_KEY_LANGUAGE,        false },
  { KEY_VOLUMEUP,           DALI_KEY_VOLUME_UP,       true  },
  { KEY_VOLUMEDOWN,         DALI_KEY_VOLUME_DOWN,     true  },
};
const std::size_t KEY_LOOKUP_COUNT = (sizeof( KeyLookupTable))/ (sizeof(KeyLookup));


// Generate a KeyPressEvent to send to Core
Dali::KeyEvent GenerateKeyPress( const std::string& keyName )
{
  KeyEvent keyPress;
  keyPress.keyPressedName = keyName;
  return keyPress;
}

int UtcDaliKeyIsKey(void)
{
  TestApplication application;

  for ( std::size_t i = 0; i < KEY_LOOKUP_COUNT; ++i )
  {
    tet_printf( "Checking %s", KeyLookupTable[i].keyName );
    DALI_TEST_CHECK( IsKey( GenerateKeyPress( KeyLookupTable[i].keyName ), KeyLookupTable[i].daliKeyCode ) );
  }
  END_TEST;
}

int UtcDaliKeyIsKeyNegative(void)
{
  TestApplication application;

  // Random value
  DALI_TEST_CHECK( IsKey( GenerateKeyPress( "invalid-key-name" ), DALI_KEY_MUTE ) == false );

  // Compare with another key value
  for ( std::size_t i = 0; i < KEY_LOOKUP_COUNT; ++i )
  {
    tet_printf( "Checking %s", KeyLookupTable[i].keyName );
    DALI_TEST_CHECK( IsKey( GenerateKeyPress( KeyLookupTable[i].keyName ), KeyLookupTable[ ( i + 1 ) % KEY_LOOKUP_COUNT ].daliKeyCode ) == false );
  }
  END_TEST;
}