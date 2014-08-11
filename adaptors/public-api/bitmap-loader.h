#ifndef __DALI_BITMAP_LOADER_H__
#define __DALI_BITMAP_LOADER_H__

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
 */

// EXTERNAL INCLUDES
#include <string>
#include <dali/public-api/images/pixel.h>

namespace Dali
{
namespace Internal
{
class BitmapLoaderImpl;
}

class BitmapLoader
{
public:
  /**
   * Create the bitmap loader. This will automatically load the image.
   * @param[in] filename  Filename of the bitmap image to load.
   */
  BitmapLoader(const std::string& filename);

  /**
   * Destructor
   */
  ~BitmapLoader();

  /**
   * Get the raw pixel data.
   * @return The pixel data. Use the GetHeight(), GetWidth(), GetStride() and GetPixelFormat() methods
   * to decode the data.
   */
  unsigned char* GetPixelData();

  /**
   * Get the buffer height in pixels
   * @return the height of the buffer in pixels
   */
  unsigned int GetImageHeight();

  /**
   * Get the buffer width in pixels
   * @return the width of the buffer in pixels
   */
  unsigned int GetImageWidth();

  /**
   * Get the number of bytes in each row of pixels
   * @return The buffer stride in bytes.
   */
  unsigned int GetBufferStride();

  /**
   * Get the pixel format of the loaded bitmap.
   */
  Pixel::Format GetPixelFormat();

private:
  Internal::BitmapLoaderImpl* mImpl;
};

} // Dali

#endif // __DALI_BITMAP_LOADER_H__
