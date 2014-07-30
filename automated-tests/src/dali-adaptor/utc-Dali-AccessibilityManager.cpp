//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//  http://floralicense.org/license/
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

void utc_dali_accessibility_manager_startup(void)
{
    ecore_x_init(NULL);
    test_return_value = TET_UNDEF;
}

void utc_dali_accessibility_manager_cleanup(void)
{
    test_return_value = TET_PASS;
    ecore_x_shutdown();
}

bool SIGNAL_IS_CHANGED = false;

//A callback function for Signals
bool callFunction(AccessibilityManager& manager) { SIGNAL_IS_CHANGED = !SIGNAL_IS_CHANGED; return SIGNAL_IS_CHANGED; }

/**
* @brief get correct accessibility manager action back signal
*
* @procedure
* @step 1 create Dali::Adaptor
* @step 2 create AccessibilityManager
* @step 3 get actionBackSignal count
* @step 4 connect a callback function
* @step 5 checking the number of slots connected to this signal
* @step 6 emit signal
*
* @Passcondition: conection count is incresed by one and callback function is called
* @id DA-031
*/
int UtcDaliAccessibilityManagerActionBackSignal_p_01(void)
{
    tet_printf("Action BackSignal - AccessibilityActionSignalV2& ActionBackSignal()\n");

    Dali::PositionSize positionSize(0, 0, 800, 1280);
    Dali::Window window = Dali::Window::New(positionSize, "Haptic Application");
    Dali::Adaptor* adaptor = &Dali::Adaptor::New(window);
    AccessibilityManager am = AccessibilityManager::Get();
    DALI_TEST_CHECK( am );
    int connectionCount = am.ActionBackSignal().GetConnectionCount();
    am.ActionBackSignal().Connect( callFunction );

    //Checking the number of slots connected to this signal.
    DALI_TEST_CHECK( (connectionCount + 1) == am.ActionBackSignal().GetConnectionCount() );

    AccessibilityManager check;
    am.ActionBackSignal().Emit( check );
    //The value returned by the last callback.
    DALI_TEST_CHECK( SIGNAL_IS_CHANGED );
    SIGNAL_IS_CHANGED = !SIGNAL_IS_CHANGED;
    delete adaptor;

    END_TEST;
}

/**
* @brief try to get accessibility manager action back signal from not existing handler
*
* @procedure
* @step 1 create Dali::Adaptor
* @step 2 create AccessibilityManager without initialized
* @step 3 call ActionBackSignal method from not existing handler
*
* @Passcondition: exception occur due not constructed handler
* @id DA-031
*/
int UtcDaliAccessibilityManagerActionBackSignal_n_01(void)
{
    tet_printf("Action BackSignal Negative - AccessibilityActionSignalV2& ActionBackSignal()\n");
    bool isCaught = false;
    Dali::PositionSize positionSize(0, 0, 800, 1280);
    Dali::Window window = Dali::Window::New(positionSize, "Haptic Application");
    Dali::Adaptor* adaptor = &Dali::Adaptor::New(window);

    try {
        AccessibilityManager am;
        am.ActionBackSignal();
    }
    catch (Dali::DaliException& e) {
        isCaught = true;
    }

    DALI_TEST_CHECK( isCaught );
    delete adaptor;

    END_TEST;
}

/**
* @brief get correct accessibility manager action next signal
*
* @procedure
* @step 1 create Dali::Adaptor
* @step 2 create AccessibilityManager
* @step 3 get actionNextSignal count
* @step 4 connect a callback function
* @step 5 checking the number of slots connected to this signal
* @step 6 emit signal
*
* @Passcondition: conection count is incresed by one and callback function is called
* @id DA-021
*/
int UtcDaliAccessibilityManagerActionNextSignal_p_01(void)
{
    tet_printf("Action NextSignal - AccessibilityActionSignalV2& ActionNextSignal()\n");

    Dali::PositionSize positionSize(0, 0, 800, 1280);
    Dali::Window window = Dali::Window::New(positionSize, "Haptic Application");
    Dali::Adaptor* adaptor = &Dali::Adaptor::New(window);
    AccessibilityManager am = AccessibilityManager::Get();
    DALI_TEST_CHECK( am );
    int connectionCount = am.ActionNextSignal().GetConnectionCount();
    am.ActionNextSignal().Connect( callFunction );

    //Checking the number of slots connected to this signal.
    DALI_TEST_CHECK( (connectionCount + 1) == am.ActionNextSignal().GetConnectionCount() );

    AccessibilityManager check;
    am.ActionNextSignal().Emit( check );

    //The value returned by the last callback.
    DALI_TEST_CHECK( SIGNAL_IS_CHANGED );
    SIGNAL_IS_CHANGED = !SIGNAL_IS_CHANGED;
    delete adaptor;

    END_TEST;
}

