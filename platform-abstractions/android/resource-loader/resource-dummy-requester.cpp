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
 */

#include "resource-dummy-requester.h"
#include <dali/integration-api/resource-cache.h>

using namespace Dali::Integration;

namespace Dali
{
namespace SlpPlatform
{

ResourceDummyRequester::ResourceDummyRequester( ResourceLoader& resourceLoader )
: ResourceRequesterBase( resourceLoader )
{
}

ResourceDummyRequester::~ResourceDummyRequester()
{
}

void ResourceDummyRequester::LoadResource( Integration::ResourceRequest& request )
{
  //LoadedResource resource( request.GetId(), request.GetType()->id, ResourcePointer(NULL) );
  FailedResource resource(request.GetId(), FailureUnknown);
  mResourceLoader.AddFailedLoad(resource);
}

ResourcePointer ResourceDummyRequester::LoadResourceSynchronously( const Integration::ResourceType& type, const std::string& path )
{
  return NULL;
}

Integration::LoadStatus ResourceDummyRequester::LoadFurtherResources( Integration::ResourceRequest& request, LoadedResource partialResource )
{
  // Nothing to do
  return RESOURCE_COMPLETELY_LOADED;
}

void ResourceDummyRequester::SaveResource(const Integration::ResourceRequest& request )
{
  // Nothing to do
}

void ResourceDummyRequester::CancelLoad(Integration::ResourceId id, Integration::ResourceTypeId typeId)
{
  // Nothing to do
}

void ResourceDummyRequester::LoadImageMetadata(const std::string fileName, Vector2 &size)
{
  // Nothing to do
}

void ResourceDummyRequester::Pause()
{
}

void ResourceDummyRequester::Resume()
{
}


} // SlpPlatform
} // Dali
