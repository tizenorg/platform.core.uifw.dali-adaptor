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

bool AccessibilityManager::HandleActionScrollUpEvent()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).HandleActionScrollUpEvent();
}

bool AccessibilityManager::HandleActionScrollDownEvent()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).HandleActionScrollDownEvent();
}

bool AccessibilityManager::HandleActionPageLeftEvent()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).HandleActionPageLeftEvent();
}

bool AccessibilityManager::HandleActionPageRightEvent()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).HandleActionPageRightEvent();
}

bool AccessibilityManager::HandleActionPageUpEvent()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).HandleActionPageUpEvent();
}

bool AccessibilityManager::HandleActionPageDownEvent()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).HandleActionPageDownEvent();
}

bool AccessibilityManager::HandleActionMoveToFirstEvent()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).HandleActionMoveToFirstEvent();
}

bool AccessibilityManager::HandleActionMoveToLastEvent()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).HandleActionMoveToLastEvent();
}

bool AccessibilityManager::HandleActionReadFromTopEvent()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).HandleActionReadFromTopEvent();
}

bool AccessibilityManager::HandleActionZoomEvent()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).HandleActionZoomEvent();
}

bool AccessibilityManager::HandleActionReadIndicatorInformationEvent()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).HandleActionReadIndicatorInformationEvent();
}

bool AccessibilityManager::HandleActionReadPauseResumeEvent()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).HandleActionReadPauseResumeEvent();
}

bool AccessibilityManager::HandleActionStartStopEvent()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).HandleActionStartStopEvent();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::StatusChangedSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).StatusChangedSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionNextSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).ActionNextSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionPreviousSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).ActionPreviousSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionActivateSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).ActionActivateSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionOverSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).ActionOverSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionReadSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).ActionReadSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionReadNextSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).ActionReadNextSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionReadPreviousSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).ActionReadPreviousSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionUpSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).ActionUpSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionDownSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).ActionDownSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionClearFocusSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).ActionClearFocusSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionBackSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).ActionBackSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionScrollUpSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).ActionScrollUpSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionScrollDownSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).ActionScrollDownSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionPageLeftSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).ActionPageLeftSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionPageRightSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).ActionPageRightSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionPageUpSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).ActionPageUpSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionPageDownSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).ActionPageDownSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionMoveToFirstSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).ActionMoveToFirstSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionMoveToLastSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).ActionMoveToLastSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionReadFromTopSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).ActionReadFromTopSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionZoomSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).ActionZoomSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionReadIndicatorInformationSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).ActionReadIndicatorInformationSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionReadPauseResumeSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).ActionReadPauseResumeSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionStartStopSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).ActionStartStopSignal();
}

AccessibilityManager::AccessibilityActionScrollSignalType& AccessibilityManager::ActionScrollSignal()
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
