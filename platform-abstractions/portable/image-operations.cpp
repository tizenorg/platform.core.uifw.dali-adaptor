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

#include "image-operations.h"

// EXTERNAL INCLUDES
#include <cstring>
#include <stddef.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES

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
Debug::Filter* gImageOpsLogFilter = Debug::Filter::New( Debug::NoLogging, false, "LOG_IMAGE_OPERATIONS" );
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
inline void DebugAssertDualScanlineParameters(
    const unsigned char * const scanline1,
    const unsigned char * const scanline2,
    unsigned char* const outputScanline,
    const size_t widthInComponents )
{
  DALI_ASSERT_DEBUG( scanline1 && "Null pointer." );
  DALI_ASSERT_DEBUG( scanline2 && "Null pointer." );
  DALI_ASSERT_DEBUG( outputScanline && "Null pointer." );
  DALI_ASSERT_DEBUG( ((scanline1 >= scanline2 + widthInComponents) || (scanline2 >= scanline1 + widthInComponents )) && "Scanlines alias." );
  DALI_ASSERT_DEBUG( ((((void*)outputScanline) >= (void*)(scanline2 + widthInComponents)) || (((void*)scanline2) >= (void*)(scanline1 + widthInComponents))) && "Scanline 2 aliases output." );
}

/**
 * @brief Work out the desired width and height according to the rules documented for the ImageAttributes class.
 *
 * Possible inputs:
 *   1. Source image dimensions.
 *   2. Both requested dimensions.
 *   3. Source image aspect ratio + single requested dimension.
 */
ImageDimensions CalculateDesiredDimensions( unsigned int bitmapWidth, unsigned int bitmapHeight, unsigned int requestedWidth, unsigned int requestedHeight )
{
  // If no dimensions have been requested, default to the source ones:
  if( requestedWidth == 0 && requestedHeight == 0 )
  {
    return ImageDimensions( bitmapWidth, bitmapHeight );
  }

  // If both dimensions have values requested, use them both:
  if( requestedWidth != 0 && requestedHeight != 0 )
  {
    return ImageDimensions( requestedWidth, requestedHeight );
  }

  // If only one of the dimensions has been requested, calculate the other from
  // the requested one and the source image aspect ratio:

  if( requestedWidth != 0 )
  {
    return ImageDimensions( requestedWidth, bitmapHeight / float(bitmapWidth) * requestedWidth + 0.5f );
  }
  return ImageDimensions( bitmapWidth / float(bitmapHeight) * requestedHeight + 0.5f, requestedHeight );
}

/**
 * @brief Converts a scaling mode to the definition of which dimensions matter when box filtering as a part of that mode.
 */
BoxDimensionTest DimensionTestForScalingMode( ImageAttributes::ScalingMode scalingMode )
{
  BoxDimensionTest dimensionTest;
  dimensionTest = BoxDimensionTestEither;

  switch( scalingMode )
  {
    // Shrink to fit attempts to make one or zero dimensions smaller than the
    // desired dimensions and one or two dimensions exactly the same as the desired
    // ones, so as long as one dimension is larger than the desired size, box
    // filtering can continue even if the second dimension is smaller than the
    // desired dimensions:
    case ImageAttributes::ShrinkToFit:
      dimensionTest = BoxDimensionTestEither;
      break;
    // Scale to fill mode keeps both dimensions at least as large as desired:
    case ImageAttributes::ScaleToFill:
      dimensionTest = BoxDimensionTestBoth;
      break;
    // Y dimension is irrelevant when downscaling in FitWidth mode:
    case ImageAttributes::FitWidth:
      dimensionTest = BoxDimensionTestX;
      break;
    // X Dimension is ignored by definition in FitHeight mode:
    case ImageAttributes::FitHeight:
      dimensionTest = BoxDimensionTestY;
  }

  return dimensionTest;
}

/**
 * \brief Work out the dimensions for a uniform scaling of the input to map it
 * into the target while effecting ShinkToFit scaling mode.
 */
ImageDimensions FitForShrinkToFit( ImageDimensions target, ImageDimensions source )
{
  DALI_ASSERT_DEBUG( true && " " );
  // Scale the input by the least extreme of the two dimensions:
  const float widthScale  = target.GetX() / float(source.GetX());
  const float heightScale = target.GetY() / float(source.GetY());
  const float scale = widthScale < heightScale ? widthScale : heightScale;

  // Do no scaling at all if the result would increase area:
  if( scale >= 1.0f )
  {
    return source;
  }

  return ImageDimensions( source.GetX() * scale + 0.5f, source.GetY() * scale + 0.5f );
}

/**
 * \brief Work out the dimensions for a uniform scaling of the input to map it
 * into the target while effecting ScaleToFill scaling mode.
 * \note The output dimensions will need either top and bottom or left and right
 * to be cropped away unless the source was pre-cropped to match the destination
 * aspect ratio.
 */
