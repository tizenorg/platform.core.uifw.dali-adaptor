//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <iostream>

#include <stdlib.h>
#include <dali/dali.h>
#include <dali-test-suite-utils.h>
#include <Ecore_X.h>

using namespace Dali;

static Dali::Window* window;
static Dali::Adaptor* adaptor;

void utc_virtual_keyboard_startup(void)
{
    ecore_x_init(NULL);
    Dali::PositionSize positionSize(0, 0, 800, 1280);
    window = new Dali::Window(Dali::Window::New(positionSize, "MyApp"));
    adaptor = &Dali::Adaptor::New(*window);

    test_return_value = TET_UNDEF;
}

void utc_virtual_keyboard_cleanup(void)
{
    delete window;
    delete adaptor;
    ecore_x_shutdown();

    test_return_value = TET_PASS;
}

/**
 * @brief showing virtual keyboard
 *
 * @procedure
 * @step 1 hiding virtual keyboard if it is visible.
 * @step 2 showing hide virtual keyboard.
 * @step 3 showing already showed keyboard.
 *
 * @Passcondition: if keyboard successfully showed.
 * @id DA-153
 */
int UtcDaliVirtualKeyboardShow_p_01(void)
{
    if (VirtualKeyboard::IsVisible())
    {
        VirtualKeyboard::Hide();
    }

    VirtualKeyboard::Show();
    DALI_TEST_CHECK(VirtualKeyboard::IsVisible());
    VirtualKeyboard::Show();
    DALI_TEST_CHECK(VirtualKeyboard::IsVisible());

    END_TEST;
}

/**
 * @brief calling Dali::VirtualKeyboard::Show() in case of no adaptor
 *
 * @procedure
 * @step 1 deleting existed adaptor.
 * @step 2 calling the method.
 * @step 3 creating new adaptor.
 *
 * @Passcondition: if exception is thrown.
 * @id DA-153
 */
int UtcDaliVirtualKeyboardShow_n_01(void)
{
    delete window;
    delete adaptor;
    bool isCaught = false;

    try
    {
        VirtualKeyboard::Show();
    }
    catch (Dali::DaliException& e)
    {
        isCaught = true;
    }

    DALI_TEST_CHECK(isCaught);

    Dali::PositionSize positionSize(0, 0, 800, 1280);
    window = new Dali::Window(Dali::Window::New(positionSize, "MyApp"));
    adaptor = &Dali::Adaptor::New(*window);

    END_TEST;
}

/**
 * @brief hiding virtual keyboard
 *
 * @procedure
 * @step 1 showing virtual keyboard if it is visible.
 * @step 2 hiding hide virtual keyboard.
 * @step 3 hiding already hidden keyboard.
 *
 * @Passcondition: if keyboard successfully hide.
 * @id DA-154
 */
int UtcDaliVirtualKeyboardHide_p_01(void)
{
    if (!VirtualKeyboard::IsVisible())
    {
        VirtualKeyboard::Show();
    }

    VirtualKeyboard::Hide();
    DALI_TEST_CHECK(!VirtualKeyboard::IsVisible());
    VirtualKeyboard::Hide();
    DALI_TEST_CHECK(!VirtualKeyboard::IsVisible());

    END_TEST;
}

/**
 * @brief calling Dali::VirtualKeyboard::Hide() in case of no adaptor
 *
 * @procedure
 * @step 1 deleting existed adaptor.
 * @step 2 calling the method.
 * @step 3 creating new adaptor.
 *
 * @Passcondition: if exception is thrown.
 * @id DA-154
 */
int UtcDaliVirtualKeyboardHide_n_01(void)
{
    delete window;
    delete adaptor;
    bool isCaught = false;

    try
    {
        VirtualKeyboard::Hide();
    }
    catch (Dali::DaliException& e)
    {
        isCaught = true;
    }

    DALI_TEST_CHECK(isCaught);

    Dali::PositionSize positionSize(0, 0, 800, 1280);
    window = new Dali::Window(Dali::Window::New(positionSize, "MyApp"));
    adaptor = &Dali::Adaptor::New(*window);

    END_TEST;
}

