#ifndef __DALI_SLP_PLATFORM_IMAGE_LOADER_H__
#define __DALI_SLP_PLATFORM_IMAGE_LOADER_H__

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

#include <dali/integration-api/resource-cache.h>
#include <dali/integration-api/resource-types.h>
#include <dali/integration-api/bitmap.h>
#include "resource-loading-client.h"
#include <dali/integration-api/file-abstraction.h>

namespace Dali
{
namespace Integration
{
  class File;
}

namespace SlpPlatform
{
namespace ImageLoader
{
/**
 * Convert a file stream into a bitmap.
 * @param[in] resourceType The type of resource to convert.
 * @param[in] fp File Pointer. Closed on exit.
 * @param[in] client The component that is initiating the conversion.
 * @param[out] bitmap Pointer to write bitmap to
 * @return true on success, false on failure
 */
bool ConvertStreamToBitmap( const Integration::ResourceType& resourceType, Integration::File* fp, const ResourceLoadingClient& client, Integration::BitmapPtr& ptr);

/**
 * Convert a bitmap and write to a file stream.
 * @param[in] path The path to the resource.
 * @param[in] fp File Pointer. Closed on exit.
 * @param[out] bitmap Pointer from which to read bitmap
 * @return true on success, false on failure
 */
bool ConvertBitmapToStream( std::string path, Integration::File* fp, Integration::BitmapPtr& ptr );


Integration::ResourcePointer LoadResourceSynchronously( const Integration::ResourceType& resourceType, Integration::File* file_data );

void GetClosestImageSize( Integration::File* file_data,
                          const ImageAttributes& attributes,
                          Vector2 &closestSize );

void GetClosestImageSize( Integration::ResourcePointer resourceBuffer,
                          const ImageAttributes& attributes,
                          Vector2 &closestSize );

} // ImageLoader
} // SlpPlatform
} // Dali

#endif // __DALI_SLP_PLATFORM_IMAGE_LOADER_H__
