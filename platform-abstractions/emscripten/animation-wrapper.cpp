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

Dali::AlphaFunction GetAlphaFunction(const std::string& name)
{
  Dali::AlphaFunction func = Dali::AlphaFunction::LINEAR;

  if( Dali::Scripting::CompareEnums( name.c_str(), "Default") )
  {
    func = Dali::AlphaFunction::LINEAR;
  }
  else if( Dali::Scripting::CompareEnums( name.c_str(), "Linear") )
  {
    func = Dali::AlphaFunction::LINEAR;
  }
  else if( Dali::Scripting::CompareEnums( name.c_str(), "EaseInSquare") )
  {
    func = Dali::AlphaFunction::EASE_IN_SQUARE;
  }
  else if( Dali::Scripting::CompareEnums( name.c_str(), "Reverse") )
  {
    func = Dali::AlphaFunction::REVERSE;
  }
  else if( Dali::Scripting::CompareEnums( name.c_str(), "EaseIn") )
  {
    func = Dali::AlphaFunction::EASE_IN;
  }
  else if( Dali::Scripting::CompareEnums( name.c_str(), "EaseOut") )
  {
    func = Dali::AlphaFunction::EASE_OUT;
  }
  else if( Dali::Scripting::CompareEnums( name.c_str(), "EaseInOut") )
  {
    func = Dali::AlphaFunction::EASE_IN_OUT;
  }
  else if( Dali::Scripting::CompareEnums( name.c_str(), "EaseInSine") )
  {
    func = Dali::AlphaFunction::EASE_IN_SINE;
  }
  else if( Dali::Scripting::CompareEnums( name.c_str(), "EaseOutSine") )
  {
    func = Dali::AlphaFunction::EASE_OUT_SINE;
  }
  else if( Dali::Scripting::CompareEnums( name.c_str(), "EaseInOutSine") )
  {
    func = Dali::AlphaFunction::EASE_IN_OUT_SINE;
  }
  // else if( Dali::Scripting::CompareEnums( name.c_str(), "EaseInSine33") )
  // {
  //   func = Dali::AlphaFunction::EASE_IN_SINE33;
  // }
  // else if( Dali::Scripting::CompareEnums( name.c_str(), "EaseOutSine33") )
  // {
  //   func = Dali::AlphaFunction::EASE_OUT_SINE33;
  // }
  // else if( Dali::Scripting::CompareEnums( name.c_str(), "EaseInOutSine33") )
  // {
  //   func = Dali::AlphaFunction::EASE_IN_OUT_SINE33;
  // }
  // else if( Dali::Scripting::CompareEnums( name.c_str(), "EaseInOutSine50") )
  // {
  //   func = Dali::AlphaFunction::EASE_IN_OUT_SINE50;
  // }
  // else if( Dali::Scripting::CompareEnums( name.c_str(), "EaseInOutSine60") )
  // {
  //   func = Dali::AlphaFunction::EASE_IN_OUT_SINE60;
  // }
  // else if( Dali::Scripting::CompareEnums( name.c_str(), "EaseInOutSine70") )
  // {
  //   func = Dali::AlphaFunction::EASE_IN_OUT_SINE70;
  // }
  // else if( Dali::Scripting::CompareEnums( name.c_str(), "EaseInOutSine80") )
  // {
  //   func = Dali::AlphaFunction::EASE_IN_OUT_SINE80;
  // }
  // else if( Dali::Scripting::CompareEnums( name.c_str(), "EaseInOutSine90") )
  // {
  //   func = Dali::AlphaFunction::EASE_IN_OUT_SINE90;
  // }
  // else if( Dali::Scripting::CompareEnums( name.c_str(), "DoubleEaseInOutSine60") )
  // {
  //   func = Dali::AlphaFunction::DOUBLE_EASE_IN_OUT_SINE60;
  // }
  // else if( Dali::Scripting::CompareEnums( name.c_str(), "EaseOutQuint50") )
  // {
  //   func = Dali::AlphaFunctions::EaseOutQuint50;
  // }
  // else if( Dali::Scripting::CompareEnums( name.c_str(), "EaseOutQuint80") )
  // {
  //   func = Dali::AlphaFunctions::EaseOutQuint80;
  // }
  else if( Dali::Scripting::CompareEnums( name.c_str(), "Bounce") )
  {
    func = Dali::AlphaFunction::BOUNCE;
  }
  // else if( Dali::Scripting::CompareEnums( name.c_str(), "BounceBack") )
  // {
  //   func = Dali::AlphaFunctions::BounceBack;
  // }
  // else if( Dali::Scripting::CompareEnums( name.c_str(), "EaseInBack") )
  // {
  //   func = Dali::AlphaFunctions::EaseInBack;
  // }
  else if( Dali::Scripting::CompareEnums( name.c_str(), "EaseOutBack") )
  {
    func = Dali::AlphaFunction::EASE_OUT_BACK;
  }
  // else if( Dali::Scripting::CompareEnums( name.c_str(), "EaseInOutBack") )
  // {
  //   func = Dali::AlphaFunctions::EaseInOutBack;
  // }
  else if( Dali::Scripting::CompareEnums( name.c_str(), "Sin") )
  {
    func = Dali::AlphaFunction::SIN;
  }
 //  else if( Dali::Scripting::CompareEnums( name.c_str(), "Sin2x") )
 //  {
 //    func = Dali::AlphaFunctions::Sin2x;
 // }
  return func;
}

Dali::Animation::EndAction GetEndAction(const std::string& action)
{
  Dali::Animation::EndAction end = Dali::Animation::Discard;

  if( Dali::Scripting::CompareEnums( action.c_str(), "Bake" ) )
  {
    end = Dali::Animation::Bake;
  }
  else if( Dali::Scripting::CompareEnums( action.c_str(), "Discard" ) )
  {
    end = Dali::Animation::Discard;
  }
  else if( Dali::Scripting::CompareEnums( action.c_str(), "BakeFinal" ) )
  {
    end = Dali::Animation::BakeFinal;
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