/**
 * @brief calling IsVisible() method for different
 * show state of virtual keyboard
 *
 * @procedure
 * @step 1 showing keyboard and checking its state.
 * @step 2 showing keyboard and checking its state.
 * @step 3 again showing keyboard and checking its state.
 *
 * @Passcondition: if keyboard successfully hide.
 * @id DA-155
 */
int UtcDaliVirtualKeyboardIsVisible_p_01(void)
{
    VirtualKeyboard::Show();
    DALI_TEST_CHECK(VirtualKeyboard::IsVisible());
    VirtualKeyboard::Hide();
    DALI_TEST_CHECK(!VirtualKeyboard::IsVisible());
    VirtualKeyboard::Show();
    DALI_TEST_CHECK(VirtualKeyboard::IsVisible());

    END_TEST;
}

/**
 * @brief calling Dali::VirtualKeyboard::IsVisible() in case of no adaptor
 *
 * @procedure
 * @step 1 deleting existed adaptor.
 * @step 2 calling the method.
 * @step 3 creating new adaptor.
 *
 * @Passcondition: if exception is thrown.
 * @id DA-155
 */
int UtcDaliVirtualKeyboardIsVisible_n_01(void)
{
    delete window;
    delete adaptor;
    bool isCaught = false;

    try
    {
        VirtualKeyboard::IsVisible();
    }
    catch (Dali::DaliException& e)
    {
        isCaught = true;
    }

    DALI_TEST_CHECK(isCaught);

    Dali::PositionSize positionSize(0, 0, 800, 1280);
    window = new Dali::Window(Dali::Window::New(positionSize, "MyApp"));
    adaptor = &Dali::Adaptor::New(*window);

    END_TEST;
}

/**
 * @brief setting VirtualKeyboard::DEFAULT return key type.
 *
 * @procedure
 * @step 1 set VirtualKeyboard::DEFAULT return key type.
 * @step 2 get return key type.
 *
 * @Passcondition: if set and get values are equal.
 * @id DA-156
 */
int UtcDaliVirtualKeyboardSetReturnKeyType_p_01(void)
{
    VirtualKeyboard::ReturnKeyType value = VirtualKeyboard::DEFAULT;
    VirtualKeyboard::SetReturnKeyType(value);
    DALI_TEST_CHECK( VirtualKeyboard::GetReturnKeyType() == value);
    END_TEST;
}

/**
 * @brief setting VirtualKeyboard::DONE return key type.
 *
 * @procedure
 * @step 1 set VirtualKeyboard::DONE return key type.
 * @step 2 get return key type.
 *
 * @Passcondition: if set and get values are equal.
 * @id DA-156
 */
int UtcDaliVirtualKeyboardSetReturnKeyType_p_02(void)
{
    VirtualKeyboard::ReturnKeyType value = VirtualKeyboard::DONE;
    VirtualKeyboard::SetReturnKeyType(value);
    DALI_TEST_CHECK( VirtualKeyboard::GetReturnKeyType() == value);
    END_TEST;
}

/**
 * @brief setting VirtualKeyboard::GO return key type.
 *
 * @procedure
 * @step 1 set VirtualKeyboard::GO return key type.
 * @step 2 get return key type.
 *
 * @Passcondition: if set and get values are equal.
 * @id DA-156
 */
int UtcDaliVirtualKeyboardSetReturnKeyType_p_03(void)
{
    VirtualKeyboard::ReturnKeyType value = VirtualKeyboard::GO;
    VirtualKeyboard::SetReturnKeyType(value);
    DALI_TEST_CHECK( VirtualKeyboard::GetReturnKeyType() == value);
    END_TEST;
}

