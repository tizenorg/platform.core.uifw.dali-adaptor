#ifndef __DALI_TEXT_ABSTRACTION_GLPYH_METRICS_H__
#define __DALI_TEXT_ABSTRACTION_GLYPH_METRICS_H__

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
#include "text-type-definitions.h"

namespace Dali
{

namespace TextAbstraction
{

/**
 * POD structure for storing glyph metric information
 */
struct GlyphMetrics
{
  GlyphIndex index; ///< Uniquely identifies a glyph for a given FontId
  float width;      ///< The width of the glyph
  float height;     ///< The height of the glyph
  float top;        ///< Top side bearing
  float bottom;     ///< Bottom side bearing
  float left;       ///< Left side bearing
  float right;      ///< Right side bearing
  float xAdvance;   ///< Advance for horizontal layouts
  float yAdvance;   ///< Advance for vertical layouts (+ve is down)
};

} // Dali

} // TextAbstraction

#endif //__DALI_TEXT_ABSTRACTION_GLYPH_METRICS_H__
