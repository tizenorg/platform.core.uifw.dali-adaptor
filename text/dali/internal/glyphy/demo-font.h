/*
 * Copyright 2012 Google, Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Google Author(s): Behdad Esfahbod
 */

#ifndef DEMO_FONT_H
#define DEMO_FONT_H

#include <vector>

#include <dali/devel-api/text-abstraction/glyph-info.h>
#include <dali/devel-api/text-abstraction/text-abstraction-definitions.h>

#include "glyphy.h"

#include <ft2build.h>
#include FT_FREETYPE_H

typedef Dali::TextAbstraction::GlyphInfo GlyphInfo;
typedef Dali::TextAbstraction::VectorBlob VectorBlob;

typedef struct {
  glyphy_extents_t extents;
  double           advance;
  glyphy_bool_t    is_empty; /* has no outline; eg. space; don't draw it */
  unsigned int     nominal_w;
  unsigned int     nominal_h;
  unsigned int     atlas_x;
  unsigned int     atlas_y;

  GlyphInfo        glyphInfo;
  std::vector<glyphy_rgba_t> blob;

} glyph_info_t;

typedef struct demo_font_t demo_font_t;

demo_font_t *
demo_font_create (FT_Face face, float scale);

void
demo_font_destroy (demo_font_t *font);

void
demo_font_lookup_glyph( demo_font_t  *font,
                        GlyphInfo& glyph_info );

void
demo_font_lookup_blob( demo_font_t  *font,
                       unsigned int fontId,
                       unsigned int glyphIndex,
                       VectorBlob** blob,
                       unsigned int* blobLength,
                       unsigned int* nominalWidth,
                       unsigned int* nominalHeight );

#endif /* DEMO_FONT_H */
