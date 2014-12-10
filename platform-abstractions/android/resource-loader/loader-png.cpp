/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

#include "loader-png.h"

#include <png.h>
#include <stdlib.h>

#include <dali/integration-api/bitmap.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/images/image.h>
#include <dali/public-api/images/image-attributes.h>
#include "dali/public-api/math/math-utils.h"
#include "dali/public-api/math/vector2.h"
#include "platform-capabilities.h"
#include "nativeLogging.h"

#include <android/log.h>

namespace Dali
{

using Integration::Bitmap;

namespace SlpPlatform
{

namespace
{

struct PngMemoryReaderState
{
  const unsigned char *buffer;
  png_uint_32 bufsize;
  png_uint_32 current_pos;
};

static void read_data_memory(png_structp png_ptr, png_bytep data, png_uint_32 length)
{
  PngMemoryReaderState *f = (PngMemoryReaderState *)png_get_io_ptr(png_ptr);
  if (length > (f->bufsize - f->current_pos)) png_error(png_ptr, "read error in read_data_memory (loadpng)");
  memcpy(data, f->buffer + f->current_pos, length);
  f->current_pos += length;
}

// simple class to enforce clean-up of PNG structures
struct auto_png
{
  auto_png(png_structp& _png, png_infop& _info)
  : png(_png),
    info(_info)
  {
  }

  ~auto_png()
  {
    if(NULL != png)
    {
      png_destroy_read_struct(&png, &info, NULL);
    }
  }

