#ifndef __DALI_TIZEN_PLATFORM_ABSTRACTION_H__
#define __DALI_TIZEN_PLATFORM_ABSTRACTION_H__

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

class DynamicsFactory;
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
   * Destructor
   */
  virtual ~TizenPlatformAbstraction();

public: // PlatformAbstraction overrides

  /**
   * @copydoc PlatformAbstraction::GetTimeMicroseconds()
   */
  virtual void GetTimeMicroseconds(unsigned int &seconds, unsigned int &microSeconds);

  /**
   * @copydoc PlatformAbstraction::Suspend()
   */
  virtual void Suspend();

  /**
   * @copydoc PlatformAbstraction::Resume()
   */
  virtual void Resume();

  /**
   * @copydoc PlatformAbstraction::GetClosestImageSize()
   */
  virtual ImageDimensions GetClosestImageSize( const std::string& filename,
                                               ImageDimensions size,
                                               FittingMode::Type fittingMode,
                                               SamplingMode::Type samplingMode,
                                               bool orientationCorrection );

  /**
   * @copydoc PlatformAbstraction::GetClosestImageSize()
   */
  virtual ImageDimensions GetClosestImageSize( Integration::ResourcePointer resourceBuffer,
                                               ImageDimensions size,
                                               FittingMode::Type fittingMode,
                                               SamplingMode::Type samplingMode,
                                               bool orientationCorrection );

  /**
   * @copydoc PlatformAbstraction::LoadResource()
   */
  virtual void LoadResource(const Integration::ResourceRequest& request);

  /**
   * @copydoc PlatformAbstraction::LoadResourceSynchronously()
   */
  virtual Integration::ResourcePointer LoadResourceSynchronously(const Integration::ResourceType& resourceType, const std::string& resourcePath);

  /**
   * @copydoc PlatformAbstraction::SaveResource()
   */
  virtual void SaveResource(const Integration::ResourceRequest& request);

  /**
   * @copydoc PlatformAbstraction::CancelLoad()
   */
  virtual void CancelLoad(Integration::ResourceId id, Integration::ResourceTypeId typeId);

  /**
   * @copydoc PlatformAbstraction::GetResources()
   */
  virtual void GetResources(Integration::ResourceCache& cache);

  /**
   * @copydoc PlatformAbstraction::IsLoading()
   */
  virtual bool IsLoading();

  /**
   * @copydoc PlatformAbstraction::JoinLoaderThreads()
   */
  virtual void JoinLoaderThreads();

  /**
   * @copydoc PlatformAbstraction::GetDefaultFontDescription()
   */
  virtual void GetDefaultFontDescription( std::string& family, std::string& style ) const;

  /**
   * @copydoc PlatformAbstraction::GetDefaultFontSize()
   */
  virtual int GetDefaultFontSize() const;

  /**
   * @copydoc PlatformAbstraction::SetDpi()
   */
  virtual void SetDpi (unsigned int DpiHorizontal, unsigned int DpiVertical);

  /**
   * @copydoc PlatformAbstraction::LoadFile()
   */
  virtual bool LoadFile( const std::string& filename, std::vector< unsigned char >& buffer ) const;

  /**
   * @copydoc PlatformAbstraction::LoadFile()
   */
  virtual std::string LoadFile( const std::string& filename );

  /**
   * @copydoc PlatformAbstraction::SaveFile()
   */
  virtual bool SaveFile(const std::string& filename, std::vector< unsigned char >& buffer) const;

  /**
   * @copydoc PlatformAbstraction::GetDynamicsFactory();
   */
  virtual Integration::DynamicsFactory* GetDynamicsFactory();

  /**
   * @copydoc PlatformAbstraction::LoadShaderBinFile()
   */
  virtual bool LoadShaderBinFile( const std::string& filename, std::vector< unsigned char >& buffer ) const;

  /**
   * Sets path for data/resource storage.
   * @param[in] path data/resource storage path
   */
  void SetDataStoragePath( const std::string& path );

private:
  ResourceLoader* mResourceLoader;
  DynamicsFactory* mDynamicsFactory;
  std::string mDataStoragePath;
};

}  // namespace TizenPlatform
}  // namespace Dali

#endif // __DALI_TIZEN_PLATFORM_ABSTRACTION_H__
