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

/**
 * Apply requested attributes to bitmap
 */
Integration::BitmapPtr ApplyAttributesToBitmap( Integration::BitmapPtr bitmap, const ImageAttributes& requestedAttributes, const std::string& path );

namespace
{
using Integration::Bitmap;
using Integration::BitmapPtr;
typedef unsigned char PixelBuffer;

#if defined(DEBUG_ENABLED)
/**
 * Disable logging of image operations or make it verbose from the commandline as follows:
 * <code>
 * LOG_IMAGE_OPERATIONS=0 dali-demo #< off
 * LOG_IMAGE_OPERATIONS=3 dali-demo #< on, verbose
 * </code>
 */
Debug::Filter* gImageOpsLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_IMAGE_OPERATIONS");
#endif

/** @return The greatest even number less than or equal to the argument. */
inline unsigned evenDown( const unsigned a )
{
  const unsigned evened = a & ~1u;
  return evened;
}

/** @return The average of two uint arguments. */
inline unsigned averageComponent( const unsigned a, const unsigned b )
{
  const unsigned avg = (a + b) >> 1u;
  return avg;
}

/** @return The average of two RGBA8888 pixels. */
inline uint32_t averagePixelRGBA8888( const uint32_t a, const uint32_t b )
{
  const unsigned avg =
      ((averageComponent( (a & 0xff000000) >> 1u, (b & 0xff000000) >> 1u ) << 1u) & 0xff000000 ) +
      (averageComponent( a & 0x00ff0000, b & 0x00ff0000 ) & 0x00ff0000 ) +
      (averageComponent( a & 0x0000ff00, b & 0x0000ff00 ) & 0x0000ff00 ) +
      (averageComponent( a & 0x000000ff, b & 0x000000ff ) & 0x000000ff );
  return avg;
  ///@ToDo: Optimise by trying return (((a ^ b) & 0xfefefefeUL) >> 1) + (a & b);
  ///@ToDo: Optimise for ARM using the single ARMV6 instruction: UHADD8  R4, R0, R5. This is not neon. It runs in the normal integer pipeline so there is no downside like a stall moving between integer and copro.
}

} // namespace - unnamed

void HalveScanlineInPlaceRGB888(
    unsigned char * const pixels,
    const unsigned width )
{
  DALI_ASSERT_DEBUG( pixels && "Null pointer." );
  DALI_ASSERT_DEBUG( width > 1u && "Can't average fewer than two pixels." );
  DALI_ASSERT_DEBUG( width < 131072u && "Unusually wide image: are you sure you meant to pass that value in?" );

  const unsigned lastPair = evenDown( width - 2 );

  for( unsigned pixel = 0, outPixel = 0; pixel <= lastPair; pixel += 2, ++outPixel )
  {
    // Load all the byte pixel components we need:
    const unsigned c11 = pixels[pixel * 3];
    const unsigned c12 = pixels[pixel * 3 + 1];
    const unsigned c13 = pixels[pixel * 3 + 2];
    const unsigned c21 = pixels[pixel * 3 + 3];
    const unsigned c22 = pixels[pixel * 3 + 4];
    const unsigned c23 = pixels[pixel * 3 + 5];

    // Save the averaged byte pixel components:
    pixels[outPixel * 3]     = averageComponent( c11, c21 );
    pixels[outPixel * 3 + 1] = averageComponent( c12, c22 );
    pixels[outPixel * 3 + 2] = averageComponent( c13, c23 );
  }
}

void AverageScanlines(
    const unsigned char * const scanline1,
    const unsigned char * const scanline2,
    unsigned char* const outputScanline,
    /** Image width in bytes, not pixels (unless 1 byte == 1 pixel: e.g. lum8 or alpha8).*/
    const unsigned widthInComponents )
{
  DALI_ASSERT_DEBUG( scanline1 && "Null pointer." );
  DALI_ASSERT_DEBUG( scanline2 && "Null pointer." );
  DALI_ASSERT_DEBUG( widthInComponents >= 1u && "Can't average fewer than two pixels." );

  for( unsigned component = 0; component < widthInComponents; ++component )
  {
    outputScanline[component] = averageComponent( scanline1[component], scanline2[component] );
  }
}

