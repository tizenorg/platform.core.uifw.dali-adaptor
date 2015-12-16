#ifndef __DALI_INTERNAL_ACCESSIBILITY_ADAPTOR_H__
#define __DALI_INTERNAL_ACCESSIBILITY_ADAPTOR_H__

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

// EXTERNAL INCLUDES
#include <string>

#include <dali/public-api/object/base-object.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/events/touch-point.h>
#include <dali/integration-api/events/touch-event-combiner.h>

// INTERNAL INCLUDES
#include <accessibility-adaptor.h>
#include <accessibility-action-handler.h>
#include <accessibility-gesture-handler.h>
#include <indicator-interface.h>
#include <accessibility-gesture-detector.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

/**
 * This class detects to accessibility action
 */
class AccessibilityAdaptor : public Dali::BaseObject
{
public:

  /**
   * Constructor.
   */
  AccessibilityAdaptor();

  /**
   * @brief Get an instance of the AccessibilityAdaptor.
   *
   * @note This singleton-style getter can be reimplemented for different platforms.
   * The default implementation is in accessibility-adaptor-singleton-default.cpp.
   * @return The instance of the AccessibilityAdaptor.
   */
  static Dali::AccessibilityAdaptor Get();

  /**
   * Turn on accessibility action
   * This method should be called by vconf callback
   */
  void EnableAccessibility();

  /**
   * Turn off accessibility action
   * This method should be called by vconf callback
   */
  void DisableAccessibility();

  /**
   * @copydoc Dali::AccessibilityAdaptor::IsEnabled()
   */
  bool IsEnabled() const;

  /**
   * @copydoc Dali::AccessibilityAdaptor::GetReadPosition() const
   */
  Vector2 GetReadPosition() const;

  /**
   * @copydoc Dali::AccessibilityAdaptor::SetActionHandler()
   */
  void SetActionHandler(AccessibilityActionHandler& handler);

  /**
   * @copydoc Dali::AccessibilityAdaptor::SetGestureHandler()
   */
  void SetGestureHandler(AccessibilityGestureHandler& handler);

  /**
   * @brief Set the Indicator
   * @param[in] Indiciator interface
   */
  void SetIndicator( IndicatorInterface* indicator );

  /**
   * @copydoc Dali::AccessibilityAdaptor::HandleActionNextEvent()
   */
  virtual bool HandleActionNextEvent( bool allowEndFeedback );

  /**
   * @copydoc Dali::AccessibilityAdaptor::HandleActionPreviousEvent()
   */
  virtual bool HandleActionPreviousEvent( bool allowEndFeedback );

  /**
   * @copydoc Dali::AccessibilityAdaptor::HandleActionActivateEvent()
   */
  virtual bool HandleActionActivateEvent();

  /**
   * @copydoc Dali::AccessibilityAdaptor::HandleActionReadEvent()
   */
  virtual bool HandleActionReadEvent( unsigned int x, unsigned int y, bool allowReadAgain );

  /**
   * @copydoc Dali::AccessibilityAdaptor::HandleActionReadNextEvent()
   */
  virtual bool HandleActionReadNextEvent( bool allowEndFeedback );

  /**
   * @copydoc Dali::AccessibilityAdaptor::HandleActionReadPreviousEvent()
   */
  virtual bool HandleActionReadPreviousEvent( bool allowEndFeedback );

  /**
   * @copydoc Dali::AccessibilityAdaptor::HandleActionUpEvent()
   */
  virtual bool HandleActionUpEvent();

  /**
   * @copydoc Dali::AccessibilityAdaptor::HandleActionDownEvent()
   */
  virtual bool HandleActionDownEvent();

  /**
   * @copydoc Dali::AccessibilityAdaptor::HandleActionClearFocusEvent()
   */
  bool HandleActionClearFocusEvent();

  /**
   * @copydoc Dali::AccessibilityAdaptor::HandleActionScrollEvent()
   */
  bool HandleActionScrollEvent(const TouchPoint& point, unsigned long timeStamp);

  /**
   * @copydoc Dali::AccessibilityAdaptor::HandleActionTouchEvent()
   */
  bool HandleActionTouchEvent(const TouchPoint& point, unsigned long timeStamp);

  /**
   * @copydoc Dali::AccessibilityAdaptor::HandleActionBackEvent()
   */
  bool HandleActionBackEvent();

  /**
   * @copydoc Dali::AccessibilityAdaptor::HandleActionEnableEvent()
   */
  void HandleActionEnableEvent();

