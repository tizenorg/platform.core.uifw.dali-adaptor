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

// CLASS HEADER
#include "resource-bitmap-requester.h"

// EXTERNAL INCLUDES
#include <dali/integration-api/resource-cache.h>

// INTERNAL INCLUDES

using namespace Dali::Integration;

namespace Dali
{
namespace SlpPlatform
{

ResourceBitmapRequester::ResourceBitmapRequester( ResourceLoader& resourceLoader )
: ResourceRequesterBase( resourceLoader )
{
  mThreadImage  = new ResourceThreadImage( resourceLoader );
}

ResourceBitmapRequester::~ResourceBitmapRequester()
{
  delete mThreadImage;
}

void ResourceBitmapRequester::Pause()
{
  mThreadImage->Pause();
}

void ResourceBitmapRequester::Resume()
{
  mThreadImage->Resume();
}

void ResourceBitmapRequester::LoadResource( Integration::ResourceRequest& request )
{
  DALI_ASSERT_DEBUG( (0 != dynamic_cast<BitmapResourceType*>(request.GetType())) && "Only requsts for bitmap resources can ever be routed to ResourceBitmapRequester.\n");
  BitmapResourceType* resType = static_cast<BitmapResourceType*>(request.GetType());
  if( resType )
  {
    // Work out if the resource is in memory or a file:
    const ResourceThreadBase::RequestType requestType = request.GetResource().Get() ? ResourceThreadBase::RequestDecode : ResourceThreadBase::RequestLoad;

    mThreadImage->AddRequest( request, requestType );
  }
}

Integration::LoadStatus ResourceBitmapRequester::LoadFurtherResources( Integration::ResourceRequest& request, LoadedResource partialResource )
{
  // Nothing to do
  return RESOURCE_COMPLETELY_LOADED;
}

void ResourceBitmapRequester::CancelLoad(Integration::ResourceId id, Integration::ResourceTypeId typeId)
{
  mThreadImage->CancelRequest(id);
}


} // SlpPlatform
} // Dali
