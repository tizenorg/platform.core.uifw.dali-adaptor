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

#include <iostream>
#include <stdlib.h>
#include <dali-test-suite-utils.h>

#include "platform-abstractions/portable/image-operations.h"

using namespace Dali::Internal::Platform;

namespace
{

uint32_t RandomInRange( const uint32_t max )
{
  return floor( double(rand()) / RAND_MAX * (max + 0.999999) );
}

inline uint32_t RandomComponent8()
{
  return RandomInRange( 255u );
}

inline uint32_t RandomComponent5()
{
  return RandomInRange( 31u );
}

inline uint32_t RandomComponent6()
{
  return RandomInRange( 63u );
}

inline uint32_t PixelRGBA8888( const uint32_t r, const uint32_t g, const uint32_t b, const uint32_t a )
{
  return (r << 24) + (g << 16) + (b << 8) + a;
}

inline uint16_t PixelRGB565( const uint32_t r, const uint32_t g, const uint32_t b )
{
  return (r << 11) + (g << 5) + b;
}

void SetupScanlineForHalvingTestsRGBA8888( const size_t scanlineLength, Dali::Vector<uint32_t>& scanline, Dali::Vector<uint32_t>& reference )
{
  scanline.Resize( scanlineLength );
  reference.Reserve( scanlineLength / 2 + 32 );

  // Prepare some random pixels:
  srand( 19 * 23 * 47 * 53 );
  for( size_t i = 0; i < scanlineLength / 2; ++i )
  {
    // Generate random colors:
    const uint32_t red1   = RandomComponent8();
    const uint32_t red2   = RandomComponent8();
    const uint32_t green1 = RandomComponent8();
    const uint32_t green2 = RandomComponent8();
    const uint32_t blue1  = RandomComponent8();
    const uint32_t blue2  = RandomComponent8();
    const uint32_t alpha1 = RandomComponent8();
    const uint32_t alpha2 = RandomComponent8();

    // The average of these pixels should equal the reference:
    scanline[i * 2]     = PixelRGBA8888( red1, green1, blue1, alpha1 );
    scanline[i * 2 + 1] = PixelRGBA8888( red2, green2, blue2, alpha2 );

    // Average the two pixels manually as a reference:
    reference.PushBack( PixelRGBA8888( (red1 + red2) >> 1u, (green1 + green2) >> 1u, (blue1 + blue2) >> 1u, (alpha1 + alpha2) >> 1u ) );
  }

  for( size_t i = scanlineLength / 2; i < reference.Capacity(); ++i )
  {
    reference[i] = 0xEEEEEEEE;
  }
}

void SetupScanlineForHalvingTestsRGB565( const size_t scanlineLength, Dali::Vector<uint16_t>& scanline, Dali::Vector<uint16_t>& reference )
{
  scanline.Resize( scanlineLength );
  reference.Reserve( scanlineLength / 2 + 32 );

  // Prepare some random pixels:
  srand( 19 * 23 * 47 * 53 );
  for( size_t i = 0; i < scanlineLength / 2; ++i )
  {
    // Generate random colors:
    const uint32_t red1   = RandomComponent5();
    const uint32_t red2   = RandomComponent5();
    const uint32_t green1 = RandomComponent6();
    const uint32_t green2 = RandomComponent6();
    const uint32_t blue1  = RandomComponent5();
    const uint32_t blue2  = RandomComponent5();

    // The average of these pixels should equal the reference:
    scanline[i * 2]     = PixelRGB565( red1, green1, blue1 );
    scanline[i * 2 + 1] = PixelRGB565( red2, green2, blue2 );

    // Average the two pixels manually as a reference:
    reference.PushBack( PixelRGB565( (red1 + red2) >> 1u, (green1 + green2) >> 1u, (blue1 + blue2) >> 1u ) );
  }

  for( size_t i = scanlineLength / 2; i < reference.Capacity(); ++i )
  {
    reference[i] = 0xEEEE;
  }
}

void SetupScanlineForHalvingTests2Bytes( const size_t scanlineLength, Dali::Vector<uint8_t>& scanline, Dali::Vector<uint8_t>& reference )
{
  scanline.Resize( scanlineLength * 2 );
  reference.Reserve( scanlineLength + 32 );

  // Prepare some random pixels:
  srand( 19 * 23 * 47 * 53 * 59 );
  for( size_t i = 0; i < scanlineLength / 2; ++i )
  {
    // Generate random colors:
    const uint32_t c11   = RandomComponent8();
    const uint32_t c12   = RandomComponent8();
    const uint32_t c21   = RandomComponent8();
    const uint32_t c22   = RandomComponent8();

    // The average of these pixels should equal the reference:
    scanline[i * 4]     = c11;
    scanline[i * 4 + 1] = c12;
    scanline[i * 4 + 2] = c21;
    scanline[i * 4 + 3] = c22;

    // Average the two pixels manually as a reference:
    reference.PushBack( (c11 + c21) >> 1u );
    reference.PushBack( (c12 + c22) >> 1u );
  }

  for( size_t i = scanlineLength; i < reference.Capacity(); ++i )
  {
    reference[i] = 0xEE;
  }
}

void SetupScanlineForHalvingTests1Byte( const size_t scanlineLength, Dali::Vector<uint8_t>& scanline, Dali::Vector<uint8_t>& reference )
{
  scanline.Resize( scanlineLength * 2 );
  reference.Reserve( scanlineLength + 32 );

  // Prepare some random pixels:
  srand( 19 * 23 * 47 * 53 * 63 );
  for( size_t i = 0; i < scanlineLength / 2; ++i )
  {
    // Generate random colors:
    const uint32_t c1 = RandomComponent8();
    const uint32_t c2 = RandomComponent8();

    // The average of these pixels should equal the reference:
    scanline[i * 2]     = c1;
    scanline[i * 2 + 1] = c2;

    // Average the two pixels manually as a reference:
    reference.PushBack( (c1 + c2) >> 1u );

  }

  for( size_t i = scanlineLength; i < reference.Capacity(); ++i )
  {
    reference[i] = 0xEE;
  }
}

void SetupScanlinesRGBA8888( const size_t scanlineLength, Dali::Vector<uint32_t>& scanline1, Dali::Vector<uint32_t>& scanline2, Dali::Vector<uint32_t>& reference, Dali::Vector<uint32_t>& output )
{
  scanline1.Reserve( scanlineLength );
  scanline2.Reserve( scanlineLength );
  reference.Reserve( scanlineLength + 32 );
  output.Reserve( scanlineLength + 32 );

  for( size_t i = scanlineLength; i < output.Capacity(); ++i )
  {
    output[i]    = 0xDEADBEEF;
    reference[i] = 0xDEADBEEF;
  }

  // Prepare some random pixels:
  srand( 19 * 23 * 47 );
  for( size_t i = 0; i < scanlineLength; ++i )
  {
    // Generate random colors:
    const uint32_t red1   = RandomComponent8();
    const uint32_t red2   = RandomComponent8();
    const uint32_t green1 = RandomComponent8();
    const uint32_t green2 = RandomComponent8();
    const uint32_t blue1  = RandomComponent8();
    const uint32_t blue2  = RandomComponent8();
    const uint32_t alpha1 = RandomComponent8();
    const uint32_t alpha2 = RandomComponent8();

    // The average of these pixels should equal the reference:
    scanline1.PushBack( PixelRGBA8888( red1, green1, blue1, alpha1 ) );
    scanline2.PushBack( PixelRGBA8888( red2, green2, blue2, alpha2 ) );

    // Average the two pixels manually as a reference:
    reference.PushBack( PixelRGBA8888( (red1 + red2) >> 1u, (green1 + green2) >> 1u, (blue1 + blue2) >> 1u, (alpha1 + alpha2) >> 1u ) );
  }
}

void MatchScanlinesRGBA8888( Dali::Vector<uint32_t>& reference, Dali::Vector<uint32_t>& output, size_t &numMatches )
{
  numMatches = 0;
  for( size_t i = 0, length = reference.Capacity(); i < length; ++i )
  {
    DALI_TEST_EQUALS( output[i], reference[i], TEST_LOCATION );
    numMatches += output[i] == reference[i];
  }
}

} //< namespace unnamed

