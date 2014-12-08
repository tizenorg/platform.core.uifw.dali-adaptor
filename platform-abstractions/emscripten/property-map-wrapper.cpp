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

#include "property-map-wrapper.h"

// EXTERNAL INCLUDES

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
