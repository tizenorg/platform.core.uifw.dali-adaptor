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
#include "accessibility-manager-impl.h"

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/integration-api/events/gesture-requests.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace
{
void AccessibilityOnOffNotification(int* node, void* data)
{
//  AccessibilityManager* manager = static_cast<AccessibilityManager*>(data);
//  int isEnabled = 0;
//  vconf_get_bool(VCONFKEY_SETAPPL_ACCESSIBILITY_TTS, &isEnabled);
//
//  DALI_LOG_WARNING("[%s:%d] %s\n", __FUNCTION__, __LINE__, isEnabled?"ENABLED":"DISABLED");
//
//  if(isEnabled == 1)
//  {
//    manager->EnableAccessibility();
//  }
//  else
//  {
//    manager->DisableAccessibility();
//  }
}
} // unnamed namespace

//Dali::AccessibilityManager AccessibilityManager::New()
//{
//  // Create the implementation, temporarily owned on stack
//  Dali::IntrusivePtr< AccessibilityManager > internalAccessibilityManager = new AccessibilityManager();

//  // Pass the ownership
//  Dali::AccessibilityManager manager( *internalAccessibilityManager );

//  // Register the object
//  internalAccessibilityManager->RegisterObject();

//  return manager;
//}

Vector2 AccessibilityManager::GetReadPosition() const
{
  return mReadPosition;
}

Dali::AccessibilityManager AccessibilityManager::Get()
{
//  Dali::AccessibilityManager manager;

//  if ( Adaptor::IsAvailable() )
//  {
//    // Check whether the singleton is already created
//    Dali::BaseHandle handle = Dali::Adaptor::Get().GetSingleton( typeid( Dali::AccessibilityManager ) );
//    if(handle)
//    {
//      // If so, downcast the handle
//      manager = Dali::AccessibilityManager( dynamic_cast< AccessibilityManager* >( handle.GetObjectPtr() ) );
//    }
//  }
//
//  return manager;
}

void AccessibilityManager::SetActionHandler(AccessibilityActionHandler& handler)
{
  mActionHandler = &handler;
}

void AccessibilityManager::SetGestureHandler(AccessibilityGestureHandler& handler)
{
  if( mAccessibilityGestureDetector )
  {
    // mAccessibilityGestureDetector->SetGestureHandler(handler);
  }
}


bool AccessibilityManager::HandleActionNextEvent(bool allowEndFeedback)
{
  bool ret = false;
//  Dali::AccessibilityManager handle( this );
//
//  if( mIndicator && mIndicatorFocused )
//  {
//    Elm_Access_Action_Info actionInfo;
//    actionInfo.action_type = ELM_ACCESS_ACTION_HIGHLIGHT_NEXT;
//
//    ret = mIndicator->SendMessage(MSG_DOMAIN_CONTROL_ACCESS, actionInfo.action_type, &actionInfo, sizeof(actionInfo));
//  }
//  else if( mActionHandler )
//  {
//    ret = mActionHandler->AccessibilityActionNext();
//  }
//
//  if ( !mIndicatorFocused && !mActionNextSignal.Empty() )
//  {
//    mActionNextSignal.GetSignal()( handle );
//  }
//
//  DALI_LOG_WARNING("[%s:%d] %s\n", __FUNCTION__, __LINE__, ret?"TRUE":"FALSE");

  return ret;
}

bool AccessibilityManager::HandleActionPreviousEvent(bool allowEndFeedback)
{
  bool ret = false;

//  Dali::AccessibilityManager handle( this );
//
//  if( mIndicator && mIndicatorFocused )
//  {
//    Elm_Access_Action_Info actionInfo;
//    actionInfo.action_type = ELM_ACCESS_ACTION_HIGHLIGHT_PREV;
//
//    ret = mIndicator->SendMessage(MSG_DOMAIN_CONTROL_ACCESS, actionInfo.action_type, &actionInfo, sizeof(actionInfo));
//  }
//  else if( mActionHandler )
//  {
//    ret = mActionHandler->AccessibilityActionPrevious();
//  }
//
//  if ( !mIndicatorFocused && !mActionPreviousSignal.Empty() )
//  {
//    mActionPreviousSignal.GetSignal()( handle );
//  }
//
//  DALI_LOG_WARNING("[%s:%d] %s\n", __FUNCTION__, __LINE__, ret?"TRUE":"FALSE");

  return ret;
}

