#ifndef __DALI_INTERNAL_ACCESSIBILITY_MANAGER_H__
#define __DALI_INTERNAL_ACCESSIBILITY_MANAGER_H__

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

// EXTERNAL INCLUDES
#include <string>

#include <dali/public-api/object/base-object.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/events/touch-point.h>
#include <dali/integration-api/events/touch-event-combiner.h>

// INTERNAL INCLUDES
#include <dali/public-api/adaptor-framework/common/accessibility-manager.h>
#include <public-api/adaptor-framework/common/accessibility-action-handler.h>
#include <public-api/adaptor-framework/common/accessibility-gesture-handler.h>
#include <internal/common/accessibility-gesture-detector.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

typedef Dali::IntrusivePtr< AccessibilityManager > AccessibilityManagerPtr;

/**
 * This class detects to accessibility action
 */
class AccessibilityManager : public Dali::BaseObject
{
public:

  typedef Dali::AccessibilityManager::AccessibilityActionSignalV2 AccessibilityActionSignalV2;

  // Creation

  /**
   * Create a AccessibilityManager.
   * This should only be called once by the Adaptor class.
   * @return A newly allocated AccessibilityManager.
   */
  static Dali::AccessibilityManager New();

  // Public API

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
   * @copydoc Dali::AccessibilityManager::IsEnabled()
   */
  bool IsEnabled() const;

  /**
   * @copydoc Dali::AccessibilityManager::GetReadPosition() const
   */
  Vector2 GetReadPosition() const;

  /**
   * @copydoc Dali::AccessibilityManager::SetActionHandler()
   */
  void SetActionHandler(AccessibilityActionHandler& handler);

  /**
   * @copydoc Dali::AccessibilityManager::SetGestureHandler()
   */
  void SetGestureHandler(AccessibilityGestureHandler& handler);

  /**
   * Set the PPD scale that should be applied to when processing accessibility gesture.
   * @param[in]  ppdScale  The PPD Scale.
   */
  void SetPpdScale(float ppdScale);

  /**
   * @copydoc Dali::AccessibilityManager::HandleActionNextEvent()
   */
  bool HandleActionNextEvent();

  /**
   * @copydoc Dali::AccessibilityManager::HandleActionPreviousEvent()
   */
  bool HandleActionPreviousEvent();

  /**
   * @copydoc Dali::AccessibilityManager::HandleActionActivateEvent()
   */
  bool HandleActionActivateEvent();

  /**
   * @copydoc Dali::AccessibilityManager::HandleActionReadEvent()
   */
  bool HandleActionReadEvent(unsigned int x, unsigned int y);

  /**
   * @copydoc Dali::AccessibilityManager::HandleActionReadNextEvent()
   */
  bool HandleActionReadNextEvent();

  /**
   * @copydoc Dali::AccessibilityManager::HandleActionReadPreviousEvent()
   */
  bool HandleActionReadPreviousEvent();

  /**
   * @copydoc Dali::AccessibilityManager::HandleActionUpEvent()
   */
  bool HandleActionUpEvent();

  /**
   * @copydoc Dali::AccessibilityManager::HandleActionDownEvent()
   */
  bool HandleActionDownEvent();

  /**
   * @copydoc Dali::AccessibilityManager::HandleActionClearFocusEvent()
   */
  bool HandleActionClearFocusEvent();

  /**
   * @copydoc Dali::AccessibilityManager::HandleActionScrollEvent()
   */
  bool HandleActionScrollEvent(TouchPoint& point, unsigned long timeStamp);

  /**
   * @copydoc Dali::AccessibilityManager::HandleActionBackEvent()
   */
  bool HandleActionBackEvent();

  /**
   * @copydoc Dali::AccessibilityManager::HandleActionEnableEvent()
   */
  void HandleActionEnableEvent();

  /**
   * @copydoc Dali::AccessibilityManager::HandleActionDisableEvent()
   */
  void HandleActionDisableEvent();

public: // Signals

  /**
   * @copydoc Dali::AccessibilityManager::StatusChangedSignal
   */
  AccessibilityActionSignalV2& StatusChangedSignal()
  {
    return mStatusChangedSignalV2;
  }

  /**
   * @copydoc Dali::AccessibilityManager::ActionNextSignal
   */
  AccessibilityActionSignalV2& ActionNextSignal()
  {
    return mActionNextSignalV2;
  }

