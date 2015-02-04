#ifndef __DALI_TEXT_ABSTRACTION_GLYPH_INFO_H__
#define __DALI_TEXT_ABSTRACTION_GLYPH_INFO_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/text-abstraction/text-abstraction-definitions.h>

namespace Dali
{

namespace TextAbstraction
{

/**
 * The information describing a glyph (font ID, index, metrics)
 * The metrics are expressed in 26.6 fractional pixel format.
 */
struct GlyphInfo
{
  /**
   * @brief Default constructor.
   */
  GlyphInfo();

  /**
   * @brief Creates the GlyphInfo without metrics.
   */
  GlyphInfo( FontId font, GlyphIndex i );

  FontId fontId;       ///< Identifies the font containing the glyph
  GlyphIndex index;    ///< Uniquely identifies a glyph for a given FontId
  uint32_t width;      ///< The width of the glyph
  uint32_t height;     ///< The height of the glyph
  int32_t xBearing;   ///< The distance from the cursor position to the leftmost border of the glyph
  int32_t yBearing;   ///< The distance from the baseline to the topmost border of the glyph
  uint32_t advance;    ///< The distance to move the cursor for this glyph
};

} // Dali

} // TextAbstraction

#endif //__DALI_TEXT_ABSTRACTION_GLYPH_INFO_H__
