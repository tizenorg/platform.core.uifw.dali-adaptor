#ifndef __DALI_TEXT_ABSTRACTION_GLYPH_METRICS_INTERFACE_H__
#define __DALI_TEXT_ABSTRACTION_GLYPH_METRICS_INTERFACE_H__

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
#include <text-abstraction/public-api/text-type-definitions.h>

namespace Dali
{

namespace TextAbstraction
{

class GlyphMetricsInterface
{

public:
  /**
   * Retrieves glyph's metrics.
   *
   * @param[in] fontId The font Id.
   * @param[in,out] glyphInfo Pointer to the glyph info. It contains the glyph indices and metrics.
   * @param[in] numberOfGlyphs The number of glyphs.
   */
  virtual void GetMetrics( FontId fontId,
                           GlyphInfo* glyphInfo,
                           Length numberOfGlyphs ) = 0;

protected:

  // constructor
  GlyphMetricsInterface() {};

  // virtual destructor
  virtual ~GlyphMetricsInterface() {};

private:

  // Undefined copy constructor.
  GlyphMetricsInterface( const GlyphMetricsInterface& );

  // Undefined assignment operator.
  GlyphMetricsInterface& operator=( const GlyphMetricsInterface& );
};

} // namespace TextAbstraction

} // namespace Dali

#endif // __DALI_TEXT_ABSTRACTION_GLYPH_METRICS_INTERFACE_H__
