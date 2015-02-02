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

// CLASS HEADER
#include <dali/public-api/text-abstraction/shaping.h>

// INTERNAL INCLUDES
#include <dali/internal/text-abstraction/shaping-impl.h>

namespace Dali
{

namespace TextAbstraction
{

Shaping::Shaping()
{
}
Shaping::~Shaping()
{
}
Shaping::Shaping(Dali::Internal::TextAbstraction::Shaping *impl)
  : BaseHandle(impl)
{
}

Shaping Shaping::Get()
{
  return Dali::Internal::TextAbstraction::Shaping::Get();
}

Length Shaping::Shape( const Character* const text,
                       Length numberOfCharacters,
                       FontId fontId,
                       Script scriptId )
{
  return GetImplementation( *this ).Shape( text,
                                           numberOfCharacters,
                                           fontId,
                                           scriptId );
}

void Shaping::GetGlyphs( GlyphInfo* glyphInfo,
                         CharacterIndex* glyphToCharacterMap )
{
  GetImplementation( *this ).GetGlyphs( glyphInfo,
                                        glyphToCharacterMap );
}

} // namespace TextAbstraction

} // namespace Dali