/**
* @brief try to get accessibility manager action next signal from not existing handler
*
* @procedure
* @step 1 create Dali::Adaptor
* @step 2 create AccessibilityManager without initialized
* @step 3 call ActionNextSignal method from not existing handler
*
* @Passcondition: exception occur due not constructed handler
* @id DA-021
*/
int UtcDaliAccessibilityManagerActionNextSignal_n_01(void)
{
    tet_printf("Action NextSignal Negative - AccessibilityActionSignalV2& ActionNextSignal()\n");
    bool isCaught = false;
    Dali::PositionSize positionSize(0, 0, 800, 1280);
    Dali::Window window = Dali::Window::New(positionSize, "Haptic Application");
    Dali::Adaptor* adaptor = &Dali::Adaptor::New(window);

    try {
        AccessibilityManager am;
        am.ActionNextSignal();
    }
    catch (Dali::DaliException& e) {
        isCaught = true;
    }

    DALI_TEST_CHECK( isCaught );
    delete adaptor;

    END_TEST;
}

/**
* @brief get correct accessibility manager action previous signal
*
* @procedure
* @step 1 create Dali::Adaptor
* @step 2 create AccessibilityManager
* @step 3 get actionPreviousSignal count
* @step 4 connect a callback function
* @step 5 checking the number of slots connected to this signal
* @step 6 emit signal
*
* @Passcondition: conection count is incresed by one and callback function is called
* @id DA-022
*/
int UtcDaliAccessibilityManagerActionPreviousSignal_p_01(void)
{
    tet_printf("Action PreviousSignal - AccessibilityActionSignalV2& ActionPreviousSignal()\n");

    Dali::PositionSize positionSize(0, 0, 800, 1280);
    Dali::Window window = Dali::Window::New(positionSize, "Haptic Application");
    Dali::Adaptor* adaptor = &Dali::Adaptor::New(window);
    AccessibilityManager am = AccessibilityManager::Get();
    DALI_TEST_CHECK( am );
    int connectionCount = am.ActionPreviousSignal().GetConnectionCount();
    am.ActionPreviousSignal().Connect( callFunction );

    //Checking the number of slots connected to this signal.
    DALI_TEST_CHECK( (connectionCount + 1) == am.ActionPreviousSignal().GetConnectionCount() );

    AccessibilityManager check;
    am.ActionPreviousSignal().Emit( check );
    //The value returned by the last callback.
    DALI_TEST_CHECK( SIGNAL_IS_CHANGED );
    SIGNAL_IS_CHANGED = !SIGNAL_IS_CHANGED;
    delete adaptor;

    END_TEST;
}

/**
* @brief try to get accessibility manager action previous signal from not existing handler
*
* @procedure
* @step 1 create Dali::Adaptor
* @step 2 create AccessibilityManager without initialized
* @step 3 call ActionPreviousSignal method from not existing handler
*
* @Passcondition: exception occur due not constructed handler
* @id DA-022
*/
int UtcDaliAccessibilityManagerActionPreviousSignal_n_01(void)
{
    tet_printf("Action PreviousSignal Negative - AccessibilityActionSignalV2& ActionPreviousSignal()\n");
    bool isCaught = false;
    Dali::PositionSize positionSize(0, 0, 800, 1280);
    Dali::Window window = Dali::Window::New(positionSize, "Haptic Application");
    Dali::Adaptor* adaptor = &Dali::Adaptor::New(window);

    try {
        AccessibilityManager am;
        am.ActionPreviousSignal();
    }
    catch (Dali::DaliException& e) {
        isCaught = true;
    }

    DALI_TEST_CHECK( isCaught );
    delete adaptor;

    END_TEST;
}

