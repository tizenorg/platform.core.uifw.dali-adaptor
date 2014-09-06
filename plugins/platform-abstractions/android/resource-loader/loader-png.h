#ifndef __DALI_SLP_PLATFORM_LOADER_PNG_H__
#define __DALI_SLP_PLATFORM_LOADER_PNG_H__

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

#include <cstdio>
#include <dali/public-api/common/dali-vector.h>
#include <stdint.h>

struct AAsset;

namespace Dali
{

namespace Integration
{
  class Bitmap;
}
struct ImageAttributes;

namespace SlpPlatform
{

namespace Png
{
const unsigned char MAGIC_BYTE_1 = 0x89;
const unsigned char MAGIC_BYTE_2 = 0x50;
} // namespace Png

/**
 * Loads the bitmap from an PNG file.  This function checks the header first
 * and if it is not a PNG file, then it returns straight away.
 * @param[in]  fp      Pointer to the Image file
 * @param[in]  bitmap  The bitmap class where the decoded image will be stored
 * @param[in]  attributes  Describes the dimensions, pixel format and other details for loading the image data
 * @return  true if file decoded successfully, false otherwise
 */
bool LoadBitmapFromPng(const Dali::Vector<uint8_t>&, Integration::Bitmap& bitmap, ImageAttributes& attributes);

/**
 * Loads the header of a PNG file and fills in the width and height appropriately.
 * @param[in]   fp      Pointer to the Image file
 * @param[out]  width   Is set with the width of the image
 * @param[out]  height  Is set with the height of the image
 * @return true if the file's header was read successully, false otherwise
 */
bool LoadPngHeader(const Dali::Vector<uint8_t>&, const ImageAttributes& attrs, unsigned int &width, unsigned int &height);

} // namespace SlpPlatform

} // namespace Dali

#endif // __DALI_SLP_PLATFORM_LOADER_PNG_H__
