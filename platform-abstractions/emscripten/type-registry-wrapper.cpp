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

#include "type-registry-wrapper.h"

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include "type-info-wrapper.h"

namespace Dali
{
namespace Internal
{
namespace Emscripten
{

//
// TypeRegistryWrapper
//
TypeRegistryWrapper::TypeRegistryWrapper()
{
}


Dali::TypeRegistry::NameContainer TypeRegistryWrapper::GetTypeNames()
{
  return Dali::TypeRegistry::Get().GetTypeNames();
}

TypeInfoWrapper TypeRegistryWrapper::GetTypeInfo(const std::string& typeName)
{
  Dali::TypeInfo info = Dali::TypeRegistry::Get().GetTypeInfo(typeName);
  return TypeInfoWrapper( info );
}

}; // namespace Emscripten
}; // namespace Internal
}; // namespace Dali
