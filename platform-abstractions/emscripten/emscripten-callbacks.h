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

};

#endif // header
