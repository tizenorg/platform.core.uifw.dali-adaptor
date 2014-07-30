//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
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

using namespace Dali;

void utc_virtual_keyboard_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_virtual_keyboard_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliVirtualKeyboardShow(void)
{
	bool initialState = VirtualKeyboard::IsVisible();

	VirtualKeyboard::Hide();
	VirtualKeyboard::Hide();
	VirtualKeyboard::Show();
	DALI_TEST_CHECK(VirtualKeyboard::IsVisible());
	VirtualKeyboard::Show();
	DALI_TEST_CHECK(VirtualKeyboard::IsVisible());
	if (initialState)
		VirtualKeyboard::Show();
	else
		VirtualKeyboard::Hide();
	END_TEST;
}

int UtcDaliVirtualKeyboardHide(void)
{
	bool initialState = VirtualKeyboard::IsVisible();

	VirtualKeyboard::Show();
	VirtualKeyboard::Show();
	VirtualKeyboard::Hide();
	DALI_TEST_CHECK(!VirtualKeyboard::IsVisible());
	VirtualKeyboard::Hide();
	DALI_TEST_CHECK(!VirtualKeyboard::IsVisible());
	if (initialState)
		VirtualKeyboard::Hide();
	else
		VirtualKeyboard::Show();
	END_TEST;
}

int UtcDaliVirtualKeyboardIsVisible(void)
{
	bool initialState = VirtualKeyboard::IsVisible();

	VirtualKeyboard::Show();
	DALI_TEST_CHECK(VirtualKeyboard::IsVisible());
	VirtualKeyboard::Hide();
	DALI_TEST_CHECK(!VirtualKeyboard::IsVisible());
	if (initialState) {
		VirtualKeyboard::Hide();
		DALI_TEST_CHECK(!VirtualKeyboard::IsVisible());
	}
	else {
		VirtualKeyboard::Show();
		DALI_TEST_CHECK(VirtualKeyboard::IsVisible());
	}
	END_TEST;
}

int UtcDaliVirtualKeyboardSetReturnKeyType(void)
{
	VirtualKeyboard::ReturnKeyType initialType = VirtualKeyboard::GetReturnKeyType();
	VirtualKeyboard::ReturnKeyType wrongType = static_cast<VirtualKeyboard::ReturnKeyType> (-10);

	VirtualKeyboard::SetReturnKeyType(wrongType);
	DALI_TEST_CHECK( VirtualKeyboard::GetReturnKeyType() != wrongType );
	VirtualKeyboard::SetReturnKeyType(initialType);
	DALI_TEST_CHECK( VirtualKeyboard::GetReturnKeyType() == initialType );
	END_TEST;
}

int UtcDaliVirtualKeyboardGetReturnKeyType(void)
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

int UtcDaliVirtualKeyboardEnablePrediction(void)
{
	bool initialState = VirtualKeyboard::IsPredictionEnabled();

	VirtualKeyboard::EnablePrediction(true);
	VirtualKeyboard::EnablePrediction(true);
	DALI_TEST_CHECK( VirtualKeyboard::IsPredictionEnabled() == true);
	VirtualKeyboard::EnablePrediction(false);
	VirtualKeyboard::EnablePrediction(false);
	DALI_TEST_CHECK( VirtualKeyboard::IsPredictionEnabled() == false);
	VirtualKeyboard::EnablePrediction(initialState);
	DALI_TEST_CHECK( VirtualKeyboard::IsPredictionEnabled() == initialState );
	END_TEST;
}

int UtcDaliVirtualKeyboardIsPredictionEnabled(void)
{
	bool initialState = VirtualKeyboard::IsPredictionEnabled();

	VirtualKeyboard::EnablePrediction(true);
	DALI_TEST_CHECK( VirtualKeyboard::IsPredictionEnabled() == true);
	VirtualKeyboard::EnablePrediction(false);
	DALI_TEST_CHECK( VirtualKeyboard::IsPredictionEnabled() == false);
	VirtualKeyboard::EnablePrediction(initialState);
	DALI_TEST_CHECK( VirtualKeyboard::IsPredictionEnabled() == initialState );
	END_TEST;
}