bool AccessibilityManager::HandleActionActivateEvent()
{
  bool ret = false;

//  Dali::AccessibilityManager handle( this );
//
//  if( mIndicator && mIndicatorFocused )
//  {
//    Elm_Access_Action_Info actionInfo;
//    actionInfo.action_type = ELM_ACCESS_ACTION_ACTIVATE;
//
//    ret = mIndicator->SendMessage(MSG_DOMAIN_CONTROL_ACCESS, actionInfo.action_type, &actionInfo, sizeof(actionInfo));
//  }
//  else if( mActionHandler )
//  {
//    ret = mActionHandler->AccessibilityActionActivate();
//  }
//
//  if ( !mIndicatorFocused && !mActionActivateSignal.Empty() )
//  {
//    mActionActivateSignal.GetSignal()( handle );
//  }
//
//  DALI_LOG_WARNING("[%s:%d] %s\n", __FUNCTION__, __LINE__, ret?"TRUE":"FALSE");

  return ret;
}

bool AccessibilityManager::HandleActionReadEvent(unsigned int x, unsigned int y, bool allowReadAgain)
{
  bool ret = false;

//  DALI_LOG_WARNING("[%s:%d] %d , %d\n", __FUNCTION__, __LINE__, x, y);
//
//  mReadPosition.x = x;
//  mReadPosition.y = y;
//
//  Dali::AccessibilityManager handle( this );
//
//  bool indicatorFocused = false;
//
//  // Check whether the Indicator is focused
//  if( mIndicator && mIndicator->IsConnected() )
//  {
//    // Check the position and size of Indicator actor
//    Dali::Actor indicatorActor = mIndicator->GetActor();
//    Vector3 position = Vector3(0.0f, 0.0f, 0.0f);
//    Vector3 size = indicatorActor.GetCurrentSize();
//
//    if(mReadPosition.x >= position.x &&
//       mReadPosition.x <= position.x + size.width &&
//       mReadPosition.y >= position.y &&
//       mReadPosition.y <= position.y + size.height)
//    {
//      indicatorFocused = true;
//      DALI_LOG_WARNING("[%s:%d] Indicator area!!!!\n", __FUNCTION__, __LINE__);
//    }
//  }
//
//  if( mIndicator )
//  {
//    if( !mIndicatorFocused && indicatorFocused )
//    {
//      // If Indicator is focused, the focus should be cleared in Dali focus chain.
//      if( mActionHandler )
//      {
//        mActionHandler->ClearAccessibilityFocus();
//      }
//    }
//    else if( mIndicatorFocused && !indicatorFocused )
//    {
//      Elm_Access_Action_Info actionInfo;
//      actionInfo.action_type = ELM_ACCESS_ACTION_UNHIGHLIGHT;
//
//      // Indicator should be unhighlighted
//      ret = mIndicator->SendMessage(MSG_DOMAIN_CONTROL_ACCESS, actionInfo.action_type, &actionInfo, sizeof(actionInfo));
//      DALI_LOG_WARNING("[%s:%d] Send unhighlight message to indicator!!!!\n", __FUNCTION__, __LINE__);
//    }
//
//    mIndicatorFocused = indicatorFocused;
//
//    // Send accessibility READ action information to Indicator
//    if( mIndicatorFocused )
//    {
//      Elm_Access_Action_Info actionInfo;
//      actionInfo.x = mReadPosition.x;
//      actionInfo.y = mReadPosition.y;
//      actionInfo.action_type = ELM_ACCESS_ACTION_READ;
//      ret = mIndicator->SendMessage(MSG_DOMAIN_CONTROL_ACCESS, actionInfo.action_type, &actionInfo, sizeof(actionInfo));
//
//      DALI_LOG_WARNING("[%s:%d] Send READ message to indicator!!!!\n", __FUNCTION__, __LINE__);
//    }
//  }
//
//  if( mActionHandler && !mIndicatorFocused)
//  {
//    // If Indicator is not focused, the accessibility actions should be handled by the registered
//    // accessibility action handler (e.g. focus manager)
//    ret = mActionHandler->AccessibilityActionRead();
//    DALI_LOG_WARNING("[%s:%d] %s\n", __FUNCTION__, __LINE__, ret?"TRUE":"FALSE");
//  }
//
//  if ( !mIndicatorFocused && !mActionReadSignal.Empty() )
//  {
//    mActionReadSignal.GetSignal()( handle );
//  }


  return ret;
}

