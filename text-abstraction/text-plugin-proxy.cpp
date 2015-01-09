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


namespace Dali
{

namespace TextAbstraction
{

TextPluginProxy::TextPluginProxy()
: mPlugin( NULL)
{

}


unsigned int TextPluginProxy::Shape( const uint32_t* const text,
                      uint32_t startCharacter,
                      uint32_t characterCount,
                      uint32_t fontId,
                      uint32_t scriptId
                      )
{
  if( !mPlugin )
  {
    LoadPlugin();
  }
  return mPlugin->Shape( text, startCharacter, characterCount, fontId, scriptId );

}

/**
 * Gets the shaped text data
 * @param[out] glyphInfo Vector with indices to the glyph within the font, glyph's metrics and advance.
 * @param[out] glyphToCharacterMap The glyph to character conversion map.
 */
void TextPluginProxy::GetGlyphs( GlyphInfo* glyphInfo, uint32_t*  glyphToCharacterMap )
{
  if( !mPlugin )
  {
    LoadPlugin();
  }
  return mPlugin->GetGlyphs( glyphInfo, glyphToCharacterMap );

}



} // namespace TextAbstraction

} // namespace Dali

#endif // __DALI_PLATFORM_TEXT_ABSTRACTION_H__