/**
* @brief get correct accessibility manager action activate signal
*
* @procedure
* @step 1 create Dali::Adaptor
* @step 2 create AccessibilityManager
* @step 3 get actionActivateSignal count
* @step 4 connect a callback function
* @step 5 checking the number of slots connected to this signal
* @step 6 emit signal
*
* @Passcondition: conection count is incresed by one and callback function is called
* @id DA-023
*/
int UtcDaliAccessibilityManagerActionActivateSignal_p_01(void)
{
    tet_printf("Action ActivateSignal - AccessibilityActionSignalV2& ActionActivateSignal()\n");

    Dali::PositionSize positionSize(0, 0, 800, 1280);
    Dali::Window window = Dali::Window::New(positionSize, "Haptic Application");
    Dali::Adaptor* adaptor = &Dali::Adaptor::New(window);
    AccessibilityManager am = AccessibilityManager::Get();
    DALI_TEST_CHECK( am );
    int connectionCount = am.ActionActivateSignal().GetConnectionCount();
    am.ActionActivateSignal().Connect( callFunction );

    //Checking the number of slots connected to this signal.
    DALI_TEST_CHECK( (connectionCount + 1) == am.ActionActivateSignal().GetConnectionCount() );

    AccessibilityManager check;
    am.ActionActivateSignal().Emit( check );
    //The value returned by the last callback.
    DALI_TEST_CHECK( SIGNAL_IS_CHANGED );
    SIGNAL_IS_CHANGED = !SIGNAL_IS_CHANGED;
    delete adaptor;

    END_TEST;
}

/**
* @brief try to get accessibility manager action activate signal from not existing handler
*
* @procedure
* @step 1 create Dali::Adaptor
* @step 2 create AccessibilityManager without initialized
* @step 3 call ActionActivateSignal method from not existing handler
*
* @Passcondition: exception occur due not constructed handler
* @id DA-023
*/
int UtcDaliAccessibilityManagerActionActivateSignal_n_01(void)
{
    tet_printf("Action ActivateSignal Negative - AccessibilityActionSignalV2& ActionActivateSignal()\n");
    bool isCaught = false;
    Dali::PositionSize positionSize(0, 0, 800, 1280);
    Dali::Window window = Dali::Window::New(positionSize, "Haptic Application");
    Dali::Adaptor* adaptor = &Dali::Adaptor::New(window);

    try {
        AccessibilityManager am;
        am.ActionActivateSignal();
    }
    catch (Dali::DaliException& e) {
        isCaught = true;
    }

    DALI_TEST_CHECK( isCaught );
    delete adaptor;

    END_TEST;
}

/**
* @brief get correct accessibility manager action read signal
*
* @procedure
* @step 1 create Dali::Adaptor
* @step 2 create AccessibilityManager
* @step 3 get actionReadSignal count
* @step 4 connect a callback function
* @step 5 checking the number of slots connected to this signal
* @step 6 emit signal
*
* @Passcondition: conection count is incresed by one and callback function is called
* @id DA-024
*/
int UtcDaliAccessibilityManagerActionReadSignal_p_01(void)
{
    tet_printf("Action ReadSignal - AccessibilityActionSignalV2& ActionReadSignal()\n");

    Dali::PositionSize positionSize(0, 0, 800, 1280);
    Dali::Window window = Dali::Window::New(positionSize, "Haptic Application");
    Dali::Adaptor* adaptor = &Dali::Adaptor::New(window);
    AccessibilityManager am = AccessibilityManager::Get();
    DALI_TEST_CHECK( am );
    int connectionCount = am.ActionReadSignal().GetConnectionCount();
    am.ActionReadSignal().Connect( callFunction );

    //Checking the number of slots connected to this signal.
    DALI_TEST_CHECK( (connectionCount + 1) == am.ActionReadSignal().GetConnectionCount() );

    AccessibilityManager check;
    am.ActionReadSignal().Emit( check );
    //The value returned by the last callback.
    DALI_TEST_CHECK( SIGNAL_IS_CHANGED );
    SIGNAL_IS_CHANGED = !SIGNAL_IS_CHANGED;
    delete adaptor;

    END_TEST;
}

