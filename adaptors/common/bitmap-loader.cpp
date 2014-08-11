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

#include "bitmap-loader.h"
#include <dali/integration-api/bitmap.h>
#include <dali/integration-api/resource-types.h>
#include <dali/integration-api/resource-cache.h>
#include "image-loaders/image-loader.h"

#include <string>

namespace Dali
{

namespace Internal
{

struct BitmapLoaderImpl
{
  BitmapLoaderImpl( const std::string& filename )
  {
    ImageAttributes attributes;
    Integration::BitmapResourceType bitmapResourceType( attributes );
    Integration::ResourcePointer resource = SlpPlatform::ImageLoader::LoadResourceSynchronously( bitmapResourceType, filename );
    mBitmap = static_cast<Integration::Bitmap*>(resource.Get());
  }

  Integration::BitmapPtr mBitmap;
};

} // namespace Internal


BitmapLoader::BitmapLoader(const std::string& filename)
{
  mImpl = new Internal::BitmapLoaderImpl(filename);
}

BitmapLoader::~BitmapLoader()
{
  delete mImpl;
}

unsigned char* BitmapLoader::GetPixelData()
{
  return mImpl->mBitmap->GetBuffer();
}

unsigned int BitmapLoader::GetImageHeight()
{
  return mImpl->mBitmap->GetImageHeight();
}

unsigned int BitmapLoader::GetImageWidth()
{
  return mImpl->mBitmap->GetImageWidth();
}

unsigned int BitmapLoader::GetBufferStride()
{
  return mImpl->mBitmap->GetPackedPixelsProfile()->GetBufferStride();
}

Pixel::Format BitmapLoader::GetPixelFormat()
{
  return mImpl->mBitmap->GetPixelFormat();
}

} // namespace Dali
