#ifndef __DALI_ACCESSIBILITY_GESTURE_HANDLER_H__
#define __DALI_ACCESSIBILITY_GESTURE_HANDLER_H__

/*
Copyright (c) 2000-2013 Samsung Electronics Co., Ltd All Rights Reserved

This file is part of Dali

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

// INTERNAL INCLUDES
#include <dali/integration-api/events/pan-gesture-event.h>

namespace Dali DALI_IMPORT_API
{

/**
 * AccessibilityGestureHandler is an interface used by Dali to handle accessibility gestures
 * passed by the accessibility manager.
 */
class AccessibilityGestureHandler
{
public:

  /**
   * Handle the accessibility pan gesture.
   * @param[in]  panEvent  The pan event to be handled.
   * @param[in]  ppdScale  The PPD scale to be applied while handling the pan gesture.
   * @return whether the gesture is handled successfully or not.
   */
  virtual bool HandlePanGesture(const Integration::PanGestureEvent& panEvent, float ppdScale) = 0;

}; // class AccessibilityGestureHandler

} // namespace Dali

#endif // __DALI_ACCESSIBILITY_GESTURE_HANDLER_H__
