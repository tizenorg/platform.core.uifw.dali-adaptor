//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "resource-bitmap-requester.h"
#include <dali/integration-api/resource-cache.h>

using namespace Dali::Integration;

namespace Dali
{
namespace SlpPlatform
{

ResourceBitmapRequester::ResourceBitmapRequester( ResourceLoader& resourceLoader )
: ResourceRequesterBase( resourceLoader )
{
  mThreadImage  = new ResourceThreadImage( resourceLoader );
  mThreadDistanceField = new ResourceThreadDistanceField( resourceLoader );
}

ResourceBitmapRequester::~ResourceBitmapRequester()
{
  delete mThreadImage;
  delete mThreadDistanceField;
}

void ResourceBitmapRequester::Pause()
{
  mThreadImage->Pause();
  mThreadDistanceField->Pause();
}

void ResourceBitmapRequester::Resume()
{
  mThreadImage->Resume();
  mThreadDistanceField->Resume();
}

void ResourceBitmapRequester::LoadResource( Integration::ResourceRequest& request )
{
  BitmapResourceType* resType = dynamic_cast<BitmapResourceType*>(request.GetType());
  if( resType )
  {
    ImageAttributes& attributes  = resType->imageAttributes;
    if (!attributes.IsDistanceField())
    {
      mThreadImage->AddRequest(request, ResourceThreadBase::RequestLoad);
    }
    else
    {
      mThreadDistanceField->AddRequest(request, ResourceThreadBase::RequestLoad);
    }
  }
}

Integration::LoadStatus ResourceBitmapRequester::LoadFurtherResources( Integration::ResourceRequest& request, LoadedResource partialResource )
{
  // Nothing to do
  return RESOURCE_COMPLETELY_LOADED;
}

void ResourceBitmapRequester::SaveResource(const Integration::ResourceRequest& request )
{
  // Nothing to do
}

void ResourceBitmapRequester::CancelLoad(Integration::ResourceId id, Integration::ResourceTypeId typeId)
{
  mThreadImage->CancelRequest(id);
  mThreadDistanceField->CancelRequest(id);
}

void ResourceBitmapRequester::LoadImageMetadata(const std::string fileName, Vector2 &size)
{
  mThreadImage->LoadImageMetadata(fileName, size);
}


} // SlpPlatform
} // Dali
