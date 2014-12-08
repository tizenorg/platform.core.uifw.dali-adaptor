#ifndef __DALI_HANDLE_WRAPPER_H__
#define __DALI_HANDLE_WRAPPER_H__

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
#include "type-info-wrapper.h"


namespace Dali
{
namespace Internal
{
namespace Emscripten
{

//
// HandleWrapper
//
class HandleWrapper
{
public:
  HandleWrapper() {};
  explicit HandleWrapper(Dali::Handle handle);
  ~HandleWrapper();

  bool Ok();

  void SetProperty(const std::string& name, const Dali::Property::Value& value);

  Dali::Property::Value GetProperty(const std::string& name) const;

  std::string GetPropertyType(const std::string& name) const;

  int GetPropertyIndex(const std::string& name) const;

  std::string GetPropertyName(int index) const;

  Dali::Handle GetHandle() { return mHandle ; }

  TypeInfoWrapper GetTypeInfo() const;

  int GetPropertyCount() const;

  std::vector<int> GetPropertyIndices() const;

  std::vector<std::string> GetProperties() const;

  Dali::Property::Index RegisterProperty(const std::string& name, const Dali::Property::Value& propertyValue);

  Dali::Property::Index RegisterAnimatedProperty(const std::string& name, const Dali::Property::Value& propertyValue);

  bool IsPropertyAnimatable(int index) const;

  bool IsPropertyWritable(Dali::Property::Index index) const;

  bool IsPropertyAConstraintInput(Dali::Property::Index index) const;

public:
  HandleWrapper( const HandleWrapper&o ) { mHandle = o.mHandle ; };
  HandleWrapper& operator=( const HandleWrapper&o ) { mHandle = o.mHandle; return *this; };

protected:
  Dali::Handle mHandle;
};

}; // namespace Emscripten
}; // namespace Internal
}; // namespace Dali

#endif // header
