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

#include "animation-wrapper.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/scripting/scripting.h>

// INTERNAL INCLUDES
#include "emscripten-utils.h"

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

Dali::Scripting::StringEnum AnmationTable[] =
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

  unsigned int index = FindEnumIndex( action.c_str(), AlphaFunctionTable, AlphaFunctionTableSize );

  if( index < AnimationTableSize )
  {
    end = static_cast<Dali::Animation::EndAction>(AlphaFunctionTable[index].value);
  }

  return end;
}

}; // anon namespace

namespace Dali
{
namespace Internal
{
namespace Emscripten
{

//
// AnimationWrapper
//
AnimationWrapper::AnimationWrapper()
{
  mAnimation = Dali::Animation::New(0);
}

AnimationWrapper::~AnimationWrapper()
{
}

void AnimationWrapper::SetDuration(float seconds)
{
  DALI_ASSERT_ALWAYS(mAnimation);
  mAnimation.SetDuration(seconds);
}

void AnimationWrapper::SetLooping(bool looping)
{
  DALI_ASSERT_ALWAYS(mAnimation);
  mAnimation.SetLooping(looping);
}

void AnimationWrapper::SetCurrentProgress( float progress )
{
  DALI_ASSERT_ALWAYS(mAnimation);
  mAnimation.SetCurrentProgress(progress);
}

float AnimationWrapper::GetCurrentProgress()
{
  DALI_ASSERT_ALWAYS(mAnimation);
  return mAnimation.GetCurrentProgress();
}

void AnimationWrapper::SetSpeedFactor( float factor )
{
  DALI_ASSERT_ALWAYS(mAnimation);
  mAnimation.SetSpeedFactor(factor);
}

void AnimationWrapper::SetPlayRange( const Dali::Vector2& range )
{
  DALI_ASSERT_ALWAYS(mAnimation);
  mAnimation.SetPlayRange(range);
}

void AnimationWrapper::Play()
{
  DALI_ASSERT_ALWAYS(mAnimation);
  mAnimation.Play();
}

void AnimationWrapper::Pause()
{
  DALI_ASSERT_ALWAYS(mAnimation);
  mAnimation.Pause();
}

void AnimationWrapper::Stop()
{
  DALI_ASSERT_ALWAYS(mAnimation);
  mAnimation.Stop();
}

void AnimationWrapper::Clear()
{
  DALI_ASSERT_ALWAYS(mAnimation);
  mAnimation.Clear();
}

void AnimationWrapper::AnimateTo(HandleWrapper handleWrapper,
                                 const std::string& property,
                                 Dali::Property::Value destinationValue,
                                 const std::string& alphaFunction,
                                 float delay, float duration)
{
  DALI_ASSERT_ALWAYS(mAnimation);

  Dali::AlphaFunction func = GetAlphaFunction(alphaFunction);

  Dali::Handle handle(handleWrapper.GetHandle());

  // we try the string as given (which may be true especially for camelCase shader uniforms)
  Dali::Property::Index propertyIndex = handle.GetPropertyIndex(property);

  if( propertyIndex == Dali::Property::INVALID_INDEX )
  {
    // alternatively we'll try with the usual camel case to hyphenated conversion
    propertyIndex = handle.GetPropertyIndex( Dali::Internal::Emscripten::JavaScriptNameToPropertyName(property) );
  }


  if( propertyIndex != Dali::Property::INVALID_INDEX )
  {
    Dali::Property target( handle, propertyIndex );
    mAnimation.AnimateTo( target, destinationValue, func, Dali::TimePeriod(delay,duration) );
  }
  else
  {
    printf("Unable to add animation, bad property:%s\n", property.c_str());
  }

}

void AnimationWrapper::AnimateBy(HandleWrapper handleWrapper,
                                 const std::string& property,
                                 Dali::Property::Value destinationValue,
                                 const std::string& alphaFunction,
                                 float delay, float duration)
{
  DALI_ASSERT_ALWAYS(mAnimation);

  Dali::AlphaFunction func = GetAlphaFunction(alphaFunction);

  Dali::Handle handle(handleWrapper.GetHandle());

  Dali::Property::Index propertyIndex = handle.GetPropertyIndex(JavaScriptNameToPropertyName(property));
  if( propertyIndex != Dali::Property::INVALID_INDEX )
  {
    Dali::Property target( handle, propertyIndex );
    mAnimation.AnimateBy( target, destinationValue, func, Dali::TimePeriod(delay,duration));
  }
  else
  {
    printf("Unable to add animation, bad property:%s\n", property.c_str());
  }
}

void AnimationWrapper::AnimatePath(HandleWrapper targetHandle,
                                   HandleWrapper pathHandle,
                                   Dali::Vector3& forward,
                                   const std::string& alphaFunction,
                                   float delay, float duration)
{
  DALI_ASSERT_ALWAYS(mAnimation);

  Dali::AlphaFunction func = GetAlphaFunction(alphaFunction);

  Dali::Path path = Dali::Path::DownCast(pathHandle.GetHandle());

  if(!path)
  {
    printf("Unable to add animation, bad path object\n");
  }
  else
  {
    Dali::Actor actor = Dali::Actor::DownCast(targetHandle.GetHandle());
    if( !actor )
    {
      printf("Unable to add path animation, bad actor\n");
    }
    else
    {
      mAnimation.Animate( actor, path, forward, func, Dali::TimePeriod( delay, duration ) );
    }
  }
}

void AnimationWrapper::SetEndAction(const std::string& action)
{
  mAnimation.SetEndAction( GetEndAction(action) );
}

void AnimationWrapper::SetDisconnectAction(const std::string& action)
{
  mAnimation.SetDisconnectAction( GetEndAction(action) );
}


}; // namespace Emscripten
}; // namespace Internal
}; // namespace Dali
