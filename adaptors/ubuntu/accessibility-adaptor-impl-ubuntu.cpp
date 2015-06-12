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
#include "accessibility-adaptor-impl.h"

// EXTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>
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
Debug::Filter* gAccessibilityAdaptorLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_ACCESSIBILITY_ADAPTOR");
#endif

BaseHandle Create()
{
  BaseHandle handle( AccessibilityAdaptor::Get() );

  if ( !handle )
  {
    Dali::SingletonService service( SingletonService::Get() );
    if ( service )
    {
      Dali::AccessibilityAdaptor adaptor = Dali::AccessibilityAdaptor( new AccessibilityAdaptor() );
      service.Register( typeid( adaptor ), adaptor );
      handle = adaptor;
    }
  }

  return handle;
}
TypeRegistration ACCESSIBILITY_ADAPTOR_TYPE( typeid(Dali::AccessibilityAdaptor), typeid(Dali::BaseHandle), Create, true /* Create Instance At Startup */ );

} // unnamed namespace

Dali::AccessibilityAdaptor AccessibilityAdaptor::Get()
{
  Dali::AccessibilityAdaptor adaptor;

  Dali::SingletonService service( SingletonService::Get() );
  if ( service )
  {
    // Check whether the singleton is already created
    Dali::BaseHandle handle = service.GetSingleton( typeid( Dali::AccessibilityAdaptor ) );
    if(handle)
    {
      // If so, downcast the handle
      adaptor = Dali::AccessibilityAdaptor( dynamic_cast< AccessibilityAdaptor* >( handle.GetObjectPtr() ) );
    }
  }

  return adaptor;
}

Vector2 AccessibilityAdaptor::GetReadPosition() const
{
  return mReadPosition;
}

void AccessibilityAdaptor::SetActionHandler(AccessibilityActionHandler& handler)
{
  mActionHandler = &handler;
}

void AccessibilityAdaptor::SetGestureHandler(AccessibilityGestureHandler& handler)
{
  if( mAccessibilityGestureDetector )
  {
    mAccessibilityGestureDetector->SetGestureHandler(handler);
  }
}

bool AccessibilityAdaptor::HandleActionClearFocusEvent()
{
  bool ret = false;

  Dali::AccessibilityAdaptor handle( this );

  /*
   * In order to application decide reading action first,
   * emit ActionClearFocus signal in first, ClearAccessibilityFocus for handler in next
   */
  if ( !mIndicatorFocused )
  {
    if( !mActionClearFocusSignal.Empty() )
    {
      mActionClearFocusSignal.Emit( handle );
    }
  }

  if( mActionHandler )
  {
    ret = mActionHandler->ClearAccessibilityFocus();
  }

  DALI_LOG_INFO(gAccessibilityAdaptorLogFilter, Debug::General, "[%s:%d] %s\n", __FUNCTION__, __LINE__, ret?"TRUE":"FALSE");

  return ret;
}

