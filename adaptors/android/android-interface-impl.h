#ifndef ADAPTOR_ANDROID_INTERFACE_IMPL_H
#define ADAPTOR_ANDROID_INTERFACE_IMPL_H

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

#include <dali/public-api/dali-core.h>
#include <dali/public-api/adaptor-framework/application.h>

#include "adaptors/android/android-core-input-handlers.h"





AndroidCore::AndroidInputCallback* androidcore_register_input_handler(
  void* context,
  bool (*callback)(void*, const AndroidTouchEvent&));
void androidcore_unregister_input_handler(AndroidCore::AndroidInputCallback* callback);

/**
 * Get the image loader function
 * @return The image loader function (if one has been set)
 */
LoadImageFunction androidcore_GetLoadImageFunc();

Android::ImageDataHandle androidcore_LoadBitmap(const std::string& path);


bool androidcore_is_on_emulator ( );

bool androidcore_read_fake_pinch ( float *cx, float *cy );


#endif //ADAPTOR_ANDROID_INTERFACE_IMPL_H
