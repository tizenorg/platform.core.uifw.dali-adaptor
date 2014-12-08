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

// #include "animation-wrapper.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/scripting/scripting.h>

// INTERNAL INCLUDES
#include "property-value-wrapper.h"

namespace
{

Dali::Scripting::StringEnum AlphaFunctionTable[] =
  { { "Default",        Dali::AlphaFunction::LINEAR},
    { "Linear",         Dali::AlphaFunction::LINEAR},
    {"EaseInSquare",    Dali::AlphaFunction::EASE_IN_SQUARE},
    {"Reverse",         Dali::AlphaFunction::REVERSE},
    {"EaseIn",          Dali::AlphaFunction::EASE_IN},
    {"EaseOut",         Dali::AlphaFunction::EASE_OUT},
    {"EaseInOut",       Dali::AlphaFunction::EASE_IN_OUT}
  };

const unsigned int AlphaFunctionTableSize = sizeof( AlphaFunctionTable ) / sizeof( AlphaFunctionTable[0] );

Dali::Scripting::StringEnum AnimationTable[] =
  { { "Bake",        Dali::Animation::Bake},
    { "Discard",     Dali::Animation::Discard},
    { "BakeFinal",   Dali::Animation::BakeFinal}
  };

const unsigned int AnimationTableSize = sizeof( AlphaFunctionTable ) / sizeof( AlphaFunctionTable[0] );

Dali::AlphaFunction GetAlphaFunction(const std::string& name)
{
  Dali::AlphaFunction::BuiltinFunction builtin = Dali::AlphaFunction::LINEAR;

  (void)Dali::Scripting::GetEnumeration< Dali::AlphaFunction::BuiltinFunction >( name.c_str(), AlphaFunctionTable, AlphaFunctionTableSize, builtin );

  return Dali::AlphaFunction(builtin);
}

Dali::Animation::EndAction GetEndAction(const std::string& action)
{
  Dali::Animation::EndAction end = Dali::Animation::Discard;

  (void)Dali::Scripting::GetEnumeration< Dali::Animation::EndAction >( action.c_str(), AnimationTable, AnimationTableSize, end );

  return Dali::Animation::EndAction(end);
}

}; // anon namespace

namespace Dali
{
namespace Internal
{
namespace Emscripten
{

void AnimateTo(Dali::Animation self,
               Dali::Handle handle,
               const std::string& property,
               Dali::Property::Value destinationValue,
               const std::string& alphaFunction,
               float delay, float duration)
{
  DALI_ASSERT_ALWAYS(self);

  Dali::AlphaFunction func = GetAlphaFunction(alphaFunction);

  // we try the string as given (which may be true especially for camelCase shader uniforms)
  Dali::Property::Index propertyIndex = handle.GetPropertyIndex(property);

  if( propertyIndex == Dali::Property::INVALID_INDEX )
  {
    // alternatively we'll try with the usual camel case to hyphenated conversion
    propertyIndex = handle.GetPropertyIndex( Dali::Internal::Emscripten::JavaScriptNameToPropertyName(property) );
  }

  Dali::Property::Type propertyType = handle.GetPropertyType(propertyIndex);

  if( propertyType != destinationValue.GetType() )
  {
    EM_ASM( throw "animateTo property types are not the same" );
  }
  else if( propertyIndex != Dali::Property::INVALID_INDEX )
  {
    Dali::Property target( handle, propertyIndex );
    self.AnimateTo( target, destinationValue, func, Dali::TimePeriod(delay,duration) );
  }
  else
  {
    EM_ASM( throw "unknown property" );
  }

}

void AnimateBy(Dali::Animation self,
               Dali::Handle handle,
               const std::string& property,
               Dali::Property::Value destinationValue,
               const std::string& alphaFunction,
               float delay, float duration)
{
  DALI_ASSERT_ALWAYS(self);

  Dali::AlphaFunction func = GetAlphaFunction(alphaFunction);

  Dali::Property::Index propertyIndex = handle.GetPropertyIndex(JavaScriptNameToPropertyName(property));

  Dali::Property::Type propertyType = handle.GetPropertyType(propertyIndex);

  if( propertyType != destinationValue.GetType() )
  {
    EM_ASM( throw "animateTo property types are not the same" );
  }
  else if( propertyIndex != Dali::Property::INVALID_INDEX )
  {
    Dali::Property target( handle, propertyIndex );
    self.AnimateBy( target, destinationValue, func, Dali::TimePeriod(delay,duration));
  }
  else
  {
    EM_ASM( throw "unknown property" );
  }

}

void AnimatePath(Dali::Animation self,
                 Dali::Handle target,
                 Dali::Path path,
                 Dali::Vector3& forward,
                 const std::string& alphaFunction,
                 float delay, float duration)
{
  DALI_ASSERT_ALWAYS(self);

  Dali::AlphaFunction func = GetAlphaFunction(alphaFunction);

  if(!path)
  {
    printf("Unable to add animation, bad path object\n");
  }
  else
  {
    Dali::Actor actor = Dali::Actor::DownCast(target);
    if( !actor )
    {
      printf("Unable to add path animation, bad actor\n");
    }
    else
    {
      self.Animate( actor, path, forward, func, Dali::TimePeriod( delay, duration ) );
    }
  }
}


}; // namespace Emscripten
}; // namespace Internal
}; // namespace Dali
