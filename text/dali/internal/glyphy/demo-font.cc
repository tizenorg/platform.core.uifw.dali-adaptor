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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "demo-font.h"

#include "../glyphy/glyphy-freetype.h"

#include <vector>
#include <map>
#include <iostream>

#include <dali/integration-api/debug.h>
#include <dali/devel-api/text-abstraction/glyph-info.h>

using namespace __gnu_cxx; /* This is ridiculous */


typedef std::map<unsigned int, glyph_info_t> glyph_cache_t;

struct demo_font_t {

  FT_Face        face;
  float          scale; // point-size * dpi/72
  glyph_cache_t *glyph_cache;
  glyphy_arc_accumulator_t *acc;
};

demo_font_t *
demo_font_create( FT_Face face, float scale )
{
  demo_font_t *font = (demo_font_t *) calloc (1, sizeof (demo_font_t));

  font->face = face;
  font->scale = scale;
  font->glyph_cache = new glyph_cache_t ();
  font->acc = glyphy_arc_accumulator_create ();

  return font;
}

void
demo_font_destroy (demo_font_t *font)
{
  glyphy_arc_accumulator_destroy (font->acc);
  delete font->glyph_cache;
  free (font);
}

static glyphy_bool_t
accumulate_endpoint (glyphy_arc_endpoint_t         *endpoint,
		     std::vector<glyphy_arc_endpoint_t> *endpoints)
{
  endpoints->push_back (*endpoint);
  return true;
}

static void
encode_ft_glyph (demo_font_t      *font,
		 unsigned int      glyph_index,
		 double            tolerance_per_em,
		 glyphy_rgba_t    *buffer,
		 unsigned int      buffer_len,
		 unsigned int     *output_len,
		 unsigned int     *nominal_width,
		 unsigned int     *nominal_height,
		 glyphy_extents_t *extents,
		 double           *advance,
                 GlyphInfo        *glyphInfo )
{
/* Used for testing only */
#define SCALE  (1. * (1 << 0))

  FT_Face face = font->face;

  if (FT_Err_Ok != FT_Load_Glyph (face,
				  glyph_index,
				  FT_LOAD_NO_BITMAP |
				  FT_LOAD_NO_HINTING |
				  FT_LOAD_NO_AUTOHINT |
				  FT_LOAD_NO_SCALE |
				  FT_LOAD_LINEAR_DESIGN |
				  FT_LOAD_IGNORE_TRANSFORM))
  {
    DALI_LOG_ERROR("Failed loading FreeType glyph");
    return;
  }

  unsigned int upem = face->units_per_EM;
  double tolerance = upem * tolerance_per_em; /* in font design units */
  double faraway = double (upem) / (MIN_FONT_SIZE * M_SQRT2);
  std::vector<glyphy_arc_endpoint_t> endpoints;

  glyphy_arc_accumulator_reset (font->acc);
  glyphy_arc_accumulator_set_tolerance (font->acc, tolerance);
  glyphy_arc_accumulator_set_callback (font->acc,
				       (glyphy_arc_endpoint_accumulator_callback_t) accumulate_endpoint,
				       &endpoints);

  if (FT_Err_Ok != glyphy_freetype(outline_decompose) (&face->glyph->outline, font->acc))
  {
    DALI_LOG_ERROR("Failed converting glyph outline to arcs");
    return;
  }

  DALI_ASSERT_DEBUG( glyphy_arc_accumulator_get_error (font->acc) <= tolerance );

  if (endpoints.size ())
  {
    glyphy_outline_winding_from_even_odd (&endpoints[0], endpoints.size (), false);
  }

  if (SCALE != 1.)
    for (unsigned int i = 0; i < endpoints.size (); i++)
    {
      endpoints[i].p.x /= SCALE;
      endpoints[i].p.y /= SCALE;
    }

  double avg_fetch_achieved;
  if (!glyphy_arc_list_encode_blob (endpoints.size () ? &endpoints[0] : NULL, endpoints.size (),
				    buffer,
				    buffer_len,
				    faraway / SCALE,
				    4, /* UNUSED */
				    &avg_fetch_achieved,
				    output_len,
				    nominal_width,
				    nominal_height,
				    extents))
  {
    DALI_LOG_ERROR("Failed encoding arcs");
    return;
  }

  glyphy_extents_scale (extents, 1. / upem, 1. / upem);
  glyphy_extents_scale (extents, SCALE, SCALE);

  *advance = face->glyph->metrics.horiAdvance / (double) upem;

  // Convert to Dali GlyphInfo

  if( glyphy_extents_is_empty( extents ) )
  {
    glyphInfo->width  = 0.0f;
    glyphInfo->height = 0.0f;

    glyphInfo->xBearing = 0.0f;
    glyphInfo->yBearing = 0.0f;
  }
  else
  {
    glyphInfo->width  = font->scale * (extents->max_x - extents->min_x);
    glyphInfo->height = font->scale * (extents->max_y - extents->min_y);

    glyphInfo->xBearing = font->scale * extents->min_x;
    glyphInfo->yBearing = glyphInfo->height + (extents->min_y);
  }

  glyphInfo->advance = font->scale * *advance;
  glyphInfo->scaleFactor = 0.0f;
}

