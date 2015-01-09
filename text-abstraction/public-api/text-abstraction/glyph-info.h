#ifndef __DALI_TEXT_ABSTRACTION_GLPYH_INFO_H__
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
#include <public-api/text-abstraction/text-type-definitions.h>

namespace Dali
{

namespace TextAbstraction
{

/**
 * POD structure for storing glyph metric information
 */
struct GlyphInfo
{
  GlyphInfo()
  : glyphId( 0u ),
    top( 0.f ),
    bottom( 0.f ),
    left( 0.f ),
    right( 0.f ),
    xAdvance( 0.f ),
    yAdvance( 0.f )
  {}

  Index glyphId;    ///< glyph index
  float top;
  float bottom;
  float left;
  float right;
  float xAdvance;
  float yAdvance;
};

} // DALi

} // TextAbstraction

#endif //__DALI_TEXT_ABSTRACTION_GLYPH_INFO_H__
