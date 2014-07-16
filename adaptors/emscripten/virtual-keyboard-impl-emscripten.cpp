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

// CLASS HEADER
#include "virtual-keyboard-impl.h"

// EXTERNAL INCLUDES
#include <algorithm>

#include <dali/public-api/common/vector-wrapper.h>
#include <adaptor.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <locale-utils.h>
#include <imf-manager-impl.h>


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
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::Verbose, false, "LOG_VIRTUAL_KEYBOARD");
#endif

// Signals
Dali::VirtualKeyboard::StatusSignalV2 gKeyboardStatusSignalV2;
Dali::VirtualKeyboard::VoidSignalV2   gKeyboardResizeSignalV2;
Dali::VirtualKeyboard::VoidSignalV2   gKeyboardLanguageChangedSignalV2;

Dali::VirtualKeyboard::ReturnKeyType gReturnKeyType = Dali::VirtualKeyboard::DEFAULT;  // the currently used return key type.

} // unnamed namespace

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

void RotateTo(int angle)
{
}

void SetReturnKeyType( Dali::VirtualKeyboard::ReturnKeyType type )
{
  if ( Dali::Adaptor::IsAvailable() )
  {
    Dali::ImfManager imfManager = ImfManager::Get();

    DALI_LOG_INFO( gLogFilter, Debug::General, "VKB Retrun key type is changed[%d]\n", type );

    gReturnKeyType = type;
  }
}

Dali::VirtualKeyboard::ReturnKeyType GetReturnKeyType()
{
  return gReturnKeyType;
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
  int xPos, yPos, width, height;

  width = height = xPos = yPos = 0;

  return Rect<int>(xPos,yPos,width,height);
}

Dali::VirtualKeyboard::StatusSignalV2& StatusChangedSignal()
{
  return gKeyboardStatusSignalV2;
}

Dali::VirtualKeyboard::VoidSignalV2& ResizedSignal()
{
  return gKeyboardResizeSignalV2;
}

Dali::VirtualKeyboard::VoidSignalV2& LanguageChangedSignal()
{
  return gKeyboardLanguageChangedSignalV2;
}

Dali::VirtualKeyboard::TextDirection GetTextDirection()
{
  Dali::VirtualKeyboard::TextDirection direction ( Dali::VirtualKeyboard::LeftToRight );

  return direction;
}

} // namespace VirtualKeyboard

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
