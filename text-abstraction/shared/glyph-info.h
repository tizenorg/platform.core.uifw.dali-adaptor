#ifndef __DALI_TEXT_ABSTRACTION_REORDERING_INTERFACE_H__
#define __DALI_TEXT_ABSTRACTION_REORDERING_INTERFACE_H__

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



namespace Dali
{

namespace TextAbstraction
{

/**
 * POD structure for storing glyph metric information
 */
struct GlyphInfo
{
  // not sure about this...
  union
  {
    uint32_t glyphId;    ///< glyph index
    uint32_t unicodeId;  ///< unicode code point
  };
  float top;
  float bottom;
  float left;
  float right;
  float xAdvance;
  float yAdvance;
};

}; // DALi
}; // TextAbstraction