/**
* @brief try to get accessibility manager action read signal from not existing handler
*
* @procedure
* @step 1 create Dali::Adaptor
* @step 2 create AccessibilityManager without initialized
* @step 3 call ActionReadSignal method from not existing handler
*
* @Passcondition: exception occur due not constructed handler
* @id DA-024
*/
int UtcDaliAccessibilityManagerActionReadSignal_n_01(void)
{
    tet_printf("Action ReadSignal Negative - AccessibilityActionSignalV2& ActionReadSignal()\n");
    bool isCaught = false;
    Dali::PositionSize positionSize(0, 0, 800, 1280);
    Dali::Window window = Dali::Window::New(positionSize, "Haptic Application");
    Dali::Adaptor* adaptor = &Dali::Adaptor::New(window);

    try {
        AccessibilityManager am;
        am.ActionReadSignal();
    }
    catch (Dali::DaliException& e) {
        isCaught = true;
    }

    DALI_TEST_CHECK( isCaught );
    delete adaptor;

    END_TEST;
}

/**
* @brief get correct accessibility manager action over signal
*
* @procedure
* @step 1 create Dali::Adaptor
* @step 2 create AccessibilityManager
* @step 3 get actionOverSignal count
* @step 4 connect a callback function
* @step 5 checking the number of slots connected to this signal
* @step 6 emit signal

* @Passcondition: conection count is incresed by one and callback function is called
* @id DA-025
*/
int UtcDaliAccessibilityManagerActionOverSignal_p_01(void)
{
    tet_printf("Action OverSignal - AccessibilityActionSignalV2& ActionOverSignal()\n");

    Dali::PositionSize positionSize(0, 0, 800, 1280);
    Dali::Window window = Dali::Window::New(positionSize, "Haptic Application");
    Dali::Adaptor* adaptor = &Dali::Adaptor::New(window);
    AccessibilityManager am = AccessibilityManager::Get();
    DALI_TEST_CHECK( am );
    int connectionCount = am.ActionOverSignal().GetConnectionCount();
    am.ActionOverSignal().Connect( callFunction );

    //Checking the number of slots connected to this signal.
    DALI_TEST_CHECK( (connectionCount + 1) == am.ActionOverSignal().GetConnectionCount() );

    AccessibilityManager check;
    am.ActionOverSignal().Emit( check );
    //The value returned by the last callback.
    DALI_TEST_CHECK( SIGNAL_IS_CHANGED );
    SIGNAL_IS_CHANGED = !SIGNAL_IS_CHANGED;
    delete adaptor;

    END_TEST;
}

/**
* @brief try to get accessibility manager action over signal from not existing handler
*
* @procedure
* @step 1 create Dali::Adaptor
* @step 2 create AccessibilityManager without initialized
* @step 3 call ActionOverSignal method from not existing handler
*
* @Passcondition: exception occur due not constructed handler
* @id DA-025
*/
int UtcDaliAccessibilityManagerActionOverSignal_n_01(void)
{
    tet_printf("Action OverSignal Negative - AccessibilityActionSignalV2& ActionOverSignal()\n");
    bool isCaught = false;
    Dali::PositionSize positionSize(0, 0, 800, 1280);
    Dali::Window window = Dali::Window::New(positionSize, "Haptic Application");
    Dali::Adaptor* adaptor = &Dali::Adaptor::New(window);

    try {
        AccessibilityManager am;
        am.ActionOverSignal();
    }
    catch (Dali::DaliException& e) {
        isCaught = true;
    }

    DALI_TEST_CHECK( isCaught );
    delete adaptor;

    END_TEST;
}

