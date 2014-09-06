/*
Copyright (c) 2000-2012 Samsung Electronics Co., Ltd All Rights Reserved

This file is part of Dali Adaptor

PROPRIETARY/CONFIDENTIAL

This software is the confidential and proprietary information of
SAMSUNG ELECTRONICS ("Confidential Information"). You shall not
disclose such Confidential Information and shall use it only in
accordance with the terms of the license agreement you entered
into with SAMSUNG ELECTRONICS.

SAMSUNG make no representations or warranties about the suitability
of the software, either express or implied, including but not limited
to the implied warranties of merchantability, fitness for a particular
purpose, or non-infringement. SAMSUNG shall not be liable for any
damages suffered by licensee as a result of using, modifying or
distributing this software or its derivatives.
*/

#include "loader-bmp.h"

#include <cstdio>
#include <cstdlib>

#include <dali/integration-api/debug.h>
#include <dali/integration-api/bitmap.h>
#include <dali/public-api/images/image-attributes.h>
#include "vectorImgStreamer.h"


namespace Dali
{
using Integration::Bitmap;
using Integration::PixelBuffer;

namespace SlpPlatform
{

namespace
{
const unsigned int FileHeaderOffsetOfBF32V4 = 0x7A;
const unsigned int MaskForBFRGB565 = 0x80;
const unsigned int FileHeaderOffsetOfRGB24V5 = 0x8A;

enum BmpFormat
{
  BMP_RGB1 = 14,    //BI_RGB & bpp =1
  BMP_RGB4,         //BI_RGB & bpp = 4
  BMP_RGB8,         //BI_RGB & bpp = 8
  BMP_RGB555,       //BI_RGB & bpp = 16
  BMP_BITFIELDS555, //BI_BITFIELDS & 16bit & R:G:B = 5:5:5
  BMP_BITFIELDS32,  //BI_BITFIELDS & 32bit & R:G:B:A = 8:8:8:8
  BMP_RLE8,         //BI_RLE8
  BMP_RLE4,         //BI_RLE4
  BMP_BITFIELDS32V4,//BI_BITFIELDS & 32bit
  BMP_RGB24V5,      //BI_RGB & bpp = 24 & bmp version5
  BMP_NOTEXIST
};

struct BmpFileHeader
{
  unsigned short signature; // Bitmap file signature
  unsigned int   fileSize;  // Bitmap file size in bytes
  unsigned short reserved1; // Reserved bits
  unsigned short reserved2; // Reserved bits
  unsigned int   offset;    // Offset from BMP file header to BMP bits
} __attribute__ ( (__packed__)); // Stops the structure from being aligned to every 4 bytes

struct BmpInfoHeader
{
  unsigned int   infoHeaderSize;  // Specifies the number of bytes required by the info header
  unsigned int   width;           // The Image Width
  int            height;          // The Image Height (negative value represents image data is flipped)
  unsigned short planes;          // The number of color planes, must be 1
  unsigned short bitsPerPixel;    // The bits per pixel
  unsigned int   compression;     // The type of compression used by the image
  unsigned int   imageSize;       // The size of the image in bytes
  unsigned int   xPixelsPerMeter; // The number of pixels per meter in x axis
  unsigned int   yPixelsPerMeter; // The number of pixels per meter in y axis
  unsigned int   numberOfColors;  // The number of colors in the color table
  unsigned int   importantColors; // The important color count
} __attribute__ ( (__packed__)); // Stops the structure from being aligned to every 4 bytes

/**
 * Template function to read from the file directly into our structure.
 * @param[in]  fp     The file to read from
 * @param[out] header The structure we want to store our information in
 * @return true, if read successful, false otherwise
 */
template<typename T>
inline bool ReadHeader(VectorImgStreamer &vBmpStream, T& header)
{
  unsigned int readLength = sizeof(T);

  // Load the information directly into our structure
  if (vBmpStream.vRead( (char *)&header, readLength) != readLength)
  {
    return false;
  }
  return true;
}

bool LoadBmpHeader(const std::vector<uint8_t>& data, unsigned int &width, unsigned int &height, BmpFileHeader &fileHeader, BmpInfoHeader &infoHeader)
{

  VectorImgStreamer vBmpStream(data);

  if (!ReadHeader(vBmpStream, fileHeader))
  {
    return false;
  }

  if (!ReadHeader(vBmpStream, infoHeader))
  {
    return false;
  }

  width = infoHeader.width;
  height = infoHeader.height;

  return true;
}

/**
 * function to decode format BI_RGB & bpp = 24 & bmp version5.
 * @param[in]  fp      The file to read from
 * @param[out] pixels  The pointer that  we want to store bmp data  in
 * @param[in]  width   bmp width
 * @param[in]  height  bmp height
 * @param[in]  offset  offset from bmp header to bmp image data
 * @param[in]  topDown indicate image data is read from bottom or from top
 * @param[in]  padding padded to a u_int32 boundary for each line
 * @return true, if decode successful, false otherwise
 */
bool DecodeRGB24V5(VectorImgStreamer &vBmpStream,
                   PixelBuffer *pixels,
                   unsigned int width,
                   unsigned int height,
                   unsigned int offset,
                   bool topDown,
                   unsigned int rowStride,
                   unsigned int padding)
{
  if(/*fp == NULL ||*/ pixels == NULL)
  {
    DALI_LOG_ERROR("Error decoding BMP_RGB24V5 format\n");
    return false;
  }
  PixelBuffer *pixelsPtr = pixels;

  vBmpStream.vSeek(offset, VectorImgStreamer::V_SEEK_SET);
  for(unsigned int yPos = 0; yPos < height; yPos ++)
  {
    if(topDown)
    {
      pixelsPtr = pixels + ( yPos * rowStride);
    }
    else
    {
      pixelsPtr = pixels + (((height-1)-yPos) * rowStride);
    }

    if (vBmpStream.vRead((char *)pixelsPtr, rowStride) != rowStride)
    {
      DALI_LOG_ERROR("Error reading the BMP image\n");
      return false;
    }
    for(unsigned int i = 0; i < rowStride; i += 3)
    {
      unsigned char temp = pixelsPtr[i];
      pixelsPtr[i] = pixelsPtr[i + 2];
      pixelsPtr[i + 2] = temp;
    }

    if (padding)
    {
      // move past the padding.
	  vBmpStream.vSeek(padding, VectorImgStreamer::V_SEEK_CUR);
    }
  }
  return true;
}

/**
 * function to decode format BI_BITFIELDS & bpp = 32 & bmp version4.
 * @param[in]  fp        The file to read from
 * @param[out] pixels    The pointer that  we want to store bmp data  in
 * @param[in]  width     bmp width
 * @param[in]  height    bmp height
 * @param[in]  offset    offset from bmp header to bmp image data
 * @param[in]  topDown   indicate image data is read from bottom or from top
 * @param[in]  rowStride bits span for each line
 * @param[in]  padding   padded to a u_int32 boundary for each line
 * @return true, if decode successful, false otherwise
 */
bool DecodeBF32V4(VectorImgStreamer &vBmpStream,
                  PixelBuffer *pixels,
                  unsigned int width,
                  unsigned int height,
                  unsigned int offset,
                  bool topDown,
                  unsigned int rowStride,
                  unsigned int padding)
{
  if(/*fp == NULL ||*/  pixels == NULL)
  {
    DALI_LOG_ERROR("Error decoding BMP_BITFIELDS32V4 format\n");
    return false;
  }
  PixelBuffer *pixelsPtr = pixels;

  vBmpStream.vSeek(offset, VectorImgStreamer::V_SEEK_SET);

  for(unsigned int yPos = 0; yPos < height; yPos ++)
  {
    if(topDown)
    {
      pixelsPtr = pixels + ( yPos * rowStride);
    }
    else
    {
      pixelsPtr = pixels + (((height-1)-yPos) * rowStride);
    }

	if (vBmpStream.vRead((char *)pixelsPtr, rowStride) != rowStride)
    {
      DALI_LOG_ERROR("Error reading the BMP image\n");
      return false;
    }
    for(unsigned int i = 0; i < rowStride; i += 4)
    {
      unsigned char temp = pixelsPtr[i];
      pixelsPtr[i] = pixelsPtr[i + 2];
      pixelsPtr[i + 2] = temp;
    }
    if (padding)
    {
      // move past the padding.
	  vBmpStream.vSeek(padding, VectorImgStreamer::V_SEEK_CUR);

    }

  }
  return true;
}

/**
 * function to decode format BI_BITFIELDS & bpp = 32
 * @param[in]  fp        The file to read from
 * @param[out] pixels    The pointer that  we want to store bmp data  in
 * @param[in]  width     bmp width
 * @param[in]  height    bmp height
 * @param[in]  offset    offset from bmp header to bmp image data
 * @param[in]  topDown   indicate image data is read from bottom or from top
 * @param[in]  rowStride bits span for each line
 * @param[in]  padding   padded to a u_int32 boundary for each line
 * @return true, if decode successful, false otherwise
 */
bool DecodeBF32(VectorImgStreamer &vBmpStream,
                PixelBuffer *pixels,
                unsigned int width,
                unsigned int height,
                unsigned int offset,
                bool topDown,
                unsigned int rowStride,
                unsigned int padding)
{
  if(/*fp == NULL ||*/  pixels == NULL)
  {
    DALI_LOG_ERROR("Error decoding BMP_BITFIELDS32 format\n");
    return false;
  }
  PixelBuffer *pixelsPtr = pixels;

  vBmpStream.vSeek(offset, VectorImgStreamer::V_SEEK_SET);

  for (unsigned int yPos = 0; yPos < height; yPos++)
  {
    if (topDown)
    {
      // the data in the file is top down, and we store the data top down
      pixelsPtr = pixels + ( yPos * rowStride);
    }
    else
    {
      // the data in the file is bottom up, and we store the data top down
      pixelsPtr = pixels + (((height-1)-yPos) * rowStride);
    }

	//if (fread(pixelsPtr, 1, rowStride, fp) != rowStride)
	if (vBmpStream.vRead((char *)pixelsPtr, rowStride) != rowStride)
    {
      DALI_LOG_ERROR("Error reading the BMP image\n");
      return false;
    }
    for(unsigned int i = 0; i < rowStride; i += 4)
    {
      unsigned char temp = pixelsPtr[i];
      pixelsPtr[i] = pixelsPtr[i + 2];
      pixelsPtr[i + 2] = temp;
    }

    if (padding)
    {
      // move past the padding.
	  vBmpStream.vSeek(padding, VectorImgStreamer::V_SEEK_CUR);
    }
  }
  return true;
}

/**
 * function to decode format BI_BITFIELDS & bpp = 16 & R:G:B = 5:6:5
 * @param[in]  fp      The file to read from
 * @param[out] pixels  The pointer that  we want to store bmp data  in
 * @param[in]  width   bmp width
 * @param[in]  height  bmp height
 * @param[in]  offset  offset from bmp header to bmp image data
 * @param[in]  topDown indicate image data is read from bottom or from top
 * @return true, if decode successful, false otherwise
 */
bool DecodeBF565(VectorImgStreamer &vBmpStream,
                 PixelBuffer *pixels,
                 unsigned int width,
                 unsigned int height,
                 unsigned int offset,
                 bool topDown)
{
  if(/*fp == NULL ||*/ pixels == NULL)
  {
    DALI_LOG_ERROR("Error decoding RGB565 format\n");
    return false;
  }
  PixelBuffer *pixelsPtr = pixels;
  width = ((width & 3) != 0) ? width + 4 - (width & 3) : width;
  unsigned int rowStride = width * 2;
  vBmpStream.vSeek(offset, VectorImgStreamer::V_SEEK_SET);

  for(unsigned int i = 0; i < height; i++)
  {
    if (topDown)
    {
      // the data in the file is top down, and we store the data top down
      pixelsPtr = pixels + ( i * rowStride);
    }
    else
    {
      // the data in the file is bottom up, and we store the data top down
      pixelsPtr = pixels + (((height - 1) - i) * rowStride);
    }
	if (vBmpStream.vRead((char *)pixelsPtr, rowStride) != rowStride)
    {
      return false;
    }
  }

  return true;
}

/**
 * function to decode format BI_BITFIELDS & bpp = 16 & R:G:B = 5:5:5
 * @param[in]  fp      The file to read from
 * @param[out] pixels  The pointer that  we want to store bmp data  in
 * @param[in]  width   bmp width
 * @param[in]  height  bmp height
 * @param[in]  offset  offset from bmp header to bmp image data
 * @param[in]  topDown indicate image data is read from bottom or from top
 * @return true, if decode successful, false otherwise
 */
bool DecodeBF555(VectorImgStreamer &vBmpStream,
                 PixelBuffer *pixels,
                 unsigned int width,
                 unsigned int height,
                 unsigned int offset,
                 bool topDown)
{
  if( pixels == NULL)
  {
    DALI_LOG_ERROR("Error decoding BMP_BITFIELDS555 format\n");
    return false;
  }
  PixelBuffer *pixelsPtr = pixels;

  width = ((width & 3) != 0) ? width + 4 - (width & 3) : width;

  std::vector<char> raw(width * height * 2);
  char *rawPtr = &raw[0];
  unsigned int rawStride = width * 2;
  unsigned int rowStride = width * 3;
  vBmpStream.vSeek(offset, VectorImgStreamer::V_SEEK_SET);

  for(unsigned int j = 0; j <  height; j ++)
  {
    rawPtr = &raw[0] + ( j * rawStride);
	if (vBmpStream.vRead((char *)rawPtr, rawStride) != rawStride)
    {
      return false;
    }
  }

  for (unsigned int yPos = 0; yPos < height; yPos++)
  {
    if (topDown)
    {
      // the data in the file is top down, and we store the data top down
      pixelsPtr = pixels + ( yPos * rowStride);
    }
    else
    {
      // the data in the file is bottom up, and we store the data top down
      pixelsPtr = pixels + (((height-1)-yPos) * rowStride);
    }

    for(unsigned int k = 0; k < width; k ++)
    {
      int index = yPos * rawStride + 2 * k;
      pixelsPtr[3 * k] = ((raw[ index + 1] >> 2) & 0x1F) * 0xFF / 0x1F;
      pixelsPtr[3 * k + 1] = (((raw[index + 1] & 0x03) << 3) | (raw[ index] >> 5))  * 0xFF/ 0x1F;
      pixelsPtr[3 * k + 2] = (raw[ index] & 0x1F) * 0xFF / 0x1F;
    }

  }
  return true;
}

/**
 * function to decode format BI_RGB & bpp = 16 & R:G:B = 5:5:5
 * @param[in]  fp      The file to read from
 * @param[out] pixels  The pointer that  we want to store bmp data  in
 * @param[in]  width   bmp width
 * @param[in]  height  bmp height
 * @param[in]  offset  offset from bmp header to bmp image data
 * @param[in]  topDown indicate image data is read from bottom or from top
 * @return true, if decode successful, false otherwise
 */
bool DecodeRGB555(VectorImgStreamer &vBmpStream,
                  PixelBuffer *pixels,
                  unsigned int width,
                  unsigned int height,
                  unsigned int offset,
                  bool topDown)
{
  if(/*fp == NULL ||*/  pixels == NULL)
  {
    DALI_LOG_ERROR("Error decoding BMP_RGB555 format\n");
    return false;
  }
  PixelBuffer *pixelsPtr = pixels;
  width = ((width & 3) != 0) ? width + 4 - (width & 3) : width;
  std::vector<char> raw(width * height * 2);
  char *rawPtr = &raw[0];
  unsigned int rawStride = width * 2;
  unsigned int rowStride = width * 3;

  vBmpStream.vSeek(offset, VectorImgStreamer::V_SEEK_SET);

  for(unsigned int j = 0; j <  height; j ++)
  {
    rawPtr = &raw[0] + ( j * rawStride);

	if (vBmpStream.vRead((char *)rawPtr, rawStride) != rawStride)
    {
      return false;
    }
  }
  for(unsigned int i = 0; i < height; i++)
  {
    if (topDown)
    {
      // the data in the file is top down, and we store the data top down
      pixelsPtr = pixels + ( i * rowStride);
    }
    else
    {
      // the data in the file is bottom up, and we store the data top down
      pixelsPtr = pixels + (((height - 1) - i) * rowStride);
    }
    for(unsigned int k = 0; k < width; k ++)
    {
      int index = i * rawStride + 2 * k;
      pixelsPtr[3 * k] = ((raw[ index + 1] >> 2) & 0x1F) * 0xFF / 0x1F;
      pixelsPtr[3 * k + 1] = (((raw[index + 1] & 0x03) << 3) | (raw[ index] >> 5))  * 0xFF/ 0x1F;
      pixelsPtr[3 * k + 2] = (raw[ index] & 0x1F) * 0xFF / 0x1F;
    }

  }
  return true;
}

/**
 * function to decode format BI_RGB & bpp = 1
 * @param[in]  fp      The file to read from
 * @param[out] pixels  The pointer that  we want to store bmp data  in
 * @param[in]  width   bmp width
 * @param[in]  height  bmp height
 * @param[in]  offset  offset from bmp header to bmp palette data
 * @param[in]  topDown indicate image data is read from bottom or from top
 * @return true, if decode successful, false otherwise
 */
bool DecodeRGB1(VectorImgStreamer &vBmpStream,
                PixelBuffer *pixels,
                unsigned int width,
                unsigned int height,
                unsigned int offset,
                bool topDown)
{
  if(/*fp == NULL ||*/ pixels == NULL)
  {
    DALI_LOG_ERROR("Error decoding BMP_RGB1 format\n");
    return false;
  }
  PixelBuffer *pixelsPtr = pixels;
  char colorTable[8];
  char cmd;
  unsigned int fillw = ((width & 63) != 0) ? width + 64 - (width & 63) : width;
  std::vector<char> colorIndex(fillw * height);
  unsigned int rowStride = fillw * 3; // RGB

  vBmpStream.vSeek(offset, VectorImgStreamer::V_SEEK_SET);

  if (vBmpStream.vRead((char *)colorTable, 8) != 8)
  {
    return false;
  }
  for(unsigned int i = 0; i < fillw * height; i += 8)
  {
	if (vBmpStream.vRead((char *)&cmd, 1) != 1)
    {
      return false;
    }

    colorIndex[i]     = (cmd >> 7) & 0x01;
    colorIndex[i + 1] = (cmd >> 6) & 0x01;
    colorIndex[i + 2] = (cmd >> 5) & 0x01;
    colorIndex[i + 3] = (cmd >> 4) & 0x01;
    colorIndex[i + 4] = (cmd >> 3) & 0x01;
    colorIndex[i + 5] = (cmd >> 2) & 0x01;
    colorIndex[i + 6] = (cmd >> 1) & 0x01;
    colorIndex[i + 7] = (cmd & 0x01);
  }
  int ctIndex = 0;

  for(unsigned int index = 0; index < height; index = index + 1)
  {
    if (topDown)
    {
      // the data in the file is top down, and we store the data top down
      pixelsPtr = pixels + ( index * rowStride);
    }
    else
    {
      // the data in the file is bottom up, and we store the data top down
      pixelsPtr = pixels + (((height - 1) - index) * rowStride);
    }
    for(unsigned int j = 0; j < fillw; j ++)
    {
      ctIndex = colorIndex[ fillw * index + j ];

      pixelsPtr[ 3 * j ] = colorTable[4 * ctIndex + 2];
      pixelsPtr[(3 * j + 1)] = colorTable[4 * ctIndex + 1];
      pixelsPtr[(3 * j + 2)] = colorTable[4 * ctIndex ];
    }
  }
  return true;
}

/**
 * function to decode format BI_RGB & bpp = 4
 * @param[in]  fp      The file to read from
 * @param[out] pixels  The pointer that  we want to store bmp data  in
 * @param[in]  width   bmp width
 * @param[in]  height  bmp height
 * @param[in]  offset  offset from bmp header to bmp palette data
 * @param[in]  topDown indicate image data is read from bottom or from top
 * @return true, if decode successful, false otherwise
 */
bool DecodeRGB4(VectorImgStreamer &vBmpStream,
                PixelBuffer *pixels,
                unsigned int width,
                unsigned int height,
                unsigned int offset,
                bool topDown)
{
  if(/*fp == NULL ||*/  pixels == NULL)
  {
    DALI_LOG_ERROR("Error decoding BMP_RGB4 format\n");
    return false;
  }
  PixelBuffer *pixelsPtr = pixels;
  char colorTable[64];
  char cmd;
  unsigned int fillw = ((width & 3) != 0) ? width + 4 - (width & 3) : width;
  std::vector<char> colorIndex(fillw * height);
  unsigned int rowStride = fillw  * 3;
  vBmpStream.vSeek(offset, VectorImgStreamer::V_SEEK_SET);

  if (vBmpStream.vRead((char *)colorTable, 64) != 64)
  {
    return false;
  }
  for(unsigned int i = 0; i < fillw * height; i += 2)
  {
	if (vBmpStream.vRead((char *)&cmd, 1) != 1)
    {
      return false;
    }

    colorIndex[i] = cmd >> 4;
    colorIndex[i + 1] = cmd & (0x0F);
  }
  int ctIndex = 0;

  for(unsigned int index = 0; index < height; index = index + 1)
  {
    if (topDown)
    {
      // the data in the file is top down, and we store the data top down
      pixelsPtr = pixels + ( index * rowStride);
    }
    else
    {
      // the data in the file is bottom up, and we store the data top down
      pixelsPtr = pixels + (((height - 1) - index) * rowStride);
    }
    for(unsigned int j = 0; j < fillw; j ++)
    {
      ctIndex = colorIndex[ fillw * index + j ];
      pixelsPtr[ 3 * j ] = colorTable[4 * ctIndex + 2];
      pixelsPtr[(3 * j + 1)] = colorTable[4 * ctIndex + 1];
      pixelsPtr[(3 * j + 2)] = colorTable[4 * ctIndex ];
    }
  }

  return true;
}

/**
 * function to decode format BI_RGB & bpp = 8
 * @param[in]  fp      The file to read from
 * @param[out] pixels  The pointer that  we want to store bmp data  in
 * @param[in]  width   bmp width
 * @param[in]  height  bmp height
 * @param[in]  offset  offset from bmp header to bmp palette data
 * @param[in]  topDown indicate image data is read from bottom or from top
 * @return true, if decode successful, false otherwise
 */
bool DecodeRGB8(VectorImgStreamer &vBmpStream,
                PixelBuffer *pixels,
                unsigned int width,
                unsigned int height,
                unsigned int offset,
                bool topDown)
{
  if(/*fp == NULL ||*/  pixels == NULL)
  {
    DALI_LOG_ERROR("Error decoding BMP_RGB8 format\n");
    return false;
  }
  PixelBuffer *pixelsPtr = pixels;
  std::vector<char> colorTable(1024);
  width = ((width & 3) != 0) ? width + 4 - (width & 3) : width;
  char cmd;
  std::vector<char> colorIndex(width * height);
  unsigned int rowStride = width * 3;//RGB8->RGB24

  vBmpStream.vSeek(offset, VectorImgStreamer::V_SEEK_SET);

  if (vBmpStream.vRead((char *)&colorTable[0], 1024) != 1024)
  {
    return false;
  }
  for(unsigned int i = 0; i < width * height; i ++)
  {
	if (vBmpStream.vRead((char *)&cmd, 1) != 1)
    {
      return false;
    }

    colorIndex[i] = cmd;
  }
  int ctIndex = 0;
  for(unsigned int index = 0; index < height; index = index + 1)
  {
    if (topDown)
    {
      // the data in the file is top down, and we store the data top down
      pixelsPtr = pixels + ( index * rowStride);
    }
    else
    {
      // the data in the file is bottom up, and we store the data top down
      pixelsPtr = pixels + (((height - 1) - index) * rowStride);
    }
    for(unsigned int j = 0; j < width; j ++)
    {
      ctIndex = colorIndex[ width * index + j ];
      pixelsPtr[ 3 * j ] = colorTable[4 * ctIndex + 2];
      pixelsPtr[(3 * j + 1)] = colorTable[4 * ctIndex + 1];
      pixelsPtr[(3 * j + 2)] = colorTable[4 * ctIndex ];
    }
  }
  return true;
}

/**
 * function to decode format BI_RLE4 & bpp = 4
 * @param[in]  fp      The file to read from
 * @param[out] pixels  The pointer that  we want to store bmp data  in
 * @param[in]  width   bmp width
 * @param[in]  height  bmp height
 * @param[in]  offset  offset from bmp header to bmp palette data
 * @param[in]  topDown indicate image data is read from bottom or from top
 * @return true, if decode successful, false otherwise
 */
bool DecodeRLE4(VectorImgStreamer &vBmpStream,
                PixelBuffer *pixels,
                unsigned int width,
                unsigned int height,
                unsigned int offset,
                bool topDown)
{
  if(/*fp == NULL ||*/  pixels == NULL)
  {
    DALI_LOG_ERROR("Error decoding BMP_RLE4 format\n");
    return false;
  }
  PixelBuffer *pixelsPtr = pixels;
  width = ((width & 3) != 0) ? width + 4 - (width & 3) : width;
  char cmd[2];
  unsigned int cmdStride = 2;
  char colorTable[64];
  std::vector<char> colorIndex(width * height >> 1);
  std::vector<char> run;
  int x = 0;
  int y = 0;
  int dx = 0;
  int dy = 0;
  width += (width & 1);
  width = width >> 1;

  bool finish = false;

  vBmpStream.vSeek(offset, VectorImgStreamer::V_SEEK_SET);

  if (vBmpStream.vRead((char *)colorTable, 64) != 64)  	
  {
    return false;
  }

  while((x >> 1) + y * width < width * height)
  {
    if (finish)
    {
      break;
    }
	if (vBmpStream.vRead((char *)cmd, cmdStride) != cmdStride)
    {
      return false;
    }
    if(cmd[0] == 0) // ESCAPE
    {
      switch(cmd[1])
      {
        case 1: //end of bitmap
          finish = true;
          break;
        case 0: // end of line
          x = 0;
          y ++;
          break;
        case 2: // delta
		  if (vBmpStream.vRead((char *)cmd, cmdStride) != cmdStride)
          {
            DALI_LOG_ERROR("Error reading the BMP image\n");
            return false;
          }
          dx = cmd[0] & (0xFF);
          dy = cmd[1] & (0xFF);
          x += dx;
          y += dy;
          break;
        default:
          // decode a literal run
          unsigned int length = cmd[1] & (0xFF);
          //size of run, which is word aligned
          unsigned int bytesize = length;
          bytesize += (bytesize & 1);
          bytesize >>= 1;
          bytesize += (bytesize & 1);
          run.resize(bytesize);
		  if (vBmpStream.vRead((char *)&run[0], bytesize) != bytesize)
          {
            DALI_LOG_ERROR("Error reading the BMP image\n");
            return false;
          }
          if((x & 1) == 0)
          {
            length += (length & 1);
            length >>= 1;
            for(unsigned int i = 0; i < length; i += 1)
            {
              colorIndex[(x >> 1) + width * (height - y - 1) + i] = run[i];
            }
          }
          else
          {
            for(unsigned int i = 0; i < length; i ++)
            {
              if((i & 1) == 0)//copy high to low
              {
                colorIndex[((x + i) >> 1) + width * (height - y - 1)] |= ((run[i >> 1] & 0xF0) >> 4);
              }
              else //copy low to high
              {
                colorIndex[((x + i) >> 1) + width * (height - y - 1)] |= ((run[i >> 1] & 0x0F) << 4);
              }
            }
          }
          x += cmd[1] & (0xFF);
          break;
      }
    }
    else
    {
      unsigned int length = cmd[0] & (0xFF);
      if((x & 1) == 0)
      {
        length += (length & 1);
        length >>= 1;
        for(unsigned int i = 0; i < length; i ++)
        {
          colorIndex[(height-y-1)*width + i + (x >> 1)] = cmd[1];
        }
      }
      else
      {
        for(unsigned int i = 0; i < length; i ++)
        {
          if((i & 1) == 0)
          {
            colorIndex[((x + i) >> 1) + width * (height - y - 1)] |= ((cmd[1] & 0xF0) >> 4);
          }
          else
          {
            colorIndex[((x + i) >> 1) + width * (height - y - 1)] |= ((cmd[1] & 0x0F) << 4);
          }
        }
      }
      x += cmd[0] & (0xFF);
    }
  }

  int ctIndexHigh = 0;
  int ctIndexLow = 0;
  for(unsigned int index = 0; index < (width * height ); index = index + 1)
  {
    ctIndexHigh = colorIndex[ index] >> 4;
    ctIndexLow = colorIndex[index] & (0x0F);
    pixelsPtr[6 * index ] = colorTable[4 * ctIndexHigh + 2];
    pixelsPtr[6 * index + 1] = colorTable[4 * ctIndexHigh + 1];
    pixelsPtr[6 * index + 2] = colorTable[4 * ctIndexHigh ];
    pixelsPtr[6 * index + 3] = colorTable[4 * ctIndexLow + 2];
    pixelsPtr[6 * index + 4] = colorTable[4 * ctIndexLow + 1];
    pixelsPtr[6 * index + 5] = colorTable[4 * ctIndexLow ];
  }
  return true;
}

/**
 * function to decode format BI_RLE8 & bpp = 8
 * @param[in]  fp      The file to read from
 * @param[out] pixels  The pointer that  we want to store bmp data  in
 * @param[in]  width   bmp width
 * @param[in]  height  bmp height
 * @param[in]  offset  offset from bmp header to bmp palette data
 * @param[in]  topDown indicate image data is read from bottom or from top
 * @return true, if decode successful, false otherwise
 */
bool DecodeRLE8(VectorImgStreamer &vBmpStream,
                PixelBuffer *pixels,
                unsigned int width,
                unsigned int height,
                unsigned int offset,
                bool topDown)
{
  if(/*fp == NULL ||*/  pixels == NULL)
  {
    DALI_LOG_ERROR("Error decoding BMP_RLE8 format\n");
    return false;
  }
  PixelBuffer *pixelsPtr = pixels;
  int x = 0;
  int y = 0;
  unsigned int cmdStride = 2;

  width = ((width & 3) != 0) ? width + 4 - (width & 3) : width;
  std::vector<char> colorTable(1024);
  char cmd[2];
  std::vector<char> colorIndex(width * height);

  vBmpStream.vSeek(offset, VectorImgStreamer::V_SEEK_SET);

  if (vBmpStream.vRead((char *)&colorTable[0], 1024) != 1024)
  {
    return false;
  }

  int dx = 0;
  int dy = 0;
  bool finish = false;
  unsigned int length = 0;
  unsigned int copylength = 0;
  std::vector<char> run;
  while((x + y * width) < width * height )
  {
    if (finish)
    {
      break;
    }
	if (vBmpStream.vRead((char *)cmd, cmdStride) != cmdStride)
    {
      return false;
    }

    if(cmd[0] == 0)//ESCAPE
    {
      switch(cmd[1])
      {
        case 1: // end of bitmap
          finish = true;
          break;
        case 0: // end of line
          x = 0;
          y ++;
          break;
        case 2: // delta
		  if (vBmpStream.vRead((char *)cmd, cmdStride) != cmdStride)
          {
            DALI_LOG_ERROR("Error reading the BMP image\n");
            return false;
          }
          dx = cmd[0] & (0xFF);
          dy = cmd[1] & (0xFF);
          x += dx;
          y += dy;
          break;
        default:
          //decode a literal run
          length = cmd[1] & (0xFF);
          copylength = length;
          //absolute mode must be word-aligned
          length += (length & 1);
          run.resize(length);
		  if (vBmpStream.vRead((char *)&run[0], length) != length)
          {
            DALI_LOG_ERROR("Error reading the BMP image\n");
            return false;
          }

          for(unsigned int i = 0; i < length; i += 1)
          {
            colorIndex[x + width * (height - y - 1) + i] = run[i];
          }
          x += copylength;
          break;
      }
    }// end if cmd[0] ==
    else
    {
      length = cmd[0] & (0xFF);
      for(unsigned int i = 0; i < length; i ++)
      {
        colorIndex[(height - y - 1) * width + x] = cmd[1];
        x++;
      }
    }
  }
  int ctIndex = 0;
  for(unsigned int index = 0; index < width * height; index = index + 1)
  {
    ctIndex = colorIndex[ index];
    pixelsPtr[3 * index ] = colorTable[4 * ctIndex + 2];
    pixelsPtr[3 * index + 1] = colorTable[4 * ctIndex + 1];
    pixelsPtr[3 * index + 2] = colorTable[4 * ctIndex ];
  }
  return true;
}

} // unnamed namespace

bool LoadBmpHeader(const std::vector<uint8_t>& data, unsigned int &width, unsigned int &height)
{
  BmpFileHeader fileHeader;
  BmpInfoHeader infoHeader;

  bool ret = LoadBmpHeader(data, width, height, fileHeader, infoHeader);

  if (width<0)
  {
    width = abs(width);
  }

  return ret;
}

bool LoadBitmapFromBmp(const std::vector<uint8_t>& data, Bitmap& bitmap, ImageAttributes& attributes)
{
  
  BmpFormat customizedFormat = BMP_NOTEXIST;
  BmpFileHeader fileHeader;
  BmpInfoHeader infoHeader;

  // Load the header info
  unsigned int width, height;

  VectorImgStreamer vBmpStream(data);
  

  if (!LoadBmpHeader(data, width, height, fileHeader, infoHeader))
  {
      return false;
  }

  Pixel::Format pixelFormat = Pixel::RGB888;
  switch(infoHeader.compression)
  {
    case 0:
      switch (infoHeader.bitsPerPixel)
      {
        case 32:
          pixelFormat = Pixel::BGR8888;
          break;

        case 24:
          if(fileHeader.offset == FileHeaderOffsetOfRGB24V5)//0x8A
          {
            customizedFormat = BMP_RGB24V5;
          }
          else
          {
            pixelFormat = Pixel::RGB888;
          }
          break;

        case 16:
          customizedFormat = BMP_RGB555;
          break;

        case 8:
          customizedFormat = BMP_RGB8;
          break;

        case 4: // RGB4
          customizedFormat = BMP_RGB4;
          break;

        case 1: //RGB1
          customizedFormat = BMP_RGB1;
          break;
        default:
          DALI_LOG_WARNING("%d bits per pixel not supported for BMP files\n", infoHeader.bitsPerPixel);
          return false;
      }
    break;
    case 1: //// RLE8
    {
      if(infoHeader.bitsPerPixel == 8)
      {
        customizedFormat = BMP_RLE8;
      }
      break;
    }
    case 2: // RLE4
    {
      if(infoHeader.bitsPerPixel == 4)
      {
        customizedFormat = BMP_RLE4;
      }
      break;
    }
    case 3: // // BI_BITFIELDS
    {
      if(infoHeader.bitsPerPixel == 16)
      {
		vBmpStream.vSeek(14 + infoHeader.infoHeaderSize + 1, VectorImgStreamer::V_SEEK_SET);

        char mask;
		if (vBmpStream.vRead((char *)&mask, 1) != 1)
        {
          return false;
        }

        if((mask & 0x80) == MaskForBFRGB565) // mask is 0xF8
        {
          pixelFormat = Pixel::RGB565;
        }
        else if((mask & 0x80) == 0)// mask is 0x 7C
        {
          customizedFormat = BMP_BITFIELDS555;
        }
        else
        {
          return false;
        }
      }
      else if(infoHeader.bitsPerPixel == 32)
      {
        if(fileHeader.offset == FileHeaderOffsetOfBF32V4)// 0x7A
        {
          customizedFormat = BMP_BITFIELDS32V4;
        }
        else
        {
          customizedFormat = BMP_BITFIELDS32;
        }
      }
      break;
    }
    default:
      DALI_LOG_WARNING("Compression not supported for BMP files\n");
      return false;
  }

  bool topDown = false;

  // if height is negative, bitmap data is top down
  if (infoHeader.height<0)
  {
    infoHeader.height =  abs(infoHeader.height);
    height = infoHeader.height;
    topDown = true;
  }

  unsigned int rowStride = infoHeader.width * (infoHeader.bitsPerPixel >>3);

  // bitmaps row stride is padded to 4 bytes
  unsigned int padding = (rowStride % 4);
  if (padding)
  {
    padding = 4 - padding;
  }

  PixelBuffer *pixels =  NULL;
  int imageW = infoHeader.width;
  int pixelBufferW = 0;
  switch(customizedFormat)
  {
  case BMP_RLE8:
  case BMP_RGB8:
  case BMP_RGB4:
  case BMP_RLE4:
  case BMP_RGB555:
  case BMP_BITFIELDS555:
  {
    pixelBufferW = ((imageW & 3) != 0) ? imageW + 4 - (imageW & 3) : imageW;
    pixels = bitmap.GetPackedPixelsProfile()->ReserveBuffer(Pixel::RGB888, pixelBufferW, abs(infoHeader.height));
    break;
  }
  case BMP_RGB1:
  {
    pixelBufferW = ((imageW & 63) != 0) ? imageW + 64 - (imageW & 63) : imageW;
    pixels = bitmap.GetPackedPixelsProfile()->ReserveBuffer(Pixel::RGB888, pixelBufferW, abs(infoHeader.height));
    break;
  }
  case BMP_BITFIELDS32:
  case BMP_BITFIELDS32V4:
  {
    pixels = bitmap.GetPackedPixelsProfile()->ReserveBuffer(Pixel::RGB8888, infoHeader.width, abs(infoHeader.height));
    break;
  }
  case BMP_RGB24V5:
  {
    pixels = bitmap.GetPackedPixelsProfile()->ReserveBuffer(Pixel::RGB888, infoHeader.width, infoHeader.height);
    break;
  }
  default:
    if(pixelFormat == Pixel::RGB565 )
    {
      pixelBufferW = ((imageW & 3) != 0) ? imageW + 4 - (imageW & 3) : imageW;
      pixels = bitmap.GetPackedPixelsProfile()->ReserveBuffer(Pixel::RGB565, pixelBufferW, abs(infoHeader.height));
    }
    else
    {
      pixels = bitmap.GetPackedPixelsProfile()->ReserveBuffer(pixelFormat, infoHeader.width, infoHeader.height);
    }
    break;
  }

  // TODO: Add scaling support

  // Read the raw bitmap data
  PixelBuffer *pixelsPtr;
  bool decodeResult(false);
  switch(customizedFormat)
  {
    case BMP_RGB1:
    {
      decodeResult = DecodeRGB1( vBmpStream, pixels, infoHeader.width, infoHeader.height, 14 + infoHeader.infoHeaderSize, topDown);
      break;
    }
    case BMP_RGB4:
    {
      decodeResult = DecodeRGB4(vBmpStream, pixels, infoHeader.width, infoHeader.height, 14 + infoHeader.infoHeaderSize, topDown);
      break;
    }
    case BMP_RLE4:
    {
      decodeResult = DecodeRLE4( vBmpStream, pixels, infoHeader.width, abs(infoHeader.height), 14 + infoHeader.infoHeaderSize, topDown);
      break;
    }
    case BMP_BITFIELDS32:
    {
      decodeResult = DecodeBF32(vBmpStream, pixels, infoHeader.width,  abs(infoHeader.height), fileHeader.offset, topDown, rowStride, padding);
      break;
    }
    case BMP_BITFIELDS555:
    {
      decodeResult = DecodeBF555(vBmpStream, pixels,infoHeader.width,  abs(infoHeader.height), fileHeader.offset, topDown);
      break;
    }
    case BMP_RGB555:
    {
      decodeResult = DecodeRGB555(vBmpStream, pixels, infoHeader.width, abs(infoHeader.height), fileHeader.offset, topDown);
      break;
    }
    case BMP_RGB8:
    {
      decodeResult = DecodeRGB8(vBmpStream, pixels, infoHeader.width, abs(infoHeader.height), 14 + infoHeader.infoHeaderSize, topDown);
      break;
    }
    case BMP_RLE8:
    {
      decodeResult = DecodeRLE8(vBmpStream, pixels, infoHeader.width, abs(infoHeader.height), 14 + infoHeader.infoHeaderSize, topDown);
      break;
    }
    case BMP_RGB24V5:
    {
      decodeResult = DecodeRGB24V5(vBmpStream, pixels, infoHeader.width, abs(infoHeader.height), fileHeader.offset, topDown, rowStride, padding);
      break;
    }
    case BMP_BITFIELDS32V4:
    {
      decodeResult = DecodeBF32V4(vBmpStream, pixels, infoHeader.width, abs(infoHeader.height), fileHeader.offset, topDown, rowStride, padding);
      break;
    }
    default:
    {
      if(pixelFormat == Pixel::RGB565)
      {
        decodeResult = DecodeBF565(vBmpStream, pixels, infoHeader.width, abs(infoHeader.height), fileHeader.offset,  topDown);
      }
      else
      {
        for (unsigned int yPos = 0; yPos < height; yPos++)
        {
          if (topDown)
          {
            // the data in the file is top down, and we store the data top down
            pixelsPtr = pixels + ( yPos * rowStride);
          }
          else
          {
            // the data in the file is bottom up, and we store the data top down
            pixelsPtr = pixels + (((height-1)-yPos) * rowStride);
          }

		  if (vBmpStream.vRead((char *)pixelsPtr, rowStride) != rowStride)
          {
            DALI_LOG_ERROR("Error reading the BMP image\n");
            break;
          }

          // If 24 bit mode then swap Blue and Red pixels
          // BGR888 doesn't seem to be supported by dali-core
          if (infoHeader.bitsPerPixel == 24 )
          {
            for(unsigned int i = 0; i < rowStride; i += 3)
            {
              unsigned char temp = pixelsPtr[i];
              pixelsPtr[i] = pixelsPtr[i+2];
              pixelsPtr[i+2] = temp;
            }
          }

          if (padding)
          {
            vBmpStream.vSeek(padding, VectorImgStreamer::V_SEEK_CUR);  // move past the padding.
          }
        }
        decodeResult = true;
      }
      break;
    }
  } // switch

  if( !decodeResult )
  {
    return false;
  }

  attributes.SetSize(infoHeader.width, infoHeader.height);
  attributes.SetPixelFormat(pixelFormat);

  switch(customizedFormat)
  {
    case BMP_RLE8:
    case BMP_RGB8:
    case BMP_RGB4:
    case BMP_RGB1:
    case BMP_RLE4:
    case BMP_RGB555:
    case BMP_BITFIELDS555:
    case BMP_RGB24V5:
      attributes.SetPixelFormat(Pixel::RGB888);
      break;
    case BMP_BITFIELDS32:
    case BMP_BITFIELDS32V4:
      attributes.SetPixelFormat(Pixel::RGB8888);
      break;
    default:
      break;
  }
  return true;
}

} // namespace SlpPlatform

} // namespace Dali

