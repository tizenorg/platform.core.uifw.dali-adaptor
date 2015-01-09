#ifndef __DALI_TEXT_ABSTRACTION_GLYPH_CREATION_INTERFACE_H__
#define __DALI_TEXT_ABSTRACTION_GLYPH_CREATION_INTERFACE_H__

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

struct GlyphInfo;

struct GlyphBitmap
{
  GlyphBitmap():
    width(0),
    height(0),
    pixelData(NULL)
  {
  };

  uint16_t width;
  uint16_t height;
  char* pixelData;
};

/**
 * Used to create glyphs
 */
class GlyphCreationInterface
{

public:

  /**
   * Creates Bitmaps for the given glyphs.
   *
   * @param[out] bitmapArray Pointer to the bitmap array.
   * @param[in] glyphInfo Pointer to the glyph array.
   * @param[in] numberOfGlyphs The number of glyphs.
   */
  virtual void CreateBitmapGlyphs( GlyphBitmap* bitmapArray,
                                   GlyphInfo* glyphInfo,
                                   uint32_t numberOfGlyphs ) = 0;
protected:

  // constructor
  GlyphCreationInterface() {};

  // virtual destructor
  virtual ~GlyphCreationInterface() {};

private:

  // Undefined copy constructor.
  GlyphCreationInterface( const GlyphCreationInterface& );

  // Undefined assignment operator.
  GlyphCreationInterface& operator=( const GlyphCreationInterface& );
};

} // namespace TextAbstraction

} // namespace Dali

#endif // __DALI_TEXT_ABSTRACTION_GLYPH_CREATION_INTERFACE_H__