int UtcDaliVirtualKeyboardGetSizeAndPosition(void)
{
	Dali::Rect<int> keyboardRect;
	bool initialState = VirtualKeyboard::IsVisible();

	VirtualKeyboard::Hide();
	keyboardRect = VirtualKeyboard::GetSizeAndPosition();
	DALI_TEST_CHECK( !keyboardRect.IsEmpty());
	VirtualKeyboard::Show();
	keyboardRect = VirtualKeyboard::GetSizeAndPosition();
	DALI_TEST_CHECK( !keyboardRect.IsEmpty());
	if (initialState)
		VirtualKeyboard::Show();
	else
		VirtualKeyboard::Hide();
	END_TEST;
}

int UtcDaliVirtualKeyboardRotateTo(void)
{
	VirtualKeyboard::RotateTo(0);
	VirtualKeyboard::RotateTo(90);
	VirtualKeyboard::RotateTo(180);
	VirtualKeyboard::RotateTo(270);
	VirtualKeyboard::RotateTo(0);
	/*TODO Write a method that will help to check/get virtual keyboard rotation angle*/
	return TET_UNDEF;
}

int UtcDaliVirtualKeyboardGetTextDirection(void)
{
	VirtualKeyboard::TextDirection textDirection = VirtualKeyboard::GetTextDirection();

	DALI_TEST_CHECK (textDirection > 0);
	END_TEST;
}

bool SIGNAL_KEYBOARD_IS_SHOWN = false;

void OnStatusChanged(bool IsKeyboardShown)
{
	SIGNAL_KEYBOARD_IS_SHOWN = IsKeyboardShown;
}

int UtcDaliVirtualKeyboardStatusChangedSignal(void)
{
	VirtualKeyboard::StatusSignalV2& signal = VirtualKeyboard::StatusChangedSignal();
	unsigned connectionsCount = signal.GetConnectionCount();

	signal.Connect(OnStatusChanged);
	DALI_TEST_CHECK( signal.GetConnectionCount() == connectionsCount + 1);
	signal.Emit(true);
	DALI_TEST_CHECK( SIGNAL_KEYBOARD_IS_SHOWN == true);
	signal.Emit(true);
	DALI_TEST_CHECK( SIGNAL_KEYBOARD_IS_SHOWN == true);
	signal.Emit(false);
	DALI_TEST_CHECK( SIGNAL_KEYBOARD_IS_SHOWN == false);
	signal.Emit(false);
	DALI_TEST_CHECK( SIGNAL_KEYBOARD_IS_SHOWN == false);
	END_TEST;
}

bool SIGNAL_IS_CHANGED = false;

void OnSignal(void)
{
	SIGNAL_IS_CHANGED = true;
}

int UtcDaliVirtualKeyboardResizedSignal(void)
{
	SIGNAL_IS_CHANGED = false;
	VirtualKeyboard::VoidSignalV2& signal = VirtualKeyboard::ResizedSignal();
	unsigned connectionsCount = signal.GetConnectionCount();

	signal.Connect(OnSignal);
	DALI_TEST_CHECK( signal.GetConnectionCount() == connectionsCount + 1);
	signal.Emit();
	DALI_TEST_CHECK( SIGNAL_IS_CHANGED );
	SIGNAL_IS_CHANGED = false;
	END_TEST;
}

int UtcDaliVirtualKeyboardLanguageChangedSignal(void)
{
	SIGNAL_IS_CHANGED = false;
	VirtualKeyboard::VoidSignalV2& signal = VirtualKeyboard::LanguageChangedSignal();
	unsigned connectionCount = signal.GetConnectionCount();

	signal.Connect(OnSignal);
	DALI_TEST_CHECK( signal.GetConnectionCount() == connectionCount + 1 );
	signal.Emit();
	DALI_TEST_CHECK( SIGNAL_IS_CHANGED );
	SIGNAL_IS_CHANGED = false;
	END_TEST;
}