bool AccessibilityManager::HandleActionReadNextEvent(bool allowEndFeedback)
{
  bool ret = false;

//  Dali::AccessibilityManager handle( this );
//
//  if( mIndicator && mIndicatorFocused )
//  {
//    Elm_Access_Action_Info actionInfo;
//    actionInfo.action_type = ELM_ACCESS_ACTION_HIGHLIGHT_NEXT;
//
//    ret = mIndicator->SendMessage(MSG_DOMAIN_CONTROL_ACCESS, actionInfo.action_type, &actionInfo, sizeof(actionInfo));
//  }
//  else if( mActionHandler )
//  {
//    ret = mActionHandler->AccessibilityActionReadNext();
//  }
//
//  if ( !mIndicatorFocused && !mActionReadNextSignal.Empty() )
//  {
//    mActionReadNextSignal.GetSignal()( handle );
//  }
//
//  DALI_LOG_WARNING("[%s:%d] %s\n", __FUNCTION__, __LINE__, ret?"TRUE":"FALSE");

  return ret;
}

bool AccessibilityManager::HandleActionReadPreviousEvent(bool allowEndFeedback)
{
  bool ret = false;

//  Dali::AccessibilityManager handle( this );
//
//  if( mIndicator && mIndicatorFocused )
//  {
//    Elm_Access_Action_Info actionInfo;
//    actionInfo.action_type = ELM_ACCESS_ACTION_HIGHLIGHT_PREV;
//
//    ret = mIndicator->SendMessage(MSG_DOMAIN_CONTROL_ACCESS, actionInfo.action_type, &actionInfo, sizeof(actionInfo));
//  }
//  else if( mActionHandler )
//  {
//    ret = mActionHandler->AccessibilityActionReadPrevious();
//  }
//
//  if ( !mIndicatorFocused && !mActionReadPreviousSignal.Empty() )
//  {
//    mActionReadPreviousSignal.GetSignal()( handle );
//  }
//
//  DALI_LOG_WARNING("[%s:%d] %s\n", __FUNCTION__, __LINE__, ret?"TRUE":"FALSE");

  return ret;
}

bool AccessibilityManager::HandleActionUpEvent()
{
  bool ret = false;

//  Dali::AccessibilityManager handle( this );
//
//  if( mIndicator && mIndicatorFocused )
//  {
//    Elm_Access_Action_Info actionInfo;
//    actionInfo.action_type = ELM_ACCESS_ACTION_UP;
//
//    ret = mIndicator->SendMessage(MSG_DOMAIN_CONTROL_ACCESS, actionInfo.action_type, &actionInfo, sizeof(actionInfo));
//  }
//  else if( mActionHandler )
//  {
//    ret = mActionHandler->AccessibilityActionUp();
//  }
//
//  if ( !mIndicatorFocused && !mActionUpSignal.Empty() )
//  {
//    mActionUpSignal.GetSignal()( handle );
//  }
//
//  DALI_LOG_WARNING("[%s:%d] %s\n", __FUNCTION__, __LINE__, ret?"TRUE":"FALSE");

  return ret;
}

bool AccessibilityManager::HandleActionDownEvent()
{
  bool ret = false;

//  Dali::AccessibilityManager handle( this );
//
//  if( mIndicator && mIndicatorFocused )
//  {
//    Elm_Access_Action_Info actionInfo;
//    actionInfo.action_type = ELM_ACCESS_ACTION_DOWN;
//
//    ret = mIndicator->SendMessage(MSG_DOMAIN_CONTROL_ACCESS, actionInfo.action_type, &actionInfo, sizeof(actionInfo));
//  }
//  else if( mActionHandler )
//  {
//    ret = mActionHandler->AccessibilityActionDown();
//  }
//
//  if ( !mIndicatorFocused && !mActionDownSignal.Empty() )
//  {
//    mActionDownSignal.GetSignal()( handle );
//  }
//
//  DALI_LOG_WARNING("[%s:%d] %s\n", __FUNCTION__, __LINE__, ret?"TRUE":"FALSE");

  return ret;
}

