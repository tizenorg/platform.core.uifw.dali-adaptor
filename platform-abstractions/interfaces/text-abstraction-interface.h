#ifndef __DALI_PLATFORM_TEXT_ABSTRACTION_H__
#define __DALI_PLATFORM_TEXT_ABSTRACTION_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <string>
#include <dali/public-api/common/vector-wrapper.h>

namespace Dali
{

namespace Platform
{


class TextAbstractionInterface
{

public:

  /**
   * Retrieves the line break positions.
   *
   * @param[in] paragraph The paragraph's text coded in utf32.
   * @param[out] positions Vector with possible indices where to break the paragraph in lines.
   */
  virtual void GetLineBreakPositions( TBD ) = 0;

  //
  // Text Shaping
  //
  struct GlyphInfo
  {
    std::size_t glyphId;
    float top;
    float bottom;
    float left;
    float right;
    float advance;
  };

  /**
   * Shapes the text.
   *
   * @param[in] text The text to be shaped.
   * @param[in] fontId The font to be used to shape the text.
   * @param[in] scriptId The text's script.
   * @param[out] glyphInfo Vector with indices to the glyph within the font, glyph's metrics and advance.
   * @param[out] glyphToCharacterMap The glyph to character conversion map.
   */
  virtual void Shape( TBD text,
                    std::size_t fontId,
                    std::size_t scriptId,
                    Vector<GlyphInfo>& glyphInfo,
                    Vector<std::size_t>& glyphToCharacterMap ) = 0;

   //
   // Font/metrics API
   //
  struct CharactersFont
  {
    std::size_t fontId;
    std::size_t numberOfCharacters;
  };

  /**
   * Validates a font.
   *
   * @param[in] fontFamilyName The font's family name.
   * @param[in] fontStyle The font's style.
   *
   * @return A font id.
   */
  virtual std::size_t virtualValidateFont( const std::string& fontFamilyName,
                            const std::string& fontStyle ) = 0;

  /**
   * Validates all characters are supported by the given fonts.
   *
   * @param[in] text The text to be validated.
   * @param[in] charactersFont Vector containing the font Id and the number of consecutive characters with the same font.
   * @param[in] validatedCharactersFont Vector containing the font Id and the number of consecutive characters with the validated font.
   */
  virtual void Validate( const Vector<uint32_t>& text,
                 const Vector<CharactersFont>& charactersFont,
                       Vector<CharactersFont>& validatedCharactersFont ) =0;

  /**
   * Retrieves glyph's metrics.
   *
   * @param[in] fontId The font to be used to shape the text.
   * @param[in,out] glyphInfo Vector with indices to the glyph within the font, glyph's metrics and advance.
   */
  virtual void GetMetrics( std::size_t fontId,
                   Vector<GlyphInfo>& glyphInfo ) = 0;


  /**
   *Bidirectional Text Support
   */

  /**
   * Creates bidirectional data for the whole paragraph.
   *
   * @param[in] paragraph The paragraph's text coded in utf32.
   *
   * @return An index to a table storing the bidirectional data.
   */
  virtual std::size_t CreateBidirectionalInfo( const Vector<uint32_t>& paragraph ) = 0;

  /**
   * Reorders a line of a paragraph.
   *
   * @param[in] bidiInfoIndex The index to the table storing the bidirectional data for current paragraph.
   * @param[in] firstCharacterIndex The first character of the line within the whole paragraph.
   * @param[in] numberOfCharacters The number of characters of the line.
   * @param[out] visualToLogicalMap The visual to logical conversion map.
   */
  virtual void Reorder( std::size_t bidiInfoIndex,
                std::size_t firstCharacterIndex,
                std::size_t numberOfCharacters,
                Vector<std::size_t>& visualToLogicalMap ) = 0;

private:

  // Undefined copy constructor.
  TextAbstractionInterface( const TextAbstractionInterface& );

  // Undefined assignment operator.
  TextAbstractionInterface& operator=( const TextAbstractionInterface& );
};

} // namespace Platform

} // namespace Dali

#endif // __DALI_PLATFORM_TEXT_ABSTRACTION_H__
