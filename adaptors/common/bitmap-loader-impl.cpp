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

#include "bitmap-loader-impl.h"
#include <dali/integration-api/resource-types.h>
#include <dali/integration-api/resource-cache.h>
#include "image-loaders/image-loader.h"

#include <string>

namespace Dali
{
namespace Internal
{

IntrusivePtr<BitmapLoader> BitmapLoader::New(const std::string& filename)
{
  IntrusivePtr<BitmapLoader> internal = new BitmapLoader();
  internal->Initialize(filename);
  return internal;
}

BitmapLoader::BitmapLoader()
: mBitmap(NULL)
{
}

BitmapLoader::~BitmapLoader()
{
}

void BitmapLoader::Initialize(const std::string& filename)
{
  ImageAttributes attributes;
  Integration::BitmapResourceType bitmapResourceType( attributes );
  Integration::ResourcePointer resource = SlpPlatform::ImageLoader::LoadResourceSynchronously( bitmapResourceType, filename );

  // If this is null, then the file was not opened or some other error occured
  mBitmap = static_cast<Integration::Bitmap*>(resource.Get());
}

unsigned char* BitmapLoader::GetPixelData() const
{
  unsigned char* pixelData = NULL;

  if( mBitmap != NULL )
  {
    pixelData = mBitmap->GetBuffer();
  }

  return pixelData;
}

unsigned int BitmapLoader::GetImageHeight() const
{
  unsigned int imageHeight = 0.0f;
  if( mBitmap != NULL )
  {
    imageHeight = mBitmap->GetImageHeight();
  }
  return imageHeight;
}

unsigned int BitmapLoader::GetImageWidth() const
{
  unsigned int imageWidth = 0.0f;
  if( mBitmap != NULL )
  {
    imageWidth = mBitmap->GetImageWidth();
  }
  return imageWidth;
}


unsigned int BitmapLoader::GetBufferStride() const
{
  unsigned int bufferStride = 0;
  if( mBitmap != NULL  )
  {
    bufferStride = mBitmap->GetPackedPixelsProfile()->GetBufferStride();
  }

  return bufferStride;
}

Pixel::Format BitmapLoader::GetPixelFormat() const
{
  Pixel::Format pixelFormat = Pixel::RGBA8888;
  if( mBitmap != NULL )
  {
    pixelFormat = mBitmap->GetPixelFormat();
  }
  return pixelFormat;
}

} // namespace Internal
} // namespace Dali
