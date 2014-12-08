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

#include "property-map-wrapper.h"

// EXTERNAL INCLUDES

// INTERNAL INCLUDES

namespace Dali
{
namespace Internal
{
namespace Emscripten
{

PropertyMapWrapper::PropertyMapWrapper()
  : mOwner(true)
{
  mValue = new Dali::Property::Value(Dali::Property::MAP);
}

PropertyMapWrapper::PropertyMapWrapper(Dali::Property::Value* notOwnedValue)
  : mOwner(false)
{
  mValue = notOwnedValue;
}

PropertyMapWrapper::~PropertyMapWrapper()
{
  if(mOwner)
  {
    delete mValue;
  }
}

void PropertyMapWrapper::Add(const std::string& key, Dali::Property::Value& value)
{
  assert(mValue);
  assert(Dali::Property::MAP == mValue->GetType());
  mValue->SetValue(key, value);
}

void PropertyMapWrapper::Append(Dali::Property::Value& value)
{
  assert(mValue);
  assert(Dali::Property::ARRAY == mValue->GetType());
  mValue->AppendItem(value);
}

PropertyMapWrapper PropertyMapWrapper::AddMap(const std::string& key)
{
  assert(mValue);
  assert(Dali::Property::MAP == mValue->GetType());
  mValue->SetValue(key, Dali::Property::Value(Dali::Property::MAP));
  return PropertyMapWrapper(&mValue->GetValue(key));
}

PropertyMapWrapper PropertyMapWrapper::AddArray(const std::string& key)
{
  assert(mValue);
  assert(Dali::Property::MAP == mValue->GetType());
  mValue->SetValue(key, Dali::Property::Value(Dali::Property::ARRAY));
  return PropertyMapWrapper(&mValue->GetValue(key));
}


}; // namespace Emscripten
}; // namespace Internal
}; // namespace Dali