ImageDimensions FitForScaleToFill( ImageDimensions target, ImageDimensions source )
{
  DALI_ASSERT_DEBUG( source.GetX() > 0 && source.GetY() > 0  && "Zero-area rectangles should not be passed-in" );
  // Scale the input by the least extreme of the two dimensions:
  const float widthScale  = target.GetX() / float(source.GetX());
  const float heightScale = target.GetY() / float(source.GetY());
  const float scale = widthScale > heightScale ? widthScale : heightScale;

  // Do no scaling at all if the result would increase area:
  if( scale >= 1.0f )
  {
    return source;
  }

  return ImageDimensions( source.GetX() * scale + 0.5f, source.GetY() * scale + 0.5f );
}

/**
 * \brief Work out the dimensions for a uniform scaling of the input to map it
 * into the target while effecting FitWidth scaling mode.
 */
ImageDimensions FitForFitWidth( ImageDimensions target, ImageDimensions source )
{
  DALI_ASSERT_DEBUG( source.GetX() > 0 && "Cant fit a zero-dimension rectangle." );
  const float scale  = target.GetX() / float(source.GetX());

  // Do no scaling at all if the result would increase area:
  if( scale >= 1.0f )
  {
   return source;
  }
  return ImageDimensions( source.GetX() * scale + 0.5f, source.GetY() * scale + 0.5f );
}

/**
 * \brief Work out the dimensions for a uniform scaling of the input to map it
 * into the target while effecting FitHeight scaling mode.
 */
ImageDimensions FitForFitHeight( ImageDimensions target, ImageDimensions source )
{
  DALI_ASSERT_DEBUG( source.GetY() > 0 && "Cant fit a zero-dimension rectangle." );
  const float scale = target.GetY() / float(source.GetY());

  // Do no scaling at all if the result would increase area:
  if( scale >= 1.0f )
  {
    return source;
  }

  return ImageDimensions( source.GetX() * scale + 0.5f, source.GetY() * scale + 0.5f );
}

/**
 * \brief Generate the rectangle to use as the target of a pixel sampling pass
 * (e.g., nearest or linear).
 */
ImageDimensions FitToScalingMode( ImageDimensions requestedSize, ImageDimensions sourceSize, ImageAttributes::ScalingMode scalingMode )
{
  ImageDimensions fitDimensions;
  switch( scalingMode )
  {
    case ImageAttributes::ShrinkToFit:
      fitDimensions = FitForShrinkToFit( requestedSize, sourceSize );
      break;
    case ImageAttributes::ScaleToFill:
      fitDimensions = FitForScaleToFill( requestedSize, sourceSize );
      break;
    case ImageAttributes::FitWidth:
      fitDimensions = FitForFitWidth( requestedSize, sourceSize );
      break;
    case ImageAttributes::FitHeight:
      fitDimensions = FitForFitHeight( requestedSize, sourceSize );
      break;
  };

  return fitDimensions;
}

BitmapPtr MakeBitmap(const uint8_t * const pixels, Pixel::Format pixelFormat, unsigned int width, unsigned int height )
{
  DALI_ASSERT_DEBUG( pixels && "Null bitmap buffer to copy." );
  DALI_ASSERT_DEBUG( Pixel::GetBytesPerPixel(pixelFormat) && "Compressed formats not supported." );

  // Allocate a pixel buffer to hold the image passed in:
  Integration::BitmapPtr newBitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::DISCARD );
  newBitmap->GetPackedPixelsProfile()->ReserveBuffer( pixelFormat, width, height, width, height );

  // Copy over the pixels from the downscaled image that was generated in-place in the pixel buffer of the input bitmap:
  memcpy( newBitmap->GetBuffer(), pixels, width * height * Pixel::GetBytesPerPixel( pixelFormat ) );
  return newBitmap;
}

} // namespace - unnamed

/**
 * @brief Implement ImageAttributes::ScaleTofill scaling mode cropping.
 *
 * Implement the cropping required for ImageAttributes::ScaleToFill mode,
 * returning a new bitmap with the aspect ratio specified by the scaling mode.
 * This scaling mode selects the central portion of a source image so any spare
 * pixels off one of either the top or bottom edge need to be removed.
 *
 * @note If the input bitmap was not previously downscaled to exactly encompass
 * the desired output size, the resulting bitmap will have the correct aspect
 * ratio but will have larger dimensions than requested. This can be used to
 * fake the scaling mode by relying on the GPU scaling at render time.
 * If the input bitmap was previously maximally downscaled using a
 * repeated box filter, this is a reasonable approach.
 *
 * @return The bitmap passed in if no scaling is needed or possible, else a new,
 * smaller bitmap with the cropping required for the scaling mode applied.
 */
Integration::BitmapPtr CropForScaleToFill( Integration::BitmapPtr bitmap, ImageDimensions desiredDimensions );

