#ifndef __DALI_VIRTUAL_KEYBOARD_H__
#define __DALI_VIRTUAL_KEYBOARD_H__

/*
Copyright (c) 2000-2012 Samsung Electronics Co., Ltd All Rights Reserved

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
#include <dali/public-api/signals/dali-signal-v2.h>
#include <dali/public-api/math/rect.h>

namespace Dali DALI_IMPORT_API
{

/**
 * This namespace is provided for application developers to be able to show and hide the on-screen
 * keyboard. Key events are sent to the actor in focus. Focus is set through the actor Public API.
 */
namespace VirtualKeyboard
{

// Types

typedef SignalV2< void () > VoidSignalV2;
typedef SignalV2< void (bool) > StatusSignalV2;

//typedef boost::signals2::signal_type< void (bool), DummyMutex >::type StatusSignalType;
//typedef boost::function<void (bool)> StatusCallbackType;
//typedef Signal<StatusSignalType, StatusCallbackType> StatusSignal;

//typedef boost::signals2::signal_type< void (void), DummyMutex >::type SignalType;
//typedef boost::function<void (void)> CallbackType;
//typedef Signal<SignalType, CallbackType> Signal;

// Enumerations

enum TextDirection
{
  LeftToRight,
  RightToLeft,
};

// Functions

/**
 * Show the virtual keyboard.
 */
void Show();

/**
 * Hide the virtual keyboard.
 */
void Hide();

/**
 * Returns whether the virtual keyboard is visible or not.
 * @return true if visible, false otherwise.
 */
bool IsVisible();

/**
 * Enable/disable prediction (predictive text)
 * By default prediction text is enabled.
 * @param[in] enable true or false to enable or disable
 * Prediction can not be changed while the keyboard is visible. It must be set in advance of showing keyboard.
 */
void EnablePrediction(const bool enable);

/**
 * Returns whether prediction is enabled in the virtual keyboard
 * @return true if predictive text enabled, false otherwise.
 */
bool IsPredictionEnabled();

/*
 * Provides size and position of keyboard, position is relative to whether keyboard is visible or not.
 * If keyboard is not visible then position will be off the screen.
 * If keyboard is not being shown when this method is called the keyboard is partially setup (IMFContext) to get
 * the values then taken down.  So ideally GetSizeAndPosition() should be called after Show().
 * @return rect which is keyboard panel x, y, width, height
 */
Dali::Rect<int> GetSizeAndPosition();

/**
 * Rotates the keyboards orientation to the given angle
 * @param angle the angle is degrees the keyboard should be at 0 would be portrait then 90, 180, 270
 */
void RotateTo(int angle);

/**
 * Notifies IMF context that the cursor position has changed, required for features like auto-capitalisation
 */
void NotifyCursorPosition();

/**
 * Notifies IMF context that the cursor has moved and pre-edit state will need to be reset.
 */
void IMFKeyboardReset();

/**
 * Sets cursor position stored in VirtualKeyboard, this is required by the IMF context
 * @param[in] cursorPosition position of cursor
 */
void SetCursorPosition( unsigned int cursorPosition );

/**
 * Gets cursor position stored in VirtualKeyboard, this is required by the IMF context
 * @return current position of cursor
 */
int GetCursorPosition();

/**
 * Method to send a string to the virtual keyboard
 * @param[in] text The text string surrounding the current cursor point.
 */
void SetSurroundingText( std::string text );

/**
 * Method to get a string from the virtual keyboard
 * @return string stored in VirtualKeyboard
 */
std::string GetSurroundingText();

/**
 * Returns text direction of the keyboard's current input language.
 * @return The direction of the text.
 */
TextDirection GetTextDirection();

/**
 * Connect to this signal to be notified when the virtual keyboard is shown or hidden.
 * A callback of the following type may be connected:
 * @code
 *   void YourCallbackName(bool keyboardShown);
 * @endcode
 * If the parameter keyboardShown is true, then the keyboard has just shown, if it is false, then it
 * has just been hidden.
 * @return The signal to connect to.
 */
//StatusSignal SignalStatusChanged();
StatusSignalV2& StatusChangedSignal();

/**
 * Connect to this signal to be notified when the virtual keyboard is reszied.
 * A callback of the following type may be connected:
 * @code
 *   void YourCallbackName();
 * @endcode
 * User can get changed size by using GetSizeAndPosition() in the callback
 * @return The signal to connect to.
 */
//Signal SignalResized();
VoidSignalV2& ResizedSignal();

/**
 * Connect to this signal to be notified when the virtual keyboard's language is changed.
 * A callback of the following type may be connected:
 * @code
 *   void YourCallbackName();
 * @endcode
 * User can get the text direction of the language by calling GetTextDirection() in the callback.
 * @return The signal to connect to.
 */
//Signal SignalLanguageChanged();
VoidSignalV2& LanguageChangedSignal();

} // namespace VirtualKeyboard

} // namespace Dali

#endif // __DALI_VIRTUAL_KEYBOARD_H__
