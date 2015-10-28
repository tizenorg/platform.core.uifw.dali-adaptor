#ifndef __DALI_BITMAP_LOADER_IMPL_H__
#define __DALI_BITMAP_LOADER_IMPL_H__

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
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/integration-api/bitmap.h>
#include <dali/integration-api/resource-types.h>

// INTERNAL INCLUDES
#include <bitmap-loader.h>

namespace Dali
{
namespace Internal
{

class BitmapLoader : public BaseObject
{
public:

  /**
   * @copydoc Dali::BitmapLoader::New
   */
  static IntrusivePtr<BitmapLoader> New( const std::string& url,
                                         ImageDimensions size,
                                         FittingMode::Type fittingMode,
                                         SamplingMode::Type samplingMode,
                                         bool orientationCorrection);

  /**
   * Create the bitmap loader object.
   */
   BitmapLoader(const std::string& url,
               ImageDimensions size,
               FittingMode::Type fittingMode,
               SamplingMode::Type samplingMode,
               bool orientationCorrection);

protected:
  /**
   * Destructor
   */
  ~BitmapLoader();

public:

  /**
   * @copydoc Dali::BitmapLoader::Load
   */
  void Load();

  /**
   * @copydoc Dali::BitmapLoader::IsLoaded
   */
  bool IsLoaded();

  /**
   * @copydoc Dali::BitmapLoader::GetPixelData
   */
  unsigned char* GetPixelData() const;

  /**
   * @copydoc Dali::BitmapLoader::GetPixelDataOwnership
   */
  Dali::Atlas::PixelDataPtr GetPixelDataOwnership() const;

  /**
   * Get the buffer height in pixels
   * @return the height of the buffer in pixels
   */
  unsigned int GetImageHeight() const;

  /**
   * Get the buffer width in pixels
   * @return the width of the buffer in pixels
   */
  unsigned int GetImageWidth() const;

  /**
   * Get the pixel format of the loaded bitmap.
   */
  Pixel::Format GetPixelFormat() const;

private:
  Integration::BitmapResourceType mResourceType;
  Integration::BitmapPtr mBitmap;
  const std::string mUrl;
};

} // Internal


inline Internal::BitmapLoader& GetImplementation(Dali::BitmapLoader& handle)
{
  DALI_ASSERT_ALWAYS( handle && "handle is empty" );

  BaseObject& object = handle.GetBaseObject();

  return static_cast<Internal::BitmapLoader&>(object);
}

inline const Internal::BitmapLoader& GetImplementation(const Dali::BitmapLoader& handle)
{
  DALI_ASSERT_ALWAYS( handle && "handle is empty" );

  const BaseObject& object = handle.GetBaseObject();

  return static_cast<const Internal::BitmapLoader&>(object);
}

} // Dali

#endif // __DALI_BITMAP_LOADER_IMPL_H__
