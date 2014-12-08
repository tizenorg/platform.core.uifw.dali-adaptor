#ifndef __DALI_ANIMATION_WRAPPER_H__
#define __DALI_ANIMATION_WRAPPER_H__

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

// EXTERNAL INCLUDES
#include <dali/public-api/dali-core.h>
#include "emscripten/emscripten.h"
#include "emscripten/bind.h"

// INTERNAL INCLUDES
#include "handle-wrapper.h"

namespace Dali
{
namespace Internal
{
namespace Emscripten
{

//
// AnimationWrapper
//
class AnimationWrapper
{
public:
  AnimationWrapper();
  AnimationWrapper(const AnimationWrapper& copy)
    : mAnimation(copy.mAnimation) {};

  ~AnimationWrapper();

  void SetDuration(float seconds);

  void SetLooping(bool looping);

  void SetEndAction(const std::string& action);

  void SetDisconnectAction(const std::string& action);

  void SetCurrentProgress( float progress );

  void SetSpeedFactor( float factor );

  void SetPlayRange( const Dali::Vector2& range );

  float GetCurrentProgress();

  void Play();

  void Pause();

  void Stop();

  void Clear();

  void AnimateTo(HandleWrapper handleWrapper,
                 const std::string& property,
                 Dali::Property::Value destinationValue,
                 const std::string& alphaFunction,
                 float delay, float duration);

  void AnimateBy(HandleWrapper handleWrapper,
                 const std::string& property,
                 Dali::Property::Value destinationValue,
                 const std::string& alphaFunction,
                 float delay, float duration);

  void AnimatePath(HandleWrapper targetHandle,
                   HandleWrapper pathHandle,
                   Dali::Vector3& forward,
                   const std::string& alphaFunction,
                   float delay, float duration);

private:
  const AnimationWrapper& operator=(const AnimationWrapper& assign) = delete;

  Dali::Animation mAnimation;
};


}; // namespace Emscripten
}; // namespace Internal
}; // namespace Dali

#endif // header
