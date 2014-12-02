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

// INTERNAL INCLUDES
#include "image-operations.h"
#include <dali/integration-api/debug.h>
#include <dali/public-api/common/ref-counted-dali-vector.h>
#include <dali/public-api/images/image-attributes.h>
#include <dali/integration-api/bitmap.h>

// EXTERNAL INCLUDES
#include <cstring>

namespace Dali
{
namespace Internal
{
namespace Platform
{

namespace
{
using Integration::Bitmap;
using Integration::BitmapPtr;
typedef unsigned char PixelBuffer;

#if defined(DEBUG_ENABLED)
/**
 * Disable logging of image operations or make it verbose from the commandline
 * as follows (e.g., for dali demo app):
 * <code>
 * LOG_IMAGE_OPERATIONS=0 dali-demo #< off
 * LOG_IMAGE_OPERATIONS=3 dali-demo #< on, verbose
 * </code>
 */
Debug::Filter* gImageOpsLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_IMAGE_OPERATIONS");
#endif

/** @return The greatest even number less than or equal to the argument. */
inline unsigned int EvenDown( const unsigned int a )
{
  const unsigned int evened = a & ~1u;
  return evened;
}

/**
 * @brief Log bad parameters.
 */
void ValidateScalingParameters(
  const unsigned int inputWidth,    const unsigned int inputHeight,
  const unsigned int desiredWidth, const unsigned int desiredHeight )
{
  if( desiredWidth > inputWidth || desiredHeight > inputHeight )
  {
    DALI_LOG_INFO( gImageOpsLogFilter, Dali::Integration::Log::Verbose, "Upscaling not supported (%u, %u -> %u, %u).\n", inputWidth, inputHeight, desiredWidth, desiredHeight );
  }

  if( desiredWidth == 0u || desiredHeight == 0u )
  {
    DALI_LOG_INFO( gImageOpsLogFilter, Dali::Integration::Log::Verbose, "Downscaling to a zero-area target is pointless." );
  }

  if( inputWidth == 0u || inputHeight == 0u )
  {
    DALI_LOG_INFO( gImageOpsLogFilter, Dali::Integration::Log::Verbose, "Zero area images cannot be scaled" );
  }
}

/**
 * @brief Do debug assertions common to all scanline halving functions.
 * @note Inline and in anon namespace so should boil away in release builds.
 */
inline void DebugAssertScanlineParameters(
    unsigned char * const pixels,
    const unsigned int width )
{
  DALI_ASSERT_DEBUG( pixels && "Null pointer." );
  DALI_ASSERT_DEBUG( width > 1u && "Can't average fewer than two pixels." );
  DALI_ASSERT_DEBUG( width < 131072u && "Unusually wide image: are you sure you meant to pass that value in?" );
}

/**
 * @brief Assertions on params to functions averaging pairs of scanlines.
 */
template<typename IN_POINTED_TO, typename OUT_POINTED_TO>
void DebugAssertDualScanlineParameters(
    const IN_POINTED_TO * const scanline1,
    const IN_POINTED_TO * const scanline2,
    OUT_POINTED_TO* const outputScanline,
    const unsigned int width )
{
  DALI_ASSERT_DEBUG( scanline1 && "Null pointer." );
  DALI_ASSERT_DEBUG( scanline2 && "Null pointer." );
  DALI_ASSERT_DEBUG( outputScanline && "Null pointer." );
  DALI_ASSERT_DEBUG( ((scanline1 >= scanline2 + width) || (scanline2 >= scanline1 + width )) && "Scanlines alias." );
  DALI_ASSERT_DEBUG( ((((void*)outputScanline) >= (void*)(scanline2 + width)) || (((void*)scanline2) >= (void*)(scanline1 + width))) && "Scanline 2 aliases output." );
  DALI_ASSERT_DEBUG( width >= 1u && "Can't average fewer than two pixels." );
}

} // namespace - unnamed

Integration::BitmapPtr DownscaleBitmap( Integration::Bitmap& bitmap, const ImageAttributes& requestedAttributes, const std::string& path )
{
  const Size requestedSize = requestedAttributes.GetSize();

  // If a different size than the raw one has been requested, resize the image:
  if( bitmap.GetPackedPixelsProfile() &&
      (requestedSize.x > 0.0f) && (requestedSize.y > 0.0f) &&
      (requestedSize.x < bitmap.GetImageWidth()) &&
      (requestedSize.y < bitmap.GetImageHeight()) )
  {
    const unsigned int bitmapWidth  = bitmap.GetImageWidth();
    const unsigned int bitmapHeight = bitmap.GetImageHeight();
    const Pixel::Format pixelFormat = bitmap.GetPixelFormat();

    if( pixelFormat == Pixel::RGBA8888 || pixelFormat == Pixel::RGB888 || pixelFormat == Pixel::LA88 )
    {
      unsigned int shrunkWidth = -1, shrunkHeight = -1;
      Integration::BitmapPtr shrunk = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::DISCARD );

      if( pixelFormat == Pixel::RGBA8888 )
      {
        Internal::Platform::DownscaleInPlacePow2RGBA8888( bitmap.GetBuffer(), bitmapWidth, bitmapHeight, requestedSize.x, requestedSize.y, shrunkWidth, shrunkHeight );
        if( shrunkWidth != bitmapWidth && shrunkHeight != bitmapHeight )
        {
          shrunk->GetPackedPixelsProfile()->ReserveBuffer( Pixel::RGBA8888, shrunkWidth, shrunkHeight, shrunkWidth, shrunkHeight );
        }
      }
      else if( pixelFormat == Pixel::RGB888 )
      {
        Internal::Platform::DownscaleInPlacePow2RGB888( bitmap.GetBuffer(), bitmapWidth, bitmapHeight, requestedSize.x, requestedSize.y, shrunkWidth, shrunkHeight );
        if( shrunkWidth != bitmapWidth && shrunkHeight != bitmapHeight )
        {
          shrunk->GetPackedPixelsProfile()->ReserveBuffer( Pixel::RGB888, shrunkWidth, shrunkHeight, shrunkWidth, shrunkHeight );
        }
      }
      else if( pixelFormat == Pixel::LA88 )
      {
        Internal::Platform::DownscaleInPlacePow2ComponentPair( bitmap.GetBuffer(), bitmapWidth, bitmapHeight, requestedSize.x, requestedSize.y, shrunkWidth, shrunkHeight );
        if( shrunkWidth != bitmapWidth && shrunkHeight != bitmapHeight )
        {
          shrunk->GetPackedPixelsProfile()->ReserveBuffer( Pixel::LA88, shrunkWidth, shrunkHeight, shrunkWidth, shrunkHeight );
        }
      }

      if( shrunk->GetBufferSize() > 0u )
      {
        DALI_ASSERT_DEBUG( bitmap.GetBuffer() && "Null loaded bitmap buffer." );
        DALI_ASSERT_DEBUG( shrunk->GetBuffer() && "Null shrunk bitmap buffer." );
        memcpy( shrunk->GetBuffer(), bitmap.GetBuffer(), shrunkWidth * shrunkHeight * Pixel::GetBytesPerPixel( pixelFormat ) );
        return shrunk;
      }
      else
      {
      }
    }
    else
    {
      DALI_LOG_INFO( Internal::Platform::gImageOpsLogFilter, Dali::Integration::Log::Verbose, "Bitmap was not shrunk: unsupported pixel format: %u.\n", unsigned(pixelFormat) );
    }
  }
  return Integration::BitmapPtr(&bitmap);
}

void DownscaleInPlacePow2RGBA8888(
    unsigned char * const pixels,
    const unsigned int inputWidth, const unsigned int inputHeight,
    const unsigned int desiredWidth, const unsigned int desiredHeight,
    unsigned& outWidth, unsigned& outHeight )
{
  DALI_ASSERT_DEBUG( pixels != 0 );
  DALI_ASSERT_DEBUG( ((reinterpret_cast<ptrdiff_t>(pixels) & 3u) == 0u) && "Pointer should be 4-byte aligned for performance on some platforms." );
  ValidateScalingParameters( inputWidth, inputHeight, desiredWidth, desiredHeight );

  uint32_t * const alignedPixels = reinterpret_cast<uint32_t* const>(pixels);

  // Scale the image until it would be smaller than desired, stopping if the
  // resulting height or width would be less than 1:
  unsigned int scaledWidth = inputWidth, scaledHeight = inputHeight;
  while( scaledWidth >> 1u >= desiredWidth && scaledHeight >> 1u >= desiredHeight &&
          scaledWidth >> 1u >= 1u           && scaledHeight >> 1u >= 1u )
  {
    const unsigned int lastWidth = scaledWidth;
    scaledWidth  >>= 1u;
    scaledHeight >>= 1u;

    DALI_LOG_INFO( gImageOpsLogFilter, Dali::Integration::Log::Verbose, "Scaling to %u\t%u.\n", scaledWidth, scaledHeight );

    const unsigned int lastScanlinePair = scaledHeight - 1;

    // Scale pairs of scanlines until any spare one at the end is dropped:
    for( unsigned int y = 0; y <= lastScanlinePair; ++y )
    {
      // Scale two scanlines horizontally:
      HalveScanlineInPlaceRGBA8888( &pixels[y * 2 * lastWidth * 4], lastWidth );
      HalveScanlineInPlaceRGBA8888( &pixels[(y * 2 + 1) * lastWidth * 4], lastWidth );

      // Scale vertical pairs of pixels while the last two scanlines are still warm in
      // the CPU cache(s):
      // Note, better access patterns for cache-coherence are possible for very large
      // images but even a 4k image will use just 32kB of cache (4k pixels * 4 Bpp *
      // 2 scanlines) for two scanlines on the first iteration.
      AverageScanlinesRGBA8888(
          &pixels[y * 2 * lastWidth * 4],
          &pixels[(y * 2 + 1) * lastWidth * 4],
          &alignedPixels[y * scaledWidth],
          scaledWidth );
    }
  }

  ///@note: we could finish off with one of two mutually exclusive passes, one squashing horizontally as far as possible, and the other vertically, if we knew a following cpu point or bilinear filter would restore the desired aspect ratio.
  outWidth = scaledWidth;
  outHeight = scaledHeight;
}

void DownscaleInPlacePow2RGB565(
    unsigned char * pixels,
    unsigned int inputWidth, unsigned int inputHeight,
    unsigned int desiredWidth, unsigned int desiredHeight,
    unsigned int& outWidth, unsigned int& outHeight )
{
  DALI_ASSERT_DEBUG( 1 == 0 && "Not implemented." );
}

/**
 * @copydoc DownscaleInPlacePow2RGB888
 *
 * For 2-byte formats such as lum8alpha8, but not packed 16 bit formats like RGB565.
 */
void DownscaleInPlacePow2ComponentPair(
    unsigned char * const pixels,
    const unsigned int inputWidth, const unsigned int inputHeight,
    const unsigned int desiredWidth, const unsigned int desiredHeight,
    unsigned& outWidth, unsigned& outHeight )
{
  DALI_ASSERT_DEBUG( pixels != 0 );
  ValidateScalingParameters( inputWidth, inputHeight, desiredWidth, desiredHeight );

  // Scale the image until it would be smaller than desired, stopping if the
  // resulting height or width would be less than 1:
  unsigned int scaledWidth = inputWidth, scaledHeight = inputHeight;
  while( scaledWidth >> 1u >= desiredWidth && scaledHeight >> 1u >= desiredHeight &&
          scaledWidth >> 1u >= 1u           && scaledHeight >> 1u >= 1u )
  {
    const unsigned int lastWidth = scaledWidth;
    scaledWidth  >>= 1u;
    scaledHeight >>= 1u;

    DALI_LOG_INFO( gImageOpsLogFilter, Dali::Integration::Log::Verbose, "Scaling to %u\t%u.\n", scaledWidth, scaledHeight );

    const unsigned int lastScanlinePair = scaledHeight - 1;

    // Scale pairs of scanlines until any spare one at the end is dropped:
    for( unsigned int y = 0; y <= lastScanlinePair; ++y )
    {
      // Scale two scanlines horizontally:
      HalveScanlineInPlace2Bytes( &pixels[y * 2 * lastWidth * 2], lastWidth );
      HalveScanlineInPlace2Bytes( &pixels[(y * 2 + 1) * lastWidth * 2], lastWidth );

      // Scale vertical pairs of pixels while the last two scanlines are still warm in
      // the CPU cache(s):
      AverageScanlines(
          &pixels[y * 2 * lastWidth * 2],
          &pixels[(y * 2 + 1) * lastWidth * 2],
          &pixels[y * scaledWidth * 2],
          scaledWidth * 2);
    }
  }

  ///@note: we could finish off with one of two mutually exclusive passes, one squashing horizontally as far as possible, and the other vertically, if we knew a following cpu point or bilinear filter would restore the desired aspect ratio.
  outWidth = scaledWidth;
  outHeight = scaledHeight;
}

void DownscaleInPlacePow2SingleBytePerPixel(
    unsigned char * pixels,
    unsigned int inputWidth, unsigned int inputHeight,
    unsigned int desiredWidth, unsigned int desiredHeight,
    unsigned int& outWidth, unsigned int& outHeight )
{
  DALI_ASSERT_DEBUG( 1 == 0 && "Not implemented." );
}

void DownscaleInPlacePow2RGB888(
    unsigned char * const pixels,
    const unsigned int inputWidth, const unsigned int inputHeight,
    const unsigned int desiredWidth, const unsigned int desiredHeight,
    unsigned& outWidth, unsigned& outHeight )
{
  DALI_ASSERT_DEBUG( pixels != 0 );
  ValidateScalingParameters( inputWidth, inputHeight, desiredWidth, desiredHeight );

  // Scale the image until it would be smaller than desired, stopping if the
  // resulting height or width would be less than 1:
  unsigned int scaledWidth = inputWidth, scaledHeight = inputHeight;
  while( scaledWidth >> 1u >= desiredWidth && scaledHeight >> 1u >= desiredHeight &&
          scaledWidth >> 1u >= 1u           && scaledHeight >> 1u >= 1u )
  {
    const unsigned int lastWidth = scaledWidth;
    scaledWidth  >>= 1u;
    scaledHeight >>= 1u;

    DALI_LOG_INFO( gImageOpsLogFilter, Dali::Integration::Log::Verbose, "Scaling to %u\t%u.\n", scaledWidth, scaledHeight );

    const unsigned int lastScanlinePair = scaledHeight - 1;

    // Scale pairs of scanlines until any spare one at the end is dropped:
    for( unsigned int y = 0; y <= lastScanlinePair; ++y )
    {
      // Scale two scanlines horizontally:
      HalveScanlineInPlaceRGB888( &pixels[y * 2 * lastWidth * 3], lastWidth );
      HalveScanlineInPlaceRGB888( &pixels[(y * 2 + 1) * lastWidth * 3], lastWidth );

      // Scale vertical pairs of pixels while the last two scanlines are still warm in
      // the CPU cache(s):
      // Note, better access patterns for cache-coherence are possible for very large
      // images but even a 4k image will use just 24kB of cache (4k pixels * 3 Bpp *
      // 2 scanlines) for two scanlines on the first iteration.
      AverageScanlines(
          &pixels[y * 2 * lastWidth * 3],
          &pixels[(y * 2 + 1) * lastWidth * 3],
          &pixels[y * scaledWidth * 3],
          scaledWidth * 3);
    }
  }

  ///@note: we could finish off with one of two mutually exclusive passes, one squashing horizontally as far as possible, and the other vertically, if we knew a following cpu point or bilinear filter would restore the desired aspect ratio.
  outWidth = scaledWidth;
  outHeight = scaledHeight;
}

void HalveScanlineInPlaceRGB888(
    unsigned char * const pixels,
    const unsigned int width )
{
  DebugAssertScanlineParameters( pixels, width );

  const unsigned int lastPair = EvenDown( width - 2 );

  for( unsigned int pixel = 0, outPixel = 0; pixel <= lastPair; pixel += 2, ++outPixel )
  {
    // Load all the byte pixel components we need:
    const unsigned int c11 = pixels[pixel * 3];
    const unsigned int c12 = pixels[pixel * 3 + 1];
    const unsigned int c13 = pixels[pixel * 3 + 2];
    const unsigned int c21 = pixels[pixel * 3 + 3];
    const unsigned int c22 = pixels[pixel * 3 + 4];
    const unsigned int c23 = pixels[pixel * 3 + 5];

    // Save the averaged byte pixel components:
    pixels[outPixel * 3]     = AverageComponent( c11, c21 );
    pixels[outPixel * 3 + 1] = AverageComponent( c12, c22 );
    pixels[outPixel * 3 + 2] = AverageComponent( c13, c23 );
  }
}

void HalveScanlineInPlaceRGBA8888(
    unsigned char * const pixels,
    const unsigned int width )
{
  DebugAssertScanlineParameters( pixels, width );
  DALI_ASSERT_DEBUG( ((reinterpret_cast<ptrdiff_t>(pixels) & 3u) == 0u) && "Pointer should be 4-byte aligned for performance on some platforms." );

  uint32_t* const alignedPixels = reinterpret_cast<uint32_t*>(pixels);

  const unsigned int lastPair = EvenDown( width - 2 );

  for( unsigned int pixel = 0, outPixel = 0; pixel <= lastPair; pixel += 2, ++outPixel )
  {
    const uint32_t averaged = AveragePixelRGBA8888( alignedPixels[pixel], alignedPixels[pixel + 1] );
    alignedPixels[outPixel] = averaged;
  }
}

void HalveScanlineInPlaceRGB565( unsigned char * pixels, unsigned int width )
{
  DebugAssertScanlineParameters( pixels, width );
  DALI_ASSERT_DEBUG( ((reinterpret_cast<ptrdiff_t>(pixels) & 1u) == 0u) && "Pointer should be 2-byte aligned for performance on some platforms." );

  uint16_t* const alignedPixels = reinterpret_cast<uint16_t*>(pixels);

  const unsigned int lastPair = EvenDown( width - 2 );

  for( unsigned int pixel = 0, outPixel = 0; pixel <= lastPair; pixel += 2, ++outPixel )
  {
    const uint32_t averaged = AveragePixelRGB565( alignedPixels[pixel], alignedPixels[pixel + 1] );
    alignedPixels[outPixel] = averaged;
  }
}

void HalveScanlineInPlace2Bytes(
    unsigned char * const pixels,
    const unsigned int width )
{
  DebugAssertScanlineParameters( pixels, width );

  const unsigned int lastPair = EvenDown( width - 2 );

  for( unsigned int pixel = 0, outPixel = 0; pixel <= lastPair; pixel += 2, ++outPixel )
  {
    // Load all the byte pixel components we need:
    const unsigned int c11 = pixels[pixel * 2];
    const unsigned int c12 = pixels[pixel * 2 + 1];
    const unsigned int c21 = pixels[pixel * 2 + 2];
    const unsigned int c22 = pixels[pixel * 2 + 3];

    // Save the averaged byte pixel components:
    pixels[outPixel * 2]     = AverageComponent( c11, c21 );
    pixels[outPixel * 2 + 1] = AverageComponent( c12, c22 );
  }
}

void HalveScanlineInPlace1Byte(
    unsigned char * const pixels,
    const unsigned int width )
{
  DebugAssertScanlineParameters( pixels, width );

  const unsigned int lastPair = EvenDown( width - 2 );

  for( unsigned int pixel = 0, outPixel = 0; pixel <= lastPair; pixel += 2, ++outPixel )
  {
    // Load all the byte pixel components we need:
    const unsigned int c1 = pixels[pixel];
    const unsigned int c2 = pixels[pixel + 1];

    // Save the averaged byte pixel component:
    pixels[outPixel] = AverageComponent( c1, c2 );
  }
}

/**
 * @ToDo: Optimise for ARM using a 4 bytes at a time loop wrapped around the single ARMV6 instruction: UHADD8  R4, R0, R5. Note, this is not neon. It runs in the normal integer pipeline so there is no downside like a stall moving between integer and copro.
 * if (widthInComponents >= 7) { word32* aligned1 = scanline1 + 3 & 3; word32* aligned1_end = scanline1 + widthInPixels & 3; while(aligned1 < aligned1_end) { UHADD8  *aligned1++, *aligned2++, *alignedoutput++ } .. + 0 to 3 spare pixels at each end.
 */
void AverageScanlines(
    const unsigned char * const scanline1,
    const unsigned char * const __restrict__ scanline2,
    unsigned char* const outputScanline,
    const unsigned int widthInComponents )
{
  DebugAssertDualScanlineParameters( scanline1, scanline2, outputScanline, widthInComponents / 3 );

  for( unsigned int component = 0; component < widthInComponents; ++component )
  {
    outputScanline[component] = AverageComponent( scanline1[component], scanline2[component] );
  }
}

void AverageScanlinesRGBA8888(
    const unsigned char * const scanline1,
    const unsigned char * const __restrict__ scanline2,
    uint32_t* const outputScanline,
    const unsigned int width )
{
  DebugAssertDualScanlineParameters( scanline1, scanline2, outputScanline, width );
  DALI_ASSERT_DEBUG( ((reinterpret_cast<ptrdiff_t>(scanline1) & 3u) == 0u) && "Pointer should be 4-byte aligned for performance on some platforms." );
  DALI_ASSERT_DEBUG( ((reinterpret_cast<ptrdiff_t>(scanline2) & 3u) == 0u) && "Pointer should be 4-byte aligned for performance on some platforms." );

  const uint32_t* const alignedScanline1 = reinterpret_cast<const uint32_t*>(scanline1);
  const uint32_t* const alignedScanline2 = reinterpret_cast<const uint32_t*>(scanline2);

  for( unsigned int pixel = 0; pixel < width; ++pixel )
  {
    outputScanline[pixel] = AveragePixelRGBA8888( alignedScanline1[pixel], alignedScanline2[pixel] );
  }
}

void AverageScanlinesRGB565(
    const uint16_t * const scanline1,
    const uint16_t * const __restrict__ scanline2,
    uint16_t* const outputScanline,
    const unsigned int width )
{
  DebugAssertDualScanlineParameters( scanline1, scanline2, outputScanline, width );

  for( unsigned int pixel = 0; pixel < width; ++pixel )
  {
    outputScanline[pixel] = AveragePixelRGB565( scanline1[pixel], scanline2[pixel] );
  }
}

} /* namespace Platform */
} /* namespace Internal */
} /* namespace Dali */
