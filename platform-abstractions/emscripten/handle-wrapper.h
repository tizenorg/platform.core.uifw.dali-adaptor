#ifndef __DALI_HANDLE_WRAPPER_H__
#define __DALI_HANDLE_WRAPPER_H__

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