bool AccessibilityManager::HandleActionClearFocusEvent()
{
  bool ret = false;

//  Dali::AccessibilityManager handle( this );
//
//  if( mActionHandler )
//  {
//    ret = mActionHandler->ClearAccessibilityFocus();
//  }
//
//  if ( !mIndicatorFocused && !mActionClearFocusSignal.Empty() )
//  {
//    mActionClearFocusSignal.GetSignal()( handle );
//  }
//
//  DALI_LOG_WARNING("[%s:%d] %s\n", __FUNCTION__, __LINE__, ret?"TRUE":"FALSE");

  return ret;
}

bool AccessibilityManager::HandleActionScrollEvent(const TouchPoint& point, unsigned long timeStamp)
{
  bool ret = false;

//  Integration::TouchEvent event;
//  if (mCombiner.GetNextTouchEvent(point, timeStamp, event))
//  {
//    // Process the touch event in accessibility gesture detector
//    if( mAccessibilityGestureDetector )
//    {
//      mAccessibilityGestureDetector->SendEvent(event);
//      ret = true;
//    }
//  }

  return ret;
}

bool AccessibilityManager::HandleActionTouchEvent(const TouchPoint& point, unsigned long timeStamp)
{
  bool ret = false;

//  Dali::TouchEvent touchEvent(timeStamp);
//  touchEvent.points.push_back(point);

//  if( mActionHandler )
//  {
//    ret = mActionHandler->AccessibilityActionTouch(touchEvent);
//  }
  return ret;
}


bool AccessibilityManager::HandleActionBackEvent()
{
  bool ret = false;

//  Dali::AccessibilityManager handle( this );
//
//  if( mActionHandler )
//  {
//    ret = mActionHandler->AccessibilityActionBack();
//  }
//
//  if ( !mIndicatorFocused && !mActionBackSignal.Empty() )
//  {
//    mActionBackSignal.GetSignal()( handle );
//  }
//
//  DALI_LOG_WARNING("[%s:%d] %s\n", __FUNCTION__, __LINE__, ret?"TRUE":"FALSE");

  return ret;
}

void AccessibilityManager::HandleActionEnableEvent()
{
  EnableAccessibility();
}

void AccessibilityManager::HandleActionDisableEvent()
{
  DisableAccessibility();
}

void AccessibilityManager::EnableAccessibility()
{
  if(mIsEnabled == false)
  {
    mIsEnabled = true;

    if( mActionHandler )
    {
      // mActionHandler->ChangeAccessibilityStatus();
    }

    //emit status changed signal
    Dali::AccessibilityManager handle( this );
    //mStatusChangedSignal.GetSignal()( handle );
  }
}

void AccessibilityManager::DisableAccessibility()
{
  if(mIsEnabled == true)
  {
    mIsEnabled = false;

    if( mActionHandler )
    {
      // mActionHandler->ChangeAccessibilityStatus();
    }

    //emit status changed signal
    Dali::AccessibilityManager handle( this );
    //mStatusChangedSignal.GetSignal()( handle );
  }
}

bool AccessibilityManager::IsEnabled() const
{
  return mIsEnabled;
}

AccessibilityManager::AccessibilityManager()
: mIsEnabled(false),
  mActionHandler(NULL),
  mIndicatorFocused(false)
{
  int isEnabled = 0;
  // vconf_get_bool(VCONFKEY_SETAPPL_ACCESSIBILITY_TTS, &isEnabled);
  DALI_LOG_WARNING("[%s:%d] %s\n", __FUNCTION__, __LINE__, isEnabled?"ENABLED":"DISABLED");

  if(isEnabled == 1)
  {
    mIsEnabled = true;
  }
  else
  {
    mIsEnabled = false;
  }

  // vconf_notify_key_changed( VCONFKEY_SETAPPL_ACCESSIBILITY_TTS, AccessibilityOnOffNotification, this );

  mAccessibilityGestureDetector = new AccessibilityGestureDetector();
}

AccessibilityManager::~AccessibilityManager()
{
  // vconf_ignore_key_changed( VCONFKEY_SETAPPL_ACCESSIBILITY_TTS, AccessibilityOnOffNotification );
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
