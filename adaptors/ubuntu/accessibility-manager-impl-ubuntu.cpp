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
#include "accessibility-manager-impl.h"

// EXTERNAL INCLUDES

#include <dali/public-api/dali-core.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/integration-api/events/hover-event-integ.h>
#include <dali/integration-api/events/gesture-requests.h>

// INTERNAL INCLUDES
#include <singleton-service-impl.h>
#include "system-settings.h"

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gAccessibilityManagerLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_ACCESSIBILITY_MANAGER");
#endif

BaseHandle Create()
{
  BaseHandle handle( AccessibilityManager::Get() );

  if ( !handle )
  {
    Dali::SingletonService service( SingletonService::Get() );
    if ( service )
    {
      Dali::AccessibilityManager manager = Dali::AccessibilityManager( new AccessibilityManager() );
      service.Register( typeid( manager ), manager );
      handle = manager;
    }
  }

  return handle;
}
TypeRegistration ACCESSIBILITY_MANAGER_TYPE( typeid(Dali::AccessibilityManager), typeid(Dali::BaseHandle), Create, true /* Create Instance At Startup */ );

} // unnamed namespace

Dali::AccessibilityManager AccessibilityManager::Get()
{
  Dali::AccessibilityManager manager;

  Dali::SingletonService service( SingletonService::Get() );
  if ( service )
  {
    // Check whether the singleton is already created
    Dali::BaseHandle handle = service.GetSingleton( typeid( Dali::AccessibilityManager ) );
    if(handle)
    {
      // If so, downcast the handle
      manager = Dali::AccessibilityManager( dynamic_cast< AccessibilityManager* >( handle.GetObjectPtr() ) );
    }
  }

  return manager;
}

Vector2 AccessibilityManager::GetReadPosition() const
{
  return mReadPosition;
}

void AccessibilityManager::SetActionHandler(AccessibilityActionHandler& handler)
{
  mActionHandler = &handler;
}

void AccessibilityManager::SetGestureHandler(AccessibilityGestureHandler& handler)
{
  if( mAccessibilityGestureDetector )
  {
    mAccessibilityGestureDetector->SetGestureHandler(handler);
  }
}

bool AccessibilityManager::HandleActionClearFocusEvent()
{
  bool ret = false;

  Dali::AccessibilityManager handle( this );

  /*
   * In order to application decide reading action first,
   * emit ActionClearFocus signal in first, ClearAccessibilityFocus for handler in next
   */
  if ( !mIndicatorFocused )
  {
    if( !mActionClearFocusSignalType.Empty() )
    {
      mActionClearFocusSignalType.Emit( handle );
    }
  }

  if( mActionHandler )
  {
    ret = mActionHandler->ClearAccessibilityFocus();
  }

  DALI_LOG_INFO(gAccessibilityManagerLogFilter, Debug::General, "[%s:%d] %s\n", __FUNCTION__, __LINE__, ret?"TRUE":"FALSE");

  return ret;
}

bool AccessibilityManager::HandleActionScrollEvent(const TouchPoint& point, unsigned long timeStamp)
{
  bool ret = false;

  Dali::AccessibilityManager handle( this );

  Dali::TouchEvent event(timeStamp);
  event.points.push_back(point);

  /*
   * In order to application decide touch action first,
   * emit ActionScroll signal in first, AccessibilityActionScroll  for handler in next
   */
  if ( !mIndicatorFocused )
  {
    if( !mActionScrollSignalType.Empty() )
    {
      mActionScrollSignalType.Emit( handle, event );
    }
  }

  Integration::TouchEvent touchEvent;
  Integration::HoverEvent hoverEvent;
  Integration::TouchEventCombiner::EventDispatchType type = mCombiner.GetNextTouchEvent(point, timeStamp, touchEvent, hoverEvent);
  if(type == Integration::TouchEventCombiner::DispatchTouch || type == Integration::TouchEventCombiner::DispatchBoth) // hover event is ignored
  {
    // Process the touch event in accessibility gesture detector
    if( mAccessibilityGestureDetector )
    {
      mAccessibilityGestureDetector->SendEvent(touchEvent);
      ret = true;
    }
  }

  return ret;
}