  png_structp& png;
  png_infop& info;
}; // struct auto_png;

void user_error_fn(png_structp png_ptr, png_const_charp warning_msg)
{
  LOGW("png error : %s\n", warning_msg);
}

bool LoadPngHeaderInternal(PngMemoryReaderState& reader, unsigned int &width, unsigned int &height, png_structp &png, png_infop &info)
{
  png_byte header[8] = { 0 };

  // Check header to see if it is a PNG file
  if (reader.bufsize < 8)
  {
    return false;
  }
  memcpy(header, reader.buffer, 8);
  reader.current_pos += 8;

  if (png_sig_cmp(header, 0, 8))
  {
    return false;
  }

  png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

  if (!png)
  {
    DALI_LOG_WARNING ("Can't create PNG read structure\n");
    return false;
  }

  info = png_create_info_struct(png);
  if (!info)
  {
    DALI_LOG_WARNING ("png_create_info_struct failed\n");
    return false;
  }

  png_set_expand(png);
  png_set_sig_bytes(png, 8);
  png_set_error_fn(png, (png_voidp) NULL, (png_error_ptr) NULL, user_error_fn);
  png_set_read_fn(png, &reader, (png_rw_ptr)read_data_memory);

  // read image info
  png_read_info(png, info);

  // dimensions
  width = png_get_image_width(png, info);
  height = png_get_image_height(png, info);
  LOGD("header : %d %d\n", width, height);


  return true;
}

} // namespace - anonymous

bool LoadPngHeader(const Dali::Vector<uint8_t>& data, const ImageAttributes& attrs, unsigned int &width, unsigned int &height)
{
  png_structp png = NULL;
  png_infop info = NULL;
  auto_png autoPng(png, info);

  PngMemoryReaderState reader;
  reader.current_pos = 0;
  reader.buffer = data.Begin();
  reader.bufsize = data.Size();

  return LoadPngHeaderInternal(reader, width, height, png, info);
}

bool LoadBitmapFromPng(const Dali::Vector<uint8_t>& data, Bitmap& bitmap, ImageAttributes& attributes)
{
  png_structp png = NULL;
  png_infop info = NULL;
  auto_png autoPng(png, info);

  /// @todo: consider parameters
  unsigned int y;
  unsigned int width, height;
  unsigned char *pixels;
  png_bytep *rows;
  unsigned int bpp = 0; // bytes per pixel
  bool valid = false;

  PngMemoryReaderState reader;
  reader.current_pos = 0;
  reader.buffer = data.Begin();
  reader.bufsize = data.Size();
  png_set_error_fn(png, (png_voidp) NULL, (png_error_ptr) NULL, user_error_fn);
  png_set_read_fn(png, &reader, (png_rw_ptr)read_data_memory);

  // Load info from the header
  if (!LoadPngHeaderInternal(reader, width, height, png, info))
  {
    return false;
  }

  Pixel::Format pixelFormat = Pixel::RGBA8888;

  // decide pixel format
  unsigned int colordepth = png_get_bit_depth(png, info);

  png_byte colortype = png_get_color_type(png, info);

  if(colortype == PNG_COLOR_TYPE_GRAY)
  {
    switch (colordepth)
    {
      case 8:
        pixelFormat = Pixel::L8;
        valid = true;
        break;
      default:
        break;
    }
  }
  else if(colortype == PNG_COLOR_TYPE_GRAY_ALPHA)
  {
    switch (colordepth)
    {
      case 8:
        pixelFormat = Pixel::LA88;
        valid = true;
        break;
      default:
        break;
    }
  }
  else if(colortype == PNG_COLOR_TYPE_RGB )
  {
    switch (colordepth)
    {
      case 8:
        pixelFormat = Pixel::RGB888;
        valid = true;
        break;
      case 5:      /// @todo is this correct for RGB16 5-6-5 ?
        pixelFormat = Pixel::RGB565;
        valid = true;
        break;
      default:
        break;
    }
  }
  else if(colortype == PNG_COLOR_TYPE_RGBA)
  {
    switch(colordepth)
    {
      case 8:
        pixelFormat = Pixel::RGBA8888;
        valid = true;
        break;
      default:
        break;
    }
  }
  else if(colortype == PNG_COLOR_TYPE_PALETTE)
  {
    switch(colordepth)
    {
      case 2:
      case 4:
      case 8:
        /* Expand paletted or RGB images with transparency to full alpha channels
         * so the data will be available as RGBA quartets. PNG_INFO_tRNS = 0x10
         */
        if (png_get_valid(png, info, PNG_INFO_tRNS) == 0x10)
        {
          pixelFormat = Pixel::RGBA8888;
          valid = true;
        }
        else
        {
          pixelFormat = Pixel::RGB888;
          png_set_packing(png);
          png_set_packswap(png);
          png_set_palette_to_rgb(png);
          valid = true;
        }
        break;
      default:
        break;
    }
  }

  if (!valid)
  {
    DALI_LOG_WARNING ("Unsupported png format\n");
    return false;
  }

  // bytes per pixel
  bpp = Pixel::GetBytesPerPixel(pixelFormat);

  png_read_update_info(png, info);

  unsigned int rowBytes = png_get_rowbytes(png, info);

  unsigned int bufferWidth   = GetTextureDimension(width);
  unsigned int bufferHeight  = GetTextureDimension(height);
  unsigned int stride        = bufferWidth*bpp;

  // not sure if this ever happens
  if ( rowBytes > stride)
  {
    stride = GetTextureDimension(rowBytes);
    bufferWidth = stride / bpp;
  }

  /// @todo support more scaling types
  bool crop = (attributes.GetScalingMode() == Dali::ImageAttributes::ScaleToFill);

  if (crop)
  {
    pixels = new unsigned char[stride*bufferHeight];
  }
  else
  {
    // decode the whole image into bitmap buffer
    pixels = bitmap.GetPackedPixelsProfile()->ReserveBuffer(pixelFormat, width, height, bufferWidth, bufferHeight);
  }

  DALI_ASSERT_DEBUG(pixels);
  rows = (png_bytep*) malloc(sizeof(png_bytep) * height);
  for (y=0; y<height; y++)
  {
    rows[y] = (png_byte*) (pixels + y * stride);
  }

  // decode image
  png_read_image(png, rows);

  // copy part of the buffer to bitmap
  if (crop)
  {
    Size req ((float) attributes.GetWidth(), (float) attributes.GetHeight());
    const Size orig ((float)width, (float)height);

    // calculate actual width, height
    req = FitScaleToFill(req, orig);

    // modify attributes with result
    attributes.SetSize((int) req.width, (int) req.height);
    attributes.SetPixelFormat(pixelFormat);


    bufferWidth  = GetTextureDimension(attributes.GetWidth());
    bufferHeight = GetTextureDimension(attributes.GetHeight());

    // cropped buffer's stride
    int lstride = bufferWidth*bpp;

    // calculate offsets
    int x_offset = ((width  - attributes.GetWidth())  / 2) * bpp;
    int y_offset = ((height - attributes.GetHeight()) / 2) * stride;

    // allocate bitmap buffer using requested size
    unsigned char *bitmapBuffer = bitmap.GetPackedPixelsProfile()->ReserveBuffer(pixelFormat, attributes.GetWidth(), attributes.GetHeight(), bufferWidth, bufferHeight);

    // copy memory area from y and x to bitmap buffer line-by-line
    unsigned char *bufptr = bitmapBuffer;
    unsigned char *lptr = pixels+y_offset+x_offset;
    for (unsigned int i = 0; i < attributes.GetHeight(); ++i)
    {
      memcpy (bufptr, lptr, attributes.GetWidth()*bpp);
      bufptr += lstride;
      lptr += stride;
    }

    delete[] pixels;
  }
  else
  {
    // set the attributes
     attributes.SetSize(width, height);
     attributes.SetPixelFormat(pixelFormat);
  }

  // bitmap.TestForTransparency();

  free(rows);

  return true;
}

} // namespace SlpPlatform

} // namespace Dali