/**
* @brief get correct accessibility manager action read next signal
*
* @procedure
* @step 1 create Dali::Adaptor
* @step 2 create AccessibilityManager
* @step 3 get actionReadNextSignal count
* @step 4 connect a callback function
* @step 5 checking the number of slots connected to this signal
* @step 6 emit signal
*
* @Passcondition: conection count is incresed by one and callback function is called
* @id DA-026
*/
int UtcDaliAccessibilityManagerActionReadNextSignal_p_01(void)
{
    tet_printf("Action ReadNextSignal - AccessibilityActionSignalV2& ActionReadNextSignal()\n");

    Dali::PositionSize positionSize(0, 0, 800, 1280);
    Dali::Window window = Dali::Window::New(positionSize, "Haptic Application");
    Dali::Adaptor* adaptor = &Dali::Adaptor::New(window);
    AccessibilityManager am = AccessibilityManager::Get();
    DALI_TEST_CHECK( am );
    int connectionCount = am.ActionReadNextSignal().GetConnectionCount();
    am.ActionReadNextSignal().Connect( callFunction );

    //Checking the number of slots connected to this signal.
    DALI_TEST_CHECK( (connectionCount + 1) == am.ActionReadNextSignal().GetConnectionCount() );

    AccessibilityManager check;
    am.ActionReadNextSignal().Emit( check );
    //The value returned by the last callback.
    DALI_TEST_CHECK( SIGNAL_IS_CHANGED );
    SIGNAL_IS_CHANGED = !SIGNAL_IS_CHANGED;
    delete adaptor;

    END_TEST;
}

/**
* @brief try to get accessibility manager action read next signal from not existing handler
*
* @procedure
* @step 1 create Dali::Adaptor
* @step 2 create AccessibilityManager without initialized
* @step 3 call ActionReadNextSignal method from not existing handler
*
* @Passcondition: exception occur due not constructed handler
* @id DA-026
*/
int UtcDaliAccessibilityManagerActionReadNextSignal_n_01(void)
{
    tet_printf("Action ReadNextSignal Negative - AccessibilityActionSignalV2& ActionReadNextSignal()\n");
    bool isCaught = false;
    Dali::PositionSize positionSize(0, 0, 800, 1280);
    Dali::Window window = Dali::Window::New(positionSize, "Haptic Application");
    Dali::Adaptor* adaptor = &Dali::Adaptor::New(window);

    try {
        AccessibilityManager am;
        am.ActionReadNextSignal();
    }
    catch (Dali::DaliException& e) {
        isCaught = true;
    }

    DALI_TEST_CHECK( isCaught );
    delete adaptor;

    END_TEST;
}

/**
* @brief get correct accessibility manager action read previous signal
*
* @procedure
* @step 1 create Dali::Adaptor
* @step 2 create AccessibilityManager
* @step 3 get actionReadPreviousSignal count
* @step 4 connect a callback function
* @step 5 checking the number of slots connected to this signal
* @step 6 emit signal
*
* @Passcondition: conection count is incresed by one and callback function is called
* @id DA-027
*/
int UtcDaliAccessibilityManagerActionReadPreviousSignal_p_01(void)
{
    tet_printf("Action ReadPreviousSignal - AccessibilityActionSignalV2& ActionReadPreviousSignal()\n");

    Dali::PositionSize positionSize(0, 0, 800, 1280);
    Dali::Window window = Dali::Window::New(positionSize, "Haptic Application");
    Dali::Adaptor* adaptor = &Dali::Adaptor::New(window);
    AccessibilityManager am = AccessibilityManager::Get();
    DALI_TEST_CHECK( am );
    int connectionCount = am.ActionReadPreviousSignal().GetConnectionCount();
    am.ActionReadPreviousSignal().Connect( callFunction );

    //Checking the number of slots connected to this signal.
    DALI_TEST_CHECK( (connectionCount + 1) == am.ActionReadPreviousSignal().GetConnectionCount() );

    AccessibilityManager check;
    am.ActionReadPreviousSignal().Emit( check );
    //The value returned by the last callback.
    DALI_TEST_CHECK( SIGNAL_IS_CHANGED );
    SIGNAL_IS_CHANGED = !SIGNAL_IS_CHANGED;
    delete adaptor;

    END_TEST;
}

