#ifndef __DALI_HANDLE_WRAPPER_H__
#define __DALI_HANDLE_WRAPPER_H__

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

// EXTERNAL INCLUDES
#include <dali/public-api/dali-core.h>

#include "emscripten/emscripten.h"
#include "emscripten/bind.h"

// INTERNAL INCLUDES
#include "type-info-wrapper.h"


namespace Dali
{
namespace Internal
{
namespace Emscripten
{

bool HandleOk(Dali::Handle& self);

void SetProperty(Dali::Handle& self, const std::string& javascriptName, const Dali::Property::Value& value);

Dali::Property::Value GetProperty(Dali::Handle& self, const std::string& javascriptName);

int GetPropertyIndex(Dali::Handle& self, const std::string& javascriptName);

std::vector<std::string> GetProperties(Dali::Handle& self);

std::string GetPropertyType(Dali::Handle& self, const std::string& name);

Dali::Property::Index RegisterProperty(Dali::Handle& self, const std::string& name, const Dali::Property::Value& propertyValue);

Dali::Property::Index RegisterAnimatedProperty(Dali::Handle& self, const std::string& name, const Dali::Property::Value& propertyValue);

Dali::TypeInfo GetTypeInfo(Dali::Handle& self);

}; // namespace Emscripten
}; // namespace Internal
}; // namespace Dali

#endif // header
