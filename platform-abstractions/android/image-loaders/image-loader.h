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

// EXTERNAL INCLUDES
#include <dali/integration-api/resource-cache.h>
#include <dali/integration-api/resource-types.h>
#include <dali/integration-api/bitmap.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/images/image-operations.h>

// INTERNAL INCLUDES
#include <resource-loading-client.h>

namespace Dali
{
namespace SlpPlatform
{
namespace ImageLoader
{

/**
 * Convert a file stream into a bitmap.
 * @param[in] resourceType The type of resource to convert.
 * @param[in] path The path to the resource.
 * @param[in] data The image data in memory
 * @param[in] client The component that is initiating the conversion.
 * @param[out] bitmap Pointer to write bitmap to
 * @return true on success, false on failure
 */
bool ConvertStreamToBitmap( const Integration::ResourceType& resourceType,
                            const std::string& path,
                            const Dali::Vector<uint8_t>& data,
                            const ResourceLoadingClient& client,
                            Integration::BitmapPtr& ptr);

/**
 * Convert a file stream into a bitmap.
 * @param[in] resourceType The type of resource to convert.
 * @param[in] path The path to the resource.
 * @param[in] buffer The image data in memory
 * @param[in] size The size of the image data in bytes
 * @param[in] client The component that is initiating the conversion.
 * @param[out] bitmap Pointer to write bitmap to
 * @return true on success, false on failure
 */
bool ConvertStreamToBitmap( const Integration::ResourceType& resourceType,
                            const std::string& path,
                            uint8_t* buffer,
                            size_t size,
                            const ResourceLoadingClient& client,
                            Integration::BitmapPtr& ptr );

/**
 * Load a resource immediately without using a loader thread.
 * Useful for small images.
 * @param[in] resourceType The type of resource to convert.
 * @param[in] resourcePath The path to the resource.
 * @return A pointer to the loaded resource
 */
Integration::ResourcePointer LoadResourceSynchronously(
  const Integration::ResourceType& resourceType,
  const std::string& resourcePath );

/**
 * @brief Determine the size of an image the resource loaders will provide when
 * given the same image loading parameters.
 *
 * This is a synchronous request.
 * This function is used to determine the size of an image before it has loaded.
 * @param[in] filename name of the image.
 * @param[in] size The requested size for the image.
 * @param[in] fittingMode The method to use to map the source image to the desired
 * dimensions.
 * @param[in] samplingMode The image filter to use if the image needs to be
 * downsampled to the requested size.
 * @param[in] orientationCorrection Whether to use image metadata to rotate or
 * flip the image, e.g., from portrait to landscape.
 * @return dimensions that image will have if it is loaded with given parameters.
 */
ImageDimensions GetClosestImageSize( const std::string& filename,
                                     ImageDimensions size,
                                     FittingMode::Type fittingMode,
                                     SamplingMode::Type samplingMode,
                                     bool orientationCorrection );

  /**
   * @brief Determine the size of an image the resource loaders will provide when
   * given the same image loading parameters.
   *
   * This is a synchronous request.
   * This function is used to determine the size of an image before it has loaded.
   * @param[in] filename name of the image.
   * @param[in] size The requested size for the image.
   * @param[in] fittingMode The method to use to map the source image to the desired
   * dimensions.
   * @param[in] samplingMode The image filter to use if the image needs to be
   * downsampled to the requested size.
   * @param[in] orientationCorrection Whether to use image metadata to rotate or
   * flip the image, e.g., from portrait to landscape.
   * @return dimensions that image will have if it is loaded with given parameters.
   */
ImageDimensions GetClosestImageSize( Integration::ResourcePointer resourceBuffer,
                                     ImageDimensions size,
                                     FittingMode::Type fittingMode,
                                     SamplingMode::Type samplingMode,
                                     bool orientationCorrection );

/**
 * Load the requested asset into the buffer.
 * @param[in] name The name of the asset
 * @param[out] buffer The buffer to write data into
 * @return true if the asset was loaded correctly into the buffer
 */
bool LoadAsset(const std::string& name, Dali::Vector<uint8_t>& buffer);

} // ImageLoader
} // SlpPlatform
} // Dali

#endif // __DALI_SLP_PLATFORM_IMAGE_LOADER_H__
