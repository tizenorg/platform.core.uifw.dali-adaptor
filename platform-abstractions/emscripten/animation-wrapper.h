#ifndef __DALI_ANIMATION_WRAPPER_H__
#define __DALI_ANIMATION_WRAPPER_H__

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
