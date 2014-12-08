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
