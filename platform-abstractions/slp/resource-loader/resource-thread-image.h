#ifndef __DALI_SLP_PLATFORM_RESOURCE_THREAD_IMAGE_H__
#define __DALI_SLP_PLATFORM_RESOURCE_THREAD_IMAGE_H__

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

#include <dali/integration-api/resource-cache.h>

#include "resource-thread-base.h"

namespace Dali
{

namespace Integration
{
namespace Log
{
class Filter;
}
}

namespace SlpPlatform
{

class ResourceThreadImage : public ResourceThreadBase
{
public:
  /**
   * Constructor
   * @param[in] resourceLoader A reference to the ResourceLoader
   */
  ResourceThreadImage(ResourceLoader& resourceLoader);

  /**
   * Destructor
   */
  virtual ~ResourceThreadImage();

  /**
   * @copydoc ResourceLoader::LoadImageMetadata()
   * Note, this is not threaded, but is called synchronously.
   */
  void LoadImageMetadata(const std::string fileName, Vector2 &size);

private:
  /**
   * @copydoc ResourceThreadBase::Load
   */
  virtual void Load(const Integration::ResourceRequest& request);

  /**
   *@copydoc ResourceThreadBase::Save
   */
  virtual void Save(const Integration::ResourceRequest& request);

private:

}; // class ResourceThreadImage

} // namespace SlpPlatform

} // namespace Dali

#endif // __DALI_SLP_PLATFORM_RESOURCE_THREAD_IMAGE_H__
