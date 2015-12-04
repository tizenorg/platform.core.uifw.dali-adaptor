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

// CLASS HEADER
#include "resource-thread-image.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/common/ref-counted-dali-vector.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/integration-api/bitmap.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/resource-cache.h>
#include <dali/integration-api/resource-types.h>
#include <stdint.h>
#include <android/asset_manager_jni.h>

// INTERNAL INCLUDES
#include <platform-abstractions/android/image-loaders/image-loader.h>
#include "platform-capabilities.h"

using namespace Dali::Integration;

namespace Dali
{

namespace SlpPlatform
{


ResourceThreadImage::ResourceThreadImage(ResourceLoader& resourceLoader)
: ResourceThreadBase(resourceLoader)
{
}

ResourceThreadImage::~ResourceThreadImage()
{
}

//--------------------------------------------------------------------------

ResourceThreadImage::ImageType ResourceThreadImage::GetImageType(const std::string& path)
{
  if(path.empty())
  {
    return eInvalidImage;
  }
  else
  {
    // Classify by loader
    // find_first_of(): Searches the string for the first character that matches any of the characters specified in its arguments.
    // find(): Searches the string for the first occurrence of the sequence specified by its arguments.
    if(path.find("content:") == 0 || path.find("res:") == 0 || path.find("shape:") == 0)
    {
      return eResourceFromJava;
    }
  }

  return eResourceFromAsset;
}

//----------------- Called from separate thread (mThread) -----------------

void ResourceThreadImage::Load(const ResourceRequest& request)
{
  DALI_LOG_TRACE_METHOD( mLogFilter );
  DALI_LOG_INFO( mLogFilter, Debug::Verbose, "%s(%s)\n", __FUNCTION__, request.GetPath().c_str() );

  bool fileNotFound = false;
  BitmapPtr bitmap = 0;

  ImageType imageType = GetImageType(request.GetPath());
  switch(imageType)
  {
    case eResourceFromJava:
    {
      DALI_ASSERT_ALWAYS( 0 && "Loading bitmaps through Java is not implemented" );
      break;
    }

    default:
    {
      Dali::Vector<uint8_t> data;
      ImageLoader::LoadAsset(request.GetPath(), data);
      if(data.Size())
      {
        bool result = ImageLoader::ConvertStreamToBitmap( *request.GetType(), request.GetPath(), data, *this, bitmap);
        if (result && bitmap)
        {
          // Construct LoadedResource and ResourcePointer for image data
          LoadedResource resource( request.GetId(), request.GetType()->id, ResourcePointer( bitmap.Get() ) );
          // Queue the loaded resource
          mResourceLoader.AddLoadedResource( resource );
        }
        else
        {
          DALI_LOG_WARNING( "Unable to decode %s\n", request.GetPath().c_str() );
        }
      }
      else
      {
        DALI_LOG_WARNING( "Failed to open file to load \"%s\"\n", request.GetPath().c_str() );
        fileNotFound = true;
      }
      break;
    }
  }

  if ( !bitmap )
  {
    if( fileNotFound )
    {
      FailedResource resource(request.GetId(), FailureFileNotFound  );
      mResourceLoader.AddFailedLoad(resource);
    }
    else
    {
      FailedResource resource(request.GetId(), FailureUnknown);
      mResourceLoader.AddFailedLoad(resource);
    }
  }
}

void ResourceThreadImage::Decode(const ResourceRequest& request)
{
  DALI_LOG_TRACE_METHOD( mLogFilter );
  DALI_LOG_INFO(mLogFilter, Debug::Verbose, "%s(%s)\n", __FUNCTION__, request.GetPath().c_str());

  BitmapPtr bitmap = 0;

  // Get the blob of binary data that we need to decode:
  DALI_ASSERT_DEBUG( request.GetResource() );

  DALI_ASSERT_DEBUG( 0 != dynamic_cast<Dali::RefCountedVector<uint8_t>*>( request.GetResource().Get() ) && "Only blobs of binary data can be decoded." );
  Dali::RefCountedVector<uint8_t>* const encodedBlob = reinterpret_cast<Dali::RefCountedVector<uint8_t>*>( request.GetResource().Get() );

  if( encodedBlob != 0 )
  {
    const size_t blobSize     = encodedBlob->GetVector().Size();
    uint8_t * const blobBytes = encodedBlob->GetVector().Begin();
    DALI_ASSERT_DEBUG( blobSize > 0U );
    DALI_ASSERT_DEBUG( blobBytes != 0U );

    if( blobBytes != 0 && blobSize > 0U )
    {
      bool result = ImageLoader::ConvertStreamToBitmap(*request.GetType(), request.GetPath(), encodedBlob->GetVector(), *this, bitmap);
      if ( !result )
      {
        DALI_LOG_WARNING( "Unable to decode bitmap supplied as in-memory blob.\n" );
      }
    }
  }

  if( !bitmap )
  {
    FailedResource resource(request.GetId(), FailureUnknown);
    mResourceLoader.AddFailedLoad(resource);
  }
}



} // namespace SlpPlatform

} // namespace Dali