BitmapPtr ApplyAttributesToBitmap( BitmapPtr bitmap, const ImageAttributes& requestedAttributes )
{
  ///@ToDo: Optimisation - If Scaling Mode is ScaletoFill, either do cropping here at the front of the pipe, or equivalently modify all scaling functions to take a source rectangle and have the first one to be applied, pull in a subset of source pixels to crop on the fly. That would make every scaling slightly slower but would save the memcpy()s at the end for ScaleToFill.

  if( bitmap )
  {
    // Calculate the desired box, accounting for a possible zero component:
    const ImageDimensions desiredDimensions  = CalculateDesiredDimensions( bitmap->GetImageWidth(), bitmap->GetImageHeight(), requestedAttributes.GetWidth(), requestedAttributes.GetHeight() );

    // If a different size than the raw one has been requested, resize the image
    // maximally using a repeated box filter without making it smaller than the
    // requested size in either dimension:
    bitmap = DownscaleBitmap( *bitmap, desiredDimensions, requestedAttributes.GetScalingMode(), requestedAttributes.GetFilterMode() );

    // Cut the bitmap according to the desired width and height so that the
    // resulting bitmap has the same aspect ratio as the desired dimensions:
    if( bitmap && bitmap->GetPackedPixelsProfile() && requestedAttributes.GetScalingMode() == ImageAttributes::ScaleToFill )
    {
      bitmap = CropForScaleToFill( bitmap, desiredDimensions );
    }

    // Examine the image pixels remaining after cropping and scaling to see if all
    // are opaque, allowing faster rendering, or some have non-1.0 alpha:
    if( bitmap && bitmap->GetPackedPixelsProfile() && Pixel::HasAlpha( bitmap->GetPixelFormat() ) )
    {
      bitmap->GetPackedPixelsProfile()->TestForTransparency();
    }
  }

  return bitmap;
}

BitmapPtr CropForScaleToFill( BitmapPtr bitmap, ImageDimensions desiredDimensions )
{
  const unsigned inputWidth = bitmap->GetImageWidth();
  const unsigned inputHeight = bitmap->GetImageHeight();
  const unsigned desiredWidth = desiredDimensions.GetWidth();
  const unsigned desiredHeight = desiredDimensions.GetHeight();

  if( desiredWidth < 1U || desiredHeight < 1U )
  {
    DALI_LOG_WARNING( "Image scaling aborted as desired dimensions too small (%u, %u)\n.", desiredWidth, desiredHeight );
  }
  else if( inputWidth != desiredWidth || inputHeight != desiredHeight )
  {
    const Vector2 desiredDims( desiredWidth, desiredHeight );

    // Scale the desired rectangle back to fit inside the rectangle of the loaded bitmap:
    // There are two candidates (scaled by x, and scaled by y) and we choose the smallest area one.
    const float widthsRatio = inputWidth / float(desiredWidth);
    const Vector2 scaledByWidth = desiredDims * widthsRatio;
    const float heightsRatio = inputHeight / float(desiredHeight);
    const Vector2 scaledByHeight = desiredDims * heightsRatio;
    // Trim top and bottom if the area of the horizontally-fitted candidate is less, else trim the sides:
    const bool trimTopAndBottom = scaledByWidth.width * scaledByWidth.height < scaledByHeight.width * scaledByHeight.height;
    const Vector2 scaledDims = trimTopAndBottom ? scaledByWidth : scaledByHeight;

    // Work out how many pixels to trim from top and bottom, and left and right:
    // (We only ever do one dimension)
    const unsigned scanlinesToTrim = trimTopAndBottom ? fabsf( (scaledDims.y - inputHeight) * 0.5f ) : 0;
    const unsigned columnsToTrim = trimTopAndBottom ? 0 : fabsf( (scaledDims.x - inputWidth) * 0.5f );

    DALI_LOG_INFO( gImageOpsLogFilter, Debug::Concise, "Bitmap, desired(%f, %f), loaded(%u,%u), cut_target(%f, %f), trimmed(%u, %u), vertical = %s.\n", desiredDims.x, desiredDims.y, inputWidth, inputHeight, scaledDims.x, scaledDims.y, columnsToTrim, scanlinesToTrim, trimTopAndBottom ? "true" : "false" );

    // Make a new bitmap with the central part of the loaded one if required:
    if( scanlinesToTrim > 0 || columnsToTrim > 0 )
    {
      const unsigned newWidth = inputWidth - 2 * columnsToTrim;
      const unsigned newHeight = inputHeight - 2 * scanlinesToTrim;
      BitmapPtr croppedBitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::DISCARD );
      Integration::Bitmap::PackedPixelsProfile * packedView = croppedBitmap->GetPackedPixelsProfile();
      DALI_ASSERT_DEBUG( packedView );
      const Pixel::Format pixelFormat = bitmap->GetPixelFormat();
      packedView->ReserveBuffer( pixelFormat, newWidth, newHeight, newWidth, newHeight );

      const unsigned bytesPerPixel = Pixel::GetBytesPerPixel( pixelFormat );

      const PixelBuffer * const srcPixels = bitmap->GetBuffer() + scanlinesToTrim * inputWidth * bytesPerPixel;
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
          memcpy( &destPixels[y * newWidth * bytesPerPixel], &srcPixels[y * inputWidth * bytesPerPixel + columnsToTrim * bytesPerPixel], newWidth * bytesPerPixel );
        }
      }

      // Overwrite the loaded bitmap with the cropped version:
      bitmap = croppedBitmap;
    }
  }

  return bitmap;
}