bool AccessibilityAdaptor::HandleActionScrollEvent(const TouchPoint& point, unsigned long timeStamp)
{
  bool ret = false;

  Dali::AccessibilityAdaptor handle( this );

  Dali::TouchEvent event(timeStamp);
  event.points.push_back(point);

  /*
   * In order to application decide touch action first,
   * emit ActionScroll signal in first, AccessibilityActionScroll  for handler in next
   */
  if ( !mIndicatorFocused )
  {
    if( !mActionScrollSignal.Empty() )
    {
      mActionScrollSignal.Emit( handle, event );
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

bool AccessibilityAdaptor::HandleActionTouchEvent(const TouchPoint& point, unsigned long timeStamp)
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

bool AccessibilityAdaptor::HandleActionBackEvent()
{
  bool ret = false;

  Dali::AccessibilityAdaptor handle( this );

  /*
   * In order to application decide reading action first,
   * emit ActionBack signal in first, AccessibilityActionBack for handler in next
   */
  if ( !mIndicatorFocused )
  {
    if( !mActionBackSignal.Empty() )
    {
      mActionBackSignal.Emit( handle );
    }
  }

  if( mActionHandler )
  {
    ret = mActionHandler->AccessibilityActionBack();
  }

  DALI_LOG_INFO(gAccessibilityAdaptorLogFilter, Debug::General, "[%s:%d] %s\n", __FUNCTION__, __LINE__, ret?"TRUE":"FALSE");

  return ret;
}

void AccessibilityAdaptor::HandleActionEnableEvent()
{
  EnableAccessibility();
}

void AccessibilityAdaptor::HandleActionDisableEvent()
{
  DisableAccessibility();
}

void AccessibilityAdaptor::EnableAccessibility()
{
  if(mIsEnabled == false)
  {
    mIsEnabled = true;

    if( mActionHandler )
    {
      mActionHandler->ChangeAccessibilityStatus();
    }

    //emit status changed signal
    Dali::AccessibilityAdaptor handle( this );
    mStatusChangedSignal.Emit( handle );
  }
}

void AccessibilityAdaptor::DisableAccessibility()
{
  if(mIsEnabled == true)
  {
    mIsEnabled = false;

    if( mActionHandler )
    {
      mActionHandler->ChangeAccessibilityStatus();
    }

    //emit status changed signal
    Dali::AccessibilityAdaptor handle( this );
    mStatusChangedSignal.Emit( handle );

    // Destroy the TtsPlayer if exists.
    if ( Adaptor::IsAvailable() )
    {
      Dali::Adaptor& adaptor = Dali::Adaptor::Get();
      Adaptor::GetImplementation( adaptor ).DestroyTtsPlayer( Dali::TtsPlayer::SCREEN_READER );
    }
  }
}

bool AccessibilityAdaptor::IsEnabled() const
{
  return mIsEnabled;
}

void AccessibilityAdaptor::SetIndicator(Indicator* indicator)
{
  mIndicator = indicator;
}

AccessibilityAdaptor::AccessibilityAdaptor()
: mIsEnabled(false),
  mActionHandler(NULL),
  mIndicator(NULL),
  mIndicatorFocused(false)
{
  int isEnabled = 0;
  DALI_LOG_INFO(gAccessibilityAdaptorLogFilter, Debug::General, "[%s:%d] %s\n", __FUNCTION__, __LINE__, isEnabled?"ENABLED":"DISABLED");

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

AccessibilityAdaptor::~AccessibilityAdaptor()
{
}

bool AccessibilityAdaptor::HandleActionNextEvent(bool allowEndFeedback)
{
  bool ret = false;
  Dali::AccessibilityAdaptor handle( this );

  /*
   * In order to application decide reading action first,
   * emit ActionNext signal in first, AccessibilityActionNext for handler in next
   */
  if( !mIndicatorFocused )
  {
    if( !mActionNextSignal.Empty() )
    {
      mActionNextSignal.Emit( handle );
    }
  }

  if( mActionHandler )
  {
    ret = mActionHandler->AccessibilityActionNext(allowEndFeedback);
  }

  DALI_LOG_INFO(gAccessibilityAdaptorLogFilter, Debug::General, "[%s:%d] %s\n", __FUNCTION__, __LINE__, ret?"TRUE":"FALSE");

  return ret;
}

bool AccessibilityAdaptor::HandleActionPreviousEvent(bool allowEndFeedback)
{
  bool ret = false;

  Dali::AccessibilityAdaptor handle( this );

   /*
   * In order to application decide reading action first,
   * emit ActionPrevious signal in first, AccessibilityActionPrevious for handler in next
   */
 if ( !mIndicatorFocused )
  {
    if( !mActionPreviousSignal.Empty() )
    {
      mActionPreviousSignal.Emit( handle );
    }
  }

  if( mActionHandler )
  {
    ret = mActionHandler->AccessibilityActionPrevious(allowEndFeedback);
  }

  DALI_LOG_INFO(gAccessibilityAdaptorLogFilter, Debug::General, "[%s:%d] %s\n", __FUNCTION__, __LINE__, ret?"TRUE":"FALSE");

  return ret;
}

bool AccessibilityAdaptor::HandleActionActivateEvent()
{
  bool ret = false;

  Dali::AccessibilityAdaptor handle( this );

  /*
   * In order to application decide reading action first,
   * emit ActionActivate signal in first, AccessibilityActionActivate for handler in next
   */
  if ( !mIndicatorFocused )
  {
    if( !mActionActivateSignal.Empty() )
    {
      mActionActivateSignal.Emit( handle );
    }
  }

  if( mActionHandler )
  {
    ret = mActionHandler->AccessibilityActionActivate();
  }

  DALI_LOG_INFO(gAccessibilityAdaptorLogFilter, Debug::General, "[%s:%d] %s\n", __FUNCTION__, __LINE__, ret?"TRUE":"FALSE");

  return ret;
}

bool AccessibilityAdaptor::HandleActionReadEvent(unsigned int x, unsigned int y, bool allowReadAgain)
{
  bool ret = false;

  DALI_LOG_INFO(gAccessibilityAdaptorLogFilter, Debug::General, "[%s:%d] %d , %d\n", __FUNCTION__, __LINE__, x, y);

  mReadPosition.x = x;
  mReadPosition.y = y;

  Dali::AccessibilityAdaptor handle( this );

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
      DALI_LOG_INFO(gAccessibilityAdaptorLogFilter, Debug::General, "[%s:%d] Indicator area!!!!\n", __FUNCTION__, __LINE__);
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
      DALI_LOG_INFO(gAccessibilityAdaptorLogFilter, Debug::General, "[%s:%d] Send READ message to indicator!!!!\n", __FUNCTION__, __LINE__);
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
      if ( !mActionReadSignal.Empty() )
      {
        mActionReadSignal.Emit( handle );
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
      if ( !mActionOverSignal.Empty() )
      {
        mActionOverSignal.Emit( handle );
      }
    }
  }

  if( mActionHandler && !mIndicatorFocused)
  {
    // If Indicator is not focused, the accessibility actions should be handled by the registered
    // accessibility action handler (e.g. focus manager)
    ret = mActionHandler->AccessibilityActionRead(allowReadAgain);
    DALI_LOG_INFO(gAccessibilityAdaptorLogFilter, Debug::General, "[%s:%d] %s\n", __FUNCTION__, __LINE__, ret?"TRUE":"FALSE");
  }

  return ret;
}

bool AccessibilityAdaptor::HandleActionReadNextEvent(bool allowEndFeedback)
{
  bool ret = false;

  Dali::AccessibilityAdaptor handle( this );

  /*
   * In order to application decide reading action first,
   * emit ActionReadNext signal in first, AccessibilityActionReadNext for handler in next
   */
  if ( !mIndicatorFocused )
  {
    if( !mActionReadNextSignal.Empty() )
    {
      mActionReadNextSignal.Emit( handle );
    }
  }

  if( mActionHandler )
  {
    ret = mActionHandler->AccessibilityActionReadNext(allowEndFeedback);
  }

  DALI_LOG_INFO(gAccessibilityAdaptorLogFilter, Debug::General, "[%s:%d] %s\n", __FUNCTION__, __LINE__, ret?"TRUE":"FALSE");

  return ret;
}

bool AccessibilityAdaptor::HandleActionReadPreviousEvent(bool allowEndFeedback)
{
  bool ret = false;

  Dali::AccessibilityAdaptor handle( this );

  /*
   * In order to application decide reading action first,
   * emit ActionReadPrevious signal in first, AccessibilityActionReadPrevious for handler in next
   */
  if ( !mIndicatorFocused )
  {
    if( !mActionReadPreviousSignal.Empty() )
    {
      mActionReadPreviousSignal.Emit( handle );
    }
  }

  if( mActionHandler )
  {
    ret = mActionHandler->AccessibilityActionReadPrevious(allowEndFeedback);
  }

  DALI_LOG_INFO(gAccessibilityAdaptorLogFilter, Debug::General, "[%s:%d] %s\n", __FUNCTION__, __LINE__, ret?"TRUE":"FALSE");

  return ret;
}

bool AccessibilityAdaptor::HandleActionUpEvent()
{
  bool ret = false;

  Dali::AccessibilityAdaptor handle( this );

  /*
   * In order to application decide reading action first,
   * emit ActionUp signal in first, AccessibilityActionUp for handler in next
   */
  if ( !mIndicatorFocused )
  {
    if( !mActionUpSignal.Empty() )
    {
      mActionUpSignal.Emit( handle );
    }
  }

  if( mActionHandler )
  {
    ret = mActionHandler->AccessibilityActionUp();
  }

  DALI_LOG_INFO(gAccessibilityAdaptorLogFilter, Debug::General, "[%s:%d] %s\n", __FUNCTION__, __LINE__, ret?"TRUE":"FALSE");

  return ret;
}

bool AccessibilityAdaptor::HandleActionDownEvent()
{
  bool ret = false;

  Dali::AccessibilityAdaptor handle( this );

  /*
   * In order to application decide reading action first,
   * emit ActionDown signal in first, AccessibilityActionDown for handler in next
   */
  if ( !mIndicatorFocused )
  {
    if( !mActionDownSignal.Empty() )
    {
      mActionDownSignal.Emit( handle );
    }
  }

  if( mActionHandler )
  {
    ret = mActionHandler->AccessibilityActionDown();
  }

  DALI_LOG_INFO(gAccessibilityAdaptorLogFilter, Debug::General, "[%s:%d] %s\n", __FUNCTION__, __LINE__, ret?"TRUE":"FALSE");

  return ret;
}

bool AccessibilityAdaptor::HandleActionScrollUpEvent()
{
  bool ret = false;

  Dali::AccessibilityAdaptor handle( this );

  /*
   * In order to application decide reading action first,
   * emit ActionScrollUp signal in first, AccessibilityActionScrollUp for handler in next
   */
  if ( !mIndicatorFocused )
  {
    if( !mActionScrollUpSignal.Empty() )
    {
      mActionScrollUpSignal.Emit( handle );
    }
  }

  if( mActionHandler )
  {
    ret = mActionHandler->AccessibilityActionScrollUp();
  }

  DALI_LOG_INFO(gAccessibilityAdaptorLogFilter, Debug::General, "[%s:%d] %s\n", __FUNCTION__, __LINE__, ret?"TRUE":"FALSE");

  return ret;
}


bool AccessibilityAdaptor::HandleActionScrollDownEvent()
{
  bool ret = false;

  Dali::AccessibilityAdaptor handle( this );

  /*
   * In order to application decide reading action first,
   * emit ActionScrollDown signal in first, mActionScrollDownSignal for handler in next
   */
  if ( !mIndicatorFocused )
  {
    if( !mActionScrollDownSignal.Empty() )
    {
      mActionScrollDownSignal.Emit( handle );
    }
  }

  if( mActionHandler )
  {
    ret = mActionHandler->AccessibilityActionScrollDown();
  }

  DALI_LOG_INFO(gAccessibilityAdaptorLogFilter, Debug::General, "[%s:%d] %s\n", __FUNCTION__, __LINE__, ret?"TRUE":"FALSE");

  return ret;
}

bool AccessibilityAdaptor::HandleActionPageLeftEvent()
{
  bool ret = false;

  Dali::AccessibilityAdaptor handle( this );

  /*
   * In order to application decide reading action first,
   * emit ActionPageLeft signal in first, AccessibilityActionPageLeft for handler in next
   */
  if ( !mIndicatorFocused )
  {
    if( !mActionPageLeftSignal.Empty() )
    {
      mActionPageLeftSignal.Emit( handle );
    }
  }

  if( mActionHandler )
  {
    ret = mActionHandler->AccessibilityActionPageLeft();
  }

  DALI_LOG_INFO(gAccessibilityAdaptorLogFilter, Debug::General, "[%s:%d] %s\n", __FUNCTION__, __LINE__, ret?"TRUE":"FALSE");

  return ret;
}

bool AccessibilityAdaptor::HandleActionPageRightEvent()
{
  bool ret = false;

  Dali::AccessibilityAdaptor handle( this );

  /*
   * In order to application decide reading action first,
   * emit ActionPageRight signal in first, AccessibilityActionPageRight for handler in next
   */
  if ( !mIndicatorFocused )
  {
    if( !mActionPageRightSignal.Empty() )
    {
      mActionPageRightSignal.Emit( handle );
    }
  }

  if( mActionHandler )
  {
    ret = mActionHandler->AccessibilityActionPageRight();
  }

  DALI_LOG_INFO(gAccessibilityAdaptorLogFilter, Debug::General, "[%s:%d] %s\n", __FUNCTION__, __LINE__, ret?"TRUE":"FALSE");

  return ret;
}

bool AccessibilityAdaptor::HandleActionPageUpEvent()
{
  bool ret = false;

  Dali::AccessibilityAdaptor handle( this );

  /*
   * In order to application decide reading action first,
   * emit ActionPageUp signal in first, AccessibilityActionPageUp for handler in next
   */
  if ( !mIndicatorFocused )
  {
    if( !mActionPageUpSignal.Empty() )
    {
      mActionPageUpSignal.Emit( handle );
    }
  }

  if( mActionHandler )
  {
    ret = mActionHandler->AccessibilityActionPageUp();
  }

  DALI_LOG_INFO(gAccessibilityAdaptorLogFilter, Debug::General, "[%s:%d] %s\n", __FUNCTION__, __LINE__, ret?"TRUE":"FALSE");

  return ret;
}

bool AccessibilityAdaptor::HandleActionPageDownEvent()
{
  bool ret = false;

  Dali::AccessibilityAdaptor handle( this );

  /*
   * In order to application decide reading action first,
   * emit ActionPageDown signal in first, AccessibilityActionPageDown for handler in next
   */
  if ( !mIndicatorFocused )
  {
    if( !mActionPageDownSignal.Empty() )
    {
      mActionPageDownSignal.Emit( handle );
    }
  }

  if( mActionHandler )
  {
    ret = mActionHandler->AccessibilityActionPageDown();
  }

  DALI_LOG_INFO(gAccessibilityAdaptorLogFilter, Debug::General, "[%s:%d] %s\n", __FUNCTION__, __LINE__, ret?"TRUE":"FALSE");

  return ret;
}

bool AccessibilityAdaptor::HandleActionMoveToFirstEvent()
{
  bool ret = false;

  Dali::AccessibilityAdaptor handle( this );

  /*
   * In order to application decide reading action first,
   * emit ActionMoveToFirst signal in first, AccessibilityActionMoveToFirst for handler in next
   */
  if ( !mIndicatorFocused )
  {
    if( !mActionMoveToFirstSignal.Empty() )
    {
      mActionMoveToFirstSignal.Emit( handle );
    }
  }

  if( mActionHandler )
  {
    ret = mActionHandler->AccessibilityActionMoveToFirst();
  }

  DALI_LOG_INFO(gAccessibilityAdaptorLogFilter, Debug::General, "[%s:%d] %s\n", __FUNCTION__, __LINE__, ret?"TRUE":"FALSE");

  return ret;
}

bool AccessibilityAdaptor::HandleActionMoveToLastEvent()
{
  bool ret = false;

  Dali::AccessibilityAdaptor handle( this );

  /*
   * In order to application decide reading action first,
   * emit ActionMoveToLast signal in first, AccessibilityActionMoveToLast for handler in next
   */
  if ( !mIndicatorFocused )
  {
    if( !mActionMoveToLastSignal.Empty() )
    {
      mActionMoveToLastSignal.Emit( handle );
    }
  }

  if( mActionHandler )
  {
    ret = mActionHandler->AccessibilityActionMoveToLast();
  }

  DALI_LOG_INFO(gAccessibilityAdaptorLogFilter, Debug::General, "[%s:%d] %s\n", __FUNCTION__, __LINE__, ret?"TRUE":"FALSE");

  return ret;
}

bool AccessibilityAdaptor::HandleActionReadFromTopEvent()
{
  bool ret = false;

  Dali::AccessibilityAdaptor handle( this );

  /*
   * In order to application decide reading action first,
   * emit ActionReadFromTop signal in first, AccessibilityActionReadFromTop for handler in next
   */
  if ( !mIndicatorFocused )
  {
    if( !mActionReadFromTopSignal.Empty() )
    {
      mActionReadFromTopSignal.Emit( handle );
    }
  }

  if( mActionHandler )
  {
    ret = mActionHandler->AccessibilityActionReadFromTop();
  }

  DALI_LOG_INFO(gAccessibilityAdaptorLogFilter, Debug::General, "[%s:%d] %s\n", __FUNCTION__, __LINE__, ret?"TRUE":"FALSE");

  return ret;
}

bool AccessibilityAdaptor::HandleActionReadFromNextEvent()
{
  bool ret = false;

  Dali::AccessibilityAdaptor handle( this );

  /*
   * In order to application decide reading action first,
   * emit ActionReadFromNext signal in first, AccessibilityActionReadFromNext for handler in next
   */
  if ( !mIndicatorFocused )
  {
    if( !mActionReadFromNextSignal.Empty() )
    {
      mActionReadFromNextSignal.Emit( handle );
    }
  }

  if( mActionHandler )
  {
    ret = mActionHandler->AccessibilityActionReadFromNext();
  }

  DALI_LOG_INFO(gAccessibilityAdaptorLogFilter, Debug::General, "[%s:%d] %s\n", __FUNCTION__, __LINE__, ret?"TRUE":"FALSE");

  return ret;
}

bool AccessibilityAdaptor::HandleActionZoomEvent()
{
  bool ret = false;

  Dali::AccessibilityAdaptor handle( this );

  /*
   * In order to application decide reading action first,
   * emit ActionZoom signal in first, AccessibilityActionZoom for handler in next
   */
  if ( !mIndicatorFocused )
  {
    if( !mActionZoomSignal.Empty() )
    {
      mActionZoomSignal.Emit( handle );
    }
  }

  if( mActionHandler )
  {
    ret = mActionHandler->AccessibilityActionZoom();
  }

  DALI_LOG_INFO(gAccessibilityAdaptorLogFilter, Debug::General, "[%s:%d] %s\n", __FUNCTION__, __LINE__, ret?"TRUE":"FALSE");

  return ret;
}

bool AccessibilityAdaptor::HandleActionReadIndicatorInformationEvent()
{
  bool ret = false;

  Dali::AccessibilityAdaptor handle( this );

  /*
   * In order to application decide reading action first,
   * emit ActionReadIndicatorInformation signal in first, AccessibilityActionReadIndicatorInformation for handler in next
   */
  if ( !mIndicatorFocused )
  {
    if( !mActionReadIndicatorInformationSignal.Empty() )
    {
      mActionReadIndicatorInformationSignal.Emit( handle );
    }
  }

  if( mActionHandler )
  {
    ret = mActionHandler->AccessibilityActionReadIndicatorInformation();
  }

  DALI_LOG_INFO(gAccessibilityAdaptorLogFilter, Debug::General, "[%s:%d] %s\n", __FUNCTION__, __LINE__, ret?"TRUE":"FALSE");

  return ret;
}

bool AccessibilityAdaptor::HandleActionReadPauseResumeEvent()
{
  bool ret = false;

  Dali::AccessibilityAdaptor handle( this );

  /*
   * In order to application decide reading action first,
   * emit ActionReadPauseResume signal in first, AccessibilityActionReadPauseResume for handler in next
   */
  if ( !mIndicatorFocused )
  {
    if( !mActionReadPauseResumeSignal.Empty() )
    {
      mActionReadPauseResumeSignal.Emit( handle );
    }
  }

  if( mActionHandler )
  {
    ret = mActionHandler->AccessibilityActionReadPauseResume();
  }

  DALI_LOG_INFO(gAccessibilityAdaptorLogFilter, Debug::General, "[%s:%d] %s\n", __FUNCTION__, __LINE__, ret?"TRUE":"FALSE");

  return ret;
}

bool AccessibilityAdaptor::HandleActionStartStopEvent()
{
  bool ret = false;

  Dali::AccessibilityAdaptor handle( this );

  /*
   * In order to application decide reading action first,
   * emit ActionStartStop signal in first, AccessibilityActionStartStop for handler in next
   */
  if ( !mIndicatorFocused )
  {
    if( !mActionStartStopSignal.Empty() )
    {
      mActionStartStopSignal.Emit( handle );
    }
  }

  if( mActionHandler )
  {
    ret = mActionHandler->AccessibilityActionStartStop();
  }

  DALI_LOG_INFO(gAccessibilityAdaptorLogFilter, Debug::General, "[%s:%d] %s\n", __FUNCTION__, __LINE__, ret?"TRUE":"FALSE");

  return ret;
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
