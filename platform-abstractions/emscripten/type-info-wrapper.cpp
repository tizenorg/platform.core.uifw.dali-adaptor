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

#include "type-info-wrapper.h"

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include "emscripten-utils.h"

namespace Dali
{
namespace Internal
{
namespace Emscripten
{

TypeInfoWrapper::TypeInfoWrapper(Dali::TypeInfo typeInfo)
  :mInfo(typeInfo)
{
}

std::vector<int> TypeInfoWrapper::GetPropertyIndices() const
{
  Dali::Property::IndexContainer indices;
  mInfo.GetPropertyIndices( indices );

  std::vector<int> ret( indices.begin(), indices.end() );
  return ret;
}

std::string TypeInfoWrapper::GetName() const
{
  return mInfo.GetName();
}

std::string TypeInfoWrapper::GetBaseName() const
{
  return mInfo.GetBaseName();
}

std::vector<std::string> TypeInfoWrapper::GetProperties() const
{
  std::vector<std::string> names;

  // get the other properties
  if(Dali::Handle handle = Dali::Handle::DownCast( mInfo.CreateInstance() ) )
  {
    typedef Dali::Property::IndexContainer IndexContainer;

    Dali::Property::IndexContainer indices;
    handle.GetPropertyIndices( indices );

    for(IndexContainer::iterator iter(indices.begin()); iter != indices.end(); ++iter)
    {
      std::string name = PropertyNameToJavaScriptName( handle.GetPropertyName( *iter ) );

      names.push_back(name);
    }
  }
  else
  {
    // all we can do is get the event side properties
    // get the event side properties
    mInfo.GetProperties(names);
  }

  return names;
}

std::vector<std::string> TypeInfoWrapper::GetSignals() const
{
  std::vector<std::string> names;
  mInfo.GetSignals(names);
  return names;
}

std::vector<std::string> TypeInfoWrapper::GetActions() const
{
  std::vector<std::string> names;
  mInfo.GetActions(names);
  return names;
}

}; // namespace Emscripten
}; // namespace Internal
}; // namespace Dali