bool AccessibilityManager::HandleActionTouchEvent(const TouchPoint& point, unsigned long timeStamp)
{
  bool ret = false;

  Dali::TouchEvent touchEvent(timeStamp);
  touchEvent.points.push_back(point);

  if( mActionHandler )
  {
    ret = mActionHandler->AccessibilityActionTouch(touchEvent);
  }
  return ret;
}

bool AccessibilityManager::HandleActionBackEvent()
{
  bool ret = false;

  Dali::AccessibilityManager handle( this );

  /*
   * In order to application decide reading action first,
   * emit ActionBack signal in first, AccessibilityActionBack for handler in next
   */
  if ( !mIndicatorFocused )
  {
    if( !mActionBackSignalType.Empty() )
    {
      mActionBackSignalType.Emit( handle );
    }
  }

  if( mActionHandler )
  {
    ret = mActionHandler->AccessibilityActionBack();
  }

  DALI_LOG_INFO(gAccessibilityManagerLogFilter, Debug::General, "[%s:%d] %s\n", __FUNCTION__, __LINE__, ret?"TRUE":"FALSE");

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
      mActionHandler->ChangeAccessibilityStatus();
    }

    //emit status changed signal
    Dali::AccessibilityManager handle( this );
    mStatusChangedSignalType.Emit( handle );
  }
}

void AccessibilityManager::DisableAccessibility()
{
  if(mIsEnabled == true)
  {
    mIsEnabled = false;

    if( mActionHandler )
    {
      mActionHandler->ChangeAccessibilityStatus();
    }

    //emit status changed signal
    Dali::AccessibilityManager handle( this );
    mStatusChangedSignalType.Emit( handle );

    // Destroy the TtsPlayer if exists.
    if ( Adaptor::IsAvailable() )
    {
      Dali::Adaptor& adaptor = Dali::Adaptor::Get();
      Adaptor::GetImplementation( adaptor ).DestroyTtsPlayer( Dali::TtsPlayer::SCREEN_READER );
    }
  }
}

bool AccessibilityManager::IsEnabled() const
{
  return mIsEnabled;
}

void AccessibilityManager::SetIndicator(Indicator* indicator)
{
  mIndicator = indicator;
}

AccessibilityManager::AccessibilityManager()
: mIsEnabled(false),
  mActionHandler(NULL),
  mIndicator(NULL),
  mIndicatorFocused(false)
{
  int isEnabled = 0;
  DALI_LOG_INFO(gAccessibilityManagerLogFilter, Debug::General, "[%s:%d] %s\n", __FUNCTION__, __LINE__, isEnabled?"ENABLED":"DISABLED");

  if(isEnabled == 1)
  {
    mIsEnabled = true;
  }
  else
  {
    mIsEnabled = false;
  }

  mAccessibilityGestureDetector = new AccessibilityGestureDetector();
}

AccessibilityManager::~AccessibilityManager()
{
}

bool AccessibilityManager::HandleActionNextEvent(bool allowEndFeedback)
{
  bool ret = false;
  Dali::AccessibilityManager handle( this );

  /*
   * In order to application decide reading action first,
   * emit ActionNext signal in first, AccessibilityActionNext for handler in next
   */
  if( !mIndicatorFocused )
  {
    if( !mActionNextSignalType.Empty() )
    {
      mActionNextSignalType.Emit( handle );
    }
  }

  if( mActionHandler )
  {
    ret = mActionHandler->AccessibilityActionNext(allowEndFeedback);
  }

  DALI_LOG_INFO(gAccessibilityManagerLogFilter, Debug::General, "[%s:%d] %s\n", __FUNCTION__, __LINE__, ret?"TRUE":"FALSE");

  return ret;
}