Integration::BitmapPtr DownscaleBitmap( Integration::Bitmap& bitmap,
                                          ImageDimensions desired,
                                          ImageAttributes::ScalingMode scalingMode,
                                          ImageAttributes::FilterMode filterMode )
{
  // Source dimensions as loaded from resources (e.g. filesyustem):
  const unsigned int bitmapWidth  = bitmap.GetImageWidth();
  const unsigned int bitmapHeight = bitmap.GetImageHeight();
  // Desired dimensions (the rectangle to fit the source image to):
  const unsigned int desiredWidth = desired.GetWidth();
  const unsigned int desiredHeight = desired.GetHeight();

  BitmapPtr outputBitmap( &bitmap );

  // If a different size than the raw one has been requested, resize the image:
  if( bitmap.GetPackedPixelsProfile() &&
      (desiredWidth > 0.0f) && (desiredHeight > 0.0f) &&
      ((desiredWidth < bitmapWidth) || (desiredHeight < bitmapHeight)) )
  {
    const Pixel::Format pixelFormat = bitmap.GetPixelFormat();

    // Do the fast power of 2 iterated box filter to get to roughly the right side if the filter mode requests that:
    unsigned int shrunkWidth = -1, shrunkHeight = -1;
    DownscaleInPlacePow2( bitmap.GetBuffer(), pixelFormat, bitmapWidth, bitmapHeight, desiredWidth, desiredHeight, scalingMode, filterMode, shrunkWidth, shrunkHeight );

    // Work out the dimensions of the downscaled bitmap, given the scaling mode and desired dimensions:
    const ImageDimensions filteredDimensions = FitToScalingMode( ImageDimensions( desiredWidth, desiredHeight ), ImageDimensions( shrunkWidth, shrunkHeight ), scalingMode );
    const unsigned int filteredWidth = filteredDimensions.GetWidth();
    const unsigned int filteredHeight = filteredDimensions.GetHeight();

    // Run a filter to scale down the bitmap if it needs it:
    if( (filteredWidth < shrunkWidth || filteredHeight < shrunkHeight) &&
        (filterMode == ImageAttributes::Nearest || filterMode == ImageAttributes::Linear ||
         filterMode == ImageAttributes::BoxThenNearest || filterMode == ImageAttributes::BoxThenLinear) )
    {
      ///@note If a linear filter is requested, we do our best with a point filter for now.
      PointSample( bitmap.GetBuffer(), shrunkWidth, shrunkHeight, pixelFormat, bitmap.GetBuffer(), filteredWidth, filteredHeight );

      outputBitmap =  MakeBitmap( bitmap.GetBuffer(), pixelFormat, filteredWidth, filteredHeight );
    }
    // Copy out the 2^x downscaled, box-filtered pixels if no secondary filter (point or linear) was applied:
    else if( shrunkWidth < bitmapWidth || shrunkHeight < bitmapHeight )
    {
      outputBitmap = MakeBitmap( bitmap.GetBuffer(), pixelFormat, shrunkWidth, shrunkHeight );
    }
  }

  return outputBitmap;
}

