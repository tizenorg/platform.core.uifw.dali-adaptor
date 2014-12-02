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
#include <assert.h>

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
  assert(0 && "forgot to implement or remove...");
  ImageAttributes attributes;
  Integration::BitmapResourceType bitmapResourceType( attributes );
  Integration::ResourcePointer resource = SlpPlatform::ImageLoader::LoadResourceSynchronously( bitmapResourceType, NULL );

  mBitmap = static_cast<Integration::Bitmap*>(resource.Get());
}

unsigned char* BitmapLoader::GetPixelData() const
{
  return mBitmap->GetBuffer();
}

unsigned int BitmapLoader::GetImageHeight() const
{
  return mBitmap->GetImageHeight();
}

unsigned int BitmapLoader::GetImageWidth() const
{
  return mBitmap->GetImageWidth();
}

unsigned int BitmapLoader::GetBufferStride() const
{
  return mBitmap->GetPackedPixelsProfile()->GetBufferStride();
}

Pixel::Format BitmapLoader::GetPixelFormat() const
{
  return mBitmap->GetPixelFormat();
}

} // namespace Internal
} // namespace Dali
