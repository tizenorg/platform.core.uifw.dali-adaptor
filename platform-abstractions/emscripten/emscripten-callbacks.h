#ifndef __DALI_EMSCRIPTEN_CALLBACKS_H__
#define __DALI_EMSCRIPTEN_CALLBACKS_H__

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
#include <dali/integration-api/platform-abstraction.h>
#include <dali/integration-api/resource-cache.h>
#include <dali/public-api/common/dali-common.h>

// INTERNAL INCLUDES

namespace Dali
{
namespace Internal
{
namespace Emscripten
{

Dali::Integration::BitmapPtr GetGlyphImage( const std::string& fontFamily, const std::string& fontStyle, float fontSize, uint32_t character );

void LoadImageMetadata(const std::string fileName, Dali::Vector2 &size);

Dali::Integration::BitmapPtr GetImage( const Dali::ImageAttributes& attributes, const std::string& filename );

struct Statistics
{
  bool on;
  float frameCount;

  float lastFrameDeltaSeconds;
  unsigned int lastSyncTimeMilliseconds;
  unsigned int nextSyncTimeMilliseconds;

  unsigned int keepUpdating; ///< A bitmask of KeepUpdating values
  bool needsNotification;
  float secondsFromLastFrame;

Statistics() :on(true),
    frameCount(0.0),
    lastFrameDeltaSeconds(0.0),
    lastSyncTimeMilliseconds(0.0),
    nextSyncTimeMilliseconds(0.0),
    keepUpdating(0),
    needsNotification(false),
    secondsFromLastFrame(0.0)
  {};
};

extern Statistics stats;

}; // namespace Emscripten
}; // namespace Internal
}; // namespace Dali

#endif // header
