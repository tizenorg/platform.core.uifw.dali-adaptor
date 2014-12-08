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

#include "handle-wrapper.h"

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include "type-info-wrapper.h"
#include "emscripten-utils.h"

namespace Dali
{
namespace Internal
{
namespace Emscripten
{

//
// HandleWrapper
//
HandleWrapper::HandleWrapper(Dali::Handle handle)
  : mHandle(handle)
{

}

HandleWrapper::~HandleWrapper()
{

}

bool HandleWrapper::Ok()
{
  if(mHandle)
  {
    return true;
  }
  else
  {
    return false;
  }
}

void HandleWrapper::SetProperty(const std::string& javascriptName, const Dali::Property::Value& value)
{
  DALI_ASSERT_ALWAYS(mHandle);
  if( mHandle )
  {
    // first try non converted name (for shader uniforms)
    Dali::Property::Index index = mHandle.GetPropertyIndex(javascriptName);
    if( Dali::Property::INVALID_INDEX == index )
    {
      index = mHandle.GetPropertyIndex(JavaScriptNameToPropertyName(javascriptName));
    }

    if( Dali::Property::INVALID_INDEX != index )
    {
      mHandle.SetProperty(index, value);
    }
    else
    {
      printf("ERR Invalid property name:%s", javascriptName.c_str());
      EM_ASM( throw "Invalid property name (HandleWrapper::SetProperty)" );
    }
  }
  else
  {
    EM_ASM( throw "ActorWrapper has no actor" );
  }

}

Dali::Property::Value HandleWrapper::GetProperty(const std::string& javascriptName) const
{
  DALI_ASSERT_ALWAYS(mHandle);
  Dali::Property::Value ret;
  if( mHandle )
  {
    // first try non converted name (for shader uniforms)
    Dali::Property::Index index = mHandle.GetPropertyIndex(javascriptName);
    if( Dali::Property::INVALID_INDEX == index )
    {
      index = mHandle.GetPropertyIndex(JavaScriptNameToPropertyName(javascriptName));
    }

    if( Dali::Property::INVALID_INDEX != index )
    {
      ret = mHandle.GetProperty(index);
      // printf("Get property(%d):%s=%s\n", (int)index, name.c_str(), PropertyValueToString( ret ).c_str());
    }
    else
    {
      printf("ERR Invalid property name:%s", javascriptName.c_str());
      EM_ASM( throw new Error("Invalid property name (HandleWrapper::GetProperty)") );
      EM_ASM( throw new Error("Invalid property name (HandleWrapper::GetProperty)") );
    }
  }
  else
  {
    EM_ASM( throw new Error("ActorWrapper has no actor") );
  }

  return ret;
}

int HandleWrapper::GetPropertyIndex(const std::string& javascriptName) const
{
  if( mHandle )
  {
    // first try non converted name (for shader uniforms)
    Dali::Property::Index index = mHandle.GetPropertyIndex(javascriptName);
    if( Dali::Property::INVALID_INDEX == index )
    {
      index = mHandle.GetPropertyIndex(JavaScriptNameToPropertyName(javascriptName));
    }

    return (int)index; // mHandle.GetPropertyIndex(name);
  }

  return -1;
}

std::string HandleWrapper::GetPropertyName(int index) const
{
  std::string ret;
  if(mHandle)
  {
    ret = mHandle.GetPropertyName(index);
  }
  return ret;
}

TypeInfoWrapper HandleWrapper::GetTypeInfo() const
{
  TypeInfoWrapper ret;
  if( mHandle )
  {
    Dali::TypeInfo info;
    if(mHandle.GetTypeInfo(info))
    {
      ret = TypeInfoWrapper( info );
    }
  }

  return ret;
}

int HandleWrapper::GetPropertyCount() const
{
  return mHandle.GetPropertyCount();
}

std::vector<int> HandleWrapper::GetPropertyIndices() const
{
  Dali::Property::IndexContainer indices;
  mHandle.GetPropertyIndices( indices );

  return std::vector<int>( indices.begin(), indices.end() );
}

std::vector<std::string> HandleWrapper::GetProperties() const
{
  Dali::Property::IndexContainer indices;
  mHandle.GetPropertyIndices( indices );
  std::vector<std::string> names;
  for(Dali::Property::IndexContainer::iterator iter(indices.begin()); iter != indices.end(); ++iter)
  {
    std::string name = PropertyNameToJavaScriptName( mHandle.GetPropertyName( *iter ) );

    names.push_back(name);
  }
  return names;
}

std::string HandleWrapper::GetPropertyType(const std::string& name) const
{
  if(mHandle)
  {
    Dali::Property::Index index = mHandle.GetPropertyIndex(name);
    if(Dali::Property::INVALID_INDEX != index)
    {
      return Dali::PropertyTypes::GetName(mHandle.GetPropertyType(index));
    }
  }

  // if we got here
  return Dali::PropertyTypes::GetName(Dali::Property::NONE);
}

Dali::Property::Index HandleWrapper::RegisterProperty(const std::string& name, const Dali::Property::Value& propertyValue)
{
  Dali::Property::Index ret = Dali::Property::INVALID_INDEX;

  Dali::Property::Type type = propertyValue.GetType();
  if(Dali::Property::ARRAY == type || Dali::Property::MAP == type)
  {
    // these types would need support in the javascript side of the wrapper
    EM_ASM( throw "Property type not supported" );
  }

  if(mHandle)
  {
    ret = mHandle.RegisterProperty(JavaScriptNameToPropertyName(name),
                                   propertyValue, Dali::Property::AccessMode::READ_WRITE);
  }
  return ret;
}

Dali::Property::Index HandleWrapper::RegisterAnimatedProperty(const std::string& name, const Dali::Property::Value& propertyValue)
{
  Dali::Property::Index ret = Dali::Property::INVALID_INDEX;

  Dali::Property::Type type = propertyValue.GetType();
  if(Dali::Property::ARRAY == type || Dali::Property::MAP == type)
  {
    // these types would need support in the javascript side of the wrapper
    EM_ASM( throw "Property type not supported" );
  }


  if(mHandle)
  {
    ret = mHandle.RegisterProperty(JavaScriptNameToPropertyName(name),
                                   propertyValue,  Dali::Property::AccessMode::ANIMATABLE);
  }
  return ret;
}

bool HandleWrapper::IsPropertyAnimatable(int index) const
{
  if(mHandle)
  {
    return mHandle.IsPropertyAnimatable(index);
  }
  else
  {
    return false;
  }
}

bool HandleWrapper::IsPropertyWritable(Dali::Property::Index index) const
{
  if(mHandle)
  {
    return mHandle.IsPropertyWritable(index);
  }
  else
  {
    return false;
  }
}

bool HandleWrapper::IsPropertyAConstraintInput(Dali::Property::Index index) const
{
  if(mHandle)
  {
    return mHandle.IsPropertyAConstraintInput(index);
  }
  else
  {
    return false;
  }
}

}; // namespace Emscripten
}; // namespace Internal
}; // namespace Dali