/**
* @brief try to get accessibility manager action read previous signal from not existing handler
*
* @procedure
* @step 1 create Dali::Adaptor
* @step 2 create AccessibilityManager without initialized
* @step 3 call ActionReadPreviousSignal method from not existing handler
*
* @Passcondition: exception occur due not constructed handler
* @id DA-027
*/
int UtcDaliAccessibilityManagerActionReadPreviousSignal_n_01(void)
{
    tet_printf("Action ReadPreviousSignal Negative - AccessibilityActionSignalV2& ActionReadPreviousSignal()\n");
    bool isCaught = false;
    Dali::PositionSize positionSize(0, 0, 800, 1280);
    Dali::Window window = Dali::Window::New(positionSize, "Haptic Application");
    Dali::Adaptor* adaptor = &Dali::Adaptor::New(window);

    try {
        AccessibilityManager am;
        am.ActionReadPreviousSignal();
    }
    catch (Dali::DaliException& e) {
        isCaught = true;
    }

    DALI_TEST_CHECK( isCaught );
    delete adaptor;

    END_TEST;
}

/**
* @brief get correct accessibility manager action up signal
*
* @procedure
* @step 1 create Dali::Adaptor
* @step 2 create AccessibilityManager
* @step 3 get actionUpSignal count
* @step 4 connect a callback function
* @step 5 checking the number of slots connected to this signal
* @step 6 emit signal
*
* @Passcondition: conection count is incresed by one and callback function is called
* @id DA-028
*/
int UtcDaliAccessibilityManagerActionUpSignal_p_01(void)
{
    tet_printf("Action UpSignal - AccessibilityActionSignalV2& ActionUpSignal()\n");

    Dali::PositionSize positionSize(0, 0, 800, 1280);
    Dali::Window window = Dali::Window::New(positionSize, "Haptic Application");
    Dali::Adaptor* adaptor = &Dali::Adaptor::New(window);
    AccessibilityManager am = AccessibilityManager::Get();
    DALI_TEST_CHECK( am );
    int connectionCount = am.ActionUpSignal().GetConnectionCount();
    am.ActionUpSignal().Connect( callFunction );

    //Checking the number of slots connected to this signal.
    DALI_TEST_CHECK( (connectionCount + 1) == am.ActionUpSignal().GetConnectionCount() );

    AccessibilityManager check;
    am.ActionUpSignal().Emit( check );
    //The value returned by the last callback.
    DALI_TEST_CHECK( SIGNAL_IS_CHANGED );
    SIGNAL_IS_CHANGED = !SIGNAL_IS_CHANGED;
    delete adaptor;

    END_TEST;
}

/**
* @brief try to get accessibility manager action up signal from not existing handler
*
* @procedure
* @step 1 create Dali::Adaptor
* @step 2 create AccessibilityManager without initialized
* @step 3 call ActionUpSignal method from not existing handler
*
* @Passcondition: exception occur due not constructed handler
* @id DA-028
*/
int UtcDaliAccessibilityManagerActionUpSignal_n_01(void)
{
    tet_printf("Action UpSignal Negative - AccessibilityActionSignalV2& ActionUpSignal()\n");
    bool isCaught = false;
    Dali::PositionSize positionSize(0, 0, 800, 1280);
    Dali::Window window = Dali::Window::New(positionSize, "Haptic Application");
    Dali::Adaptor* adaptor = &Dali::Adaptor::New(window);

    try {
        AccessibilityManager am;
        am.ActionUpSignal();
    }
    catch (Dali::DaliException& e) {
        isCaught = true;
    }

    DALI_TEST_CHECK( isCaught );
    delete adaptor;

    END_TEST;
}

/**
* @brief get correct accessibility manager action down signal
*
* @procedure
* @step 1 create Dali::Adaptor
* @step 2 create AccessibilityManager
* @step 3 get actionDownSignal count
* @step 4 connect a callback function
* @step 5 checking the number of slots connected to this signal
* @step 6 emit signal
*
* @Passcondition: conection count is incresed by one and callback function is called
* @id DA-029
*/
int UtcDaliAccessibilityManagerActionDownSignal_p_01(void)
{
    tet_printf("Action DownSignal - AccessibilityActionSignalV2& ActionDownSignal()\n");

    Dali::PositionSize positionSize(0, 0, 800, 1280);
    Dali::Window window = Dali::Window::New(positionSize, "Haptic Application");
    Dali::Adaptor* adaptor = &Dali::Adaptor::New(window);
    AccessibilityManager am = AccessibilityManager::Get();
    DALI_TEST_CHECK( am );
    int connectionCount = am.ActionDownSignal().GetConnectionCount();
    am.ActionDownSignal().Connect( callFunction );

    //Checking the number of slots connected to this signal.
    DALI_TEST_CHECK( (connectionCount + 1) == am.ActionDownSignal().GetConnectionCount() );

    AccessibilityManager check;
    am.ActionDownSignal().Emit( check );
    //The value returned by the last callback.
    DALI_TEST_CHECK( SIGNAL_IS_CHANGED );
    SIGNAL_IS_CHANGED = !SIGNAL_IS_CHANGED;
    delete adaptor;

    END_TEST;
}

