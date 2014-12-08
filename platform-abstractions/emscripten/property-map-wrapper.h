#ifndef __DALI_PROPERTY_MAP_WRAPPER_H__
#define __DALI_PROPERTY_MAP_WRAPPER_H__

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
