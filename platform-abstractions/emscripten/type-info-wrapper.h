#ifndef __DALI_TYPE_INFO_WRAPPER_H__
#define __DALI_TYPE_INFO_WRAPPER_H__

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
#include <dali/public-api/dali-core.h>

#include "emscripten/emscripten.h"
#include "emscripten/bind.h"
#include <vector>
#include <string>

// INTERNAL INCLUDES

namespace Dali
{
namespace Internal
{
namespace Emscripten
{


//
// TypeInfoWrapper
//
class TypeInfoWrapper
{
public:
  TypeInfoWrapper() {};
  explicit TypeInfoWrapper(Dali::TypeInfo typeInfo);

  std::vector<std::string> GetProperties() const;

  std::vector<std::string> GetSignals() const;

  std::vector<std::string> GetActions() const;

  std::vector<int> GetPropertyIndices() const;

  std::string GetName() const;

  std::string GetBaseName() const;

private:
  Dali::TypeInfo mInfo;
};


}; // namespace Emscripten
}; // namespace Internal
}; // namespace Dali

#endif // header
