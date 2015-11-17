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

// EXTERNAL INCLUDES
#include <dali/internal/glyphy/glyphy-freetype.h>
#include <dali/public-api/images/buffer-image.h>

// INTERNAL INCLUDES
#include <dali/devel-api/text-abstraction/text-abstraction-definitions.h>
#include <dali/devel-api/text-abstraction/glyphy-blob.h>

#define MIN_FONT_SIZE 10

#define ARRAY_LEN(Array) (sizeof (Array) / sizeof (*Array))

typedef struct {
  glyphy_extents_t extents;
  double           advance;
  glyphy_bool_t    is_empty; /* has no outline; eg. space; don't draw it */
  unsigned int     nominal_w;
  unsigned int     nominal_h;
  unsigned int     atlas_x;
  unsigned int     atlas_y;
} glyph_info_t;

glyphy_bool_t
accumulate_endpoint (glyphy_arc_endpoint_t         *endpoint,
		     std::vector<glyphy_arc_endpoint_t> *endpoints)
{
  endpoints->push_back (*endpoint);
  return true;
}

void
encode_ft_glyph (FT_Face face,
		 unsigned int      glyph_index,
                 glyphy_arc_accumulator_t* acc,
		 double            tolerance_per_em,
		 glyphy_rgba_t    *buffer,
		 unsigned int      buffer_len,
		 unsigned int     *output_len,
		 unsigned int     *nominal_width,
		 unsigned int     *nominal_height,
		 glyphy_extents_t *extents)
{
  /* Used for testing only */
  float SCALE = 1.;// * (1 << 0))

  if (FT_Err_Ok != FT_Load_Glyph (face,
				  glyph_index,
				  FT_LOAD_NO_BITMAP |
				  FT_LOAD_NO_HINTING |
				  FT_LOAD_NO_AUTOHINT |
				  FT_LOAD_NO_SCALE |
				  FT_LOAD_LINEAR_DESIGN |
				  FT_LOAD_IGNORE_TRANSFORM))
  {
    return; // TODO
  }

  if (face->glyph->format != FT_GLYPH_FORMAT_OUTLINE)
  {
    return; // TODO ("FreeType loaded glyph format is not outline");
  }

  unsigned int upem = face->units_per_EM;
  double tolerance = upem * tolerance_per_em; /* in font design units */
  double faraway = double (upem) / (MIN_FONT_SIZE * M_SQRT2);
  std::vector<glyphy_arc_endpoint_t> endpoints;

  glyphy_arc_accumulator_reset(acc);
  glyphy_arc_accumulator_set_tolerance (acc, tolerance);
  glyphy_arc_accumulator_set_callback (acc,
				       (glyphy_arc_endpoint_accumulator_callback_t) accumulate_endpoint,
				       &endpoints);

  if (FT_Err_Ok != glyphy_freetype(outline_decompose) (&face->glyph->outline, acc))
  {
    return; // TODO die ("Failed converting glyph outline to arcs");
  }

  //DALI_ASSERT_DEBUG( glyphy_arc_accumulator_get_error (acc) <= tolerance );

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
  {
    return; // TODO die ("Failed encoding arcs");
  }

  glyphy_extents_scale (extents, 1. / upem, 1. / upem);
  glyphy_extents_scale (extents, SCALE, SCALE);
}

void GetGlyphyBlob( FT_Face face,
                    Dali::TextAbstraction::GlyphIndex glyphIndex,
                    unsigned int requiredWidth,
                    glyphy_arc_accumulator_t* acc,
                    double tolerancePerEm,
                    Dali::TextAbstraction::GlyphyBlob& blob )
{
  glyphy_extents_t glyph_extents;

  glyphy_rgba_t buffer[1024 * 16];
  unsigned int outputlength;

  encode_ft_glyph( face,
		   glyphIndex,
                   acc,
		   tolerancePerEm,
		   buffer, ARRAY_LEN (buffer),
		   &outputlength,
		   &blob.nominalWidth,
		   &blob.nominalHeight,
		   &glyph_extents );

  glyphy_bool_t isEmpty = glyphy_extents_is_empty( &glyph_extents );

  if( !isEmpty )
  {
    unsigned int h = (outputlength + requiredWidth - 1) / requiredWidth;

    blob.image = Dali::BufferImage::New( requiredWidth, h, Dali::Pixel::RGBA8888 );

    Dali::PixelBuffer* pixbuf = blob.image.GetBuffer();
    memcpy( pixbuf, buffer, outputlength*4 );

    blob.minX = glyph_extents.min_x;
    blob.minY = glyph_extents.min_y;
    blob.maxX = glyph_extents.max_x;
    blob.maxY = glyph_extents.max_y;
  }
}

