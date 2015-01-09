#ifndef __DALI_TEXT_ABSTRACTION_TEXT_SHAPING_INTERFACE_H__
#define __DALI_TEXT_ABSTRACTION_TEXT_SHAPING_INTERFACE_H__

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

#include "../shared/glyph-info.h"

namespace Dali
{

namespace TextAbstraction
{

class TextShapingInterface
{

public:

  /**
   * Shapes the text.
   * Returns the size of the buffer required to get the shaped text, using GetGlyphs
   *
   * @param[in] text Pointer to the unicode text to be shape ( UTF-32 )
   * @param[in] startIndex Start index of the text to be shaped.
   * @param[in] count number of characters to be shaped
   * @param[in] fontId The font to be used to shape the text.
   * @param[in] scriptId The text's script.
   */
  virtual unsigned int Shape( const uint32_t*  text,
                        unsigned int startCharacter,
                        unsigned int count,
                        uint32_t fontId,
                        uint32_t scriptId
                        ) = 0 ;

  /**
   * Gets the shaped text data
   * @param[out] glyphInfo Vector with indices to the glyph within the font, glyph's metrics and advance.
   * @param[out] glyphToCharacterMap The glyph to character conversion map.
   */
  virtual void GetGlyphs( GlyphInfo* glyphInfo, uint32_t*  glyphToCharacterMap ) = 0;

private:

  // Undefined copy constructor.
  TextShapingInterface( const TextShapingInterface& );

  // Undefined assignment operator.
  TextShapingInterface& operator=( const TextShapingInterface& );
};

} // namespace TextAbstraction

} // namespace Dali

#endif // __DALI_TEXT_ABSTRACTION_TEXT_SHAPING_INTERFACE_H__