int UtcDaliImageOperationsAverageComponent(void)
{
  DALI_TEST_EQUALS( Dali::Internal::Platform::AverageComponent( 0u, 0u ), 0u, TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::Internal::Platform::AverageComponent( 1u, 1u ), 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::Internal::Platform::AverageComponent( 0xffffffffu >> 1u, 0xffffffffu >> 1u ), 0xffffffffu >> 1u, TEST_LOCATION );
  const unsigned int avg3 = Dali::Internal::Platform::AverageComponent( 0xfffffffeu, 1u );
  DALI_TEST_EQUALS( avg3, 0x7fffffffu, TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::Internal::Platform::AverageComponent( 255u, 255u ), 255u, TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::Internal::Platform::AverageComponent( 512u, 0u ), 256u, TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::Internal::Platform::AverageComponent( 511u, 0u ), 255u, TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::Internal::Platform::AverageComponent( 510u, 0u ), 255u, TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::Internal::Platform::AverageComponent( 509u, 0u ), 254u, TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::Internal::Platform::AverageComponent( 0u, 509u ), 254u, TEST_LOCATION );
  END_TEST;
}

int UtcDaliImageOperationsAveragePixelRGBA8888(void)
{
  DALI_TEST_EQUALS( Dali::Internal::Platform::AveragePixelRGBA8888( 0u, 0u ), 0u, TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::Internal::Platform::AveragePixelRGBA8888( 0x01010101, 0x01010101 ), 0x01010101u, TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::Internal::Platform::AveragePixelRGBA8888( 0x01010101, 0x03030303 ), 0x02020202u, TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::Internal::Platform::AveragePixelRGBA8888( 0xffffffff, 0xffffffff ), 0xffffffffu, TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::Internal::Platform::AveragePixelRGBA8888( 0xffffffff, 0u ), 0x7f7f7f7fu, TEST_LOCATION );
  END_TEST;
}

int UtcDaliImageOperationsAveragePixelRGB565(void)
{
  DALI_TEST_EQUALS( Dali::Internal::Platform::AveragePixelRGB565( 0u, 0u ), 0u, TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::Internal::Platform::AveragePixelRGB565( 0xf800u, 0xf800u ), 0xf800u, TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::Internal::Platform::AveragePixelRGB565( 0xf800u, 0x800u ), 1u << 15, TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::Internal::Platform::AveragePixelRGB565( 0x7e0u, 0x7e0u ), 0x7e0u, TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::Internal::Platform::AveragePixelRGB565( 0x7e0u, 0x20u ), 1u << 10, TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::Internal::Platform::AveragePixelRGB565( 0x1f, 0x1f ), 0x1fu, TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::Internal::Platform::AveragePixelRGB565( 0x1f, 0x1 ), 1u << 4, TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::Internal::Platform::AveragePixelRGB565( 0xf800u, 0x7e0u ), 0x7800u + 0x3e0u, TEST_LOCATION );
  DALI_TEST_EQUALS( Dali::Internal::Platform::AveragePixelRGB565( 0xffff, 0xffff ), 0xffffu, TEST_LOCATION );
  END_TEST;
}

/**
 * Build a bitmap, downscale it and assert it has the right dimensions.
 */
void DoBitmapScaling( Pixel::Format format, uint32_t sourceDimension, uint32_t targetDimension, uint32_t expectedDimension, const char * const location )
{
  ImageAttributes attributes;
  attributes.SetScalingMode( ImageAttributes::ShrinkToFit );
  attributes.SetSize( targetDimension, targetDimension );

  Integration::BitmapPtr sourceBitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::DISCARD );
  sourceBitmap->GetPackedPixelsProfile()->ReserveBuffer( format, sourceDimension, sourceDimension, sourceDimension, sourceDimension );

  Integration::BitmapPtr downScaled = DownscaleBitmap( *sourceBitmap, attributes );
  DALI_TEST_EQUALS( downScaled->GetImageWidth(), expectedDimension, location );
  DALI_TEST_EQUALS( downScaled->GetImageHeight(), expectedDimension, location );
  DALI_TEST_EQUALS( downScaled->GetPixelFormat(), format, location );
}

