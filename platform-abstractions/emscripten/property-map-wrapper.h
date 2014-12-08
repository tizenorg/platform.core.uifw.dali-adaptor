#ifndef __DALI_PROPERTY_MAP_WRAPPER_H__
#define __DALI_PROPERTY_MAP_WRAPPER_H__

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

// INTERNAL INCLUDES

namespace Dali
{
namespace Internal
{
namespace Emscripten
{

//
// PropertyMapWrapper
//
class PropertyMapWrapper
{
public:
  PropertyMapWrapper();
  explicit PropertyMapWrapper(Dali::Property::Value* notOwnedValue);
  ~PropertyMapWrapper();

  void Add(const std::string& key, Dali::Property::Value& value);

  void Append(Dali::Property::Value& value);

  PropertyMapWrapper AddMap(const std::string& key);

  PropertyMapWrapper AddArray(const std::string& key);

private:
  bool mOwner;
  Dali::Property::Value *mValue;

};

}; // namespace Emscripten
}; // namespace Internal
}; // namespace Dali

#endif // header
