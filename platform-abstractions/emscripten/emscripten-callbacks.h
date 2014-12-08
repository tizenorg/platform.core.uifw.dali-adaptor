#ifndef EMSCRIPTENCALLBACKS_H
#define EMSCRIPTENCALLBACKS_H

#include <dali/integration-api/platform-abstraction.h>
#include <dali/integration-api/resource-cache.h>
#include <dali/public-api/common/dali-common.h>

namespace EmscriptenCallbacks
{
  Dali::Integration::BitmapPtr GetGlyphImage( const std::string& fontFamily, const std::string& fontStyle, float fontSize, uint32_t character );

  void LoadImageMetadata(const std::string fileName, Dali::Vector2 &size);

  Dali::Integration::BitmapPtr GetImage( const Dali::ImageAttributes& attributes, const std::string& filename );

  void RenderFinished();


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


// namespace EmscriptenCallbacks
// {
//   struct Statistics
//   {
//     bool on;
//     float daliUpdate;
//     float daliTime;
//     Statistics() :on(false),
//                   daliUpdate(0.0),
//                   daliTime(0.0)
//     {};
//   };
//   Statistics stats;
// };
//   if(EmscriptenCallbacks::stats.on)
//   {
//     EmscriptenCallbacks::stats.daliUpdate++;
//   }

};

#endif // header
