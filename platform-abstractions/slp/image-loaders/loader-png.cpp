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

namespace Dali
{

using Integration::Bitmap;
using Dali::Integration::PixelBuffer;

namespace SlpPlatform
{

namespace
{

// Simple helper class to store the data and the state for the png reader, 
// also responsible for owning the data.
struct PngMemoryReaderState
{
  unsigned char* buffer;
  png_uint_32    buffer_size;
  png_uint_32    current_pos;

  PngMemoryReaderState(png_uint_32 file_size)  : buffer_size(file_size), current_pos(0) 
  {    
    buffer = (unsigned char*)malloc(buffer_size);
  }

  ~PngMemoryReaderState()
  {
    free(buffer);
  }
};

// Callback for LibPNG when it is requresting more data. 
static void read_data_memory(png_structp png_ptr, png_bytep data, png_uint_32 length)
{
  PngMemoryReaderState *f = (PngMemoryReaderState *)png_get_io_ptr(png_ptr);
  if (length > (f->buffer_size - f->current_pos)) png_error(png_ptr, "read error in read_data_memory (loadpng)");
  memcpy(data, f->buffer + f->current_pos, length);
  f->current_pos += length;
}

// Callback for LibPNG for error messages.
void user_error_fn(png_structp png_ptr, png_const_charp warning_msg)
{
  DALI_LOG_ERROR("Png error : %s\n", warning_msg);
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

bool LoadPngHeader(FILE *fp, unsigned int &width, unsigned int &height, png_structp &png, png_infop &info)
{
  png_byte header[8] = { 0 };

  // Check header to see if it is a PNG file
  size_t size = fread(header, 1, 8, fp);
  if(size != 8)
  {
    return false;
  }

  if(png_sig_cmp(header, 0, 8))
  {
    return false;
  }

  png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

  if(!png)
  {
    DALI_LOG_WARNING("Can't create PNG read structure\n");
    return false;
  }

  info = png_create_info_struct(png);
  if(!info)
  {
    DALI_LOG_WARNING("png_create_info_struct failed\n");
    return false;
  }

  png_set_expand(png);

  if(setjmp(png_jmpbuf(png)))
  {
    DALI_LOG_WARNING("error during png_init_io\n");
    return false;
  }

  png_init_io(png, fp);
  png_set_sig_bytes(png, 8);

  // read image info
  png_read_info(png, info);

  // dimensions
  width = png_get_image_width(png, info);
  height = png_get_image_height(png, info);

  return true;
}

// Modified PNG header reader, this is uses LibPNG's memory reader API.
bool LoadPngHeader(PngMemoryReaderState& reader, unsigned int &width, unsigned int &height, png_structp &png, png_infop &info)
{
  // Check header to see if it is a PNG file
  if (reader.buffer_size < 8)
  {
    return false;
  }

  png_byte header[8] = { 0 };
  memcpy(header, reader.buffer, 8);
  reader.current_pos = 8;

  if(png_sig_cmp(header, 0, 8))
  {
    return false;
  }

  png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

  if(!png)
  {
    DALI_LOG_WARNING("Can't create PNG read structure\n");
    return false;
  }

  info = png_create_info_struct(png);
  if(!info)
  {
    DALI_LOG_WARNING("png_create_info_struct failed\n");
    return false;
  }

  png_set_expand(png);

  if(setjmp(png_jmpbuf(png)))
  {
    DALI_LOG_WARNING("error during png_init_io\n");
    return false;
  }

  png_set_sig_bytes(png, 8);
  png_set_error_fn(png, (png_voidp) NULL, (png_error_ptr) NULL, user_error_fn);
  png_set_read_fn(png, &reader, (png_rw_ptr)read_data_memory);

  // read image info
  png_read_info(png, info);

  // dimensions
  width = png_get_image_width(png, info);
  height = png_get_image_height(png, info);

  return true;
}

} // namespace - anonymous

bool LoadPngHeader(FILE *fp, const ImageAttributes& attributes, unsigned int &width, unsigned int &height )
{
  png_structp png = NULL;
  png_infop info = NULL;
  auto_png autoPng(png, info);

  bool success = LoadPngHeader(fp, width, height, png, info);

  if( success )
  {
    bool crop = (attributes.GetScalingMode() == Dali::ImageAttributes::ScaleToFill);
    if(crop)
    {
      Size req((float) attributes.GetWidth(), (float) attributes.GetHeight());
      const Size orig((float)width, (float)height);

      // calculate actual width, height
      req = FitScaleToFill(req, orig);

      width = (int) req.width;
      height = (int) req.height;
    }
  }
  return success;
}

bool LoadBitmapFromPng( FILE *fp, Bitmap& bitmap, ImageAttributes& attributes, const ResourceLoadingClient& client )
{
  png_structp png = NULL;
  png_infop info = NULL;
  auto_png autoPng(png, info);

  fseek(fp, 0, SEEK_END);
  size_t file_size = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  PngMemoryReaderState pngReader(file_size);
  size_t bytesRead = fread(pngReader.buffer, 1, file_size, fp);
  if (bytesRead != file_size)
  {
    return false;
  }
  pngReader.current_pos = 0;

  /// @todo: consider parameters
  unsigned int y;
  unsigned int width, height;
  unsigned char *pixels;
  png_bytep *rows;
  unsigned int bpp = 0; // bytes per pixel
  bool valid = false;

  // Load info from the header
  // This function also initializes libpng.
  if( !LoadPngHeader(pngReader, width, height, png, info) )
  {
    return false;
  }

  Pixel::Format pixelFormat = Pixel::RGBA8888;

  // decide pixel format
  unsigned int colordepth = png_get_bit_depth(png, info);

  // Ask PNGLib to convert high precision images into something we can use:
  if (colordepth == 16)
  {
    png_set_strip_16(png);
    colordepth = 8;
  }

  png_byte colortype = png_get_color_type(png, info);

  if(colortype == PNG_COLOR_TYPE_GRAY)
  {
    switch( colordepth )
    {
      case 8:
      {
        pixelFormat = Pixel::L8;
        valid = true;
        break;
      }
      default:
      {
        break;
      }
    }
  }
  else if(colortype == PNG_COLOR_TYPE_GRAY_ALPHA)
  {
    switch(colordepth)
    {
      case 8:
      {
        pixelFormat = Pixel::LA88;
        valid = true;
        break;
      }
      default:
      {
        break;
      }
    }
  }
  else if(colortype == PNG_COLOR_TYPE_RGB )
  {
    switch(colordepth)
    {
      case 8:
      {
        pixelFormat = Pixel::RGB888;
        valid = true;
        break;
      }
      case 5:      /// @todo is this correct for RGB16 5-6-5 ?
      {
        pixelFormat = Pixel::RGB565;
        valid = true;
        break;
      }
      default:
      {
        break;
      }
    }
  }
  else if(colortype == PNG_COLOR_TYPE_RGBA)
  {
    switch(colordepth)
    {
      case 8:
      {
        pixelFormat = Pixel::RGBA8888;
        valid = true;
        break;
      }
      default:
      {
        break;
      }
    }
  }
  else if(colortype == PNG_COLOR_TYPE_PALETTE)
  {
    switch(colordepth)
    {
      case 2:
      case 4:
      case 8:
      {
        /* Expand paletted or RGB images with transparency to full alpha channels
         * so the data will be available as RGBA quartets. PNG_INFO_tRNS = 0x10
         */
        if(png_get_valid(png, info, PNG_INFO_tRNS) == 0x10)
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
      }
      default:
      {
        break;
      }
    }
  }

  if( !valid )
  {
    DALI_LOG_WARNING( "Unsupported png format\n" );
    return false;
  }

  // bytes per pixel
  bpp = Pixel::GetBytesPerPixel(pixelFormat);

  png_read_update_info(png, info);

  if(setjmp(png_jmpbuf(png)))
  {
    DALI_LOG_WARNING("error during png_read_image\n");
    return false;
  }

  unsigned int rowBytes = png_get_rowbytes(png, info);

  unsigned int bufferWidth   = GetTextureDimension(width);
  unsigned int bufferHeight  = GetTextureDimension(height);
  unsigned int stride        = bufferWidth*bpp;

  // not sure if this ever happens
  if( rowBytes > stride )
  {
    stride = GetTextureDimension(rowBytes);
    bufferWidth = stride / bpp;
  }

  /// @todo support more scaling types
  bool crop = (attributes.GetScalingMode() == Dali::ImageAttributes::ScaleToFill);

  if(crop)
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
  for(y=0; y<height; y++)
  {
    rows[y] = (png_byte*) (pixels + y * stride);
  }

  // decode image
  png_read_image(png, rows);

  // copy part of the buffer to bitmap
  if(crop)
  {
    Size req((float) attributes.GetWidth(), (float) attributes.GetHeight());
    const Size orig((float)width, (float)height);

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
    for(unsigned int i = 0; i < attributes.GetHeight(); ++i)
    {
      memcpy(bufptr, lptr, attributes.GetWidth()*bpp);
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

  bitmap.GetPackedPixelsProfile()->TestForTransparency();

  free(rows);

  return true;
}

// simple class to enforce clean-up of PNG structures
struct AutoPngWrite
{
  AutoPngWrite(png_structp& _png, png_infop& _info)
  : png(_png),
    info(_info)
  {
    DALI_ASSERT_DEBUG(&_png != 0 && &_info != 0);
  }

  ~AutoPngWrite()
  {
    if(NULL != png)
    {
      png_destroy_write_struct(&png, &info);
    }
  }

  png_structp& png;
  png_infop& info;
}; // struct AutoPngWrite;

namespace
{
  // Custom libpng write callbacks that buffer to a vector instead of a file:

  /**
   * extern "C" linkage is used because this is a callback that we pass to a C
   * library which is part of the underlying platform and so potentially compiled
   * as C rather than C++.
   * @see http://stackoverflow.com/a/2594222
   * */
  extern "C" void WriteData(png_structp png_ptr, png_bytep data, png_size_t length)
  {
    using Dali::SlpPlatform::PngBuffer;
    DALI_ASSERT_DEBUG(png_ptr && data);
    if(!png_ptr || !data)
    {
      return;
    }
    // Make sure we don't try to propagate a C++ exception up the call stack of a pure C library:
    try {
      // Recover our buffer for writing into:
      PngBuffer* const encoded_img = (PngBuffer*) png_get_io_ptr(png_ptr);
      if(encoded_img)
      {
        const PngBuffer::size_type bufferSize = encoded_img->size();
        encoded_img->resize(encoded_img->size() + length); //< Can throw OOM.
        PngBuffer::value_type* const bufferBack = &((*encoded_img)[bufferSize]);
        memcpy(bufferBack, data, length);
      }
      else
      {
        DALI_LOG_ERROR("PNG buffer for write to memory was passed from libpng as null.");
      }
    } catch(...)
    {
      DALI_LOG_ERROR("C++ Exception caught");
    }
  }

  /** Override the flush with a NOP to prevent libpng trying cstdlib file io. */
  extern "C" void FlushData(png_structp png_ptr)
  {
#ifdef DEBUG_ENABLED
    Debug::LogMessage(Debug::DebugInfo, "PNG Flush");
#endif // DEBUG_ENABLED
  }
}

/**
 * Potential improvements:
 * 1. Detect <= 256 colours and write in palette mode.
 * 2. Detect grayscale (will early-out quickly for colour images).
 * 3. Store colour space / gamma correction info related to the device screen?
 *    http://www.libpng.org/pub/png/book/chapter10.html
 * 4. Refactor with callers to write straight through to disk and save keeping a big buffer around.
 * 5. Prealloc buffer (reserve) to input size / <A number greater than 2 (expexcted few realloc but without using lots of memory) | 1 (expected zero reallocs but using a lot of memory)>.
 * 6. Set the modification time with png_set_tIME(png_ptr, info_ptr, mod_time);
 * 7. If caller asks for no compression, bypass libpng and blat raw data to
 *    disk, topped and tailed with header/tail blocks.
 */
bool EncodeToPng( const unsigned char* const pixelBuffer, PngBuffer& encodedPixels, std::size_t width, std::size_t height, Pixel::Format pixelFormat )
{
  // Translate pixel format enum:
  int pngPixelFormat = -1;
  unsigned pixelBytes = 0;
  bool rgbaOrder = true;

  // Account for RGB versus BGR and presence of alpha in input pixels:
  switch( pixelFormat )
  {
    case Pixel::RGB888:
    {
      pngPixelFormat = PNG_COLOR_TYPE_RGB;
      pixelBytes = 3;
      break;
    }
    case Pixel::BGRA8888:
    {
      rgbaOrder = false;
      ///! No break: fall through:
    }
    case Pixel::RGBA8888:
    {
      pngPixelFormat = PNG_COLOR_TYPE_RGB_ALPHA;
      pixelBytes = 4;
      break;
    }
    default:
    {
      DALI_LOG_ERROR( "Unsupported pixel format for encoding to PNG." );
      return false;
    }
  }

  const int interlace = PNG_INTERLACE_NONE;

  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if(!png_ptr)
  {
    return false;
  }
  /* Allocate/initialize the image information data.  REQUIRED */
  png_infop info_ptr = png_create_info_struct( png_ptr );
  if(!info_ptr)
  {
    png_destroy_write_struct(&png_ptr, NULL);
    return false;
  }

  /* Set error handling.  REQUIRED if you aren't supplying your own
   * error handling functions in the png_create_write_struct() call.
   */
  if(setjmp(png_jmpbuf(png_ptr)))
  {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return false;
  }

  // Since we are going to write to memory instead of a file, lets provide
  // libpng with a custom write function and ask it to pass back our
  // std::vector buffer each time it calls back to flush data to "file":
  png_set_write_fn(png_ptr, &encodedPixels, WriteData, FlushData);

  // png_set_compression_level( png_ptr, Z_BEST_COMPRESSION);
  png_set_compression_level(png_ptr, Z_BEST_SPEED);
  // png_set_compression_level( png_ptr, Z_NO_COMPRESSION); //! We could just generate png directly without libpng in this case.

  // Explicitly limit the number of filters used per scanline to speed us up:
  // png_set_filter(png_ptr, 0, PNG_FILTER_NONE); ///!ToDo: Try this once baseline profile is in place.
       // PNG_FILTER_SUB   |
       // PNG_FILTER_UP    |
       // PNG_FILTER_AVE   |
       // PNG_FILTER_PAETH |
       // PNG_ALL_FILTERS);
  // Play with Zlib parameters in optimisation phase:
    // png_set_compression_mem_level(png_ptr, 8);
    // png_set_compression_strategy(png_ptr,
        // Z_DEFAULT_STRATEGY);
    // png_set_compression_window_bits(png_ptr, 15);
    // png_set_compression_method(png_ptr, 8);
    // png_set_compression_buffer_size(png_ptr, 8192)

  // Let lib_png know if the pixel bytes are in BGR(A) order:
  if(!rgbaOrder)
  {
    png_set_bgr( png_ptr );
  }

  // Set the image information:
  png_set_IHDR(png_ptr, info_ptr, width, height, 8,
     pngPixelFormat, interlace,
     PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  // Start to output the PNG data to our buffer:
  png_write_info(png_ptr, info_ptr);

  // Walk the rows:
  const unsigned row_step = width * pixelBytes;
  png_bytep row_ptr = const_cast<png_bytep>(pixelBuffer);
  const png_bytep row_end = row_ptr + height * row_step;
  for(; row_ptr < row_end; row_ptr += row_step)
  {
    png_write_row(png_ptr, row_ptr);
  }

  /* It is REQUIRED to call this to finish writing the rest of the file */
  png_write_end(png_ptr, info_ptr);
  /* Clean up after the write, and free any memory allocated */
  png_destroy_write_struct(&png_ptr, &info_ptr);
  return true;
}

} // namespace SlpPlatform

} // namespace Dali
