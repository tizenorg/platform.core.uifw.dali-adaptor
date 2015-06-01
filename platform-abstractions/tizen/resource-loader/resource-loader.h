#ifndef __DALI_TIZEN_PLATFORM_RESOURCE_LOADER_H__
#define __DALI_TIZEN_PLATFORM_RESOURCE_LOADER_H__

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
#include <dali/public-api/common/dali-vector.h>

#include <string>

namespace Dali
{

namespace Integration
{
namespace Log
{
class Filter;
}
}

namespace TizenPlatform
{

/**
 * Contains information about a successfully loaded resource
 */
struct LoadedResource
{
  /**
   * Constructor
   * @param[in] loadedId        The ID of the resource
   * @param[in] loadedType      The resource type
   * @param[in] loadedResource  A pointer to the loaded resource data
   */
  LoadedResource(Integration::ResourceId      loadedId,
                 Integration::ResourceTypeId  loadedType,
                 Integration::ResourcePointer loadedResource)
  : id(loadedId),
    type(loadedType),
    resource(loadedResource)
  {
  }

  /// Copy constructor
  LoadedResource(const LoadedResource& loaded)
  : id(loaded.id),
    type(loaded.type),
    resource(loaded.resource)
  {
  }

  /// Assignment operator
  LoadedResource& operator=(const LoadedResource& rhs)
  {
    if( this != &rhs )
    {
      id = rhs.id;
      type = rhs.type;
      resource = rhs.resource;
    }
    return *this;
  }

  Integration::ResourceId      id;         ///< Integer ID
  Integration::ResourceTypeId  type;       ///< Type enum (bitmap, shader, ...)
  Integration::ResourcePointer resource;   ///< Reference counting pointer to the loaded / decoded representation  of the resource.
};

/**
 * Contains information about a successfully saved resource
 */
struct SavedResource
{
  /**
   * Constructor
   * @param[in] savedId   The ID of the resource
   * @param[in] savedType The resource type
   */
  SavedResource(Integration::ResourceId     savedId,
                Integration::ResourceTypeId savedType)
  : id(savedId),
    type(savedType)
  {
  }

  /// Copy constructor
  SavedResource(const LoadedResource& loaded)
  : id(loaded.id),
    type(loaded.type)
  {
  }

  /// Assignment operator
  SavedResource& operator=(const SavedResource& rhs)
  {
    if( this != &rhs )
    {
      id = rhs.id;
      type = rhs.type;
    }
    return *this;
  }

  Integration::ResourceId     id;
  Integration::ResourceTypeId type;
};

/**
 * Contains information about a failed resource load/save request
 */
struct FailedResource
{
  FailedResource(Integration::ResourceId resourceId, Integration::ResourceFailure failure):
    id(resourceId),
    failureType(failure)
  {
  }

  /// Copy constructor
  FailedResource(const FailedResource& failed)
  : id(failed.id),
    failureType(failed.failureType)
  {
  }

  /// Assignment operator
  FailedResource& operator=(const FailedResource& rhs)
  {
    if( this != &rhs )
    {
      id = rhs.id;
      failureType = rhs.failureType;
    }
    return *this;
  }

  Integration::ResourceId      id;
  Integration::ResourceFailure failureType;
};

/**
 * This implements the resource loading part of the PlatformAbstraction API.
 * The requests for a specific resource type are farmed-out to a resource
 * requester for that type which handles them in their own dedicated loading
 * threads.
 */
class ResourceLoader
{
public:

  /**
   * Create a resource loader.
   * There should exactly one of these objects per Dali Core.
   */
  ResourceLoader();

  /**
   * Non-virtual destructor.
   * ResourceLoader is NOT intended as a base class.
   */
  ~ResourceLoader();

  /**
   * Pause processing of already-queued resource requests.
   */
  void Pause();

  /**
   * Continue processing resource requests.
   */
  void Resume();

  /**
   * Check if the ResourceLoader is terminating
   * @return true if terminating else false
   */
  bool IsTerminating();

  /**
   * Add a partially loaded resource to the PartiallyLoadedResource queue
   * @param[in] resource The resource's information and data
   */
  void AddPartiallyLoadedResource(LoadedResource& resource);

  /**
   * Add a completely loaded resource to the LoadedResource queue
   * @param[in] resource The resource's information and data
   */
  void AddLoadedResource(LoadedResource& resource);

  /**
   * Add a successfully saved resource to the SavedResource queue
   * @param[in] resource The resource's information
   */
  void AddSavedResource(SavedResource& resource);

  /**
   * Add information about a failed resource load to the FailedLoads queue
   * @param[in] resource The failed resource's information
   */
  void AddFailedLoad(FailedResource& resource);

  /**
   * Add information about a failed resource save to the FailedSaves queue
   * @param[in] resource The failed resource's information
   */
  void AddFailedSave(FailedResource& resource);


  // From PlatformAbstraction

  /**
   * @copydoc PlatformAbstraction::LoadResource()
   */
  void LoadResource(const Integration::ResourceRequest& request);

  /**
   * @copydoc PlatformAbstraction::SaveResource()
   */
  void SaveResource(const Integration::ResourceRequest& request);

  /**
   * @copydoc PlatformAbstraction::CancelLoad()
   */
  void CancelLoad(Integration::ResourceId id, Integration::ResourceTypeId typeId);

  /**
   * @copydoc PlatformAbstraction::IsLoading()
   * @deprecated This is not implemented: always returns true.
   */
  bool IsLoading();

  /**
   * @copydoc PlatformAbstraction::GetResources()
   */
  void GetResources(Integration::ResourceCache& cache);

  /**
   * @copydoc PlatformAbstraction::SetDpi()
   */
  void SetDpi(unsigned int dpiHor, unsigned int dpiVer);

  /**
   * @copydoc SlpPlatformAbstraction::LoadFile()
   */
  bool LoadFile( const std::string& filename, std::vector< unsigned char >& buffer ) const;

  /**
   * @copydoc TizenPlatformAbstraction::LoadFile()
   */
  std::string LoadFile(const std::string& filename) const;

  /**
   * @copydoc TizenPlatformAbstraction::SaveFile()
   */
  static bool SaveFile(const std::string& filename, std::vector< unsigned char >& buffer);

private:
  struct ResourceLoaderImpl;
  ResourceLoaderImpl* mImpl;

  volatile int mTerminateThread;        ///< Set to <> 0 in destructor, signals threads to exit their controlling loops

};

} // namespace TizenPlatform

} // namespace Dali

#endif // __DALI_TIZEN_PLATFORM_RESOURCE_LOADER_H_