static void
_demo_font_upload_glyph (demo_font_t *font,
			 unsigned int glyph_index,
			 glyph_info_t *glyph_info)
{
  glyph_info->blob.resize( 1024 * 16 );
  unsigned int output_len( 0 );

  encode_ft_glyph (font,
		   glyph_index,
		   TOLERANCE,
		   &glyph_info->blob[0], glyph_info->blob.capacity(),
		   &output_len,
		   &glyph_info->nominal_w,
		   &glyph_info->nominal_h,
		   &glyph_info->extents,
		   &glyph_info->advance,
                   &glyph_info->glyphInfo);

  glyph_info->blob.resize( output_len );
}

void
demo_font_lookup_glyph ( demo_font_t  *font,
                         GlyphInfo& glyphInfo )
{
  if( font->glyph_cache->find(glyphInfo.index) == font->glyph_cache->end() )
  {
    glyph_info_t& glyph_info = (*font->glyph_cache)[glyphInfo.index];
    glyph_info.glyphInfo = glyphInfo;

    _demo_font_upload_glyph( font, glyphInfo.index, &glyph_info );

    glyphInfo = glyph_info.glyphInfo;
  }
  else
  {
    const glyph_info_t& glyph_info = (*font->glyph_cache)[glyphInfo.index];

    glyphInfo = glyph_info.glyphInfo;
  }
}

void
demo_font_lookup_blob( demo_font_t  *font,
                       unsigned int fontId,
                       unsigned int glyphIndex,
                       VectorBlob** blob,
                       unsigned int* blobLength,
                       unsigned int* nominalWidth,
                       unsigned int* nominalHeight )
{
  if( font->glyph_cache->find(glyphIndex) == font->glyph_cache->end() )
  {
    glyph_info_t& glyph_info = (*font->glyph_cache)[glyphIndex];
    glyph_info.glyphInfo.fontId = fontId;
    glyph_info.glyphInfo.index = glyphIndex;

    _demo_font_upload_glyph( font, glyphIndex, &glyph_info );

    *blob       = &glyph_info.blob[0];
    *blobLength = glyph_info.blob.size();
    *nominalWidth = glyph_info.nominal_w;
    *nominalHeight = glyph_info.nominal_h;
  }
  else
  {
    glyph_info_t& glyph_info = (*font->glyph_cache)[glyphIndex];

    *blob       = &glyph_info.blob[0];
    *blobLength = glyph_info.blob.size();
    *nominalWidth = glyph_info.nominal_w;
    *nominalHeight = glyph_info.nominal_h;
  }
}

