#ifndef __DALI_SLP_PLATFORM_LOADER_PNG_H__
#define __DALI_SLP_PLATFORM_LOADER_PNG_H__

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
