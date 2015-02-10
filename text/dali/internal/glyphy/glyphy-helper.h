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

#include <dali/internal/glyphy/glyphy-freetype.h>

typedef struct {
  glyphy_extents_t extents;
  double           advance;
  glyphy_bool_t    is_empty; /* has no outline; eg. space; don't draw it */
  unsigned int     nominal_w;
  unsigned int     nominal_h;
  unsigned int     atlas_x;
  unsigned int     atlas_y;
} glyph_info_t;

static void
encode_ft_glyph (FT_Face face,
		 unsigned int      glyph_index,
		 double            tolerance_per_em,
		 glyphy_rgba_t    *buffer,
		 unsigned int      buffer_len,
		 unsigned int     *output_len,
		 unsigned int     *nominal_width,
		 unsigned int     *nominal_height,
		 glyphy_extents_t *extents)
{
/* Used for testing only */
#define SCALE  (1. * (1 << 0))

  if (FT_Err_Ok != FT_Load_Glyph (face,
				  glyph_index,
				  FT_LOAD_NO_BITMAP |
				  FT_LOAD_NO_HINTING |
				  FT_LOAD_NO_AUTOHINT |
				  FT_LOAD_NO_SCALE |
				  FT_LOAD_LINEAR_DESIGN |
				  FT_LOAD_IGNORE_TRANSFORM))
    die ("Failed loading FreeType glyph");

  if (face->glyph->format != FT_GLYPH_FORMAT_OUTLINE)
    die ("FreeType loaded glyph format is not outline");

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
    die ("Failed converting glyph outline to arcs");

  assert (glyphy_arc_accumulator_get_error (font->acc) <= tolerance);

  if (endpoints.size ())
  {
#if 0
    /* Technically speaking, we want the following code,
     * however, crappy fonts have crappy flags.  So we just
     * fixup unconditionally... */
    if (face->glyph->outline.flags & FT_OUTLINE_EVEN_ODD_FILL)
      glyphy_outline_winding_from_even_odd (&endpoints[0], endpoints.size (), false);
    else if (face->glyph->outline.flags & FT_OUTLINE_REVERSE_FILL)
      glyphy_outline_reverse (&endpoints[0], endpoints.size ());
#else
    glyphy_outline_winding_from_even_odd (&endpoints[0], endpoints.size (), false);
#endif
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
    die ("Failed encoding arcs");

  glyphy_extents_scale (extents, 1. / upem, 1. / upem);
  glyphy_extents_scale (extents, SCALE, SCALE);

  *advance = face->glyph->metrics.horiAdvance / (double) upem;

  if (0)
    LOGI ("gid%3u: endpoints%3d; err%3g%%; tex fetch%4.1f; mem%4.1fkb\n",
	  glyph_index,
	  (unsigned int) glyphy_arc_accumulator_get_num_endpoints (font->acc),
	  round (100 * glyphy_arc_accumulator_get_error (font->acc) / tolerance),
	  avg_fetch_achieved,
	  (*output_len * sizeof (glyphy_rgba_t)) / 1024.);
}

BitmapImage CreateGlyph( FT_Face face, GlyphIndex glyphIndex, unsigned int requiredWidth, double tolerancePerEm, GlyphyBlob& blob )
{
  std::cout << "create_glyph: " << glyph_index << std::endl;

  glyph_info_t glyph_info;

  glyphy_extents_t glyph_extents;

  glyphy_rgba_t buffer[1024 * 16]; 
  unsigned int outputlength;

  encode_ft_glyph( face,
		   glyphIndex,
		   tolerancePerEm,
		   buffer, ARRAY_LEN (buffer),
		   &outputlength,
		   &blob.nominalWidth,
		   &blob.nominalHeight,
		   &glyph_extents );

  glyph_info->is_empty = glyphy_extents_is_empty( &glyph_extents );

  BitmapImage bitmapImage;

  if( !glyph_info->is_empty )
  {
    unsigned int h = (outputlength + requiredWidth - 1) / requiredWidth;

    bitmapImage = BitmapImage::New( requiredWidth, h, Dali::Pixel::RGBA8888 );

    Dali::PixelBuffer* pixbuf = bitmap.GetBuffer();
    memcpy( pixbuf, buffer, outputlength*4 );

    blob.minX = glyph_extents->min_x;
    blob.minY = glyph_extents->min_y;
    blob.maxX = glyph_extents->max_x;
    blob.maxY = glyph_extents->max_y;
  }

  return bitmapImage;
}

