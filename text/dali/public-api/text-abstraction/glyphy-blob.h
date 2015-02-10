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
 * The pixels of a BitmapImage contain the blob data.
 */
struct GlyphyBlob
{
  /**
   * @brief Default constructor.
   */
  GlyphyBlob();

  /**
   * @brief Creates the GlyphyBlob.
   */
  GlyphyBlob( BitmapImage image,
              unsigned int nominalWidth,
              unsigned int nominalHeight,
              double minX,
              double minY,
              double maxX,
              double maxY );

  BitmapImage image;

  // The nominal size
  unsigned int nominalWidth;
  unsigned int nominalHeight,

  // The extents
  double minX;
  double minY;
  double maxX;
  double maxY;
};

} // Dali

} // TextAbstraction

#endif //__DALI_TEXT_ABSTRACTION_GLYPH_INFO_H__