void DownscaleInPlacePow2RGB888(
    unsigned char * const pixels,
    const unsigned inputWidth, const unsigned inputHeight,
    const unsigned desiredWidth, const unsigned desiredHeight,
    unsigned& outWidth, unsigned& outHeight )
{
  DALI_ASSERT_DEBUG( pixels != 0 );
  DALI_ASSERT_DEBUG( inputWidth > 0u && inputHeight > 0u && "Zero area images cannot be scaled" );
  DALI_ASSERT_DEBUG( desiredWidth <= inputWidth && "Upscaling not supported." );
  DALI_ASSERT_DEBUG( desiredHeight <= inputHeight && "Upscaling not supported." );
  DALI_ASSERT_DEBUG( desiredWidth > 0u && desiredHeight >= 0u && "Downscaling to a zero-area target is pointless." );

  // Scale the image until it would be smaller than desired, stopping if the
  // resulting height or width would be less than 1:
  unsigned scaledWidth = inputWidth, scaledHeight = inputHeight;
  while( scaledWidth >> 1u >= desiredWidth && scaledHeight >> 1u >= desiredHeight &&
          scaledWidth >> 1u >= 1u           && scaledHeight >> 1u >= 1u )
  {
    const unsigned lastWidth = scaledWidth;
    scaledWidth  >>= 1u;
    scaledHeight >>= 1u;

    DALI_LOG_INFO( gImageOpsLogFilter, Dali::Integration::Log::Verbose, "Scaling to %u\t%u.\n", scaledWidth, scaledHeight );

    const unsigned lastScanlinePair = scaledHeight - 1;

    // Scale pairs of scanlines until any spare one at the end is dropped:
    for( unsigned y = 0; y <= lastScanlinePair; ++y )
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

void HalveScanlineInPlace2Bytes(
    unsigned char * const pixels,
    const unsigned width )
{
  DALI_ASSERT_DEBUG( pixels && "Null pointer." );
  DALI_ASSERT_DEBUG( width > 1u && "Can't average fewer than two pixels." );
  DALI_ASSERT_DEBUG( width < 131072u && "Unusually wide image: are you sure you meant to pass that value in?" );

  const unsigned lastPair = evenDown( width - 2 );

  for( unsigned pixel = 0, outPixel = 0; pixel <= lastPair; pixel += 2, ++outPixel )
  {
    // Load all the byte pixel components we need:
    const unsigned c11 = pixels[pixel * 2];
    const unsigned c12 = pixels[pixel * 2 + 1];
    const unsigned c21 = pixels[pixel * 2 + 2];
    const unsigned c22 = pixels[pixel * 2 + 3];

    // Save the averaged byte pixel components:
    pixels[outPixel * 2]     = averageComponent( c11, c21 );
    pixels[outPixel * 2 + 1] = averageComponent( c12, c22 );
  }
}

/**
 * @copydoc DownscaleInPlacePow2RGB888
 *
 * For 2-byte formats such as lum8alpha8, but not packed 16 bit formats like RGB565.
 */
void DownscaleInPlacePow2ComponentPair(
    unsigned char * const pixels,
    const unsigned inputWidth, const unsigned inputHeight,
    const unsigned desiredWidth, const unsigned desiredHeight,
    unsigned& outWidth, unsigned& outHeight )
{
  DALI_ASSERT_DEBUG( pixels != 0 );
  DALI_ASSERT_DEBUG( inputWidth > 0u && inputHeight > 0u && "Zero area images cannot be scaled" );
  DALI_ASSERT_DEBUG( desiredWidth <= inputWidth && "Upscaling not supported." );
  DALI_ASSERT_DEBUG( desiredHeight <= inputHeight && "Upscaling not supported." );
  DALI_ASSERT_DEBUG( desiredWidth > 0u && desiredHeight >= 0u && "Downscaling to a zero-area target is pointless." );

  // Scale the image until it would be smaller than desired, stopping if the
  // resulting height or width would be less than 1:
  unsigned scaledWidth = inputWidth, scaledHeight = inputHeight;
  while( scaledWidth >> 1u >= desiredWidth && scaledHeight >> 1u >= desiredHeight &&
          scaledWidth >> 1u >= 1u           && scaledHeight >> 1u >= 1u )
  {
    const unsigned lastWidth = scaledWidth;
    scaledWidth  >>= 1u;
    scaledHeight >>= 1u;

    DALI_LOG_INFO( gImageOpsLogFilter, Dali::Integration::Log::Verbose, "Scaling to %u\t%u.\n", scaledWidth, scaledHeight );

    const unsigned lastScanlinePair = scaledHeight - 1;

    // Scale pairs of scanlines until any spare one at the end is dropped:
    for( unsigned y = 0; y <= lastScanlinePair; ++y )
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

void HalveScanlineInPlaceRGBA8888(
    unsigned char * const pixels,
    const unsigned width )
{
  DALI_ASSERT_DEBUG( pixels && "Null pointer." );
  DALI_ASSERT_DEBUG( ((reinterpret_cast<ptrdiff_t>(pixels) & 3u) == 0u) && "Pointer should be 4-byte aligned for performance on some platforms." );
  DALI_ASSERT_DEBUG( width > 1u && "Can't average fewer than two pixels." );
  DALI_ASSERT_DEBUG( width < 131072u && "Unusually wide image: are you sure you meant to pass that value in?" );

  uint32_t* const alignedPixels = reinterpret_cast<uint32_t*>(pixels);

  const unsigned lastPair = evenDown( width - 2 );

  for( unsigned pixel = 0, outPixel = 0; pixel <= lastPair; pixel += 2, ++outPixel )
  {
    const uint32_t averaged = averagePixelRGBA8888( alignedPixels[pixel], alignedPixels[pixel + 1] );
    alignedPixels[outPixel] = averaged;
  }
}

void AverageScanlinesRGBA8888(
    const unsigned char * const scanline1,
    const unsigned char * const scanline2,
    uint32_t* const outputScanline,
    /** Image width in pixels, not bytes.*/
    const unsigned width )
{
  DALI_ASSERT_DEBUG( scanline1 && "Null pointer." );
  DALI_ASSERT_DEBUG( scanline2 && "Null pointer." );
  DALI_ASSERT_DEBUG( ((reinterpret_cast<ptrdiff_t>(scanline1) & 3u) == 0u) && "Pointer should be 4-byte aligned for performance on some platforms." );
  DALI_ASSERT_DEBUG( ((reinterpret_cast<ptrdiff_t>(scanline2) & 3u) == 0u) && "Pointer should be 4-byte aligned for performance on some platforms." );
  DALI_ASSERT_DEBUG( width >= 1u && "Can't average fewer than two pixels." );
  DALI_ASSERT_DEBUG( width < 131072u && "Unusually wide image: are you sure you meant to pass that value in?" );

  const uint32_t* const alignedScanline1 = reinterpret_cast<const uint32_t*>(scanline1);
  const uint32_t* const alignedScanline2 = reinterpret_cast<const uint32_t*>(scanline2);

  for( unsigned pixel = 0; pixel < width; ++pixel )
  {
    outputScanline[pixel] = averagePixelRGBA8888( alignedScanline1[pixel], alignedScanline2[pixel] );
  }
}

void DownscaleInPlacePow2RGBA8888(
    unsigned char * const pixels,
    const unsigned inputWidth, const unsigned inputHeight,
    const unsigned desiredWidth, const unsigned desiredHeight,
    unsigned& outWidth, unsigned& outHeight )
{
  DALI_ASSERT_DEBUG( pixels != 0 );
  DALI_ASSERT_DEBUG( ((reinterpret_cast<ptrdiff_t>(pixels) & 3u) == 0u) && "Pointer should be 4-byte aligned for performance on some platforms." );
  DALI_ASSERT_DEBUG( inputWidth > 0u && inputHeight > 0u && "Zero area images cannot be scaled" );
  DALI_ASSERT_DEBUG( desiredWidth <= inputWidth && "Upscaling not supported." );
  DALI_ASSERT_DEBUG( desiredHeight <= inputHeight && "Upscaling not supported." );
  DALI_ASSERT_DEBUG( desiredWidth > 0u && desiredHeight >= 0u && "Downscaling to a zero-area target is pointless." );

  uint32_t * const alignedPixels = reinterpret_cast<uint32_t* const>(pixels);

  // Scale the image until it would be smaller than desired, stopping if the
  // resulting height or width would be less than 1:
  unsigned scaledWidth = inputWidth, scaledHeight = inputHeight;
  while( scaledWidth >> 1u >= desiredWidth && scaledHeight >> 1u >= desiredHeight &&
          scaledWidth >> 1u >= 1u           && scaledHeight >> 1u >= 1u )
  {
    const unsigned lastWidth = scaledWidth;
    scaledWidth  >>= 1u;
    scaledHeight >>= 1u;

    DALI_LOG_INFO( gImageOpsLogFilter, Dali::Integration::Log::Verbose, "Scaling to %u\t%u.\n", scaledWidth, scaledHeight );

    const unsigned lastScanlinePair = scaledHeight - 1;

    // Scale pairs of scanlines until any spare one at the end is dropped:
    for( unsigned y = 0; y <= lastScanlinePair; ++y )
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

Integration::BitmapPtr DownscaleBitmap( Integration::Bitmap& bitmap, const ImageAttributes& requestedAttributes, const std::string& path )
{
  const Size requestedSize = requestedAttributes.GetSize();

  // If a different size than the raw one has been requested, resize the image:
  if( bitmap.GetPackedPixelsProfile() &&
      (requestedSize.x > 0.0f) && (requestedSize.y > 0.0f) &&
      (requestedSize.x < bitmap.GetImageWidth()) &&
      (requestedSize.y < bitmap.GetImageHeight()) )
  {
    const unsigned bitmapWidth  = bitmap.GetImageWidth();
    const unsigned bitmapHeight = bitmap.GetImageHeight();
    const Pixel::Format pixelFormat = bitmap.GetPixelFormat();

    if( pixelFormat == Pixel::RGBA8888 || pixelFormat == Pixel::RGB888 || pixelFormat == Pixel::LA88 )
    {
      unsigned shrunkWidth = -1, shrunkHeight = -1;
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

BitmapPtr ProcessBitmapScaleToFill( BitmapPtr bitmap, const ImageAttributes& requestedAttributes, const std::string& path, Integration::Log::Filter* logFilter )
{
  const unsigned loadedWidth = bitmap->GetImageWidth();
  const unsigned loadedHeight = bitmap->GetImageHeight();
  const unsigned desiredWidth = requestedAttributes.GetWidth();
  const unsigned desiredHeight = requestedAttributes.GetHeight();

  if( desiredWidth < 1U || desiredHeight < 1U )
  {
    DALI_LOG_WARNING( "Image scaling aborted for image %s as desired dimensions too small (%u, %u)\n.", path.c_str(), desiredWidth, desiredHeight );
  }
  else if( loadedWidth != desiredWidth || loadedHeight != desiredHeight )
  {
    const Vector2 desiredDims( desiredWidth, desiredHeight );

    // Scale the desired rectangle back to fit inside the rectangle of the loaded bitmap:
    // There are two candidates (scaled by x, and scaled by y) and we choose the smallest area one.
    const float widthsRatio = loadedWidth / float(desiredWidth);
    const Vector2 scaledByWidth = desiredDims * widthsRatio;
    const float heightsRatio = loadedHeight / float(desiredHeight);
    const Vector2 scaledByHeight = desiredDims * heightsRatio;
    // Trim top and bottom if the area of the horizontally-fitted candidate is less, else trim the sides:
    const bool trimTopAndBottom = scaledByWidth.width * scaledByWidth.height < scaledByHeight.width * scaledByHeight.height;
    const Vector2 scaledDims = trimTopAndBottom ? scaledByWidth : scaledByHeight;

    // Work out how many pixels to trim from top and bottom, and left and right:
    // (We only ever do one dimension)
    const unsigned scanlinesToTrim = trimTopAndBottom ? fabsf( (scaledDims.y - loadedHeight) * 0.5f ) : 0;
    const unsigned columnsToTrim = trimTopAndBottom ? 0 : fabsf( (scaledDims.x - loadedWidth) * 0.5f );

    DALI_LOG_INFO( logFilter, Debug::Concise, "Bitmap, desired(%f, %f), loaded(%u,%u), cut_target(%f, %f), trimmed(%u, %u), vertical = %s.\n", desiredDims.x, desiredDims.y, loadedWidth, loadedHeight, scaledDims.x, scaledDims.y, columnsToTrim, scanlinesToTrim, trimTopAndBottom ? "true" : "false" );

    // Make a new bitmap with the central part of the loaded one if required:
    if( scanlinesToTrim > 0 || columnsToTrim > 0 )
    {
      const unsigned newWidth = loadedWidth - 2 * columnsToTrim;
      const unsigned newHeight = loadedHeight - 2 * scanlinesToTrim;
      BitmapPtr croppedBitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::DISCARD );
      Integration::Bitmap::PackedPixelsProfile * packedView = croppedBitmap->GetPackedPixelsProfile();
      DALI_ASSERT_DEBUG( packedView );
      const Pixel::Format pixelFormat = bitmap->GetPixelFormat();
      packedView->ReserveBuffer( pixelFormat, newWidth, newHeight, newWidth, newHeight );

      const unsigned bytesPerPixel = Pixel::GetBytesPerPixel( pixelFormat );

      const PixelBuffer * const srcPixels = bitmap->GetBuffer() + scanlinesToTrim * loadedWidth * bytesPerPixel;
      PixelBuffer * const destPixels = croppedBitmap->GetBuffer();
      DALI_ASSERT_DEBUG( srcPixels && destPixels );

      // Optimize to a single memcpy if the left and right edges don't need a crop, else copy a scanline at a time:
      if( trimTopAndBottom )
      {
        memcpy( destPixels, srcPixels, newHeight * newWidth * bytesPerPixel );
      }
      else
      {
        for( unsigned y = 0; y < newHeight; ++y )
        {
          memcpy( &destPixels[y * newWidth * bytesPerPixel], &srcPixels[y * loadedWidth * bytesPerPixel + columnsToTrim * bytesPerPixel], newWidth * bytesPerPixel );
        }
      }

      // Overwrite the loaded bitmap with the cropped version:
      bitmap = croppedBitmap;
    }
  }

  return bitmap;
}

BitmapPtr ApplyAttributesToBitmap( BitmapPtr bitmap, const ImageAttributes& requestedAttributes, const std::string& path )
{
  // If a different size than the raw one has been requested, resize the image
  // maximally using a repeated box filter without making it smaller than the
  // requested size in either dimension:
  if( bitmap )
  {
    bitmap = DownscaleBitmap( *bitmap, requestedAttributes, path );
  }

  // Cut the bitmap according to the desired width and height so that the
  // resulting bitmap has the same aspect ratio as the desired dimensions:
  if( bitmap && bitmap->GetPackedPixelsProfile() && requestedAttributes.GetScalingMode() == ImageAttributes::ScaleToFill )
  {
    bitmap = ProcessBitmapScaleToFill( bitmap, requestedAttributes, path );
  }

  // Examine the image pixels remaining after cropping and scaling to see if all
  // are opaque, allowing faster rendering, or some have non-1.0 alpha:
  if( bitmap && bitmap->GetPackedPixelsProfile() && Pixel::HasAlpha( bitmap->GetPixelFormat() ) )
  {
    bitmap->GetPackedPixelsProfile()->TestForTransparency();
  }
  return bitmap;
}

} /* namespace Platform */
} /* namespace Internal */
} /* namespace Dali */