/**
 * @brief setting VirtualKeyboard::JOIN return key type.
 *
 * @procedure
 * @step 1 set VirtualKeyboard::JOIN return key type.
 * @step 2 get return key type.
 *
 * @Passcondition: if set and get values are equal.
 * @id DA-156
 */
int UtcDaliVirtualKeyboardSetReturnKeyType_p_04(void)
{
    VirtualKeyboard::ReturnKeyType value = VirtualKeyboard::JOIN;
    VirtualKeyboard::SetReturnKeyType(value);
    DALI_TEST_CHECK( VirtualKeyboard::GetReturnKeyType() == value);
    END_TEST;
}

/**
 * @brief setting VirtualKeyboard::LOGIN return key type.
 *
 * @procedure
 * @step 1 set VirtualKeyboard::LOGIN return key type.
 * @step 2 get return key type.
 *
 * @Passcondition: if set and get values are equal.
 * @id DA-156
 */
int UtcDaliVirtualKeyboardSetReturnKeyType_p_05(void)
{
    VirtualKeyboard::ReturnKeyType value = VirtualKeyboard::LOGIN;
    VirtualKeyboard::SetReturnKeyType(value);
    DALI_TEST_CHECK( VirtualKeyboard::GetReturnKeyType() == value);
    END_TEST;
}

/**
 * @brief setting VirtualKeyboard::NEXT return key type.
 *
 * @procedure
 * @step 1 set VirtualKeyboard::NEXT return key type.
 * @step 2 get return key type.
 *
 * @Passcondition: if set and get values are equal.
 * @id DA-156
 */
int UtcDaliVirtualKeyboardSetReturnKeyType_p_06(void)
{
    VirtualKeyboard::ReturnKeyType value = VirtualKeyboard::NEXT;
    VirtualKeyboard::SetReturnKeyType(value);
    DALI_TEST_CHECK( VirtualKeyboard::GetReturnKeyType() == value);
    END_TEST;
}

/**
 * @brief setting VirtualKeyboard::SEARCH return key type.
 *
 * @procedure
 * @step 1 set VirtualKeyboard::SEARCH return key type.
 * @step 2 get return key type.
 *
 * @Passcondition: if set and get values are equal.
 * @id DA-156
 */
int UtcDaliVirtualKeyboardSetReturnKeyType_p_07(void)
{
    VirtualKeyboard::ReturnKeyType value = VirtualKeyboard::SEARCH;
    VirtualKeyboard::SetReturnKeyType(value);
    DALI_TEST_CHECK( VirtualKeyboard::GetReturnKeyType() == value);
    END_TEST;
}

/**
 * @brief setting VirtualKeyboard::SEND return key type.
 *
 * @procedure
 * @step 1 set VirtualKeyboard::SEND return key type.
 * @step 2 get return key type.
 *
 * @Passcondition: if set and get values are equal.
 * @id DA-156
 */
int UtcDaliVirtualKeyboardSetReturnKeyType_p_08(void)
{
    VirtualKeyboard::ReturnKeyType value = VirtualKeyboard::SEND;
    VirtualKeyboard::SetReturnKeyType(value);
    DALI_TEST_CHECK( VirtualKeyboard::GetReturnKeyType() == value);
    END_TEST;
}

/**
 * @brief setting VirtualKeyboard::SIGNIN return key type.
 *
 * @procedure
 * @step 1 set VirtualKeyboard::SIGNIN return key type.
 * @step 2 get return key type.
 *
 * @Passcondition: if set and get values are equal.
 * @id DA-156
 */
int UtcDaliVirtualKeyboardSetReturnKeyType_p_09(void)
{
    VirtualKeyboard::ReturnKeyType value = VirtualKeyboard::SIGNIN;
    VirtualKeyboard::SetReturnKeyType(value);
    DALI_TEST_CHECK( VirtualKeyboard::GetReturnKeyType() == value);
    END_TEST;
}

