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

// INTERNAL INCLUDES
#include "bitmap-loader-impl.h"
#include "image-loaders/image-loader.h"

namespace Dali
{
namespace Internal
{

IntrusivePtr<BitmapLoader> BitmapLoader::New(const std::string& url,
                                             ImageDimensions size,
                                             FittingMode::Type fittingMode,
                                             SamplingMode::Type samplingMode,
                                             bool orientationCorrection)
{
  IntrusivePtr<BitmapLoader> internal = new BitmapLoader( url, size, fittingMode, samplingMode, orientationCorrection );
  return internal;
}

BitmapLoader::BitmapLoader(const std::string& url,
                           ImageDimensions size,
                           FittingMode::Type fittingMode,
                           SamplingMode::Type samplingMode,
                           bool orientationCorrection)
: mResourceType( size, fittingMode, samplingMode, orientationCorrection ),
  mPixelData(),
  mUrl(url)
{
}

BitmapLoader::~BitmapLoader()
{
}

void BitmapLoader::Load()
{
  IntrusivePtr<Dali::RefObject> resource = TizenPlatform::ImageLoader::LoadResourceSynchronously( mResourceType, mUrl );

  if( resource )
  {
    Integration::Bitmap* bitmap = static_cast<Integration::Bitmap*>(resource.Get());
    mPixelData = new PixelData( bitmap->GetBufferOwnership(),
                                bitmap->GetImageWidth(),
                                bitmap->GetImageHeight(),
                                bitmap->GetPixelFormat(),
                                PixelData::FREE);
  }
}

bool BitmapLoader::IsLoaded()
{
  return mPixelData ? true : false ;
}

const std::string& BitmapLoader::GetUrl() const
{
  return mUrl;
}

PixelDataPtr BitmapLoader::GetPixelData() const
{
  return mPixelData;
}

unsigned char* BitmapLoader::GetPixelBuffer() const
{
  if( mPixelData )
  {
    return mPixelData->GetBuffer();
  }

  return NULL;
}

unsigned int BitmapLoader::GetImageHeight() const
{
  if( mPixelData )
  {
    return mPixelData->GetHeight();
  }

  return 0u;
}

unsigned int BitmapLoader::GetImageWidth() const
{
  if( mPixelData )
  {
    return mPixelData->GetWidth();
  }

  return 0u;
}

Pixel::Format BitmapLoader::GetPixelFormat() const
{
  if( mPixelData )
  {
    return mPixelData->GetPixelFormat();
  }

  return Pixel::RGBA8888;
}

} // namespace Internal
} // namespace Dali
