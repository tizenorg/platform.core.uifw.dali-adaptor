#ifndef __DALI_TEXT_ABSTRACTION_VIRTUAL_KEYBOARD_INTERFACE_H__
#define __DALI_TEXT_ABSTRACTION_VIRTUAL_KEYBOARD_INTERFACE_H__

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

// EXTERNAL INCLUDES


namespace Dali
{

namespace TextAbstraction
{

class VirtualKeyboardInterface
{
    // Types

    typedef SignalV2< void () > VoidSignalV2;
    typedef SignalV2< void (bool) > StatusSignalV2;

    // Enumerations

    /**
     * @brief The direction of text.
     */
    enum TextDirection
    {
      LeftToRight,
      RightToLeft,
    };

    /**
     * @brief The meaning of the return key.
     */
    enum ReturnKeyType
    {
      DEFAULT,
      DONE,
      GO,
      JOIN,
      LOGIN,
      NEXT,
      SEARCH,
      SEND,
      SIGNIN
    };

    // Functions
    /**
     * @brief Show the virtual keyboard.
     */
    virtual void Show() = 0;

    /**
     * @brief Hide the virtual keyboard.
     */
    virtual void Hide() = 0;

    /**
     * @brief Returns whether the virtual keyboard is visible or not.
     * @return true if visible, false otherwise.
     */
    virtual bool IsVisible() = 0;

    /**
     * @brief Set the specific return key into the virtual keyboard.
     * @param[in] type the kind of return key types.
     */
    virtual void SetReturnKeyType( ReturnKeyType type ) = 0;

    /**
     * @brief Retrieve the current return key type.
     * @return the type of retun key.
     */
    virtual ReturnKeyType GetReturnKeyType() = 0;

    /**
     * @brief Enable/disable prediction (predictive text).
     *
     * By default prediction text is enabled.
     * @param[in] enable true or false to enable or disable
     * Prediction can not be changed while the keyboard is visible. It must be set in advance of showing keyboard.
     */
    virtual void EnablePrediction(const bool enable) = 0;

    /**
     * @brief Returns whether prediction is enabled in the virtual keyboard
     * @return true if predictive text enabled, false otherwise.
     */
    virtual bool IsPredictionEnabled() = 0;

    /**
     * @brief Provides size and position of keyboard.
     *
     * Position is relative to whether keyboard is visible or not.
     * If keyboard is not visible then position will be off the screen.
     * If keyboard is not being shown when this method is called the keyboard is partially setup (IMFContext) to get
     * the values then taken down.  So ideally GetSizeAndPosition() should be called after Show().
     * @return rect which is keyboard panel x, y, width, height
     */
    virtual Dali::Rect<int> GetSizeAndPosition() = 0;

    /**
     * @brief Rotates the keyboard orientation to the given angle.
     *
     * A value of 0 indicates the portrait orientation.
     * Other valid values are 90, 180, 270.
     * @param angle the angle is in degrees.
     */
    virtual void RotateTo(int angle) = 0;

    /**
     * @brief Returns text direction of the keyboard's current input language.
     * @return The direction of the text.
     */
    virtual TextDirection GetTextDirection() = 0;

    /**
     * @brief Connect to this signal to be notified when the virtual keyboard is shown or hidden.
     *
     * A callback of the following type may be connected:
     * @code
     *   void YourCallbackName(bool keyboardShown);
     * @endcode
     * If the parameter keyboardShown is true, then the keyboard has just shown, if it is false, then it
     * has just been hidden.
     * @return The signal to connect to.
     */
    virtual StatusSignalV2& StatusChangedSignal() = 0;

    /**
     * @brief Connect to this signal to be notified when the virtual keyboard is resized.
     *
     * A callback of the following type may be connected:
     * @code
     *   void YourCallbackName();
     * @endcode
     * User can get changed size by using GetSizeAndPosition() in the callback
     * @return The signal to connect to.
     */
    virtual VoidSignalV2& ResizedSignal() = 0;

    /**
     * @brief Connect to this signal to be notified when the virtual keyboard's language is changed.
     *
     * A callback of the following type may be connected:
     * @code
     *   void YourCallbackName();
     * @endcode
     * User can get the text direction of the language by calling GetTextDirection() in the callback.
     * @return The signal to connect to.
     */
    virtual VoidSignalV2& LanguageChangedSignal() = 0;


protected:

    VirtualKeyboardInterface() {}

    virtual ~VirtualKeyboardInterface() {}

  // Undefined copy constructor.
    VirtualKeyboardInterface( const VirtualKeyboardInterface& );

  // Undefined assignment operator.
    VirtualKeyboardInterface& operator=( const VirtualKeyboardInterface& );
};


// TBD
} // namespace TextAbstraction

} // namespace Dali

#endif // __DALI_PLATFORM_TEXT_ABSTRACTION_H__
