#ifndef __DALI_PLATFORM_TEXT_ABSTRACTION_FONT_CLIENT_H__
#define __DALI_PLATFORM_TEXT_ABSTRACTION_FONT_CLIENT_H__

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

#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/images/bitmap-image.h>
#include <dali/public-api/common/dali-vector.h>
#include "text-type-definitions.h"
#include "glyph-metrics.h"
#include "font-description.h"

namespace Dali
{

namespace TextAbstraction
{

namespace Internal DALI_INTERNAL
{
class FontClient;
}

/**
 * FontClient API
 *
 * The font client caches information for each available font face, accessed using a FontId.
 */
class DALI_IMPORT_API FontClient : public BaseHandle
{
public:

  /**
   * @brief Create an uninitialized TextAbstraction handle.
   */
  FontClient();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~FontClient();

  /**
   * @brief Retrieve a handle to the FontClient instance.
   *
   * @return A handle to the FontClient
   */
  static FontClient Get();

  /**
   * @brief Set the DPI of the target window.
   *
   * @note Multiple windows are not currently supported.
   * @param[in] horizontalDpi The horizontal resolution in DPI.
   * @param[in] verticalDpi The vertical resolution in DPI.
   */
  void SetDpi( unsigned int horizontalDpi, unsigned int verticalDpi );

  /**
   * @brief Retrieve the list of fonts supported by the system.
   *
   * @param[out] systemFonts A list of font paths, family & style strings.
   */
  void GetSystemFonts( FontList& systemFonts );

  /**
   * @brief Retrieve the unique identifier for a font face.
   *
   * @param[in] path The path to a font file.
   * @param[in] pointSize The point size in 26.6 fractional points; the default point size is 12.
   * @param[in] face The index of the font face (optional).
   * @return A valid font ID, or zero if the font does not exist.
   */
  FontId GetFontId( const FontPath& path, PointSize26Dot6 pointSize = 12*64, FaceIndex faceIndex = 0 );

  /**
   * @brief Retrieve the metrics for a series of glyphs.
   *
   * @param[in] fontId The ID of the font for these glyphs.
   * @param[in,out] metrics An array of metrics with initialized glyph IDs.
   * On return, the remaining values will be initialized e.g. glyph size & bearing values.
   * @param[in] size The size of the array.
   * @param[in] horizontal True for horizontal layouts (set to false for vertical layouting).
   * @return True if metrics were found.
   */
  bool CreateMetrics( FontId fontId, GlyphMetrics* array, uint32_t size, bool horizontal = true );

  /**
   * @brief Render a bitmap representation of a glyph.
   *
   * @param[in] fontId The ID of the font.
   * @param[in] glyphIndex The index of a glyph within the specified font.
   * @return A valid BitmapImage, or an empty handle if the glyph could not be rendered.
   */
  BitmapImage CreateBitmap( FontId fontId, GlyphIndex glyphIndex );

public: // Not intended for application developers
  /**
   * @brief This constructor is used by FontClient::Get().
   *
   * @param[in] fontClient  A pointer to the internal fontClient object.
   */
  explicit DALI_INTERNAL FontClient( Internal::FontClient* fontClient );
};

} // namespace TextAbstraction

} // namespace Dali

#endif // __DALI_PLATFORM_TEXT_ABSTRACTION_FONT_CLIENT_H__