/**
 * Build a bitmap, downscale it and assert it has the right dimensions.
 */
void DoBitmapScaling( Pixel::Format format, uint32_t sourceDimension, uint32_t targetDimension, const char * const location )
{
  DoBitmapScaling( format, sourceDimension, targetDimension, targetDimension, location );
}

int UtcDaliImageOperationsDownscaleBitmap(void)
{
  // Do Scalings that are expected to work for all pixels modes and assert the resulting bitmap dimensions:

  DoBitmapScaling( Pixel::RGBA8888, 1024, 8, TEST_LOCATION );
  DoBitmapScaling( Pixel::RGB888, 1024, 8, TEST_LOCATION );
  DoBitmapScaling( Pixel::RGB565, 1024, 8, TEST_LOCATION );
  DoBitmapScaling( Pixel::LA88, 1024, 8, TEST_LOCATION );
  DoBitmapScaling( Pixel::L8, 1024, 8, TEST_LOCATION );

  DoBitmapScaling( Pixel::RGBA8888, 773, 1, TEST_LOCATION );
  DoBitmapScaling( Pixel::RGB888, 787, 1, TEST_LOCATION );
  DoBitmapScaling( Pixel::RGB565, 797, 1, TEST_LOCATION );
  DoBitmapScaling( Pixel::LA88, 809, 1, TEST_LOCATION );
  DoBitmapScaling( Pixel::L8, 811, 1, TEST_LOCATION );

  // Do Scalings that are expected to produce a slightly larger than requested image:
  DoBitmapScaling( Pixel::RGBA8888, 47, 7, 11, TEST_LOCATION );
  DoBitmapScaling( Pixel::RGB888, 73, 17, 18, TEST_LOCATION );
  DoBitmapScaling( Pixel::RGB565, 61, 8, 15, TEST_LOCATION );
  DoBitmapScaling( Pixel::LA88, 19, 5, 9, TEST_LOCATION );
  DoBitmapScaling( Pixel::L8, 353, 23, 44, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageOperationsDownscaleInPlacePow2RGB888(void)
{
  unsigned char check_4x4 [16 * 3] = {
      0xff, 0xff, 0xff,  0x00, 0x00, 0x00,  0xff, 0xff, 0xff,  0x00, 0x00, 0x00,
      0x00, 0x00, 0x00,  0xff, 0xff, 0xff,  0x00, 0x00, 0x00,  0xff, 0xff, 0xff,
      0xff, 0xff, 0xff,  0x00, 0x00, 0x00,  0xff, 0xff, 0xff,  0x00, 0x00, 0x00,
      0x00, 0x00, 0x00,  0xff, 0xff, 0xff,  0x00, 0x00, 0x00,  0xff, 0xff, 0xff
  };
  unsigned outWidth = -1, outHeight = -1;
  Dali::Internal::Platform::DownscaleInPlacePow2RGB888(check_4x4, 4, 4, 1, 1, outWidth, outHeight );
  DALI_TEST_EQUALS( outWidth, 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( outHeight, 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( check_4x4[0], 0x7f, TEST_LOCATION );

  unsigned char single_4x4 [16 * 3] = {
    0xff, 0xff, 0xff,  0x00, 0x00, 0x00,  0x00, 0x00, 0x00,  0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,  0x00, 0x00, 0x00,  0x00, 0x00, 0x00,  0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,  0x00, 0x00, 0x00,  0x00, 0x00, 0x00,  0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,  0x00, 0x00, 0x00,  0x00, 0x00, 0x00,  0x00, 0x00, 0x00
  };
  Dali::Internal::Platform::DownscaleInPlacePow2RGB888(single_4x4, 4, 4, 1, 1, outWidth, outHeight );
  DALI_TEST_EQUALS( outWidth, 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( outHeight, 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( single_4x4[0], 0xf, TEST_LOCATION );

  unsigned char single_4x4_2 [16 * 3] = {
      0x00, 0x00, 0x00,  0x00, 0x00, 0x00,  0x00, 0x00, 0x00,  0x00, 0x00, 0x00,
      0x00, 0x00, 0x00,  0x00, 0x00, 0x00,  0x00, 0x00, 0x00,  0x00, 0x00, 0x00,
      0x00, 0x00, 0x00,  0x00, 0x00, 0x00,  0x00, 0x00, 0x00,  0x00, 0x00, 0x00,
      0x00, 0x00, 0x00,  0x00, 0x00, 0x00,  0x00, 0x00, 0x00,  0xff, 0xff, 0xff
    };
  Dali::Internal::Platform::DownscaleInPlacePow2RGB888(single_4x4_2, 4, 4, 1, 1, outWidth, outHeight );
  DALI_TEST_EQUALS( outWidth, 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( outHeight, 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( single_4x4_2[0], 0xf, TEST_LOCATION );

  unsigned char magenta_600_x_600[608*608 * 3];
  for( unsigned int i = 0; i < sizeof(magenta_600_x_600); i += 3 )
  {
    magenta_600_x_600[i] = 0xff;
    magenta_600_x_600[i + 1] = 0;
    magenta_600_x_600[i + 2] = 0xff;
  }

  Dali::Internal::Platform::DownscaleInPlacePow2RGB888( magenta_600_x_600, 352, 352, 0, 0, outWidth, outHeight );
  DALI_TEST_EQUALS( outWidth, 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( outHeight, 1u, TEST_LOCATION );

  Dali::Internal::Platform::DownscaleInPlacePow2RGB888( magenta_600_x_600, 608, 608, 1, 1, outWidth, outHeight );
  DALI_TEST_EQUALS( outWidth, 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( outHeight, 1u, TEST_LOCATION );

  Dali::Internal::Platform::DownscaleInPlacePow2RGB888( magenta_600_x_600, 384, 384, 384, 384, outWidth, outHeight );
  DALI_TEST_EQUALS( outWidth, 384u, TEST_LOCATION );
  DALI_TEST_EQUALS( outHeight, 384u, TEST_LOCATION );

  Dali::Internal::Platform::DownscaleInPlacePow2RGB888( magenta_600_x_600, 352, 352, 44, 11, outWidth, outHeight );
  DALI_TEST_EQUALS( outWidth, 44u, TEST_LOCATION );
  DALI_TEST_EQUALS( outHeight, 44u, TEST_LOCATION );

  Dali::Internal::Platform::DownscaleInPlacePow2RGB888( magenta_600_x_600, 384, 384, 3, 48, outWidth, outHeight );
  DALI_TEST_EQUALS( outWidth, 48u, TEST_LOCATION );
  DALI_TEST_EQUALS( outHeight, 48u, TEST_LOCATION );

  Dali::Internal::Platform::DownscaleInPlacePow2RGB888( magenta_600_x_600, 384, 384, 3, 3, outWidth, outHeight );
  DALI_TEST_CHECK( outWidth == 3u && outHeight == 3u );

  Dali::Internal::Platform::DownscaleInPlacePow2RGB888( magenta_600_x_600, 320, 320, 5, 5, outWidth, outHeight );
  DALI_TEST_CHECK( outWidth == 5u && outHeight == 5u );

  Dali::Internal::Platform::DownscaleInPlacePow2RGB888( magenta_600_x_600, 448, 448, 7, 7, outWidth, outHeight );
  DALI_TEST_CHECK( outWidth == 7u && outHeight == 7u );

  Dali::Internal::Platform::DownscaleInPlacePow2RGB888( magenta_600_x_600, 352, 352, 11, 11, outWidth, outHeight );
  DALI_TEST_CHECK( outWidth == 11u && outHeight == 11u );

  unsigned int numNonMagenta = 0u;
  for( unsigned i = 0; i < sizeof(magenta_600_x_600); i += 3 )
  {
    numNonMagenta += magenta_600_x_600[i] == 0xff && magenta_600_x_600[i + 1] == 0x00 && magenta_600_x_600[i + 2] == 0xff ? 0 : 1;
  }
  DALI_TEST_EQUALS( numNonMagenta, 0u, TEST_LOCATION );

  END_TEST;
}

void DoResizeTestRGBA8888( uint32_t pixels[], const unsigned inputWidth, const unsigned inputHeight, const unsigned int desiredWidth, const unsigned int desiredHeight, const unsigned int expectedWidth, const unsigned int expectedHeight, const char * const location )
{
  unsigned int resultingWidth = -1, resultingHeight = -1;
  Dali::Internal::Platform::DownscaleInPlacePow2RGBA8888(
      reinterpret_cast<unsigned char *> (pixels),
      inputWidth, inputHeight,
      desiredWidth, desiredHeight,
      resultingWidth, resultingHeight );

  DALI_TEST_EQUALS( resultingWidth, expectedWidth, location );
  DALI_TEST_EQUALS( resultingHeight, expectedHeight, location );
}

void DoResizeTestRGB565( uint16_t pixels[], const unsigned inputWidth, const unsigned inputHeight, const unsigned int desiredWidth, const unsigned int desiredHeight, const unsigned int expectedWidth, const unsigned int expectedHeight, const char * const location )
{
  unsigned int resultingWidth = -1, resultingHeight = -1;
  Dali::Internal::Platform::DownscaleInPlacePow2RGB565(
      reinterpret_cast<unsigned char *> (pixels),
      inputWidth, inputHeight,
      desiredWidth, desiredHeight,
      resultingWidth, resultingHeight );

  DALI_TEST_EQUALS( resultingWidth, expectedWidth, location );
  DALI_TEST_EQUALS( resultingHeight, expectedHeight, location );
}

void DoResizeTest2ComponentPair( uint8_t pixels[], const unsigned inputWidth, const unsigned inputHeight, const unsigned int desiredWidth, const unsigned int desiredHeight, const unsigned int expectedWidth, const unsigned int expectedHeight, const char * const location )
{
  unsigned int resultingWidth = -1, resultingHeight = -1;
  Dali::Internal::Platform::DownscaleInPlacePow2ComponentPair(
      pixels,
      inputWidth, inputHeight,
      desiredWidth, desiredHeight,
      resultingWidth, resultingHeight );

  DALI_TEST_EQUALS( resultingWidth, expectedWidth, location );
  DALI_TEST_EQUALS( resultingHeight, expectedHeight, location );
}

void DoResizeTestSingleComponent( uint8_t pixels[], const unsigned inputWidth, const unsigned inputHeight, const unsigned int desiredWidth, const unsigned int desiredHeight, const unsigned int expectedWidth, const unsigned int expectedHeight, const char * const location )
{
  unsigned int resultingWidth = -1, resultingHeight = -1;
  Dali::Internal::Platform::DownscaleInPlacePow2SingleBytePerPixel(
      pixels,
      inputWidth, inputHeight,
      desiredWidth, desiredHeight,
      resultingWidth, resultingHeight );

  DALI_TEST_EQUALS( resultingWidth, expectedWidth, location );
  DALI_TEST_EQUALS( resultingHeight, expectedHeight, location );
}

void DoResizeTestRGBA8888( uint32_t pixels[], const unsigned inputWidth, const unsigned inputHeight, const unsigned int desiredWidth, const unsigned int desiredHeight, const char * const location )
{
  DoResizeTestRGBA8888( pixels, inputWidth, inputHeight, desiredWidth, desiredHeight, desiredWidth, desiredHeight, location );
}

void DoResizeTestRGB565( uint16_t pixels[], const unsigned inputWidth, const unsigned inputHeight, const unsigned int desiredWidth, const unsigned int desiredHeight, const char * const location )
{
  DoResizeTestRGB565( pixels, inputWidth, inputHeight, desiredWidth, desiredHeight, desiredWidth, desiredHeight, location );
}

void DoResizeTest2ComponentPair( uint8_t pixels[], const unsigned inputWidth, const unsigned inputHeight, const unsigned int desiredWidth, const unsigned int desiredHeight, const char * const location )
{
  DoResizeTest2ComponentPair( pixels, inputWidth, inputHeight, desiredWidth, desiredHeight, desiredWidth, desiredHeight, location );
}

void DoResizeTestSingleComponent( uint8_t pixels[], const unsigned inputWidth, const unsigned inputHeight, const unsigned int desiredWidth, const unsigned int desiredHeight, const char * const location )
{
  DoResizeTestSingleComponent( pixels, inputWidth, inputHeight, desiredWidth, desiredHeight, desiredWidth, desiredHeight, location );
}

void DoResizeTestSequenceRGBA8888( uint32_t pixels[], const unsigned int xOffsetIn, const unsigned int yOffsetIn, const unsigned int xOffsetOut, const unsigned int yOffsetOut, const char * const location )
{
  DoResizeTestRGBA8888( pixels, 600 + xOffsetIn, 600 + yOffsetIn, 75 + xOffsetOut, 75 + yOffsetOut, location );
  DoResizeTestRGBA8888( pixels, 512 + xOffsetIn, 512 + yOffsetIn, 16 + xOffsetOut, 16 + yOffsetOut, location );
  DoResizeTestRGBA8888( pixels, 512 + xOffsetIn, 256 + yOffsetIn, 16 + xOffsetOut, 8 + yOffsetOut, location );
  DoResizeTestRGBA8888( pixels, 256 + xOffsetIn, 512 + yOffsetIn, 8 + xOffsetOut, 16 + yOffsetOut, location );
  DoResizeTestRGBA8888( pixels, 256 + xOffsetIn, 512 + yOffsetIn, 4 + xOffsetOut, 8 + yOffsetOut, location );
  DoResizeTestRGBA8888( pixels, 256 + xOffsetIn, 512 + yOffsetIn, 2 + xOffsetOut, 4 + yOffsetOut, location );
  DoResizeTestRGBA8888( pixels, 512 + xOffsetIn, 128 + yOffsetIn, 16 + xOffsetOut, 4 + yOffsetOut, location );
  DoResizeTestRGBA8888( pixels, 512 + xOffsetIn, 64 + yOffsetIn, 16 + xOffsetOut, 2 + yOffsetOut, location );
}

void DoResizeTestSequenceRGB565( uint16_t pixels[], const unsigned int xOffsetIn, const unsigned int yOffsetIn, const unsigned int xOffsetOut, const unsigned int yOffsetOut, const char * const location )
{
  DoResizeTestRGB565( pixels, 600 + xOffsetIn, 600 + yOffsetIn, 75 + xOffsetOut, 75 + yOffsetOut, location );
  DoResizeTestRGB565( pixels, 512 + xOffsetIn, 512 + yOffsetIn, 16 + xOffsetOut, 16 + yOffsetOut, location );
  DoResizeTestRGB565( pixels, 512 + xOffsetIn, 256 + yOffsetIn, 16 + xOffsetOut, 8 + yOffsetOut, location );
  DoResizeTestRGB565( pixels, 256 + xOffsetIn, 512 + yOffsetIn, 8 + xOffsetOut, 16 + yOffsetOut, location );
  DoResizeTestRGB565( pixels, 256 + xOffsetIn, 512 + yOffsetIn, 4 + xOffsetOut, 8 + yOffsetOut, location );
  DoResizeTestRGB565( pixels, 256 + xOffsetIn, 512 + yOffsetIn, 2 + xOffsetOut, 4 + yOffsetOut, location );
  DoResizeTestRGB565( pixels, 512 + xOffsetIn, 128 + yOffsetIn, 16 + xOffsetOut, 4 + yOffsetOut, location );
  DoResizeTestRGB565( pixels, 512 + xOffsetIn, 64 + yOffsetIn, 16 + xOffsetOut, 2 + yOffsetOut, location );
}

int UtcDaliImageOperationsDownscaleInPlacePow2RGBA8888(void)
{
  uint32_t white_600_x_600[608*608];
  for( unsigned i = 0; i < sizeof(white_600_x_600) / sizeof(white_600_x_600[0]); ++i )
  {
    white_600_x_600[i] = 0xffffffff;
  }

  // Test downscales with exact multiples:
  DoResizeTestSequenceRGBA8888( white_600_x_600, 0, 0, 0, 0, TEST_LOCATION );

  // Test downscales with odd multiples, over by one or three
  DoResizeTestSequenceRGBA8888( white_600_x_600, 1, 0, 0, 0, TEST_LOCATION );
  DoResizeTestSequenceRGBA8888( white_600_x_600, 0, 1, 0, 0, TEST_LOCATION );
  DoResizeTestSequenceRGBA8888( white_600_x_600, 3, 0, 0, 0, TEST_LOCATION );
  DoResizeTestSequenceRGBA8888( white_600_x_600, 0, 3, 0, 0, TEST_LOCATION );
  DoResizeTestSequenceRGBA8888( white_600_x_600, 1, 3, 0, 0, TEST_LOCATION );
  DoResizeTestSequenceRGBA8888( white_600_x_600, 3, 1, 0, 0, TEST_LOCATION );

  // Test downscales with odd multiples, under by one:
  DoResizeTestRGBA8888( white_600_x_600, 63, 31, 7, 3, TEST_LOCATION );
  DoResizeTestRGBA8888( white_600_x_600, 63, 31, 4, 2, 7, 3, TEST_LOCATION );
  DoResizeTestRGBA8888( white_600_x_600, 15, 127, 4, 32, 7, 63, TEST_LOCATION );

  // Test downscales between sizes made up only of prime factors:
  DoResizeTestRGBA8888( white_600_x_600, 11 * 13/*143*/, 17 * 19/*323*/, 3 * 5 /*15*/, 3 * 7 /*21*/, 17 /* 3 truncating halvings */, 40 /* 3 truncating halvings */, TEST_LOCATION );

  // Test downscales to the same size:
  DoResizeTestRGBA8888( white_600_x_600, 600, 600, 600, 600, TEST_LOCATION );
  DoResizeTestRGBA8888( white_600_x_600, 512, 128, 512, 128, TEST_LOCATION );
  DoResizeTestRGBA8888( white_600_x_600, 17, 1001, 17, 1001, TEST_LOCATION );
  DoResizeTestRGBA8888( white_600_x_600, 2, 2, 2, 2, TEST_LOCATION );
  DoResizeTestRGBA8888( white_600_x_600, 11 * 13, 17 * 19, 11 * 13, 17 * 19, 11 * 13, 17 * 19, TEST_LOCATION );

  // Test downscales that request a larger size:
  DoResizeTestRGBA8888( white_600_x_600, 300, 300, 600, 600, 300, 300, TEST_LOCATION );
  DoResizeTestRGBA8888( white_600_x_600, 3, 127, 99, 599, 3, 127, TEST_LOCATION );
  DoResizeTestRGBA8888( white_600_x_600, 600, 600, 999, 999, 600, 600, TEST_LOCATION ); //< checks no out of bounds mem access in this case

  // Test downscales to zero in one or more dimensions:
  DoResizeTestRGBA8888( white_600_x_600, 512, 512, 0, 0, 1, 1, TEST_LOCATION );
  DoResizeTestRGBA8888( white_600_x_600, 512, 256, 0, 0, 2, 1, TEST_LOCATION );
  DoResizeTestRGBA8888( white_600_x_600, 512, 128, 0, 0, 4, 1, TEST_LOCATION );
  DoResizeTestRGBA8888( white_600_x_600, 512, 16,  0, 0, 32, 1, TEST_LOCATION );
  DoResizeTestRGBA8888( white_600_x_600, 128, 512, 0, 0, 1, 4, TEST_LOCATION );
  DoResizeTestRGBA8888( white_600_x_600, 32,  512, 0, 0, 1, 16, TEST_LOCATION );
  DoResizeTestRGBA8888( white_600_x_600, 8,   512, 0, 0, 1, 64, TEST_LOCATION );
  DoResizeTestRGBA8888( white_600_x_600, 2,   512, 0, 0, 1, 256, TEST_LOCATION );
  DoResizeTestRGBA8888( white_600_x_600, 11 * 13/*143*/, 17 * 19/*323*/, 0, 3 * 7 /*21*/, 17 /* 3 truncating halvings */, 40 /* 3 truncating halvings */, TEST_LOCATION );
  DoResizeTestRGBA8888( white_600_x_600, 11 * 13/*143*/, 17 * 19/*323*/, 3 * 5 /*15*/, 0, 17 /* 3 truncating halvings */, 40 /* 3 truncating halvings */, TEST_LOCATION );

  // Test downscales to 1 in one or both dimensions:
  DoResizeTestRGBA8888( white_600_x_600, 512, 512, 1, 1, 1, 1, TEST_LOCATION );
  DoResizeTestRGBA8888( white_600_x_600, 512, 32, 16, 1, 16, 1, TEST_LOCATION );
  DoResizeTestRGBA8888( white_600_x_600, 512, 32, 7, 1, 16, 1, TEST_LOCATION );
  DoResizeTestRGBA8888( white_600_x_600, 512, 32, 7, 1, 16, 1, TEST_LOCATION );
  DoResizeTestRGBA8888( white_600_x_600, 512, 32, 5, 1, 16, 1, TEST_LOCATION );
  DoResizeTestRGBA8888( white_600_x_600, 512, 32, 3, 1, 16, 1, TEST_LOCATION );
  DoResizeTestRGBA8888( white_600_x_600, 32, 512, 1, 1, 1, 16, TEST_LOCATION );
  DoResizeTestRGBA8888( white_600_x_600, 32, 512, 1, 16, 1, 16, TEST_LOCATION );
  DoResizeTestRGBA8888( white_600_x_600, 32, 512, 1, 3, 1, 16, TEST_LOCATION );
  DoResizeTestRGBA8888( white_600_x_600, 33, 33, 1, 1, 1, 1, TEST_LOCATION );
  DoResizeTestRGBA8888( white_600_x_600, 17*19, 17*19, 1, 1, 1, 1, TEST_LOCATION );
  DoResizeTestRGBA8888( white_600_x_600, 33, 33, 3, 1, 4, 4, TEST_LOCATION );
  DoResizeTestRGBA8888( white_600_x_600, 33, 9, 3, 1, 4, 1, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageOperationsDownscaleInPlacePow2RGB565(void)
{
  unsigned int outWidth, outHeight;
  DownscaleInPlacePow2RGB565( 0, 0, 0, 0, 0, outWidth, outHeight );

  uint16_t white_600_x_600[608*608];
  for( unsigned i = 0; i < sizeof(white_600_x_600) / sizeof(white_600_x_600[0]); ++i )
  {
    white_600_x_600[i] = 0xffff;
  }

  DoResizeTestRGB565( white_600_x_600, 600, 600, 75, 75, TEST_LOCATION );
  DoResizeTestRGB565( white_600_x_600, 600, 600, 71, 69, 75, 75, TEST_LOCATION );
  DoResizeTestSequenceRGB565( white_600_x_600, 1, 1, 0, 0, TEST_LOCATION );
  DoResizeTestSequenceRGB565( white_600_x_600, 3, 3, 0, 0, TEST_LOCATION );
  DoResizeTestSequenceRGB565( white_600_x_600, 7, 7, 0, 0, TEST_LOCATION );
  ///@note: No need to be as comprehensive as with RGB888 and RGBA8888 as the logic is the same.

  END_TEST;
}

int UtcDaliImageOperationsDownscaleInPlacePow2ComponentPair(void)
{
  unsigned int outWidth, outHeight;
  DownscaleInPlacePow2ComponentPair( 0, 0, 0, 0, 0, outWidth, outHeight );

  uint8_t white_600_x_600[608*608*2];
  for( unsigned i = 0; i < sizeof(white_600_x_600) / sizeof(white_600_x_600[0]); ++i )
  {
    white_600_x_600[i] = 0xff;
  }

  DoResizeTest2ComponentPair( white_600_x_600, 600, 600, 75, 75, TEST_LOCATION );
  DoResizeTest2ComponentPair( white_600_x_600, 600, 600, 71, 69, 75, 75, TEST_LOCATION );
  ///@note: No need to be as comprehensive as with RGB888 and RGBA8888 as the logic is the same.

  END_TEST;
}

int UtcDaliImageOperationsDownscaleInPlacePow2SingleBytePerPixel(void)
{
  unsigned int outWidth, outHeight;
  DownscaleInPlacePow2SingleBytePerPixel( 0, 0, 0, 0, 0, outWidth, outHeight );

  uint8_t white_600_x_600[608*608];
  for( unsigned i = 0; i < sizeof(white_600_x_600) / sizeof(white_600_x_600[0]); ++i )
  {
    white_600_x_600[i] = 0xff;
  }

  DoResizeTestSingleComponent( white_600_x_600, 600, 600, 75, 75, TEST_LOCATION );
  DoResizeTestSingleComponent( white_600_x_600, 600, 600, 69, 69, 75, 75, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageOperationsHalveScanlineInPlaceRGB888(void)
{
  // Red and cyan, averaging to grey:
  unsigned char shortEven[] =    { 0xff, 0, 0,   0, 0xff, 0xff,   0xff, 0, 0,   0, 0xff, 0xff };
  unsigned char shortOdd[] =     { 0xff, 0, 0,  0, 0xff, 0xff,  0xff, 0, 0,  0, 0xff, 0xff,  0xC, 0xC, 0xC };
  //unsigned char shortEvenRef[] = { 0x7f, 0x7f, 0x7f,  0x7f, 0x7f, 0x7f,  0x7f, 0x7f, 0x7f,  0x7f, 0x7f, 0x7f };

  Dali::Internal::Platform::HalveScanlineInPlaceRGB888( shortEven, 4u );
  Dali::Internal::Platform::HalveScanlineInPlaceRGB888( shortOdd, 4u );
  for( unsigned i = 0; i < sizeof(shortEven) >> 1u ; ++i )
  {
    DALI_TEST_EQUALS( unsigned(shortEven[i]), 0x7fu, TEST_LOCATION );
    DALI_TEST_EQUALS( unsigned(shortOdd[i]), 0x7fu, TEST_LOCATION );
  }

  END_TEST;
}

int UtcDaliImageOperationsHalveScanlineInPlaceRGBA8888(void)
{
  const size_t scanlineLength = 4096u;
  Dali::Vector<uint32_t> scanline;
  Dali::Vector<uint32_t> reference;
  SetupScanlineForHalvingTestsRGBA8888( scanlineLength, scanline, reference );

  HalveScanlineInPlaceRGBA8888( (uint8_t *) &scanline[0], scanlineLength );

  size_t numMatches = 0;
  for( int i = 0, length = reference.Size(); i < length; ++i )
  {
    DALI_TEST_EQUALS( scanline[i], reference[i], TEST_LOCATION );
    numMatches += scanline[i] == reference[i];
  }
  DALI_TEST_EQUALS( numMatches, scanlineLength / 2, TEST_LOCATION );

  for( size_t i = scanlineLength / 2; i < reference.Capacity(); ++i )
  {
    DALI_TEST_EQUALS( reference[i],  0xEEEEEEEE, TEST_LOCATION );
  }

  END_TEST;
}

int UtcDaliImageOperationsHalveScanlineInPlaceRGB565(void)
{
  const size_t scanlineLength = 4096u;
  Dali::Vector<uint16_t> scanline;
  Dali::Vector<uint16_t> reference;
  SetupScanlineForHalvingTestsRGB565( scanlineLength, scanline, reference );

  HalveScanlineInPlaceRGB565( (unsigned char *) (&scanline[0]), scanlineLength );

  size_t numMatches = 0;
  for( int i = 0, length = reference.Size(); i < length; ++i )
  {
    DALI_TEST_EQUALS( scanline[i], reference[i], TEST_LOCATION );
    numMatches += scanline[i] == reference[i];
  }
  DALI_TEST_EQUALS( numMatches, scanlineLength / 2, TEST_LOCATION );

  for( size_t i = scanlineLength / 2; i < reference.Capacity(); ++i )
  {
    DALI_TEST_EQUALS( reference[i],  0xEEEE, TEST_LOCATION );
  }

  END_TEST;
}

int UtcDaliImageOperationsHalveScanlineInPlace2Bytes(void)
{
  const size_t scanlineLength = 4096u;
  Dali::Vector<uint8_t> scanline;
  Dali::Vector<uint8_t> reference;
  SetupScanlineForHalvingTests2Bytes( scanlineLength, scanline, reference );

  HalveScanlineInPlace2Bytes( &scanline[0], scanlineLength );

  size_t numMatches = 0;
  for( int i = 0, length = reference.Size(); i < length; ++i )
  {
    DALI_TEST_EQUALS( 1u * scanline[i], 1u * reference[i], TEST_LOCATION );
    numMatches += scanline[i] == reference[i];
  }
  DALI_TEST_EQUALS( numMatches, scanlineLength, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageOperationsHalveScanlineInPlace1Byte(void)
{
  const size_t scanlineLength = 4096u;
  Dali::Vector<uint8_t> scanline;
  Dali::Vector<uint8_t> reference;
  SetupScanlineForHalvingTests1Byte( scanlineLength, scanline, reference );

  HalveScanlineInPlace1Byte( &scanline[0], scanlineLength );

  size_t numMatches = 0;
  for( int i = 0, length = reference.Size(); i < length; ++i )
  {
    DALI_TEST_EQUALS( 1u * scanline[i], 1u * reference[i], TEST_LOCATION );
    numMatches += scanline[i] == reference[i];
  }
  DALI_TEST_EQUALS( numMatches, scanlineLength / 2, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageOperationsAverageScanlines1(void)
{
  // Red and cyan, averaging to grey:
  unsigned char shortEven1[] =    { 0xff, 0, 0,    0, 0xff, 0xff,  0xff, 0, 0,      0, 0xff, 0xff };
  unsigned char shortEven2[] =    { 0, 0xff, 0xff, 0xff, 0, 0,     0, 0xff,  0xff,  0xff, 0, 0 };
  unsigned char outputBuffer[sizeof(shortEven1)];

  AverageScanlines1( shortEven1, shortEven2, outputBuffer, sizeof(shortEven1) );
  for( unsigned i = 0; i < sizeof(shortEven1) ; ++i )
  {
    DALI_TEST_EQUALS( unsigned(outputBuffer[i]), 0x7fu, TEST_LOCATION );
  }

  // Longer test reusing RGBA setup/test logic:
  const size_t scanlineLength = 4096u;
  Dali::Vector<uint32_t> scanline1;
  Dali::Vector<uint32_t> scanline2;
  Dali::Vector<uint32_t> reference;
  Dali::Vector<uint32_t> output;
  SetupScanlinesRGBA8888( scanlineLength, scanline1, scanline2, reference, output );

  AverageScanlines1( (const unsigned char*) &scanline1[0], (const unsigned char*) &scanline2[0], (unsigned char*) &output[0], scanlineLength * 4 );

  size_t numMatches = 0;
  MatchScanlinesRGBA8888( reference, output, numMatches );
  DALI_TEST_EQUALS( numMatches, reference.Capacity(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageOperationsAverageScanlines2(void)
{
  // Red and cyan, averaging to grey:
  unsigned char shortEven1[] =    { 0xff, 0, 0,    0, 0xff, 0xff,  0xff, 0, 0,      0, 0xff, 0xff };
  unsigned char shortEven2[] =    { 0, 0xff, 0xff, 0xff, 0, 0,     0, 0xff,  0xff,  0xff, 0, 0 };
  unsigned char outputBuffer[sizeof(shortEven1)];

  AverageScanlines2( shortEven1, shortEven2, outputBuffer, sizeof(shortEven1) / 2 );

  for( unsigned i = 0; i < sizeof(shortEven1); ++i )
  {
    DALI_TEST_EQUALS( unsigned(outputBuffer[i]), 0x7fu, TEST_LOCATION );
  }

  // Longer test reusing RGBA setup/test logic:
  const size_t scanlineLength = 4096u;
  Dali::Vector<uint32_t> scanline1;
  Dali::Vector<uint32_t> scanline2;
  Dali::Vector<uint32_t> reference;
  Dali::Vector<uint32_t> output;
  SetupScanlinesRGBA8888( scanlineLength, scanline1, scanline2, reference, output );

  AverageScanlines2( (const unsigned char*) &scanline1[0], (const unsigned char*) &scanline2[0], (unsigned char*) &output[0], scanlineLength * 2 );

  size_t numMatches = 0;
  MatchScanlinesRGBA8888( reference, output, numMatches );
  DALI_TEST_EQUALS( numMatches, reference.Capacity(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageOperationsAverageScanlines3(void)
{
  // Red and cyan, averaging to grey:
  unsigned char shortEven1[] =    { 0xff, 0, 0,    0, 0xff, 0xff,  0xff, 0, 0,      0, 0xff, 0xff };
  unsigned char shortEven2[] =    { 0, 0xff, 0xff, 0xff, 0, 0,     0, 0xff,  0xff,  0xff, 0, 0 };
  unsigned char outputBuffer[sizeof(shortEven1)];

  AverageScanlines3( shortEven1, shortEven2, outputBuffer, sizeof(shortEven1) / 3 );
  for( unsigned i = 0; i < sizeof(shortEven1) ; ++i )
  {
    DALI_TEST_EQUALS( unsigned(outputBuffer[i]), 0x7fu, TEST_LOCATION );
  }

  // Longer test reusing RGBA setup/test logic:
  const size_t scanlineLength = 3 * 4 * 90u;
  Dali::Vector<uint32_t> scanline1;
  Dali::Vector<uint32_t> scanline2;
  Dali::Vector<uint32_t> reference;
  Dali::Vector<uint32_t> output;
  SetupScanlinesRGBA8888( scanlineLength, scanline1, scanline2, reference, output );

  AverageScanlines3( (const unsigned char*) &scanline1[0], (const unsigned char*) &scanline2[0], (unsigned char*) &output[0], scanlineLength * 4 / 3 );

  size_t numMatches = 0;
  MatchScanlinesRGBA8888( reference, output, numMatches );
  DALI_TEST_EQUALS( numMatches, reference.Capacity(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageOperationsAverageScanlinesRGBA8888(void)
{
  const size_t scanlineLength = 4096u;
  Dali::Vector<uint32_t> scanline1;
  Dali::Vector<uint32_t> scanline2;
  Dali::Vector<uint32_t> reference;
  Dali::Vector<uint32_t> output;
  SetupScanlinesRGBA8888( scanlineLength, scanline1, scanline2, reference, output );

  AverageScanlinesRGBA8888( (const unsigned char*) &scanline1[0], (const unsigned char*) &scanline2[0], (unsigned char*) &output[0], scanlineLength );

  size_t numMatches = 0;
  MatchScanlinesRGBA8888( reference, output, numMatches );
  DALI_TEST_EQUALS( numMatches, reference.Capacity(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageOperationsAverageScanlinesRGB565(void)
{
  // Red and cyan, averaging to grey:
  const uint16_t shortEven1[] =    { 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xf800, 0xBEEF, 0xBEEF };
  const uint16_t shortEven2[] =    { 0x7ff,  0x7ff,  0x7ff,  0x7ff,  0x7ff,  0x7ff, 0xBEEF, 0xBEEF };
  const size_t arrayLength = sizeof(shortEven1) / sizeof(shortEven1[0]) - 2;
  uint16_t outputBuffer[arrayLength + 2];
  outputBuffer[arrayLength] = 0xDEAD;
  outputBuffer[arrayLength+1] = 0xDEAD;

  Dali::Internal::Platform::AverageScanlinesRGB565( (const unsigned char*) shortEven1, (const unsigned char*) shortEven2, (unsigned char*) outputBuffer,  arrayLength );
  for( unsigned i = 0; i <  arrayLength ; ++i )
  {
    DALI_TEST_EQUALS( unsigned(outputBuffer[i]), 0xffff - (1u << 15) - (1u << 10) - (1u << 4), TEST_LOCATION );
  }

  // Check for buffer overrun:
  DALI_TEST_EQUALS( outputBuffer[arrayLength], 0xDEAD, TEST_LOCATION );
  DALI_TEST_EQUALS( outputBuffer[arrayLength+1], 0xDEAD, TEST_LOCATION );

  END_TEST;
}