  /**
   * @copydoc Dali::AccessibilityManager::ActionPreviousSignal
   */
  AccessibilityActionSignalV2& ActionPreviousSignal()
  {
    return mActionPreviousSignalV2;
  }

  /**
   * @copydoc Dali::AccessibilityManager::ActionActivateSignal
   */
  AccessibilityActionSignalV2& ActionActivateSignal()
  {
    return mActionActivateSignalV2;
  }

  /**
   * @copydoc Dali::AccessibilityManager::ActionReadSignal
   */
  AccessibilityActionSignalV2& ActionReadSignal()
  {
    return mActionReadSignalV2;
  }

  /**
   * @copydoc Dali::AccessibilityManager::ActionReadNextSignal
   */
  AccessibilityActionSignalV2& ActionReadNextSignal()
  {
    return mActionReadNextSignalV2;
  }

  /**
   * @copydoc Dali::AccessibilityManager::ActionReadPreviousSignal
   */
  AccessibilityActionSignalV2& ActionReadPreviousSignal()
  {
    return mActionReadPreviousSignalV2;
  }

  /**
   * @copydoc Dali::AccessibilityManager::ActionUpSignal
   */
  AccessibilityActionSignalV2& ActionUpSignal()
  {
    return mActionUpSignalV2;
  }

  /**
   * @copydoc Dali::AccessibilityManager::ActionDownSignal
   */
  AccessibilityActionSignalV2& ActionDownSignal()
  {
    return mActionDownSignalV2;
  }

  /**
   * @copydoc Dali::AccessibilityManager::ActionClearFocusSignal
   */
  AccessibilityActionSignalV2& ActionClearFocusSignal()
  {
    return mActionClearFocusSignalV2;
  }

  /**
   * @copydoc Dali::AccessibilityManager::ActionBackSignal
   */
  AccessibilityActionSignalV2& ActionBackSignal()
  {
    return mActionBackSignalV2;
  }

private:

  // Construction & Destruction

  /**
   * Constructor.
   */
  AccessibilityManager();

  /**
   * Destructor.
   */
  virtual ~AccessibilityManager();

  // Undefined
  AccessibilityManager( const AccessibilityManager& );
  AccessibilityManager& operator=( AccessibilityManager& );

private:

  Dali::Integration::TouchEventCombiner mCombiner; ///< Combines multi-touch events.

  bool mIsEnabled; ///< enable/disable the accessibility action
  Vector2 mReadPosition; ///< ActionRead position

  AccessibilityActionHandler* mActionHandler; ///< The pointer of accessibility action handler

  AccessibilityGestureDetectorPtr mAccessibilityGestureDetector; ///< The accessibility gesture detector

  bool mIndicatorFocused; ///< Whether the Indicator is focused

  AccessibilityActionSignalV2 mStatusChangedSignalV2;
  AccessibilityActionSignalV2 mActionNextSignalV2;
  AccessibilityActionSignalV2 mActionPreviousSignalV2;
  AccessibilityActionSignalV2 mActionActivateSignalV2;
  AccessibilityActionSignalV2 mActionReadSignalV2;
  AccessibilityActionSignalV2 mActionReadNextSignalV2;
  AccessibilityActionSignalV2 mActionReadPreviousSignalV2;
  AccessibilityActionSignalV2 mActionUpSignalV2;
  AccessibilityActionSignalV2 mActionDownSignalV2;
  AccessibilityActionSignalV2 mActionClearFocusSignalV2;
  AccessibilityActionSignalV2 mActionBackSignalV2;

public:

  // Helpers for public-api forwarding methods

  inline static Internal::Adaptor::AccessibilityManager& GetImplementation(Dali::AccessibilityManager& manager)
  {
    DALI_ASSERT_ALWAYS( manager && "AccessibilityManager handle is empty" );

    BaseObject& handle = manager.GetBaseObject();

    return static_cast<Internal::Adaptor::AccessibilityManager&>(handle);
  }

  inline static const Internal::Adaptor::AccessibilityManager& GetImplementation(const Dali::AccessibilityManager& manager)
  {
    DALI_ASSERT_ALWAYS( manager && "AccessibilityManager handle is empty" );

    const BaseObject& handle = manager.GetBaseObject();

    return static_cast<const Internal::Adaptor::AccessibilityManager&>(handle);
  }

};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_ACCESSIBILITY_MANAGER_H__
