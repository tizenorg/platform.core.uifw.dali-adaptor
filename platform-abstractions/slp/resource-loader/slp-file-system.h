#ifndef __DALI_SLP_FILESYSTEM_H__
#define __DALI_SLP_FILESYSTEM_H__

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

#include <stddef.h>        // for size_t
#include <dali/public-api/common/dali-vector.h>
#include <dali/integration-api/file-abstraction.h>

namespace Dali
{

namespace SlpPlatform
{

  // Its an instantiable class, so it can have a state.
  // Use case : android, resource-connector.
  // Todo: this must be a base class...

  class FileSystem : public Integration::FileSystemBase
  {
  public:
    size_t  GetFilesize(const std::string& filename);
    bool    Exists(const std::string& filename);

    // return = errorcode, 0 ok, else ... whatever.
    Integration::File*   MapToMemory(const uint8_t* data, size_t size);
    Integration::File*   OpenFile(const std::string& name);
  };

} // namespace SlpPlatform

} // namespace Dali

#endif // __DALI_SLP_PLATFORM_LOADER_FONT_H__
