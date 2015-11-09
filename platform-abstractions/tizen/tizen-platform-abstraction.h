#ifndef __DALI_TIZEN_PLATFORM_ABSTRACTION_H__
#define __DALI_TIZEN_PLATFORM_ABSTRACTION_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

#include <dali/integration-api/platform-abstraction.h>
#include <dali/integration-api/resource-cache.h>
#include <dali/public-api/common/dali-common.h>

#include <string>

namespace Dali
{

/**
 * Construct a platform abstraction and return it.
 */
Integration::PlatformAbstraction* CreatePlatformAbstraction();

namespace TizenPlatform
{

class ResourceLoader;

/**
 * Concrete implementation of the platform abstraction class.
 */
class TizenPlatformAbstraction : public Integration::PlatformAbstraction
{

public: // Construction & Destruction

  /**
   * Constructor
   */
  TizenPlatformAbstraction();

  /**
   * Non Virtual Destructor.
   */
  ~TizenPlatformAbstraction();

public: // PlatformAbstraction overrides

  /**
   * @copydoc PlatformAbstraction::Suspend()
   */
  void Suspend();

  /**
   * @copydoc PlatformAbstraction::Resume()
   */
  void Resume();

  /**
   * @copydoc PlatformAbstraction::GetClosestImageSize()
   */
  ImageDimensions GetClosestImageSize( const std::string& filename,
                                       ImageDimensions size,
                                       FittingMode::Type fittingMode,
                                       SamplingMode::Type samplingMode,
                                       bool orientationCorrection );

  /**
   * @copydoc PlatformAbstraction::GetClosestImageSize()
   */
  ImageDimensions GetClosestImageSize( Integration::ResourcePointer resourceBuffer,
                                       ImageDimensions size,
                                       FittingMode::Type fittingMode,
                                       SamplingMode::Type samplingMode,
                                       bool orientationCorrection );

  /**
   * @copydoc PlatformAbstraction::LoadResource()
   */
  void LoadResource(const Integration::ResourceRequest& request);

  /**
   * @copydoc PlatformAbstraction::LoadResourceSynchronously()
   */
  Integration::ResourcePointer LoadResourceSynchronously(const Integration::ResourceType& resourceType, const std::string& resourcePath);

  /**
   * @copydoc PlatformAbstraction::DecodeBuffer()
   */
  Integration::BitmapPtr DecodeBuffer( const Integration::ResourceType& resourceType, uint8_t * buffer, size_t size );

  /**
   * @copydoc PlatformAbstraction::CancelLoad()
   */
  void CancelLoad(Integration::ResourceId id, Integration::ResourceTypeId typeId);

  /**
   * @copydoc PlatformAbstraction::GetResources()
   */
  void GetResources(Integration::ResourceCache& cache);

  /**
   * @copydoc PlatformAbstraction::JoinLoaderThreads()
   */
  void JoinLoaderThreads();

  /**
   * @copydoc PlatformAbstraction::GetDefaultFontSize()
   */
  int GetDefaultFontSize() const;

  /**
   * @copydoc PlatformAbstraction::LoadFile()
   */
  bool LoadFile( const std::string& filename, Dali::Vector< unsigned char >& buffer ) const;

  /**
   * @copydoc PlatformAbstraction::LoadFile()
   */
  std::string LoadFile( const std::string& filename );

  /**
   * @copydoc PlatformAbstraction::LoadShaderBinaryFile()
   */
  bool LoadShaderBinaryFile( const std::string& filename, Dali::Vector< unsigned char >& buffer ) const;

  /**
   * @copydoc PlatformAbstraction::SaveShaderBinaryFile()
   */
  bool SaveShaderBinaryFile( const std::string& filename, const unsigned char * buffer, unsigned int numBytes ) const;

  /**
   * Sets path for data/resource storage.
   * @param[in] path data/resource storage path
   */
  void SetDataStoragePath( const std::string& path );

private:
  ResourceLoader* mResourceLoader;
  std::string mDataStoragePath;
};

}  // namespace TizenPlatform
}  // namespace Dali

#endif // __DALI_TIZEN_PLATFORM_ABSTRACTION_H__
