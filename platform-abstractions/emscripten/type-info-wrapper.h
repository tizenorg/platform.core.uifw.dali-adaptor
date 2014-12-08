#ifndef __DALI_TYPE_INFO_WRAPPER_H__
#define __DALI_TYPE_INFO_WRAPPER_H__

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