/**
* @brief try to get accessibility manager action down signal from not existing handler
*
* @procedure
* @step 1 create Dali::Adaptor
* @step 2 create AccessibilityManager without initialized
* @step 3 call ActionDownSignal method from not existing handler
*
* @Passcondition: exception occur due not constructed handler
* @id DA-029
*/
int UtcDaliAccessibilityManagerActionDownSignal_n_01(void)
{
    tet_printf("Action DownSignal Negative - AccessibilityActionSignalV2& ActionDownSignal()\n");
    bool isCaught = false;
    Dali::PositionSize positionSize(0, 0, 800, 1280);
    Dali::Window window = Dali::Window::New(positionSize, "Haptic Application");
    Dali::Adaptor* adaptor = &Dali::Adaptor::New(window);

    try {
        AccessibilityManager am;
        am.ActionDownSignal();
    }
    catch (Dali::DaliException& e) {
        isCaught = true;
    }

    DALI_TEST_CHECK( isCaught );
    delete adaptor;

    END_TEST;
}

/**
* @brief get correct accessibility manager action clear focus signal
*
* @procedure
* @step 1 create Dali::Adaptor
* @step 2 create AccessibilityManager
* @step 3 get actionClearFocusSignal count
* @step 4 connect a callback function
* @step 5 checking the number of slots connected to this signal
* @step 6 emit signal
*
* @Passcondition: conection count is incresed by one and callback function is called
* @id DA-030
*/
int UtcDaliAccessibilityManagerActionClearFocusSignal_p_01(void)
{
    tet_printf("Action ClearFocusSignal - AccessibilityActionSignalV2& ActionClearFocusSignal()\n");

    Dali::PositionSize positionSize(0, 0, 800, 1280);
    Dali::Window window = Dali::Window::New(positionSize, "Haptic Application");
    Dali::Adaptor* adaptor = &Dali::Adaptor::New(window);
    AccessibilityManager am = AccessibilityManager::Get();
    DALI_TEST_CHECK( am );
    int connectionCount = am.ActionClearFocusSignal().GetConnectionCount();
    am.ActionClearFocusSignal().Connect( callFunction );

    //Checking the number of slots connected to this signal.
    DALI_TEST_CHECK( (connectionCount + 1) == am.ActionClearFocusSignal().GetConnectionCount() );

    AccessibilityManager check;
    am.ActionClearFocusSignal().Emit( check );
    //The value returned by the last callback.
    DALI_TEST_CHECK( SIGNAL_IS_CHANGED );
    SIGNAL_IS_CHANGED = !SIGNAL_IS_CHANGED;
    delete adaptor;

    END_TEST;
}

/**
* @brief get try to get accessibility manager action clear focus signal from not existing handler
*
* @procedure
* @step 1 create Dali::Adaptor
* @step 2 create AccessibilityManager without initialized
* @step 3 call ActionClearFocusSignal method from not existing handler
*
* @Passcondition: exception occur due not constructed handler
* @id DA-030
*/
int UtcDaliAccessibilityManagerActionClearFocusSignal_n_01(void)
{
    tet_printf("Action ClearFocusSignal Negative - AccessibilityActionSignalV2& ActionClearFocusSignal()\n");
    bool isCaught = false;
    Dali::PositionSize positionSize(0, 0, 800, 1280);
    Dali::Window window = Dali::Window::New(positionSize, "Haptic Application");
    Dali::Adaptor* adaptor = &Dali::Adaptor::New(window);

    try {
        AccessibilityManager am;
        am.ActionClearFocusSignal();
    }
    catch (Dali::DaliException& e) {
        isCaught = true;
    }

    DALI_TEST_CHECK( isCaught );
    delete adaptor;

    END_TEST;
}
