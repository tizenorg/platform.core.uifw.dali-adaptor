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
#include <accessibility-manager.h>

// INTERNAL INCLUDES
#include <accessibility-manager-impl.h>

namespace Dali
{

AccessibilityManager::AccessibilityManager()
{
}

AccessibilityManager AccessibilityManager::Get()
{
  return Internal::Adaptor::AccessibilityManager::Get();
}

AccessibilityManager::~AccessibilityManager()
{
}

Vector2 AccessibilityManager::GetReadPosition() const
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).GetReadPosition();
}

bool AccessibilityManager::IsEnabled() const
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).IsEnabled();
}

void AccessibilityManager::SetActionHandler(AccessibilityActionHandler& handler)
{
  Internal::Adaptor::AccessibilityManager::GetImplementation(*this).SetActionHandler(handler);
}

void AccessibilityManager::SetGestureHandler(AccessibilityGestureHandler& handler)
{
  Internal::Adaptor::AccessibilityManager::GetImplementation(*this).SetGestureHandler(handler);
}

bool AccessibilityManager::HandleActionNextEvent(bool allowEndFeedback)
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).HandleActionNextEvent(allowEndFeedback);
}

bool AccessibilityManager::HandleActionPreviousEvent(bool allowEndFeedback)
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).HandleActionPreviousEvent(allowEndFeedback);
}

bool AccessibilityManager::HandleActionActivateEvent()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).HandleActionActivateEvent();
}

bool AccessibilityManager::HandleActionReadEvent(unsigned int x, unsigned int y,  bool allowReadAgain)
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).HandleActionReadEvent(x, y, allowReadAgain);
}

bool AccessibilityManager::HandleActionReadNextEvent(bool allowEndFeedback)
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).HandleActionReadNextEvent(allowEndFeedback);
}

bool AccessibilityManager::HandleActionReadPreviousEvent(bool allowEndFeedback)
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).HandleActionReadPreviousEvent(allowEndFeedback);
}

bool AccessibilityManager::HandleActionUpEvent()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).HandleActionUpEvent();
}

bool AccessibilityManager::HandleActionDownEvent()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).HandleActionDownEvent();
}

bool AccessibilityManager::HandleActionClearFocusEvent()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).HandleActionClearFocusEvent();
}

bool AccessibilityManager::HandleActionScrollEvent(const TouchPoint& point, unsigned long timeStamp)
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).HandleActionScrollEvent(point, timeStamp);
}

bool AccessibilityManager::HandleActionTouchEvent(const TouchPoint& point, unsigned long timeStamp)
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).HandleActionTouchEvent(point, timeStamp);
}

bool AccessibilityManager::HandleActionBackEvent()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).HandleActionBackEvent();
}

void AccessibilityManager::HandleActionEnableEvent()
{
  Internal::Adaptor::AccessibilityManager::GetImplementation(*this).HandleActionEnableEvent();
}

void AccessibilityManager::HandleActionDisableEvent()
{
  Internal::Adaptor::AccessibilityManager::GetImplementation(*this).HandleActionDisableEvent();
}

AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::StatusChangedSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).StatusChangedSignal();
}

AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::ActionNextSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).ActionNextSignal();
}

AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::ActionPreviousSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).ActionPreviousSignal();
}

AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::ActionActivateSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).ActionActivateSignal();
}

AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::ActionOverSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).ActionOverSignal();
}

AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::ActionReadSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).ActionReadSignal();
}

AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::ActionReadNextSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).ActionReadNextSignal();
}

AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::ActionReadPreviousSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).ActionReadPreviousSignal();
}

AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::ActionUpSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).ActionUpSignal();
}

AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::ActionDownSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).ActionDownSignal();
}

AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::ActionClearFocusSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).ActionClearFocusSignal();
}

AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::ActionBackSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).ActionBackSignal();
}

AccessibilityManager::AccessibilityActionScrollSignalV2& AccessibilityManager::ActionScrollSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).ActionScrollSignal();
}

AccessibilityManager::AccessibilityManager( Internal::Adaptor::AccessibilityManager& manager )
: BaseHandle( &manager )
{
}

AccessibilityManager::AccessibilityManager( Internal::Adaptor::AccessibilityManager* manager )
: BaseHandle( manager )
{
}

} // namespace Dali
