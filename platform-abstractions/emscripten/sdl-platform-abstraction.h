#ifndef __DALI_SDL_PLATFORM_ABSTRACTION_H__
#define __DALI_SDL_PLATFORM_ABSTRACTION_H__

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

// EXTERNAL INCLUDES
#include <set>
#include <stdint.h>
#include <cstring>
#include <queue>

// INTERNAL INCLUDES
#include <dali/integration-api/platform-abstraction.h>
#include <dali/integration-api/bitmap.h>

namespace Dali
{

/**
 * Platform abstraction for Emscripten (uses SDL which Emscripten comes bundled with)
 */
class DALI_IMPORT_API SdlPlatformAbstraction : public Dali::Integration::PlatformAbstraction
{

public:

  struct Resources
  {
    bool                         loaded;
    Integration::ResourceId      loadedId;
    Integration::ResourceTypeId  loadedType;
    Integration::ResourcePointer loadedResource;

    bool                         loadFailed;
    Integration::ResourceId      loadFailedId;
    Integration::ResourceFailure loadFailure;

    bool                         saved;
    Integration::ResourceId      savedId;
    Integration::ResourceTypeId  savedType;

    bool                         saveFailed;
    Integration::ResourceId      saveFailedId;
    Integration::ResourceFailure saveFailure;
  };

  struct LoadFileResult
  {
    LoadFileResult()
    : loadResult(false)
    {

    }

    bool loadResult;
    std::vector< unsigned char> buffer;
  };


  /**
   * Constructor
   */
  SdlPlatformAbstraction();


  /**
   * Destructor
   */
  virtual ~SdlPlatformAbstraction();

  /**
   * @copydoc PlatformAbstraction::GetTimeMicroseconds()
   */
  virtual void GetTimeMicroseconds(unsigned int &seconds, unsigned int &microSeconds);

  void IncrementGetTimeResult(size_t milliseconds);

  /**
   * @copydoc PlatformAbstraction::Suspend()
   */
  virtual void Suspend();

  /**
   * @copydoc PlatformAbstraction::Resume()
   */
  virtual void Resume();

  /**
   * @copydoc PlatformAbstraction::LoadImageMetadata()
   */
  virtual void LoadImageMetadata(const std::string fileName, Vector2 &size);

  /**
   * @copydoc PlatformAbstraction::GetClosestImageSize()
   */
  virtual void GetClosestImageSize( const std::string& filename,
                                    const ImageAttributes& attributes,
                                    Vector2& closestSize );

  /**
   * @copydoc PlatformAbstraction::GetClosestImageSize()
   */
  virtual void GetClosestImageSize( Integration::ResourcePointer resourceBuffer,
                                    const ImageAttributes& attributes,
                                    Vector2& closestSize );

  /**
   * @copydoc PlatformAbstraction::LoadResource()
   */
  virtual void LoadResource(const Integration::ResourceRequest& request);

  virtual Integration::ResourcePointer LoadResourceSynchronously( const Integration::ResourceType& resourceType, const std::string& resourcePath );

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
   * @copydoc PlatformAbstraction::GetDefaultFontFamily()
   */
  virtual const std::string& GetDefaultFontFamily() const;

  /**
   * @copydoc PlatformAbstraction::GetDefaultFontSize()
   */
  virtual int GetDefaultFontSize() const;

  /**
   * @copydoc PlatformAbstraction::LoadFile()
   */
  virtual bool LoadFile( const std::string& filename, std::vector< unsigned char >& buffer ) const;

  /**
   * @copydoc PlatformAbstraction::SaveFile()
   */
  virtual bool SaveFile(const std::string& filename, std::vector< unsigned char >& buffer) const;

  virtual void JoinLoaderThreads();

  virtual void GetDefaultFontDescription( std::string& family, std::string& style ) const;

  virtual bool LoadShaderBinFile( const std::string& filename, std::vector< unsigned char >& buffer ) const;

  virtual void UpdateDefaultsFromDevice();

  virtual Integration::DynamicsFactory* GetDynamicsFactory();

private:
  std::string                   mGetDefaultFontFamilyResult;
  Resources                     mResources;
  Vector2                       mSize;

  LoadFileResult                mLoadFileResult;

  typedef std::pair< Integration::ResourceId, Integration::BitmapPtr > ResourceIdBitmapPair;
  std::queue<ResourceIdBitmapPair> mResourceQueue;


};

} // Dali

#endif /* __DALI_TET_PLATFORM_ABSTRACTION_H__ */