/**
 * @brief calling Dali::VirtualKeyboard::SetReturnKeyType() in case of no adaptor
 *
 * @procedure
 * @step 1 deleting existed adaptor.
 * @step 2 calling the method.
 * @step 3 creating new adaptor.
 *
 * @Passcondition: if exception is thrown.
 * @id DA-156
 */
int UtcDaliVirtualKeyboardSetReturnKeyType_n_01(void)
{
    delete window;
    delete adaptor;
    bool isCaught = false;

    try
    {
        VirtualKeyboard::SetReturnKeyType(VirtualKeyboard::DEFAULT);
    }
    catch (Dali::DaliException& e)
    {
        isCaught = true;
    }

    DALI_TEST_CHECK(isCaught);

    Dali::PositionSize positionSize(0, 0, 800, 1280);
    window = new Dali::Window(Dali::Window::New(positionSize, "MyApp"));
    adaptor = &Dali::Adaptor::New(*window);

    END_TEST;
}

/**
 * @brief getting different return key types of virtual keyboard.
 *
 * @procedure
 * @step 1 set different return key types of virtual keyboard.
 * @step 2 get return key type.
 *
 * @Passcondition: if set and get values are equal.
 * @id DA-157
 */
int UtcDaliVirtualKeyboardGetReturnKeyType_p_01(void)
{
    VirtualKeyboard::ReturnKeyType initialType = VirtualKeyboard::GetReturnKeyType();

    VirtualKeyboard::SetReturnKeyType(VirtualKeyboard::DEFAULT);
    DALI_TEST_CHECK( VirtualKeyboard::GetReturnKeyType() == VirtualKeyboard::DEFAULT );
    VirtualKeyboard::SetReturnKeyType(VirtualKeyboard::DONE);
    DALI_TEST_CHECK( VirtualKeyboard::GetReturnKeyType() == VirtualKeyboard::DONE );
    VirtualKeyboard::SetReturnKeyType(VirtualKeyboard::GO);
    DALI_TEST_CHECK( VirtualKeyboard::GetReturnKeyType() == VirtualKeyboard::GO );
    VirtualKeyboard::SetReturnKeyType(VirtualKeyboard::JOIN);
    DALI_TEST_CHECK( VirtualKeyboard::GetReturnKeyType() == VirtualKeyboard::JOIN );
    VirtualKeyboard::SetReturnKeyType(VirtualKeyboard::LOGIN);
    DALI_TEST_CHECK( VirtualKeyboard::GetReturnKeyType() == VirtualKeyboard::LOGIN );
    VirtualKeyboard::SetReturnKeyType(VirtualKeyboard::NEXT);
    DALI_TEST_CHECK( VirtualKeyboard::GetReturnKeyType() == VirtualKeyboard::NEXT );
    VirtualKeyboard::SetReturnKeyType(VirtualKeyboard::SEARCH);
    DALI_TEST_CHECK( VirtualKeyboard::GetReturnKeyType() == VirtualKeyboard::SEARCH );
    VirtualKeyboard::SetReturnKeyType(VirtualKeyboard::SEND);
    DALI_TEST_CHECK( VirtualKeyboard::GetReturnKeyType() == VirtualKeyboard::SEND );
    VirtualKeyboard::SetReturnKeyType(VirtualKeyboard::SIGNIN);
    DALI_TEST_CHECK( VirtualKeyboard::GetReturnKeyType() == VirtualKeyboard::SIGNIN );
    VirtualKeyboard::SetReturnKeyType(initialType);
    DALI_TEST_CHECK( VirtualKeyboard::GetReturnKeyType() == initialType );
    END_TEST;
}

/**
 * @brief calling Dali::VirtualKeyboard::GetReturnKeyType() in case of no adaptor
 *
 * @procedure
 * @step 1 deleting existed adaptor.
 * @step 2 calling the method.
 * @step 3 creating new adaptor.
 *
 * @Passcondition: if exception is thrown.
 * @id DA-157
 */
