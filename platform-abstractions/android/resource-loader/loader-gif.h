#ifndef __DALI_SLP_PLATFORM_LOADER_GIF_H__
#define __DALI_SLP_PLATFORM_LOADER_GIF_H__

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

namespace Dali
{

namespace Integration
{
  class Bitmap;
}

struct ImageAttributes;

namespace SlpPlatform
{

namespace Gif
{
const unsigned char MAGIC_BYTE_1 = 0x47;
const unsigned char MAGIC_BYTE_2 = 0x49;
} // namespace Gif

/**
 * Loads the bitmap from a GIF file.  This function checks the header first
 * and if it is not a GIF file, then it returns straight away.
 * @note For animated GIFs, only the first image is displayed
 * @param[in]  fp      Pointer to the Image file
 * @param[in]  bitmap  The bitmap class where the decoded image will be stored
 * @param[in]  attributes  Describes the dimensions, pixel format and other details for loading the image data
 * @return  true if file decoded successfully, false otherwise
 */
bool LoadBitmapFromGif(FILE *fp, Integration::Bitmap& bitmap, ImageAttributes& attributes);

/**
 * Loads the header of a GIF file and fills in the width and height appropriately.
 * @param[in]   fp      Pointer to the Image file
 * @param[out]  width   Is set with the width of the image
 * @param[out]  height  Is set with the height of the image
 * @return true if the file's header was read successully, false otherwise
 */
bool LoadGifHeader(FILE *fp, unsigned int &width, unsigned int &height);

} // namespace SlpPlatform

} // namespace Dali

#endif // __DALI_SLP_PLATFORM_LOADER_GIF_H__