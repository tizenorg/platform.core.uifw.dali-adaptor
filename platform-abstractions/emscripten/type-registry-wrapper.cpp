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