int UtcDaliVirtualKeyboardGetReturnKeyType_n_01(void)
{
    delete window;
    delete adaptor;
    bool isCaught = false;

    try
    {
        VirtualKeyboard::GetReturnKeyType();
    }
    catch (Dali::DaliException& e)
    {
        isCaught = true;
    }

    DALI_TEST_CHECK(isCaught);

    Dali::PositionSize positionSize(0, 0, 800, 1280);
    window = new Dali::Window(Dali::Window::New(positionSize, "MyApp"));
    adaptor = &Dali::Adaptor::New(*window);

    END_TEST;
}

/**
 * @brief enabling and disabling prediction by using VirtualKeyboard::EnablePrediction() method.
 *
 * @procedure
 * @step 1 enabling prediction of virtual keyboard and check it.
 * @step 2 disabling prediction of virtual keyboard and check it.
 * @step 3 enabling prediction of virtual keyboard and check it.
 *
 * @Passcondition: if set and get values are equal.
 * @id DA-158
 */
int UtcDaliVirtualKeyboardEnablePrediction_p_01(void)
{
    VirtualKeyboard::EnablePrediction(true);
    DALI_TEST_CHECK( VirtualKeyboard::IsPredictionEnabled() == true);
    VirtualKeyboard::EnablePrediction(false);
    DALI_TEST_CHECK( VirtualKeyboard::IsPredictionEnabled() == false);
    VirtualKeyboard::EnablePrediction(true);
    DALI_TEST_CHECK( VirtualKeyboard::IsPredictionEnabled() == true);
    END_TEST;
}

/**
 * @brief calling Dali::VirtualKeyboard::EnablePrediction() in case of no adaptor
 *
 * @procedure
 * @step 1 deleting existed adaptor.
 * @step 2 calling the method.
 * @step 3 creating new adaptor.
 *
 * @Passcondition: if exception is thrown.
 * @id DA-158
 */
int UtcDaliVirtualKeyboardEnablePrediction_n_01(void)
{
    delete window;
    delete adaptor;
    bool isCaught = false;

    try
    {
        VirtualKeyboard::EnablePrediction(true);
    }
    catch (Dali::DaliException& e)
    {
        isCaught = true;
    }

    DALI_TEST_CHECK(isCaught);

    Dali::PositionSize positionSize(0, 0, 800, 1280);
    window = new Dali::Window(Dali::Window::New(positionSize, "MyApp"));
    adaptor = &Dali::Adaptor::New(*window);

    END_TEST;
}

/**
 * @brief checking is prediction enabled by using VirtualKeyboard::IsPredictionEnabled() method.
 *
 * @procedure
 * @step 1 get init prediction state of virtual keyboard.
 * @step 2 set opposite prediction state.
 * @step 3 get new prediction state.
 *
 * @Passcondition: if new prediction state equals to setted one.
 * @id DA-159
 */
int UtcDaliVirtualKeyboardIsPredictionEnabled_p_01(void)
{
    bool initialState = VirtualKeyboard::IsPredictionEnabled();

    VirtualKeyboard::EnablePrediction(!initialState);
    DALI_TEST_CHECK( VirtualKeyboard::IsPredictionEnabled() == !initialState);

    END_TEST;
}

/**
 * @brief calling Dali::VirtualKeyboard::IsPredictionEnabled() in case of no adaptor
 *
 * @procedure
 * @step 1 deleting existed adaptor.
 * @step 2 calling the method.
 * @step 3 creating new adaptor.
 *
 * @Passcondition: if exception is thrown.
 * @id DA-159
 */
int UtcDaliVirtualKeyboardIsPredictionEnabled_n_01(void)
{
    delete window;
    delete adaptor;
    bool isCaught = false;

    try
    {
        VirtualKeyboard::IsPredictionEnabled();
    }
    catch (Dali::DaliException& e)
    {
        isCaught = true;
    }

    DALI_TEST_CHECK(isCaught);

    Dali::PositionSize positionSize(0, 0, 800, 1280);
    window = new Dali::Window(Dali::Window::New(positionSize, "MyApp"));
    adaptor = &Dali::Adaptor::New(*window);

    END_TEST;
}