  /**
   * @copydoc Dali::AccessibilityAdaptor::HandleActionDisableEvent()
   */
  void HandleActionDisableEvent();

  /**
   * @copydoc Dali::AccessibilityAdaptor::HandleActionScrollUpEvent()
   */
  bool HandleActionScrollUpEvent();

  /**
   * @copydoc Dali::AccessibilityAdaptor::HandleActionScrollDownEvent()
   */
  bool HandleActionScrollDownEvent();

  /**
   * @copydoc Dali::AccessibilityAdaptor::HandleActionPageLeftEvent()
   */
  bool HandleActionPageLeftEvent();

  /**
   * @copydoc Dali::AccessibilityAdaptor::HandleActionPageRightEvent()
   */
  bool HandleActionPageRightEvent();

  /**
   * @copydoc Dali::AccessibilityAdaptor::HandleActionPageUpEvent()
   */
  bool HandleActionPageUpEvent();

  /**
   * @copydoc Dali::AccessibilityAdaptor::HandleActionPageDownEvent()
   */
  bool HandleActionPageDownEvent();

  /**
   * @copydoc Dali::AccessibilityAdaptor::HandleActionMoveToFirstEvent()
   */
  bool HandleActionMoveToFirstEvent();

  /**
   * @copydoc Dali::AccessibilityAdaptor::HandleActionMoveToLastEvent()
   */
  bool HandleActionMoveToLastEvent();

  /**
   * @copydoc Dali::AccessibilityAdaptor::HandleActionReadFromTopEvent()
   */
  bool HandleActionReadFromTopEvent();

  /**
   * @copydoc Dali::AccessibilityAdaptor::HandleActionReadFromNextEvent()
   */
  bool HandleActionReadFromNextEvent();

  /**
   * @copydoc Dali::AccessibilityAdaptor::HandleActionZoomEvent()
   */
  bool HandleActionZoomEvent();

  /**
   * @copydoc Dali::AccessibilityAdaptor::HandleActionReadIndicatorInformationEvent()
   */
  bool HandleActionReadIndicatorInformationEvent();

  /**
   * @copydoc Dali::AccessibilityAdaptor::HandleActionReadPauseResumeEvent()
   */
  bool HandleActionReadPauseResumeEvent();

  /**
   * @copydoc Dali::AccessibilityAdaptor::HandleActionStartStopEvent()
   */
  bool HandleActionStartStopEvent();

private:

  /**
   * @brief Called when the singleton is destroyed.
   *
   * @note This can be reimplemented for different platforms.
   * The default implementation is in accessibility-adaptor-singleton-default.cpp.
   * @return The instance of the AccessibilityAdaptor.
   */
  static void OnDestroy();

  /**
   * Destructor.
   */
  virtual ~AccessibilityAdaptor();

  // Undefined
  AccessibilityAdaptor( const AccessibilityAdaptor& );
  AccessibilityAdaptor& operator=( AccessibilityAdaptor& );

protected:

  Dali::Integration::TouchEventCombiner mCombiner; ///< Combines multi-touch events.

  Vector2 mReadPosition; ///< ActionRead position

  AccessibilityActionHandler* mActionHandler; ///< The pointer of accessibility action handler

  AccessibilityGestureDetectorPtr mAccessibilityGestureDetector; ///< The accessibility gesture detector

  IndicatorInterface* mIndicator; ///< The indicator

  bool mIsEnabled        : 1; ///< enable/disable the accessibility action
  bool mIndicatorFocused : 1; ///< Whether the Indicator is focused

public:

  // Helpers for public-api forwarding methods

  inline static Internal::Adaptor::AccessibilityAdaptor& GetImplementation(Dali::AccessibilityAdaptor& adaptor)
  {
    DALI_ASSERT_ALWAYS( adaptor && "AccessibilityAdaptor handle is empty" );

    BaseObject& handle = adaptor.GetBaseObject();

    return static_cast<Internal::Adaptor::AccessibilityAdaptor&>(handle);
  }

  inline static const Internal::Adaptor::AccessibilityAdaptor& GetImplementation(const Dali::AccessibilityAdaptor& adaptor)
  {
    DALI_ASSERT_ALWAYS( adaptor && "AccessibilityAdaptor handle is empty" );

    const BaseObject& handle = adaptor.GetBaseObject();

    return static_cast<const Internal::Adaptor::AccessibilityAdaptor&>(handle);
  }

};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_ACCESSIBILITY_ADAPTOR_H__
