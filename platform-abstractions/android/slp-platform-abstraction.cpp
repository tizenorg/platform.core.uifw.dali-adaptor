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

#include "slp-platform-abstraction.h"

#include <dirent.h>

#include <dali/integration-api/debug.h>
#include <dali/integration-api/bitmap.h>
#include <dali/integration-api/resource-types.h>

#include "resource-loader/resource-loader.h"
#include "slp-font-configuration-parser.h"
#include "image-loaders/image-loader.h"

namespace Dali
{

Integration::PlatformAbstraction* CreatePlatformAbstraction()
{
  return new SlpPlatform::SlpPlatformAbstraction();
}


namespace SlpPlatform
{

namespace
{
const unsigned int NANOSECS_TO_MICROSECS( 1000 );                          ///< 1000 nanoseconds = 1 microsecond
}

SlpPlatformAbstraction::SlpPlatformAbstraction()
: mResourceLoader(new ResourceLoader),
  mDataStoragePath( "" )
{
}

SlpPlatformAbstraction::~SlpPlatformAbstraction()
{
  delete mResourceLoader;
}

void SlpPlatformAbstraction::Suspend()
{
  if (mResourceLoader)
  {
    mResourceLoader->Pause();
  }
}

void SlpPlatformAbstraction::Resume()
{
  if (mResourceLoader)
  {
    mResourceLoader->Resume();
  }
}


int SlpPlatformAbstraction::GetDefaultFontSize() const
{
  int fontSize( -1 );

  return fontSize;
}

ImageDimensions SlpPlatformAbstraction::GetClosestImageSize( const std::string& filename,
                                                             ImageDimensions size,
                                                             FittingMode::Type fittingMode,
                                                             SamplingMode::Type samplingMode,
                                                             bool orientationCorrection )
{
  return ImageLoader::GetClosestImageSize( filename, size, fittingMode, samplingMode, orientationCorrection );
}

ImageDimensions SlpPlatformAbstraction::GetClosestImageSize( Integration::ResourcePointer resourceBuffer,
                                                             ImageDimensions size,
                                                             FittingMode::Type fittingMode,
                                                             SamplingMode::Type samplingMode,
                                                             bool orientationCorrection )
{
  return ImageLoader::GetClosestImageSize( resourceBuffer, size, fittingMode, samplingMode, orientationCorrection );
}

void SlpPlatformAbstraction::LoadResource(const Integration::ResourceRequest& request)
{
  if (mResourceLoader)
  {
    mResourceLoader->LoadResource(request);
  }
}

Integration::ResourcePointer SlpPlatformAbstraction::LoadResourceSynchronously(const Integration::ResourceType& resourceType, const std::string& resourcePath)
{
  return ImageLoader::LoadResourceSynchronously( resourceType, resourcePath );
}

Integration::BitmapPtr SlpPlatformAbstraction::DecodeBuffer( const Integration::ResourceType& resourceType, uint8_t * buffer, size_t size )
{
  Integration::BitmapPtr bitmap;

  bool result = ImageLoader::ConvertStreamToBitmap( resourceType, "", buffer, size, StubbedResourceLoadingClient(), bitmap );
  if( !result )
  {
    bitmap.Reset();
  }
  return bitmap;
}

void SlpPlatformAbstraction::CancelLoad(Integration::ResourceId id, Integration::ResourceTypeId typeId)
{
  if (mResourceLoader)
  {
    mResourceLoader->CancelLoad(id, typeId);
  }
}

void SlpPlatformAbstraction::GetResources(Integration::ResourceCache& cache)
{
  if (mResourceLoader)
  {
    mResourceLoader->GetResources(cache);
  }
}

bool SlpPlatformAbstraction::LoadFile( const std::string& filename, Dali::Vector< unsigned char >& buffer ) const
{
  bool result = false;

  if( mResourceLoader )
  {
    result = mResourceLoader->LoadFile( filename, buffer );
  }

  return result;
}

std::string SlpPlatformAbstraction::LoadFile( const std::string& filename )
{
  std::string result;
  if (mResourceLoader)
  {
    result = mResourceLoader->LoadFile(filename);
  }

  return result;
}

void SlpPlatformAbstraction::JoinLoaderThreads()
{
  delete mResourceLoader;
  mResourceLoader = NULL;
}

bool SlpPlatformAbstraction::LoadShaderBinaryFile( const std::string& filename, Dali::Vector< unsigned char >& buffer ) const
{
  bool result = false;

  return result;
}

bool SlpPlatformAbstraction::SaveShaderBinaryFile( const std::string& filename, const unsigned char * buffer, unsigned int numBytes ) const
{
  bool result = false;
  return result;
}

void SlpPlatformAbstraction::SetDataStoragePath( const std::string& path )
{
  mDataStoragePath = path;
}

} // namespace SlpPlatform
} // namespace Dali