/**
 * @brief getting size and position of keyboard.
 *
 * @procedure
 * @step 1 hiding the keyboard.
 * @step 2 get size and position of the keyboard.
 * @step 3 showing the keyboard.
 * @step 4 get size and position of the keyboard.
 *
 * @Passcondition: if returned rectangles are not empty.
 * @id DA-160
 */
int UtcDaliVirtualKeyboardGetSizeAndPosition_p_01(void)
{
    Dali::Rect<int> keyboardRect;

    VirtualKeyboard::Hide();
    keyboardRect = VirtualKeyboard::GetSizeAndPosition();
    DALI_TEST_CHECK( !keyboardRect.IsEmpty());
    VirtualKeyboard::Show();
    keyboardRect = VirtualKeyboard::GetSizeAndPosition();
    DALI_TEST_CHECK( !keyboardRect.IsEmpty());
    /*TODO Need method to check returned values are valid */

    END_TEST;
}

/**
 * @brief calling Dali::VirtualKeyboard::GetSizeAndPosition() in case of no adaptor
 *
 * @procedure
 * @step 1 deleting existed adaptor.
 * @step 2 calling the method.
 * @step 3 creating new adaptor.
 *
 * @Passcondition: if exception is thrown.
 * @id DA-160
 */
int UtcDaliVirtualKeyboardGetSizeAndPosition_n_01(void)
{
    delete window;
    delete adaptor;
    bool isCaught = false;

    try
    {
        VirtualKeyboard::GetSizeAndPosition();
    }
    catch (Dali::DaliException& e)
    {
        isCaught = true;
    }

    DALI_TEST_CHECK(isCaught);

    Dali::PositionSize positionSize(0, 0, 800, 1280);
    window = new Dali::Window(Dali::Window::New(positionSize, "MyApp"));
    adaptor = &Dali::Adaptor::New(*window);

    END_TEST;
}

/**
 * @brief rotating the keyboard orientation.
 *
 * @procedure
 * @step 1 rotate the keyboard to 0.
 * @step 2 rotate the keyboard to 90.
 * @step 3 rotate the keyboard to 180.
 * @step 4 rotate the keyboard to 270.
 * @step 5 rotate the keyboard to 157.
 *
 * @Passcondition: test never passes(TET_UNDEF), because get method not implemented.
 * @id DA-161
 */
int UtcDaliVirtualKeyboardRotateTo_p_01(void)
{
    VirtualKeyboard::RotateTo(0);
    VirtualKeyboard::RotateTo(90);
    VirtualKeyboard::RotateTo(180);
    VirtualKeyboard::RotateTo(270);
    VirtualKeyboard::RotateTo(157);
    /*TODO Write a method that will help to check/get virtual keyboard rotation angle*/
    return TET_UNDEF;
}

/**
 * @brief calling Dali::VirtualKeyboard::RotateTo() in case of no adaptor
 *
 * @procedure
 * @step 1 deleting existed adaptor.
 * @step 2 calling the method.
 * @step 3 creating new adaptor.
 *
 * @Passcondition: if exception is thrown.
 * @id DA-161
 */
int UtcDaliVirtualKeyboardRotateTo_n_01(void)
{
    delete window;
    delete adaptor;
    bool isCaught = false;

    try
    {
        VirtualKeyboard::RotateTo(10);
    }
    catch (Dali::DaliException& e)
    {
        isCaught = true;
    }

    DALI_TEST_CHECK(isCaught);

    Dali::PositionSize positionSize(0, 0, 800, 1280);
    window = new Dali::Window(Dali::Window::New(positionSize, "MyApp"));
    adaptor = &Dali::Adaptor::New(*window);

    END_TEST;
}

