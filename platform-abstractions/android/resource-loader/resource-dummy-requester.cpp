/*
Copyright (c) 2000-2013 Samsung Electronics Co., Ltd All Rights Reserved

This file is part of Dali

PROPRIETARY/CONFIDENTIAL

This software is the confidential and proprietary information of
SAMSUNG ELECTRONICS ("Confidential Information"). You shall not
disclose such Confidential Information and shall use it only in
accordance with the terms of the license agreement you entered
into with SAMSUNG ELECTRONICS.

SAMSUNG make no representations or warranties about the suitability
of the software, either express or implied, including but not limited
to the implied warranties of merchantability, fitness for a particular
purpose, or non-infringement. SAMSUNG shall not be liable for any
damages suffered by licensee as a result of using, modifying or
distributing this software or its derivatives.
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
