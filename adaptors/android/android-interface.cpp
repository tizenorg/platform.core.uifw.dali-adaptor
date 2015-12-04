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
 */

#include <cstdlib>
#include <jni.h>
#include <android/native_window_jni.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <sys/select.h>
#include <sys/system_properties.h>
#include <sys/prctl.h>

#include <dali/public-api/dali-core.h>
#include <adaptor.h>
#include <render-surface.h>
#include <dali/public-api/adaptor-framework/native-logging.h>
#include <android-interface-impl.h>

#ifdef ACORE_DEBUG_ENABLED
#define ACORE_LOG(mode, ...) LOG#mode(...)
#define ACORE_DECLARE(x) x
#else
#define ACORE_LOG(...)
#define ACORE_DECLARE(x)
#endif

namespace
{

bool isSurfaceCreated = false;
bool isDaliSurfaceCreated = false;
bool isAppActive = false;
bool isResumeRequested = false;

Android::ImageDataHandle (*android_LoadImageFunc)(const std::string& path) = NULL;

int shutdownRequested;

}

// @todo move this to an owned object
AndroidCore::InputHandlerFactory gInputHandlerFactory;

AndroidCore::AndroidInputCallback* androidcore_register_input_handler(
  void* context,
  bool (*callback)(void*, const AndroidTouchEvent&))
{
  return gInputHandlerFactory.Register( context, callback );
}

void androidcore_unregister_input_handler(AndroidCore::AndroidInputCallback* callback)
{
  gInputHandlerFactory.Unregister( callback );
}


typedef Android::ImageDataHandle (*LoadImageFunction)(const std::string& path);

void androidcore_SetLoadImageFunc(LoadImageFunction func)
{
  android_LoadImageFunc = func;
}

LoadImageFunction androidcore_GetLoadImageFunc()
{
  return android_LoadImageFunc;
}


void androidcore_queue_touch_event ( int pointerID, int type, float x, float y,
                                     unsigned long ntimestamp )
{
  Dali::TouchPoint::State state = Dali::TouchPoint::Last;

  switch ( type )
  {
    case ACTION_DOWN:
    case ACTION_POINTER_DOWN:
    case ACTION_HOVER_ENTER:
    {
      state = Dali::TouchPoint::Down;
      break;
    }
    case ACTION_UP:
    case ACTION_POINTER_UP:
    case ACTION_HOVER_EXIT:
    {
      state = Dali::TouchPoint::Up;
      break;
    }
    case ACTION_MOVE:
    case ACTION_HOVER_MOVE:
    {
      state = Dali::TouchPoint::Motion;
      break;
    }
    case ACTION_CANCEL:
    {
      state = Dali::TouchPoint::Interrupted;
      break;
    }
    default:
    {
      static bool reported[ACTION_LAST] = { false };
      if ( !reported[type] ) {
        reported[type] = true;
        LOGE("Unhandled message type=%x",type);
      }
      break;
    }
  }
  if ( state != Dali::TouchPoint::Last )
  {
    AndroidTouchEvent event(nativeWindow, pointerID, state, x, y, ntimestamp);
    gInputHandlerFactory.Execute( event );
  }

}

// A utility function to detect whether the current target device is the emulator.
// Some kinds of testing cannot (easily) be performed on the emulator, and this allows
// other code to take this into account.
bool androidcore_is_on_emulator ( ) {
  static bool first = true;
  static bool onEmulator = false;
  if ( first ) {
    first = false;
    char name[PROP_NAME_MAX];
    // This property only exists on the emulator
    int len = __system_property_get("ro.kernel.qemu",name);
    onEmulator = len > 0;
  }
  return onEmulator;
}

// Read the system property which allows fake 2nd touch point.
// This is only called when running on the emulator.
bool androidcore_read_fake_pinch ( float *cx, float *cy ) {
  bool sendSecondPointer = false;
  char name[PROP_NAME_MAX];
  // The parameters are the dx,dy from the current pointer position
  // where a new 'centre' of motion will be established.
  // adb shell "setprop dali.fake-pinch 100,0"    to enable it (places 2nd finger at 200,0 relative to first finger)
  // adb shell "setprop dali.fake-pinch \"\""     to disable it
  int len = __system_property_get("dali.fake-pinch",name);
  if ( len > 0 && sscanf(name,"%f,%f",cx,cy) == 2 ) {
    sendSecondPointer = true;
  } else {
    *cx = *cy = 0;                // clear any offsets
    sendSecondPointer = false;    // flag is undefined, or unparsable
  }
  return sendSecondPointer;
}