/**
 * @brief get text direction of virtual keyboard.
 *
 * @procedure
 * @step 1 get text direction of the keyboard.
 * @step 2 check is direction left to right.
 *
 * @Passcondition: if orientation of text is from left to right.
 * @id DA-162
 */
int UtcDaliVirtualKeyboardGetTextDirection_p_01(void)
{
    VirtualKeyboard::TextDirection textDirection = VirtualKeyboard::GetTextDirection();

    DALI_TEST_CHECK (textDirection == VirtualKeyboard::LeftToRight);
    END_TEST;
}

/**
 * @brief calling Dali::VirtualKeyboard::GetTextDirection() in case of no adaptor
 *
 * @procedure
 * @step 1 deleting existed adaptor.
 * @step 2 calling the method.
 * @step 3 creating new adaptor.
 *
 * @Passcondition: if exception is thrown.
 * @id DA-162
 */
int UtcDaliVirtualKeyboardGetTextDirection_n_01(void)
{
    delete window;
    delete adaptor;
    bool isCaught = false;

    try
    {
        VirtualKeyboard::GetTextDirection();
    }
    catch (Dali::DaliException& e)
    {
        isCaught = true;
    }

    DALI_TEST_CHECK(isCaught);

    Dali::PositionSize positionSize(0, 0, 800, 1280);
    window = new Dali::Window(Dali::Window::New(positionSize, "MyApp"));
    adaptor = &Dali::Adaptor::New(*window);

    END_TEST;
}

bool SIGNAL_KEYBOARD_IS_SHOWN = false;

void OnStatusChanged(bool IsKeyboardShown)
{
    SIGNAL_KEYBOARD_IS_SHOWN = IsKeyboardShown;
}

/**
 * @brief get show status change signal.
 *
 * @procedure
 * @step 1 get the signal connection count.
 * @step 2 connect callback function(OnStatusChanged).
 * @step 3 check is connections count increased.
 * @step 4 emiting signals with different input.
 *
 * @Passcondition: if the signal works proper.
 * @id DA-163
 */
int UtcDaliVirtualKeyboardStatusChangedSignal_p_01(void)
{
    unsigned connectionsCount = VirtualKeyboard::StatusChangedSignal().GetConnectionCount();

    VirtualKeyboard::StatusChangedSignal().Connect(OnStatusChanged);
    DALI_TEST_CHECK( VirtualKeyboard::StatusChangedSignal().GetConnectionCount() == connectionsCount + 1);
    VirtualKeyboard::StatusChangedSignal().Emit(true);
    DALI_TEST_CHECK( SIGNAL_KEYBOARD_IS_SHOWN == true);
    VirtualKeyboard::StatusChangedSignal().Emit(true);
    DALI_TEST_CHECK( SIGNAL_KEYBOARD_IS_SHOWN == true);
    VirtualKeyboard::StatusChangedSignal().Emit(false);
    DALI_TEST_CHECK( SIGNAL_KEYBOARD_IS_SHOWN == false);
    VirtualKeyboard::StatusChangedSignal().Emit(false);
    DALI_TEST_CHECK( SIGNAL_KEYBOARD_IS_SHOWN == false);
    END_TEST;
}

/**
 * @brief calling Dali::VirtualKeyboard::StatusChangedSignal() in case of no adaptor
 *
 * @procedure
 * @step 1 deleting existed adaptor.
 * @step 2 calling the method.
 * @step 3 creating new adaptor.
 *
 * @Passcondition: if exception is thrown.
 * @id DA-163
 */
int UtcDaliVirtualKeyboardStatusChangedSignal_n_01(void)
{
    delete window;
    delete adaptor;
    bool isCaught = false;

    try
    {
        VirtualKeyboard::StatusChangedSignal();
    }
    catch (Dali::DaliException& e)
    {
        isCaught = true;
    }

    DALI_TEST_CHECK(isCaught);

    Dali::PositionSize positionSize(0, 0, 800, 1280);
    window = new Dali::Window(Dali::Window::New(positionSize, "MyApp"));
    adaptor = &Dali::Adaptor::New(*window);

    END_TEST;
}

