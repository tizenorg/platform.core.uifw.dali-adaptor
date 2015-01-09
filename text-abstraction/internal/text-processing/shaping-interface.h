#ifndef __DALI_TEXT_ABSTRACTION_SHAPING_INTERFACE_H__
#define __DALI_TEXT_ABSTRACTION_SHAPING_INTERFACE_H__

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
#include <text-abstraction/public-api/glyph-info.h>
#include <text-abstraction/public-api/text-type-definitions.h>

namespace Dali
{

namespace TextAbstraction
{

/**
 * Responsible for shaping text
 * See http://en.wikipedia.org/wiki/Typographic_ligature
 *
 */
class ShapingInterface
{

public:

  /**
   * Shapes the text.
   *
   * Call GetGlyphs() to retrieve the glyphs.
   *
   * @param[in] text Pointer to the first character of the text coded in UTF32.
   * @param[in] numberOfCharacters The number of characters to be shaped
   * @param[in] fontId The font to be used to shape the text.
   * @param[in] scriptId The text's script.
   *
   * @return The size of the buffer required to get the shaped text.
   */
  virtual Length Shape( const Character* const text,
                        Length numberOfCharacters,
                        FontId fontId,
                        Script scriptId ) = 0;

  /**
   * Gets the shaped text data.
   *
   * Call first Shape() to shape the text and get the buffer's size.
   *
   * @param[out] glyphInfo Vector with indices to the glyph within the font, glyph's metrics and advance.
   * @param[out] glyphToCharacterMap The glyph to character conversion map.
   */
  virtual void GetGlyphs( GlyphInfo* glyphInfo,
                          Index* glyphToCharacterMap ) = 0;

protected:

  // constructor
  ShapingInterface() {};

  // virtual destructor
  virtual ~ShapingInterface() {};

private:

  // Undefined copy constructor.
  ShapingInterface( const ShapingInterface& );

  // Undefined assignment operator.
  ShapingInterface& operator=( const ShapingInterface& );
};

} // namespace TextAbstraction

} // namespace Dali

#endif // __DALI_TEXT_ABSTRACTION_SHAPING_INTERFACE_H__
