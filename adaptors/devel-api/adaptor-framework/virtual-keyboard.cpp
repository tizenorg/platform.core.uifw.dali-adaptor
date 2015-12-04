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

// CLASS HEADER
#include <virtual-keyboard.h>

// INTERNAL INCLUDES
#include <virtual-keyboard-impl.h>

namespace Dali
{

namespace VirtualKeyboard
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
  Internal::Adaptor::VirtualKeyboard::ApplySettings( settingsMap );
}

void SetReturnKeyType( const InputMethod::ActionButton type )
{
}

InputMethod::ActionButton  GetReturnKeyType()
{
  return DEFAULT;
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
  return Rect<int>();
}

void RotateTo(int angle)
{
}

TextDirection GetTextDirection()
{
  return VirtualKeyboard::LeftToRight;
}

} // namespace VirtualKeyboard

} // namespace Dali
