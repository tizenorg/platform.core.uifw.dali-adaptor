#ifndef __DALI_EMSCRIPTEN_CALLBACKS_H__
#define __DALI_EMSCRIPTEN_CALLBACKS_H__

/*
Copyright (c) 2000-2013 Samsung Electronics Co., Ltd All Rights Reserved

This file is part of Dali

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
