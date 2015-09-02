/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

// HEADER
#include "virtual-keyboard-impl.h"


// EXTERNAL INCLUDES
namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace VirtualKeyboard
{

namespace
{

void Show()
{
}

void Hide()
{

}

bool IsVisible()
{
  return false;
}

void ApplySettings( const Property::Map& settingsMap )
{
}

void EnablePrediction(const bool enable)
{

}

bool IsPredictionEnabled()
{
  return false;
}

Rect<int> GetSizeAndPosition()
{
  return Rect<int>(0,0,0,0);
}

Dali::VirtualKeyboard::StatusSignalType& StatusChangedSignal()
{
  static Dali::VirtualKeyboard::StatusSignalType dummySignal;
  return &dummySignal;
}

Dali::VirtualKeyboard::VoidSignalType& ResizedSignal()
{
  static Dali::VirtualKeyboard::VoidSignalType dummySignal;
  return &dummySignal;
}

Dali::VirtualKeyboard::VoidSignalType& LanguageChangedSignal()
{
  static Dali::VirtualKeyboard::VoidSignalType dummySignal;
  return &dummySignal;
}

Dali::VirtualKeyboard::TextDirection GetTextDirection()
{
  return  Dali::VirtualKeyboard::LeftToRight ;
}

} // namespace VirtualKeyboard

} // namespace Adaptor

} // namespace Internal

} // namespace Dali