/*
Copyright (c) 2000-2013 Samsung Electronics Co., Ltd All Rights Reserved

This file is part of Dali Adaptor

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

// CLASS HEADER
#include <dali/public-api/adaptor-framework/common/accessibility-manager.h>

// INTERNAL INCLUDES
#include <internal/common/accessibility-manager-impl.h>

namespace Dali
{

const char* const AccessibilityManager::SIGNAL_STATUS_CHANGED( "accessibility-status-changed" );
const char* const AccessibilityManager::SIGNAL_ACTION_NEXT( "accessibility-action-next" );
const char* const AccessibilityManager::SIGNAL_ACTION_PREVIOUS( "accessibility-action-previous" );
const char* const AccessibilityManager::SIGNAL_ACTION_ACTIVATE( "accessibility-action-activate" );
const char* const AccessibilityManager::SIGNAL_ACTION_READ( "accessibility-action-read" );
const char* const AccessibilityManager::SIGNAL_ACTION_READ_NEXT( "accessibility-action-read-next" );
const char* const AccessibilityManager::SIGNAL_ACTION_READ_PREVIOUS( "accessibility-action-read-previous" );
const char* const AccessibilityManager::SIGNAL_ACTION_UP( "accessibility-action-up" );
const char* const AccessibilityManager::SIGNAL_ACTION_DOWN( "accessibility-action-down" );
const char* const AccessibilityManager::SIGNAL_ACTION_CLEAR_FOCUS( "accessibility-action-clear-focus" );
const char* const AccessibilityManager::SIGNAL_ACTION_BACK( "accessibility-action-back" );

AccessibilityManager::AccessibilityManager()
{
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

bool AccessibilityManager::HandleActionNextEvent()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).HandleActionNextEvent();
}

bool AccessibilityManager::HandleActionPreviousEvent()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).HandleActionPreviousEvent();
}

bool AccessibilityManager::HandleActionActivateEvent()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).HandleActionActivateEvent();
}

bool AccessibilityManager::HandleActionReadEvent(unsigned int x, unsigned int y)
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).HandleActionReadEvent(x, y);
}

bool AccessibilityManager::HandleActionReadNextEvent()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).HandleActionReadNextEvent();
}

bool AccessibilityManager::HandleActionReadPreviousEvent()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).HandleActionReadPreviousEvent();
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

bool AccessibilityManager::HandleActionScrollEvent(TouchPoint& point, unsigned long timeStamp)
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).HandleActionScrollEvent(point, timeStamp);
}

bool AccessibilityManager::HandleActionBackEvent()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).HandleActionBackEvent();
}

//AccessibilityManager::AccessibilityActionSignal AccessibilityManager::SignalStatusChanged()
//{
//  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).SignalStatusChanged();
//}

AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::StatusChangedSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).StatusChangedSignal();
}

//AccessibilityManager::AccessibilityActionSignal AccessibilityManager::SignalActionNext()
//{
//  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).SignalActionNext();
//}

AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::ActionNextSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).ActionNextSignal();
}

//AccessibilityManager::AccessibilityActionSignal AccessibilityManager::SignalActionPrevious()
//{
//  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).SignalActionPrevious();
//}

AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::ActionPreviousSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).ActionPreviousSignal();
}

//AccessibilityManager::AccessibilityActionSignal AccessibilityManager::SignalActionActivate()
//{
//  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).SignalActionActivate();
//}

AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::ActionActivateSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).ActionActivateSignal();
}

//AccessibilityManager::AccessibilityActionSignal AccessibilityManager::SignalActionRead()
//{
//  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).SignalActionRead();
//}

AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::ActionReadSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).ActionReadSignal();
}

//AccessibilityManager::AccessibilityActionSignal AccessibilityManager::SignalActionReadNext()
//{
//  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).SignalActionReadNext();
//}

AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::ActionReadNextSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).ActionReadNextSignal();
}

//AccessibilityManager::AccessibilityActionSignal AccessibilityManager::SignalActionReadPrevious()
//{
//  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).SignalActionReadPrevious();
//}

AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::ActionReadPreviousSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).ActionReadPreviousSignal();
}

//AccessibilityManager::AccessibilityActionSignal AccessibilityManager::SignalActionUp()
//{
//  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).SignalActionUp();
//}

AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::ActionUpSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).ActionUpSignal();
}

//AccessibilityManager::AccessibilityActionSignal AccessibilityManager::SignalActionDown()
//{
//  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).SignalActionDown();
//}

AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::ActionDownSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).ActionDownSignal();
}

//AccessibilityManager::AccessibilityActionSignal AccessibilityManager::SignalActionClearFocus()
//{
//  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).SignalActionClearFocus();
//}

AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::ActionClearFocusSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).ActionClearFocusSignal();
}

//AccessibilityManager::AccessibilityActionSignal AccessibilityManager::SignalActionBack()
//{
//  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).SignalActionBack();
//}

AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::ActionBackSignal()
{
  return Internal::Adaptor::AccessibilityManager::GetImplementation(*this).ActionBackSignal();
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

