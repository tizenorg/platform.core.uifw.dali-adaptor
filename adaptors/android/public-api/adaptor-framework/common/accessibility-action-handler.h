#ifndef __DALI_ACCESSIBILITY_ACTION_HANDLER_H__
#define __DALI_ACCESSIBILITY_ACTION_HANDLER_H__

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

namespace Dali DALI_IMPORT_API
{

/**
 * AccessibilityActionHandler is an abstract interface, used by Dali to handle accessibility actions
 * passed by the accessibility manager.
 */
class AccessibilityActionHandler
{
public:

  /**
   * Change the accessibility status when Accessibility feature(screen-reader) turned on or off.
   * @return whether the status is changed or not.
   */
  virtual bool ChangeAccessibilityStatus() = 0;

  /**
   * Clear the accessibility focus from the current focused actor.
   * @return whether the focus is cleared or not.
   */
  virtual bool ClearAccessibilityFocus() = 0;

  /**
   * Perform the accessibility action to move focus to the previous focusable actor (by one finger flick up).
   * @return whether the accessibility action is performed or not.
   */
  virtual bool AccessibilityActionPrevious() = 0;

  /**
   * Perform the accessibility action to move focus to the next focusable actor (by one finger flick down).
   * @return whether the accessibility action is performed or not.
   */
  virtual bool AccessibilityActionNext() = 0;

  /**
   * Perform the accessibility action to move focus to the previous focusable actor (by one finger flick left).
   * @return whether the accessibility action is performed or not.
   */
  virtual bool AccessibilityActionReadPrevious() = 0;

  /**
   * Perform the accessibility action to move focus to the next focusable actor (by one finger flick right).
   * @return whether the accessibility action is performed or not.
   */
  virtual bool AccessibilityActionReadNext() = 0;

  /**
   * Perform the accessibility action to focus and read the actor (by one finger move).
   * @return whether the accessibility action is performed or not.
   */
  virtual bool AccessibilityActionRead() = 0;

  /**
   * Perform the accessibility action to activate the current focused actor (by one finger double tap).
   * @return whether the accessibility action is performed or not.
   */
  virtual bool AccessibilityActionActivate() = 0;

  /**
   * Perform the accessibility action to change the value when the current focused actor is a slider
   * (by double finger down and move up and right).
   * @return whether the accessibility action is performed or not.
   */
  virtual bool AccessibilityActionUp() = 0;

  /**
   * Perform the accessibility action to change the value when the current focused actor is a slider
   * (by double finger down and move down and left).
   * @return whether the accessibility action is performed or not.
   */
  virtual bool AccessibilityActionDown() = 0;

  /**
   * Perform the accessibility action to navigate back (by two fingers circle draw).
   * @return whether the accessibility action is performed or not.
   */
  virtual bool AccessibilityActionBack() = 0;

}; // class AccessibilityActionHandler

} // namespace Dali

#endif // __DALI_ACCESSIBILITY_ACTION_HANDLER_H__