bool SIGNAL_IS_CHANGED = false;

void OnSignal(void)
{
    SIGNAL_IS_CHANGED = true;
}

/**
 * @brief get on resized signal.
 *
 * @procedure
 * @step 1 get the signal connection count.
 * @step 2 connect callback function(OnSignal).
 * @step 3 check is connections count increased.
 * @step 4 emiting resized signal.
 *
 * @Passcondition: if the signal works proper.
 * @id DA-164
 */
int UtcDaliVirtualKeyboardResizedSignal_p_01(void)
{
    unsigned connectionsCount = VirtualKeyboard::ResizedSignal().GetConnectionCount();

    VirtualKeyboard::ResizedSignal().Connect(OnSignal);
    DALI_TEST_CHECK( VirtualKeyboard::ResizedSignal().GetConnectionCount() == connectionsCount + 1);
    VirtualKeyboard::ResizedSignal().Emit();
    DALI_TEST_CHECK( SIGNAL_IS_CHANGED );
    END_TEST;
}

/**
 * @brief calling Dali::VirtualKeyboard::ResizedSignal() in case of no adaptor
 *
 * @procedure
 * @step 1 deleting existed adaptor.
 * @step 2 calling the method.
 * @step 3 creating new adaptor.
 *
 * @Passcondition: if exception is thrown.
 * @id DA-164
 */
int UtcDaliVirtualKeyboardResizedSignal_n_01(void)
{
    delete window;
    delete adaptor;
    bool isCaught = false;

    try
    {
        VirtualKeyboard::ResizedSignal();
    }
    catch (Dali::DaliException& e)
    {
        isCaught = true;
    }

    DALI_TEST_CHECK(isCaught);

    Dali::PositionSize positionSize(0, 0, 800, 1280);
    window = new Dali::Window(Dali::Window::New(positionSize, "MyApp"));
    adaptor = &Dali::Adaptor::New(*window);

    END_TEST;
}

/**
 * @brief get on language changed signal.
 *
 * @procedure
 * @step 1 get the signal connection count.
 * @step 2 connect callback function(OnSignal).
 * @step 3 check is connections count increased.
 * @step 4 emiting language changed signal.
 *
 * @Passcondition: if the signal works proper.
 * @id DA-165
 */
int UtcDaliVirtualKeyboardLanguageChangedSignal_p_01(void)
{
    unsigned connectionCount = VirtualKeyboard::LanguageChangedSignal().GetConnectionCount();

    VirtualKeyboard::LanguageChangedSignal().Connect(OnSignal);
    DALI_TEST_CHECK( VirtualKeyboard::LanguageChangedSignal().GetConnectionCount() == connectionCount + 1 );
    VirtualKeyboard::LanguageChangedSignal().Emit();
    DALI_TEST_CHECK( SIGNAL_IS_CHANGED );
    END_TEST;
}

/**
 * @brief calling Dali::VirtualKeyboard::LanguageChangedSignal() in case of no adaptor
 *
 * @procedure
 * @step 1 deleting existed adaptor.
 * @step 2 calling the method.
 * @step 3 creating new adaptor.
 *
 * @Passcondition: if exception is thrown.
 * @id DA-165
 */
int UtcDaliVirtualKeyboardLanguageChangedSignal_n_01(void)
{
    delete window;
    delete adaptor;
    bool isCaught = false;

    try
    {
        VirtualKeyboard::LanguageChangedSignal();
    }
    catch (Dali::DaliException& e)
    {
        isCaught = true;
    }

    DALI_TEST_CHECK(isCaught);

    Dali::PositionSize positionSize(0, 0, 800, 1280);
    window = new Dali::Window(Dali::Window::New(positionSize, "MyApp"));
    adaptor = &Dali::Adaptor::New(*window);

    END_TEST;
}
