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

// CLASS HEADER
#include "bitmap-loader.h"

// EXTERNAL INCLUDES
#include <string>

#include <dali/integration-api/bitmap.h>
#include <dali/integration-api/resource-types.h>
#include <dali/integration-api/resource-cache.h>

// INTERNAL INCLUDES
#include "image-loaders/image-loader.h"
#include <bitmap-loader-impl.h>

namespace Dali
{

BitmapLoader BitmapLoader::New(const std::string& filename)
{
  IntrusivePtr<Internal::BitmapLoader> internal = Internal::BitmapLoader::New(filename);
  return BitmapLoader( internal.Get() );
}

BitmapLoader::BitmapLoader()
{
}

BitmapLoader::BitmapLoader(Internal::BitmapLoader* internal)
: BaseHandle( internal )
{
}

BitmapLoader::~BitmapLoader()
{
}

BitmapLoader::BitmapLoader( const BitmapLoader& handle )
: BaseHandle( handle )
{
}

BitmapLoader& BitmapLoader::operator=(const BitmapLoader& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

unsigned char* BitmapLoader::GetPixelData() const
{
  return GetImplementation(*this).GetPixelData();
}

unsigned int BitmapLoader::GetImageHeight() const
{
  return GetImplementation(*this).GetImageHeight();
}

unsigned int BitmapLoader::GetImageWidth() const
{
  return GetImplementation(*this).GetImageWidth();
}

unsigned int BitmapLoader::GetBufferStride() const
{
  return GetImplementation(*this).GetBufferStride();
}

Pixel::Format BitmapLoader::GetPixelFormat() const
{
  return GetImplementation(*this).GetPixelFormat();
}

} // namespace Dali