namespace
{
/**
 * @brief Returns whether to keep box filtering based on whether downscaled dimensions will overshoot the desired ones aty the next step.
 * @param test Which combination of the two dimensions matter for terminating the filtering.
 * @param scaledWidth The width of the current downscaled image.
 * @param scaledHeight The height of the current downscaled image.
 * @param desiredWidth The target width for the downscaling.
 * @param desiredHeight The target height for the downscaling.
 */
bool ContinueScaling( BoxDimensionTest test, unsigned int scaledWidth, unsigned int scaledHeight, unsigned int desiredWidth, unsigned int desiredHeight )
{
  bool keepScaling = false;
  const unsigned int nextWidth = scaledWidth >> 1u;
  const unsigned int nextHeight = scaledHeight >> 1u;

  if( nextWidth >= 1u && nextHeight >= 1u )
  {
    switch( test )
    {
      case BoxDimensionTestEither:
        keepScaling = nextWidth >= desiredWidth || nextHeight >= desiredHeight;
        break;
      case BoxDimensionTestBoth:
        keepScaling = nextWidth >= desiredWidth && nextHeight >= desiredHeight;
        break;
      case BoxDimensionTestX:
        keepScaling = nextWidth >= desiredWidth;
        break;
      case BoxDimensionTestY:
        keepScaling = nextHeight >= desiredHeight;
    }
  }

  return keepScaling;
}

/**
 * @brief A shared implementation of the overall iterative downscaling algorithm.
 *
 * Specialise this for particular pixel formats by supplying the number of bytes
 * per pixel and two functions: one for averaging pairs of neighbouring pixels
 * on a single scanline, and a second for averaging pixels at corresponding
 * positions on different scanlines.
 **/
template<
  int BYTES_PER_PIXEL,
  void (*HalveScanlineInPlace)( unsigned char * const pixels, const unsigned int width ),
  void (*AverageScanlines) ( const unsigned char * const scanline1, const unsigned char * const __restrict__ scanline2, unsigned char* const outputScanline, const unsigned int width )
>
void DownscaleInPlacePow2Generic(
    unsigned char * const pixels,
    const unsigned int inputWidth, const unsigned int inputHeight,
    const unsigned int desiredWidth, const unsigned int desiredHeight,
    BoxDimensionTest dimensionTest,
    unsigned& outWidth, unsigned& outHeight )
{
  if( pixels == 0 )
  {
    return;
  }
  ValidateScalingParameters( inputWidth, inputHeight, desiredWidth, desiredHeight );

  // Scale the image until it would be smaller than desired, stopping if the
  // resulting height or width would be less than 1:
  unsigned int scaledWidth = inputWidth, scaledHeight = inputHeight;
  while( ContinueScaling( dimensionTest, scaledWidth, scaledHeight, desiredWidth, desiredHeight ) )
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
      HalveScanlineInPlace( &pixels[y * 2 * lastWidth * BYTES_PER_PIXEL], lastWidth );
      HalveScanlineInPlace( &pixels[(y * 2 + 1) * lastWidth * BYTES_PER_PIXEL], lastWidth );

      // Scale vertical pairs of pixels while the last two scanlines are still warm in
      // the CPU cache(s):
      // Note, better access patterns for cache-coherence are possible for very large
      // images but even a 4k wide RGB888 image will use just 24kB of cache (4k pixels
      // * 3 Bpp * 2 scanlines) for two scanlines on the first iteration.
      AverageScanlines(
          &pixels[y * 2 * lastWidth * BYTES_PER_PIXEL],
          &pixels[(y * 2 + 1) * lastWidth * BYTES_PER_PIXEL],
          &pixels[y * scaledWidth * BYTES_PER_PIXEL],
          scaledWidth );
    }
  }

  ///@note: we could finish off with one of two mutually exclusive passes, one squashing horizontally as far as possible, and the other vertically, if we knew a following cpu point or bilinear filter would restore the desired aspect ratio.
  outWidth = scaledWidth;
  outHeight = scaledHeight;
}

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
 * @ToDo: Optimise for ARM using a 4 bytes at a time loop wrapped around the single ARMV6 instruction: UHADD8  R4, R0, R5. Note, this is not neon. It runs in the normal integer pipeline so there is no downside like a stall moving between integer and copro, or extra power for clocking-up the idle copro.
 * if (widthInComponents >= 7) { word32* aligned1 = scanline1 + 3 & 3; word32* aligned1_end = scanline1 + widthInPixels & 3; while(aligned1 < aligned1_end) { UHADD8  *aligned1++, *aligned2++, *alignedoutput++ } .. + 0 to 3 spare pixels at each end.
 */
void AverageScanlines1(
    const unsigned char * const scanline1,
    const unsigned char * const __restrict__ scanline2,
    unsigned char* const outputScanline,
    const unsigned int width )
{
  DebugAssertDualScanlineParameters( scanline1, scanline2, outputScanline, width );

  for( unsigned int component = 0; component < width; ++component )
  {
    outputScanline[component] = AverageComponent( scanline1[component], scanline2[component] );
  }
}

void AverageScanlines2(
    const unsigned char * const scanline1,
    const unsigned char * const __restrict__ scanline2,
    unsigned char* const outputScanline,
    const unsigned int width )
{
  DebugAssertDualScanlineParameters( scanline1, scanline2, outputScanline, width * 2 );

  for( unsigned int component = 0; component < width * 2; ++component )
  {
    outputScanline[component] = AverageComponent( scanline1[component], scanline2[component] );
  }
}

void AverageScanlines3(
    const unsigned char * const scanline1,
    const unsigned char * const __restrict__ scanline2,
    unsigned char* const outputScanline,
    const unsigned int width )
{
  DebugAssertDualScanlineParameters( scanline1, scanline2, outputScanline, width * 3 );

  for( unsigned int component = 0; component < width * 3; ++component )
  {
    outputScanline[component] = AverageComponent( scanline1[component], scanline2[component] );
  }
}

void AverageScanlinesRGBA8888(
    const unsigned char * const scanline1,
    const unsigned char * const __restrict__ scanline2,
    unsigned char * const outputScanline,
    const unsigned int width )
{
  DebugAssertDualScanlineParameters( scanline1, scanline2, outputScanline, width * 4 );
  DALI_ASSERT_DEBUG( ((reinterpret_cast<ptrdiff_t>(scanline1) & 3u) == 0u) && "Pointer should be 4-byte aligned for performance on some platforms." );
  DALI_ASSERT_DEBUG( ((reinterpret_cast<ptrdiff_t>(scanline2) & 3u) == 0u) && "Pointer should be 4-byte aligned for performance on some platforms." );
  DALI_ASSERT_DEBUG( ((reinterpret_cast<ptrdiff_t>(outputScanline) & 3u) == 0u) && "Pointer should be 4-byte aligned for performance on some platforms." );

  const uint32_t* const alignedScanline1 = reinterpret_cast<const uint32_t*>(scanline1);
  const uint32_t* const alignedScanline2 = reinterpret_cast<const uint32_t*>(scanline2);
  uint32_t* const alignedOutput = reinterpret_cast<uint32_t*>(outputScanline);

  for( unsigned int pixel = 0; pixel < width; ++pixel )
  {
    alignedOutput[pixel] = AveragePixelRGBA8888( alignedScanline1[pixel], alignedScanline2[pixel] );
  }
}

void AverageScanlinesRGB565(
    const unsigned char * const scanline1,
    const unsigned char * const __restrict__ scanline2,
    unsigned char * const outputScanline,
    const unsigned int width )
{
  DebugAssertDualScanlineParameters( scanline1, scanline2, outputScanline, width * 2 );
  DALI_ASSERT_DEBUG( ((reinterpret_cast<ptrdiff_t>(scanline1) & 1u) == 0u) && "Pointer should be 2-byte aligned for performance on some platforms." );
  DALI_ASSERT_DEBUG( ((reinterpret_cast<ptrdiff_t>(scanline2) & 1u) == 0u) && "Pointer should be 2-byte aligned for performance on some platforms." );
  DALI_ASSERT_DEBUG( ((reinterpret_cast<ptrdiff_t>(outputScanline) & 1u) == 0u) && "Pointer should be 2-byte aligned for performance on some platforms." );

  const uint16_t* const alignedScanline1 = reinterpret_cast<const uint16_t*>(scanline1);
  const uint16_t* const alignedScanline2 = reinterpret_cast<const uint16_t*>(scanline2);
  uint16_t* const alignedOutput = reinterpret_cast<uint16_t*>(outputScanline);

  for( unsigned int pixel = 0; pixel < width; ++pixel )
  {
    alignedOutput[pixel] = AveragePixelRGB565( alignedScanline1[pixel], alignedScanline2[pixel] );
  }
}

/// Dispatch to pixel format appropriate box filter downscaling functions.
void DownscaleInPlacePow2(
    unsigned char * const pixels,
    Pixel::Format pixelFormat,
    unsigned int inputWidth, unsigned int inputHeight,
    unsigned int desiredWidth, unsigned int desiredHeight,
    ImageAttributes::ScalingMode scalingMode,
    ImageAttributes::FilterMode filterMode,
    unsigned& outWidth, unsigned& outHeight )
{
  outWidth = inputWidth;
  outHeight = inputHeight;
  // Perform power of 2 iterated 4:1 box filtering if the requested filter mode requires it:
  if( filterMode == ImageAttributes::Box || filterMode == ImageAttributes::BoxThenNearest || filterMode == ImageAttributes::BoxThenLinear )
  {
    // Check the pixel format is one that is supported:
    if( pixelFormat == Pixel::RGBA8888 || pixelFormat == Pixel::RGB888 || pixelFormat == Pixel::RGB565 || pixelFormat == Pixel::LA88 || pixelFormat == Pixel::L8 || pixelFormat == Pixel::A8 )
    {
      const BoxDimensionTest dimensionTest = DimensionTestForScalingMode( scalingMode );

      if( pixelFormat == Pixel::RGBA8888 )
      {
        Internal::Platform::DownscaleInPlacePow2RGBA8888( pixels, inputWidth, inputHeight, desiredWidth, desiredHeight, dimensionTest, outWidth, outHeight );
      }
      else if( pixelFormat == Pixel::RGB888 )
      {
        Internal::Platform::DownscaleInPlacePow2RGB888( pixels, inputWidth, inputHeight, desiredWidth, desiredHeight, dimensionTest, outWidth, outHeight );
      }
      else if( pixelFormat == Pixel::RGB565 )
      {
        Internal::Platform::DownscaleInPlacePow2RGB565( pixels, inputWidth, inputHeight, desiredWidth, desiredHeight, dimensionTest, outWidth, outHeight );
      }
      else if( pixelFormat == Pixel::LA88 )
      {
        Internal::Platform::DownscaleInPlacePow2ComponentPair( pixels, inputWidth, inputHeight, desiredWidth, desiredHeight, dimensionTest, outWidth, outHeight );
      }
      else if( pixelFormat == Pixel::L8  || pixelFormat == Pixel::A8 )
      {
        Internal::Platform::DownscaleInPlacePow2SingleBytePerPixel( pixels, inputWidth, inputHeight, desiredWidth, desiredHeight, dimensionTest, outWidth, outHeight );
      }
    }
  }
  else
  {
    DALI_LOG_INFO( gImageOpsLogFilter, Dali::Integration::Log::Verbose, "Bitmap was not shrunk: unsupported pixel format: %u.\n", unsigned(pixelFormat) );
  }
}

void DownscaleInPlacePow2RGB888(
    unsigned char * const pixels,
    const unsigned int inputWidth, const unsigned int inputHeight,
    const unsigned int desiredWidth, const unsigned int desiredHeight,
    BoxDimensionTest dimensionTest,
    unsigned& outWidth, unsigned& outHeight )
{
  DownscaleInPlacePow2Generic<3, HalveScanlineInPlaceRGB888, AverageScanlines3>( pixels, inputWidth, inputHeight, desiredWidth, desiredHeight, dimensionTest, outWidth, outHeight );
}

void DownscaleInPlacePow2RGBA8888(
    unsigned char * const pixels,
    const unsigned int inputWidth, const unsigned int inputHeight,
    const unsigned int desiredWidth, const unsigned int desiredHeight,
    BoxDimensionTest dimensionTest,
    unsigned& outWidth, unsigned& outHeight )
{
  DALI_ASSERT_DEBUG( ((reinterpret_cast<ptrdiff_t>(pixels) & 3u) == 0u) && "Pointer should be 4-byte aligned for performance on some platforms." );
  DownscaleInPlacePow2Generic<4, HalveScanlineInPlaceRGBA8888, AverageScanlinesRGBA8888>( pixels, inputWidth, inputHeight, desiredWidth, desiredHeight, dimensionTest, outWidth, outHeight );
}

void DownscaleInPlacePow2RGB565(
    unsigned char * pixels,
    unsigned int inputWidth, unsigned int inputHeight,
    unsigned int desiredWidth, unsigned int desiredHeight,
    BoxDimensionTest dimensionTest,
    unsigned int& outWidth, unsigned int& outHeight )
{
  DownscaleInPlacePow2Generic<2, HalveScanlineInPlaceRGB565, AverageScanlinesRGB565>( pixels, inputWidth, inputHeight, desiredWidth, desiredHeight, dimensionTest, outWidth, outHeight );
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
    BoxDimensionTest dimensionTest,
    unsigned& outWidth, unsigned& outHeight )
{
  DownscaleInPlacePow2Generic<2, HalveScanlineInPlace2Bytes, AverageScanlines2>( pixels, inputWidth, inputHeight, desiredWidth, desiredHeight, dimensionTest, outWidth, outHeight );
}

void DownscaleInPlacePow2SingleBytePerPixel(
    unsigned char * pixels,
    unsigned int inputWidth, unsigned int inputHeight,
    unsigned int desiredWidth, unsigned int desiredHeight,
    BoxDimensionTest dimensionTest,
    unsigned int& outWidth, unsigned int& outHeight )
{
  DownscaleInPlacePow2Generic<1, HalveScanlineInPlace1Byte, AverageScanlines1>( pixels, inputWidth, inputHeight, desiredWidth, desiredHeight, dimensionTest, outWidth, outHeight );
}

namespace
{

// Point sample an image to a new resolution (like GL_NEAREST).
template<typename PIXEL>
void PointSampleAddressablePixels(
    const unsigned char * inPixels,
    unsigned int inputWidth, unsigned int inputHeight,
    unsigned char * outPixels,
    unsigned int desiredWidth, unsigned int desiredHeight )
{
  DALI_ASSERT_DEBUG( ((desiredHeight <= inputHeight && desiredWidth <= inputHeight) || outPixels >= inPixels + inputWidth * inputHeight * sizeof(PIXEL) || outPixels <= inPixels - desiredWidth * desiredHeight * sizeof(PIXEL)) && "The input and output buffers must not overlap for an upscaling.");
  DALI_ASSERT_DEBUG( ((uint64_t) inPixels)  % sizeof(PIXEL) == 0 && "Pixel pointers need to be aligned to the size of the pixels (E.g., 4 bytes for RGBA, 2 bytes for RGB565, ...)." );
  DALI_ASSERT_DEBUG( ((uint64_t) outPixels) % sizeof(PIXEL) == 0 && "Pixel pointers need to be aligned to the size of the pixels (E.g., 4 bytes for RGBA, 2 bytes for RGB565, ...)." );

  if( inputWidth < 1u || inputHeight < 1u || desiredWidth < 1u || desiredHeight < 1u )
  {
    return;
  }
  const PIXEL* const inAligned = reinterpret_cast<const PIXEL*>(inPixels);
  PIXEL* const       outAligned = reinterpret_cast<PIXEL*>(outPixels);
  const unsigned int deltaX = (inputWidth  << 16u) / desiredWidth;
  const unsigned int deltaY = (inputHeight << 16u) / desiredHeight;

  unsigned int inY = 0;
  for( unsigned int outY = 0; outY < desiredHeight; ++outY )
  {
    // Round fixed point y coordinate to nearest integer:
    const unsigned int integerY = (inY + (1u << 15u)) >> 16u;
    const PIXEL* const inScanline = &inAligned[inputWidth * integerY];
    PIXEL* const outScanline = &outAligned[desiredWidth * outY];
    unsigned int inX = 0;
    for( unsigned int outX = 0; outX < desiredWidth; ++outX )
    {
      // Round the fixed-point x coordinate to an integer:
      const unsigned int integerX = (inX + (1u << 15u)) >> 16u;
      const PIXEL* const inPixelAddress = &inScanline[integerX];
      const PIXEL pixel = *inPixelAddress;
      outScanline[outX] = pixel;
      inX += deltaX;
    }
    inY += deltaY;
  }
}

}

/*
 * RGBA8888
 */
void PointSample4BPP(
    const unsigned char * inPixels,
    unsigned int inputWidth, unsigned int inputHeight,
    unsigned char * outPixels,
    unsigned int desiredWidth, unsigned int desiredHeight )
{
  PointSampleAddressablePixels<uint32_t>( inPixels, inputWidth, inputHeight, outPixels, desiredWidth, desiredHeight );
}

/*
 * RGB565, LA88
 */
void PointSample2BPP(
    const unsigned char * inPixels,
    unsigned int inputWidth, unsigned int inputHeight,
    unsigned char * outPixels,
    unsigned int desiredWidth, unsigned int desiredHeight )
{
  PointSampleAddressablePixels<uint16_t>( inPixels, inputWidth, inputHeight, outPixels, desiredWidth, desiredHeight );
}

/*
 * L8, A8
 */
void PointSample1BPP(
    const unsigned char * inPixels,
    unsigned int inputWidth, unsigned int inputHeight,
    unsigned char * outPixels,
    unsigned int desiredWidth, unsigned int desiredHeight )
{
  PointSampleAddressablePixels<uint8_t>( inPixels, inputWidth, inputHeight, outPixels, desiredWidth, desiredHeight );
}

/*
 * RGB888
 * RGB888 is a special case as its pixels are not aligned addressable units.
 */
void PointSample3BPP(
    const uint8_t * inPixels,
    unsigned int inputWidth, unsigned int inputHeight,
    uint8_t * outPixels,
    unsigned int desiredWidth, unsigned int desiredHeight )
{
  if( inputWidth < 1u || inputHeight < 1u || desiredWidth < 1u || desiredHeight < 1u )
  {
    return;
  }
  const unsigned int BYTES_PER_PIXEL = 3;

  // Generate fixed-point 16.16 deltas in input image coordinates:
  const unsigned int deltaX = (inputWidth  << 16u) / desiredWidth;
  const unsigned int deltaY = (inputHeight << 16u) / desiredHeight;

  // Step through output image in whole integer pixel steps while tracking the
  // corresponding locations in the input image using 16.16 fixed-point
  // coordinates:
  unsigned int inY = 0; //< 16.16 fixed-point input image y-coord.
  for( unsigned int outY = 0; outY < desiredHeight; ++outY )
  {
    const uint8_t* const inScanline = &inPixels[inputWidth * (inY >> 16u) * BYTES_PER_PIXEL];
    uint8_t* const outScanline = &outPixels[desiredWidth * outY * BYTES_PER_PIXEL];
    unsigned int inX = 0; //< 16.16 fixed-point input image x-coord.

    for( unsigned int outX = 0; outX < desiredWidth * BYTES_PER_PIXEL; outX += BYTES_PER_PIXEL )
    {
      // Truncate the fixed-point input coordinate to the address of the input pixel to sample:
      const uint8_t* const inPixelAddress = &inScanline[(inX >> 16u) * BYTES_PER_PIXEL];

      // Issue loads for all pixel color components up-front:
      const unsigned int c0 = inPixelAddress[0];
      const unsigned int c1 = inPixelAddress[1];
      const unsigned int c2 = inPixelAddress[2];
      ///@ToDo: Benchmark one 32bit load that will be unaligned 2/3 of the time + 3 rotate and masks, versus these three aligned byte loads.

      // Output the pixel components:
      outScanline[outX]     = c0;
      outScanline[outX + 1] = c1;
      outScanline[outX + 2] = c2;

      // Increment the fixed-point input coordinate:
      inX += deltaX;
    }

    inY += deltaY;
  }
}

// Dispatch to a format-appropriate point sampling function:
void PointSample(
    const unsigned char * inPixels,
    unsigned int inputWidth, unsigned int inputHeight,
    Pixel::Format pixelFormat,
    unsigned char * outPixels,
    unsigned int desiredWidth, unsigned int desiredHeight )
{
  // Check the pixel format is one that is supported:
  if( pixelFormat == Pixel::RGBA8888 || pixelFormat == Pixel::RGB888 || pixelFormat == Pixel::RGB565 || pixelFormat == Pixel::LA88 || pixelFormat == Pixel::L8 || pixelFormat == Pixel::A8 )
  {
    if( pixelFormat == Pixel::RGBA8888 )
    {
      PointSample4BPP( inPixels, inputWidth, inputHeight, outPixels, desiredWidth, desiredHeight );
    }
    else if( pixelFormat == Pixel::RGB888 )
    {
      PointSample3BPP( inPixels, inputWidth, inputHeight, outPixels, desiredWidth, desiredHeight );
    }
    else if( pixelFormat == Pixel::RGB565 || pixelFormat == Pixel::LA88 )
    {
      PointSample2BPP( inPixels, inputWidth, inputHeight, outPixels, desiredWidth, desiredHeight );
    }
    else if( pixelFormat == Pixel::L8  || pixelFormat == Pixel::A8 )
    {
      PointSample2BPP( inPixels, inputWidth, inputHeight, outPixels, desiredWidth, desiredHeight );
    }
  }
  else
  {
  DALI_LOG_INFO( gImageOpsLogFilter, Dali::Integration::Log::Verbose, "Bitmap was not point sampled: unsupported pixel format: %u.\n", unsigned(pixelFormat) );
  }
}

} /* namespace Platform */
} /* namespace Internal */
} /* namespace Dali */