bool AccessibilityManager::HandleActionPreviousEvent(bool allowEndFeedback)
{
  bool ret = false;

  Dali::AccessibilityManager handle( this );

   /*
   * In order to application decide reading action first,
   * emit ActionPrevious signal in first, AccessibilityActionPrevious for handler in next
   */
 if ( !mIndicatorFocused )
  {
    if( !mActionPreviousSignalType.Empty() )
    {
      mActionPreviousSignalType.Emit( handle );
    }
  }

  if( mActionHandler )
  {
    ret = mActionHandler->AccessibilityActionPrevious(allowEndFeedback);
  }

  DALI_LOG_INFO(gAccessibilityManagerLogFilter, Debug::General, "[%s:%d] %s\n", __FUNCTION__, __LINE__, ret?"TRUE":"FALSE");

  return ret;
}

bool AccessibilityManager::HandleActionActivateEvent()
{
  bool ret = false;

  Dali::AccessibilityManager handle( this );

  /*
   * In order to application decide reading action first,
   * emit ActionActivate signal in first, AccessibilityActionActivate for handler in next
   */
  if ( !mIndicatorFocused )
  {
    if( !mActionActivateSignalType.Empty() )
    {
      mActionActivateSignalType.Emit( handle );
    }
  }

  if( mActionHandler )
  {
    ret = mActionHandler->AccessibilityActionActivate();
  }

  DALI_LOG_INFO(gAccessibilityManagerLogFilter, Debug::General, "[%s:%d] %s\n", __FUNCTION__, __LINE__, ret?"TRUE":"FALSE");

  return ret;
}

bool AccessibilityManager::HandleActionReadEvent(unsigned int x, unsigned int y, bool allowReadAgain)
{
  bool ret = false;

  DALI_LOG_INFO(gAccessibilityManagerLogFilter, Debug::General, "[%s:%d] %d , %d\n", __FUNCTION__, __LINE__, x, y);

  mReadPosition.x = x;
  mReadPosition.y = y;

  Dali::AccessibilityManager handle( this );

  bool indicatorFocused = false;

  // Check whether the Indicator is focused
  if( mIndicator && mIndicator->IsConnected() )
  {
    // Check the position and size of Indicator actor
    Dali::Actor indicatorActor = mIndicator->GetActor();
    Vector3 position = Vector3(0.0f, 0.0f, 0.0f);
    Vector3 size = indicatorActor.GetCurrentSize();

    if(mReadPosition.x >= position.x &&
       mReadPosition.x <= position.x + size.width &&
       mReadPosition.y >= position.y &&
       mReadPosition.y <= position.y + size.height)
    {
      indicatorFocused = true;
      DALI_LOG_INFO(gAccessibilityManagerLogFilter, Debug::General, "[%s:%d] Indicator area!!!!\n", __FUNCTION__, __LINE__);
    }
  }

  if( mIndicator )
  {
    if( !mIndicatorFocused && indicatorFocused )
    {
      // If Indicator is focused, the focus should be cleared in Dali focus chain.
      if( mActionHandler )
      {
        mActionHandler->ClearAccessibilityFocus();
      }
    }

    mIndicatorFocused = indicatorFocused;

    // Send accessibility READ action information to Indicator
    if( mIndicatorFocused )
    {
      DALI_LOG_INFO(gAccessibilityManagerLogFilter, Debug::General, "[%s:%d] Send READ message to indicator!!!!\n", __FUNCTION__, __LINE__);
    }
  }

  if(allowReadAgain)
  {
    /*
     * In order to application decide reading action first,
     * emit ActionRead signal in first, AccessibilityActionRead for handler in next
     */
    if( !mIndicatorFocused )
    {
      if ( !mActionReadSignalType.Empty() )
      {
        mActionReadSignalType.Emit( handle );
      }
    }
  }
  else
  {
    /*
     * In order to application decide reading action first,
     * emit ActionRead signal in first, AccessibilityActionRead for handler in next
     */
    if( !mIndicatorFocused )
    {
      if ( !mActionOverSignalType.Empty() )
      {
        mActionOverSignalType.Emit( handle );
      }
    }
  }

  if( mActionHandler && !mIndicatorFocused)
  {
    // If Indicator is not focused, the accessibility actions should be handled by the registered
    // accessibility action handler (e.g. focus manager)
    ret = mActionHandler->AccessibilityActionRead(allowReadAgain);
    DALI_LOG_INFO(gAccessibilityManagerLogFilter, Debug::General, "[%s:%d] %s\n", __FUNCTION__, __LINE__, ret?"TRUE":"FALSE");
  }

  return ret;
}

