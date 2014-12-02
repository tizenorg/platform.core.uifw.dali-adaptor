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

#ifndef DALI_INTERNAL_PLATFORM_IMAGE_OPERATIONS_H_
#define DALI_INTERNAL_PLATFORM_IMAGE_OPERATIONS_H_

// INTERNAL INCLUDES
#include <dali/integration-api/bitmap.h>
#include <dali/public-api/images/image-attributes.h>

// EXTERNAL INCLUDES
#include <stdint.h>

namespace Dali
{
namespace Internal
{
namespace Platform
{

/**
 * @brief Apply requested attributes to bitmap
 */
Integration::BitmapPtr ApplyAttributesToBitmap( Integration::BitmapPtr bitmap, const ImageAttributes& requestedAttributes, const std::string& path );

/**
 * @brief Implement ImageAttributes::ScaleTofill scaling mode.
 * Implement the ImageAttributes::ScaleToFill mode, returning a new bitmap with the aspect ratio specified by the scaling mode.
 * @note This fakes the scaling with a crop and relies on the GPU scaling at
 * render time. If the input bitmap was previously maximally downscaled using a
 * repeated box filter, this is a reasonable approach.
 * @return The bitmap passed in if no scaling is needed or possible, else a new,
 * smaller bitmap with the scaling mode applied.
 */
Integration::BitmapPtr ProcessBitmapScaleToFill( Integration::BitmapPtr bitmap, const ImageAttributes& requestedAttributes, const std::string& path, Integration::Log::Filter* logFilter = 0);

/**
 * @brief Apply rough, mip-map style power of 2 downscaling to a bitmap.
 * @note The input bitmap is modified and left in an invalid state so must be discarded.
 **/
Integration::BitmapPtr DownscaleBitmap( Integration::Bitmap& bitmap, const ImageAttributes& requestedAttributes, const std::string& path );

/**
 * Destructive in-place downscaling by a power of 2 factor, throwing away any
 * odd pixels.
 * A box filter with a 2x2 kernel is repeatedly applied until the desired
 * dimensions are greater than half the current one's.
 * To reach the exact final dimensions, the client can apply a further
 * downscaling step, either by bilinear or point sampling on the CPU, or by
 * uploading the image to a texture and letting the GPU downsample it on
 * the fly at render time (note, if the last image is less than 2x the desired
 * dimensions - as it will be in at least one dimension, GPU bilinear filtering
 * should work well and not introduce any additional aliasing).
 */
void DownscaleInPlacePow2RGB888(
    unsigned char * const pixels,
    const unsigned inputWidth, const unsigned inputHeight,
    const unsigned desiredWidth, const unsigned desiredHeight,
    unsigned& outWidth, unsigned& outHeight );

/**
 * @copydoc DownscaleInPlacePow2RGB888
 */
void DownscaleInPlacePow2RGBA8888(
    unsigned char * const pixels,
    const unsigned inputWidth, const unsigned inputHeight,
    const unsigned desiredWidth, const unsigned desiredHeight,
    unsigned& outWidth, unsigned& outHeight );

/**
 * @copydoc DownscaleInPlacePow2RGB888
 *
 * For 2-byte formats such as lum8alpha8, but not packed 16 bit formats like RGB565.
 */
void DownscaleInPlacePow2ComponentPair(
    unsigned char * const pixels,
    const unsigned inputWidth, const unsigned inputHeight,
    const unsigned desiredWidth, const unsigned desiredHeight,
    unsigned& outWidth, unsigned& outHeight );

/**
 * Destroy input while averaging adjacent pairs of RGB888 pixels.
 */
void HalveScanlineInPlaceRGB888( unsigned char * const pixels, const unsigned width );

/**
 * Average byte pixel components at corresponding offsets in two scanlines.
 * Works in bytes so potentially slower than a specialised function for 2 or 4
 * byte pixels.
 * Don't use on formats for which one component != 1 byte such as 16BPP ones.
 * Output is allowed to alias scanline1.
 */
void AverageScanlines(
    const unsigned char * const scanline1,
    const unsigned char * const scanline2,
    unsigned char* const outputScanline,
    /** Image width in bytes, not pixels (unless 1 byte == 1 pixel: e.g. lum8 or alpha8).*/
    const unsigned widthInComponents );

/**
 * Average pairs of RGBA8888 pixels at corresponding offsets in two scanlines.
 * Output is allowed to alias scanline1.
 */
void AverageScanlinesRGBA8888(
    const unsigned char * const scanline1,
    const unsigned char * const scanline2,
    uint32_t* const outputScanline,
    /** Image width in pixels, not bytes.*/
    const unsigned width );

/**
 * Destroy input while averaging adjacent pairs of RGBA8888 pixels.
 */
void HalveScanlineInPlaceRGBA8888(
    unsigned char * const pixels,
    const unsigned width );

/**
 * Destroy input while averaging adjacent pairs of 2-component pixels.
 */
void HalveScanlineInPlace2Bytes(
    unsigned char * const pixels,
    const unsigned width );

} /* namespace Platform */
} /* namespace Internal */
} /* namespace Dali */

#endif /* DALI_INTERNAL_PLATFORM_IMAGE_OPERATIONS_H_ */