bool AccessibilityManager::HandleActionReadNextEvent(bool allowEndFeedback)
{
  bool ret = false;

  Dali::AccessibilityManager handle( this );

  /*
   * In order to application decide reading action first,
   * emit ActionReadNext signal in first, AccessibilityActionReadNext for handler in next
   */
  if ( !mIndicatorFocused )
  {
    if( !mActionReadNextSignalType.Empty() )
    {
      mActionReadNextSignalType.Emit( handle );
    }
  }

  if( mActionHandler )
  {
    ret = mActionHandler->AccessibilityActionReadNext(allowEndFeedback);
  }

  DALI_LOG_INFO(gAccessibilityManagerLogFilter, Debug::General, "[%s:%d] %s\n", __FUNCTION__, __LINE__, ret?"TRUE":"FALSE");

  return ret;
}

bool AccessibilityManager::HandleActionReadPreviousEvent(bool allowEndFeedback)
{
  bool ret = false;

  Dali::AccessibilityManager handle( this );

  /*
   * In order to application decide reading action first,
   * emit ActionReadPrevious signal in first, AccessibilityActionReadPrevious for handler in next
   */
  if ( !mIndicatorFocused )
  {
    if( !mActionReadPreviousSignalType.Empty() )
    {
      mActionReadPreviousSignalType.Emit( handle );
    }
  }

  if( mActionHandler )
  {
    ret = mActionHandler->AccessibilityActionReadPrevious(allowEndFeedback);
  }

  DALI_LOG_INFO(gAccessibilityManagerLogFilter, Debug::General, "[%s:%d] %s\n", __FUNCTION__, __LINE__, ret?"TRUE":"FALSE");

  return ret;
}

bool AccessibilityManager::HandleActionUpEvent()
{
  bool ret = false;

  Dali::AccessibilityManager handle( this );

  /*
   * In order to application decide reading action first,
   * emit ActionUp signal in first, AccessibilityActionUp for handler in next
   */
  if ( !mIndicatorFocused )
  {
    if( !mActionUpSignalType.Empty() )
    {
      mActionUpSignalType.Emit( handle );
    }
  }

  if( mActionHandler )
  {
    ret = mActionHandler->AccessibilityActionUp();
  }

  DALI_LOG_INFO(gAccessibilityManagerLogFilter, Debug::General, "[%s:%d] %s\n", __FUNCTION__, __LINE__, ret?"TRUE":"FALSE");

  return ret;
}

bool AccessibilityManager::HandleActionDownEvent()
{
  bool ret = false;

  Dali::AccessibilityManager handle( this );

  /*
   * In order to application decide reading action first,
   * emit ActionDown signal in first, AccessibilityActionDown for handler in next
   */
  if ( !mIndicatorFocused )
  {
    if( !mActionDownSignalType.Empty() )
    {
      mActionDownSignalType.Emit( handle );
    }
  }

  if( mActionHandler )
  {
    ret = mActionHandler->AccessibilityActionDown();
  }

  DALI_LOG_INFO(gAccessibilityManagerLogFilter, Debug::General, "[%s:%d] %s\n", __FUNCTION__, __LINE__, ret?"TRUE":"FALSE");

  return ret;
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
