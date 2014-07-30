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
#include <dali.h>
#include <Ecore.h>
#include "Ecore_IMF.h"
#include "Ecore_X.h"
#include <dali-test-suite-utils.h>

using namespace Dali ;

static Window* window = NULL;
static Adaptor* adaptor= NULL;

void utc_dali_imfmanager_startup(void)
{
    test_return_value = TET_UNDEF;

    ecore_x_init(NULL);

    Rect<int> positionSize(0, 0, 800, 1280);

    window = new Window(Window::New(positionSize, "imfmanager"));
    adaptor = &Adaptor::New(*window);
}

void utc_dali_imfmanager_cleanup(void)
{
    if( adaptor != NULL)
    {
        delete adaptor;
    }

    if( window != NULL)
    {
        delete window;
    }

    ecore_x_shutdown();
}

static bool is_keyboard_activeted = false;
static bool is_event_called = false;
static Ecore_IMF_Context* imfmanager_callback_context = NULL;
static Ecore_IMF_Event_Cb imfmanager_callback_func = NULL;
static const void* imfmanager_callback_data = NULL;

extern "C"
{
void ecore_imf_context_event_callback_add( Ecore_IMF_Context* ctx, Ecore_IMF_Callback_Type type, Ecore_IMF_Event_Cb func, const void* data )
{
    if(type == ECORE_IMF_CALLBACK_COMMIT)
    {
        imfmanager_callback_context = ctx;
        imfmanager_callback_func = func;
        imfmanager_callback_data = data;
    }
}

void* ecore_imf_context_event_callback_del( Ecore_IMF_Context* ctx,    Ecore_IMF_Callback_Type    type, Ecore_IMF_Event_Cb func )
{
    imfmanager_callback_context = NULL;
    imfmanager_callback_func = NULL;
    imfmanager_callback_data = NULL;
    return NULL;
}
}

void keyboardActivated(ImfManager& manager)
{
    is_keyboard_activeted = true;
}

ImfManager::ImfCallbackData receivedEvent( ImfManager& manager, const ImfManager::ImfEventData& event)
{
    is_event_called = true;
    return ImfManager::ImfCallbackData();
}

/**
* @brief Positive test case 01: Creating ImfManager via constructor.
*
* @procedure
* @step 1 create new ImfManager
* @step 2 check handle uninitialized
*
* @Passcondition: method successfully creates new ImfManager with uninitialized handler
* @id DA-098
*/
int UtcDaliImfManagerImfManager_p_01(void)
{
    tet_printf("imfmanager creation \n");
    ImfManager* imfmanager = new ImfManager();

    DALI_TEST_CHECK( !(*imfmanager) );

    delete imfmanager;

    END_TEST;
}

/**
 * @brief Positive test case 01: Using Get for ImfManager acquisition.
 *
 * @procedure
 * @step 1 get ImfManager
 * @step 2 check the ImfManager
 *
 * @Passcondition: returned valid ImfManager
 * @id DA-084
 */
int UtcDaliImfManagerGet_p_01(void)
{
    tet_printf("imfmanager get \n");
    ImfManager imfmanager = ImfManager::Get();

    DALI_TEST_CHECK( imfmanager );

    END_TEST;
}

/**
 * @brief Negative test case 01: Using Get for ImfManager acquisition without Adaptor
 *
 * @procedure
 * @step 1 delete Adaptor
 * @step 2 get ImfManager
 *
 * @Passcondition: must throw exception
 * @id DA-084
 */
int UtcDaliImfManagerGet_n_01(void)
{
    tet_printf("imfmanager get \n");

    delete adaptor;
    adaptor = NULL;

    try
    {
        ImfManager imfmanager = ImfManager::Get();
        tet_result(TET_FAIL);
    }
    catch(Dali::DaliException& e)
    {
        tet_result(TET_PASS);
    }
    END_TEST;
}

/**
 * @brief Positive test case 01: try to invoke GetContext, should be no exception
 *
 * @procedure
 * @step 1 get ImfManager
 * @step 2 invoke GetContext
 *
 * @Passcondition: function executes successfully with no exception
 * @id DA-085
 */
int UtcDaliImfManagerGetContext_p_01(void)
{
    tet_printf("getcontext \n");

    ImfManager imfmanager = ImfManager::Get();
    DALI_TEST_CHECK(imfmanager);

    try
    {
        imfmanager.GetContext();
        tet_result(TET_PASS);
    }
    catch(Dali::DaliException& e)
    {
        tet_result(TET_FAIL);
    }
    END_TEST;
}

/**
 * @brief Positive test case 02: check GetContext (returned value isn't NULL)
 *
 * @procedure
 * @step 1 get ImfManager
 * @step 2 invoke GetContext
 * @step 3 check the ImfContext
 *
 * @Passcondition: returned value isn't NULL
 * @id DA-085
 */
int UtcDaliImfManagerGetContext_p_02(void)
{
    tet_printf("GetContext \n");

    ImfManager imfmanager = ImfManager::Get();
    DALI_TEST_CHECK(imfmanager);

    ImfContext context = imfmanager.GetContext();

    DALI_TEST_CHECK(context);

    END_TEST;
}

/**
 * @brief Negative test case 01: invoke GetContext from ImfManager with uninitialized handle
 *
 * @procedure
 * @step 1 create new imfmanager
 * @step 2 catch exception
 *
 * @Passcondition: function musts throw exception
 * @id DA-085
 */
int UtcDaliImfManagerGetContext_n_01(void)
{
    tet_printf(" uninitialized GetContext negative\n");

    ImfManager* imfmanager = new ImfManager();

    try
    {
        imfmanager->GetContext();
        tet_result(TET_FAIL);
    }
    catch (Dali::DaliException& e)
    {
        tet_result(TET_PASS);
    }

    delete imfmanager;

    END_TEST;
}

/**
 * @brief Positive test case 01: try to invoke Activate, should be no exception
 *
 * @procedure
 * @step 1 get ImfManager
 * @step 2 invoke Activate
 *
 * @Passcondition: function executes successfully with no exception
 * @id DA-086
 */
int UtcDaliImfManagerActivate_p_01(void)
{
    tet_printf("activate \n");

    ImfManager imfmanager = ImfManager::Get();
    DALI_TEST_CHECK(imfmanager);

    try
    {
        imfmanager.Activate();
        tet_result(TET_PASS);
    }
    catch(Dali::DaliException& e)
    {
        tet_result(TET_FAIL);
    }
    END_TEST;
}

/**
 * @brief Negative test case 01: invoke Activate from ImfManager with uninitialized handle
 *
 * @procedure
 * @step 1 create new imfmanager
 * @dtep 2 invoke Activate
 * @step 3 catch exception
 *
 * @Passcondition: function musts throw exception
 * @id DA-086
 */
int UtcDaliImfManagerActivate_n_01(void)
{
    tet_printf("Activate negative\n");

    ImfManager* imfmanager = new ImfManager();

    try
    {
        imfmanager->Activate();
        tet_result(TET_FAIL);
    }
    catch (Dali::DaliException& e)
    {
        tet_result(TET_PASS);
    }

    delete imfmanager;

    END_TEST;
}

/**
 * @brief Positive test case 01: try to invoke Deactivate, should be no exception
 *
 * @procedure
 * @step 1 get ImfManager
 * @step 2 invoke Deactivate
 *
 * @Passcondition: function executes successfully with no exception
 * @id DA-087
 */
int UtcDaliImfManagerDeactivate_p_01(void)
{
    tet_printf("deactivate \n");

    ImfManager imfmanager = ImfManager::Get();
    DALI_TEST_CHECK(imfmanager);

    try
    {
        imfmanager.Deactivate();
        tet_result(TET_PASS);
    }
    catch(Dali::DaliException& e)
    {
        tet_result(TET_FAIL);
    }
    END_TEST;
}

/**
 * @brief Negative test case 01: invoke Deactivate from ImfManager with uninitialized handle
 *
 * @procedure
 * @step 1 create new imfmanager
 * @step 2 invoke Deactivate
 * @step 3 catch exception
 *
 * @Passcondition: function musts throw exception
 * @id DA-087
 */
int UtcDaliImfManagerDeactivate_n_01(void)
{
    tet_printf("Deactivate negative\n");

    ImfManager* imfmanager = new ImfManager();

    try
    {
        imfmanager->Deactivate();
        tet_result(TET_FAIL);
    }
    catch (Dali::DaliException& e)
    {
        tet_result(TET_PASS);
    }

    delete imfmanager;

    END_TEST;
}

/**
 * @brief Positive test case 01: try to invoke RestoreAfterFocusLost, should be no exception
 *
 * @procedure
 * @step 1 get ImfManager
 * @step 2 invoke RestoreAfterFocusLost
 *
 * @Passcondition: function executes successfully with no exception
 * @id DA-088
 */
int UtcDaliImfManagerRestoreAfterFocusLost_p_01(void)
{
    tet_printf("RestoreAfterFocusLost \n");

    ImfManager imfmanager = ImfManager::Get();
    DALI_TEST_CHECK(imfmanager);

    try
    {
        imfmanager.RestoreAfterFocusLost();
        tet_result(TET_PASS);
    }
    catch(Dali::DaliException& e)
    {
        tet_result(TET_FAIL);
    }
    END_TEST;
}

/**
 * @brief Positive test case 02: check return value RestoreAferFocusLost. It must be same set value
 *
 * @procedure
 * @step 1 get ImfManager
 * @step 2 set RestoreAferFocusLost (true)
 * @step 3 get RestoreAferFocusLost
 * @step 4 check both value
 * @step 5 set RestoreAferFocusLost (false)
 * @step 6 get RestoreAferFocusLost
 * @step 7 check both value
 *
 * @Passcondition: get value must be same set value
 * @id DA-088
 */
int UtcDaliImfManagerRestoreAferFocusLost_p_02(void)
{
    tet_printf("SetRestoreAferFocusLost \n");

    ImfManager imfmanager = ImfManager::Get();
    DALI_TEST_CHECK(imfmanager);

    imfmanager.SetRestoreAferFocusLost(true);

    DALI_TEST_CHECK( imfmanager.RestoreAfterFocusLost() == true);

    imfmanager.SetRestoreAferFocusLost(false);

    DALI_TEST_CHECK( imfmanager.RestoreAfterFocusLost() == false);

    END_TEST;
}

/**
 * @brief Negative test case 01: invoke RestoreAfterFocusLost from ImfManager with uninitialized handle
 *
 * @procedure
 * @step 1 create new imfmanager
 * @step 2 invoke RestoreAfterFocusLost
 * @step 3 catch exception
 *
 * @Passcondition: function musts throw exception
 * @id DA-088
 */
int UtcDaliImfManagerRestoreAfterFocusLost_n_01(void)
{
    tet_printf("RestoreAfterFocusLost negative\n");

    ImfManager* imfmanager = new ImfManager();

    try
    {
        imfmanager->RestoreAfterFocusLost();
        tet_result(TET_FAIL);
    }
    catch (Dali::DaliException& e)
    {
        tet_result(TET_PASS);
    }

    delete imfmanager;

    END_TEST;
}

/**
 * @brief Positive test case 01: try to invoke SetRestoreAferFocusLost, should be no exception
 *
 * @procedure
 * @step 1 get ImfManager
 * @step 2 invoke SetRestoreAferFocusLost
 *
 * @Passcondition: function executes successfully with no exception
 * @id DA-089
 */
int UtcDaliImfManagerSetRestoreAferFocusLost_p_01(void)
{
    tet_printf("unintialized SetRestoreAferFocusLost \n");

    ImfManager imfmanager = ImfManager::Get();
    DALI_TEST_CHECK(imfmanager);

    try
    {
        imfmanager.SetRestoreAferFocusLost(true);
        tet_result(TET_PASS);
    }
    catch(Dali::DaliException& e)
    {
        tet_result(TET_FAIL);
    }
    END_TEST;
}

/**
 * @brief Positive test case 02: check SetRestoreAferFocusLost. Returned value from RestoreAfterFocusLost must be same set value.
 *
 * @procedure
 * @step 1 get ImfManager
 * @step 2 set RestoreAferFocusLost (true)
 * @step 3 get RestoreAferFocusLost
 * @step 4 check both value
 * @step 5 set RestoreAferFocusLost (false)
 * @step 6 get RestoreAferFocusLost
 * @step 7 check both value
 *
 * @Passcondition: get value must be same set value
 * @id DA-089
 */
int UtcDaliImfManagerSetRestoreAferFocusLost_p_02(void)
{
    tet_printf("SetRestoreAferFocusLost \n");

    ImfManager imfmanager = ImfManager::Get();
    DALI_TEST_CHECK(imfmanager);

    imfmanager.SetRestoreAferFocusLost(true);

    DALI_TEST_CHECK( imfmanager.RestoreAfterFocusLost() == true);

    imfmanager.SetRestoreAferFocusLost(false);

    DALI_TEST_CHECK( imfmanager.RestoreAfterFocusLost() == false);

    END_TEST;
}

/**
 * @brief Negative test case 01: invoke SetRestoreAferFocusLost from ImfManager with uninitialized handle
 *
 * @procedure
 * @step 1 create new imfmanager
 * @step 2 invoke SetRestoreAferFocusLost
 * @step 2 catch exception
 *
 * @Passcondition: function musts throw exception
 * @id DA-089
 */
int UtcDaliImfManagerSetRestoreAfterFocusLost_n_01(void)
{
    tet_printf("SetRestoreAferFocusLost negative\n");

    ImfManager* imfmanager = new ImfManager();

    try
    {
        imfmanager->SetRestoreAferFocusLost(true);
        tet_result(TET_FAIL);
    }
    catch (Dali::DaliException& e)
    {
        tet_result(TET_PASS);
    }

    delete imfmanager;

    END_TEST;
}

/**
 * @brief Positive test case 01: try to invoke Reset, should be no exception
 *
 * @procedure
 * @step 1 get ImfManager
 * @step 2 invoke Reset
 *
 * @Passcondition: function executes successfully with no exception
 * @id DA-090
 */
int UtcDaliImfManagerReset_p_01(void)
{
    tet_printf("Reset \n");

    ImfManager imfmanager = ImfManager::Get();
    DALI_TEST_CHECK(imfmanager);

    try
    {
        imfmanager.Reset();
        tet_result(TET_PASS);
    }
    catch(Dali::DaliException& e)
    {
        tet_result(TET_FAIL);
    }
    END_TEST;
}

/**
 * @brief Negative test case 01: invoke Reset from ImfManager with uninitialized handle
 *
 * @procedure
 * @step 1 create new imfmanager
 * @step 2 invoke Reset
 * @step 3 catch exception
 *
 * @Passcondition: function musts throw exception
 * @id DA-090
 */
int UtcDaliImfManagerReset_n_01(void)
{
    tet_printf("Reset negative\n");

    ImfManager* imfmanager = new ImfManager();

    try
    {
        imfmanager->Reset();
        tet_result(TET_FAIL);
    }
    catch (Dali::DaliException& e)
    {
        tet_result(TET_PASS);
    }

    delete imfmanager;

    END_TEST;
}

/**
 * @brief Positive test case 01: try to invoke NotifyCursorPosition, should be no exception
 *
 * @procedure
 * @step 1 get ImfManager
 * @step 2 invoke NotifyCursorPosition
 *
 * @Passcondition: function executes successfully with no exception
 * @id DA-091
 */
int UtcDaliImfManagerNotifyCursorPosition_p_01(void)
{
    tet_printf("NotifyCursorPosition \n");

    ImfManager imfmanager = ImfManager::Get();
    DALI_TEST_CHECK(imfmanager);

    try
    {
        imfmanager.NotifyCursorPosition();
        tet_result(TET_PASS);
    }
    catch(Dali::DaliException& e)
    {
        tet_result(TET_FAIL);
    }
    END_TEST;
}

/**
 * @brief Negative test case 01: invoke NotifyCursorPosition from ImfManager with uninitialized handle
 *
 * @procedure
 * @step 1 create new imfmanager
 * @step 2 invoke NotifyCursorPosition
 * @step 3 catch exception
 *
 * @Passcondition: function musts throw exception
 * @id DA-091
 */
int UtcDaliImfManagerNotifyCursorPosition_n_01(void)
{
    tet_printf("NotifyCursorPosition negative\n");

    ImfManager* imfmanager = new ImfManager();

    try
    {
        imfmanager->NotifyCursorPosition();
        tet_result(TET_FAIL);
    }
    catch (Dali::DaliException& e)
    {
        tet_result(TET_PASS);
    }

    delete imfmanager;

    END_TEST;
}

/**
 * @brief Positive test case 01: try to invoke SetCursorPosition, should be no exception
 *
 * @procedure
 * @step 1 get ImfManager
 * @step 2 invoke SetCursorPosition
 *
 * @Passcondition: function executes successfully with no exception
 * @id DA-092
 */
int UtcDaliImfManagerSetCursorPosition_p_01(void)
{
    tet_printf("SetCursorPosition \n");

    ImfManager imfmanager = ImfManager::Get();
    DALI_TEST_CHECK(imfmanager);

    try
    {
        imfmanager.SetCursorPosition(1);
        tet_result(TET_PASS);
    }
    catch(Dali::DaliException& e)
    {
        tet_result(TET_FAIL);
    }
    END_TEST;
}

/**
 * @brief Positive test case 02: check SetCursorPosition. Get value must be same get value
 *
 * @procedure
 * @step 1 get ImfManager
 * @step 2 set cursor position (40)
 * @step 3 get cursor position
 * @step 4 check both value
 *
 * @Passcondition: get value must be same set value
 * @id DA-092
 */
int UtcDaliImfManagerSetCursorPosition_p_02(void)
{
    tet_printf("SetGetCursorPosition \n");

    ImfManager imfmanager = ImfManager::Get();
    DALI_TEST_CHECK(imfmanager);

    imfmanager.SetCursorPosition(40);

    DALI_TEST_CHECK(imfmanager.GetCursorPosition() == 40);

    END_TEST;
}

/**
 * @brief Negative test case 01: invoke SetCursorPosition from ImfManager with uninitialized handle
 *
 * @procedure
 * @step 1 create new imfmanager
 * @step 2 invoke SetCursorPosition
 * @step 3 catch exception
 *
 * @Passcondition: function musts throw exception
 * @id DA-092
 */
int UtcDaliImfManagerSetCursorPosition_n_01(void)
{
    tet_printf("SetCursorPosition negative\n");

    ImfManager* imfmanager = new ImfManager();

    try
    {
        imfmanager->SetCursorPosition(58);
        tet_result(TET_FAIL);
    }
    catch (Dali::DaliException& e)
    {
        tet_result(TET_PASS);
    }

    delete imfmanager;

    END_TEST;
}

/**
 * @brief Positive test case 01: try to invoke GetCursorPosition, should be no exception
 *
 * @procedure
 * @step 1 get ImfManager
 * @step 2 invoke GetCursorPosition
 *
 * @Passcondition: function executes successfully with no exception
 * @id DA-093
 */
int UtcDaliImfManagerGetCursorPosition_p_01(void)
{
    tet_printf(" GetCursorPosition \n");

    ImfManager imfmanager = ImfManager::Get();
    DALI_TEST_CHECK(imfmanager);

    try
    {
        imfmanager.GetCursorPosition();
        tet_result(TET_PASS);
    }
    catch(Dali::DaliException& e)
    {
        tet_result(TET_FAIL);
    }
    END_TEST;
}

/**
 * @brief Positive test case 02: Check GetCursorPosition. It must return value same set value
 *
 * @procedure
 * @step 1 get ImfManager
 * @step 2 set cursor position (63)
 * @step 3 get cursor position
 * @step 4 check both value
 *
 * @Passcondition: get value must be same set value
 * @id DA-093
 */
int UtcDaliImfManagerGetCursorPosition_p_02(void)
{
    tet_printf("SetGetCursorPosition \n");

    ImfManager imfmanager = ImfManager::Get();
    DALI_TEST_CHECK(imfmanager);

    imfmanager.SetCursorPosition(63);

    DALI_TEST_CHECK(imfmanager.GetCursorPosition() == 63);

    END_TEST;
}

/**
 * @brief Negative test case 01: invoke GetCursorPosition from ImfManager with uninitialized handle
 *
 * @procedure
 * @step 1 create new imfmanager
 * @step 2 invoke GetCursorPosition
 * @step 3 catch exception
 *
 * @Passcondition: function musts throw exception
 * @id DA-093
 */
int UtcDaliImfManagerGetCursorPosition_n_01(void)
{
    tet_printf("GetCursorPosition negative\n");

    ImfManager* imfmanager = new ImfManager();

    try
    {
        imfmanager->GetCursorPosition();
        tet_result(TET_FAIL);
    }
    catch (Dali::DaliException& e)
    {
        tet_result(TET_PASS);
    }

    delete imfmanager;

    END_TEST;
}

/**
 * @brief Positive test case 01: try to invoke SetSurroundingText, should be no exception
 *
 * @procedure
 * @step 1 get ImfManager
 * @step 2 invoke SetSurroundingText
 *
 * @Passcondition: function executes successfully with no exception
 * @id DA-094
 */
int UtcDaliImfManagerSetSurroundingText_p_01(void)
{
    tet_printf("SetSurroundingText \n");

    ImfManager imfmanager = ImfManager::Get();
    DALI_TEST_CHECK(imfmanager);

    try
    {
        std::string text = "text";
        imfmanager.SetSurroundingText(text);
        tet_result(TET_PASS);
    }
    catch(Dali::DaliException& e)
    {
        tet_result(TET_FAIL);
    }
    END_TEST;
}

/**
 * @brief Positive test case 02: check SetSurroundingText. GetSurroundingText must return set text.
 *
 * @procedure
 * @step 1 get ImfManager
 * @step 2 set surrounding text ("some text")
 * @step 3 get surrounding text
 * @step 4 check both value
 *
 * @Passcondition: get value must be same set value
 * @id DA-094
 */
int UtcDaliImfManagerSetSurroundingText_p_02(void)
{
    tet_printf("SetGetSurroundingText \n");

    ImfManager imfmanager = ImfManager::Get();
    DALI_TEST_CHECK(imfmanager);

    std::string text = "some text";
    imfmanager.SetSurroundingText(text);

    DALI_TEST_CHECK(imfmanager.GetSurroundingText() == text);

    END_TEST;
}

/**
 * @brief Negative test case 01: invoke SetSurroundingText from ImfManager with uninitialized handle
 *
 * @procedure
 * @step 1 create new imfmanager
 * @step 2 invoke SetSurroundingText
 * @step 3 catch exception
 *
 * @Passcondition: function musts throw exception
 * @id DA-094
 */
int UtcDaliImfManagerSetSurroundingText_n_01(void)
{
    tet_printf("SetSurroundingText negative\n");

    ImfManager* imfmanager = new ImfManager();

    std::string text = "some text";

    try
    {
        imfmanager->SetSurroundingText(text);
        tet_result(TET_FAIL);
    }
    catch (Dali::DaliException& e)
    {
        tet_result(TET_PASS);
    }

    delete imfmanager;

    END_TEST;
}

/**
 * @brief Positive test case 01: try to invoke GetSurroundingText, should be no exception
 *
 * @procedure
 * @step 1 get ImfManager
 * @step 2 invoke GetSurroundingText
 *
 * @Passcondition: function executes successfully with no exception
 * @id DA-095
 */
int UtcDaliImfManagerGetSurroundingText_p_01(void)
{
    tet_printf("GetSurroundingText \n");

    ImfManager imfmanager = ImfManager::Get();
    DALI_TEST_CHECK(imfmanager);

    try
    {
        imfmanager.GetSurroundingText();
        tet_result(TET_PASS);
    }
    catch(Dali::DaliException& e)
    {
        tet_result(TET_FAIL);
    }
    END_TEST;
}

/**
 * @brief Positive test case 02: Check GetSurroundingText. It must return value same set value.
 *
 * @procedure
 * @step 1 get ImfManager
 * @step 2 set surrounding text ("some text")
 * @step 3 get surrounding text
 * @step 4 check both value
 *
 * @Passcondition: get value must be same set value
 * @id DA-095
 */
int UtcDaliImfManagerGetSurroundingText_p_02(void)
{
    tet_printf("SetGetSurroundingText \n");

    ImfManager imfmanager = ImfManager::Get();
    DALI_TEST_CHECK(imfmanager);

    std::string text = "some text";
    imfmanager.SetSurroundingText(text);

    DALI_TEST_CHECK(imfmanager.GetSurroundingText() == text);

    END_TEST;
}

/**
 * @brief Negative test case 01: invoke GetSurroundingText from ImfManager with uninitialized handle
 *
 * @procedure
 * @step 1 create new imfmanager
 * @step 2 invoke GetSurroundingText
 * @step 3 catch exception
 *
 * @Passcondition: function musts throw exception
 * @id DA-095
 */
int UtcDaliImfManagerGetSurroundingText_n_01(void)
{
    tet_printf("GetSurroundingText negative\n");

    ImfManager* imfmanager = new ImfManager();

    try
    {
        imfmanager->GetSurroundingText();
        tet_result(TET_FAIL);
    }
    catch (Dali::DaliException& e)
    {
        tet_result(TET_PASS);
    }

    delete imfmanager;

    END_TEST;
}

/**
 * @brief Positive test case 01: try to invoke ActivatedSignal, should be no exception
 *
 * @procedure
 * @step 1 get ImfManager
 * @step 2 invoke ActivatedSignal
 *
 * @Passcondition: function executes successfully with no exception
 * @id DA-096
 */
int UtcDaliImfManagerActivatedSignal_p_01(void)
{
    tet_printf("ActivatedSignal \n");

    ImfManager imfmanager = ImfManager::Get();
    DALI_TEST_CHECK(imfmanager);

    try
    {
        imfmanager.ActivatedSignal();
        tet_result(TET_PASS);
    }
    catch(Dali::DaliException& e)
    {
        tet_result(TET_FAIL);
    }
    END_TEST;
}

/**
 * @brief Positive test case 02: check to invoke ActivatedSignal.  Activate must call callback activated signal(keyboardActivated)
 *        and keyboardActivated will set flag is_keyboard_activeted to true.
 *
 * @procedure
 * @step 1 get ImfManager
 * @step 2 connect ActivatedSignal to function keyboardActivated
 * @step 3 invoke Activate
 * @step 4 Activate must invoke keyboardActivated, and keyboardActivated changes flag is_keyboard_activeted to true
 * @step 5 check flag is_keyboard_activeted
 *
 * @Passcondition: flag is_keyboard_activeted must be true
 * @id DA-096
 */
int UtcDaliImfManagerActivatedSignal_p_02(void)
{
    tet_printf("imfmanager ActivatedSignal \n");

    ImfManager imfmanager = ImfManager::Get();
    DALI_TEST_CHECK(imfmanager);

    imfmanager.ActivatedSignal().Connect(&keyboardActivated);

    imfmanager.Activate();

    DALI_TEST_CHECK( is_keyboard_activeted );

    END_TEST;
}

/**
 * @brief Negative test case 01: invoke ActivatedSignal from ImfManager with uninitialized handle
 *
 * @procedure
 * @step 1 create new imfmanager
 * @step 2 invoke ActivatedSignal
 * @step 3 catch exception
 *
 * @Passcondition: function musts throw exception
 * @id DA-096
 */
int UtcDaliImfManagerActivatedSignal_n_01(void)
{
    tet_printf("ActivatedSignal negative\n");

    ImfManager* imfmanager = new ImfManager();

    try
    {
        imfmanager->ActivatedSignal();
        tet_result(TET_FAIL);
    }
    catch (Dali::DaliException& e)
    {
        tet_result(TET_PASS);
    }

    delete imfmanager;

    END_TEST;
}

/**
 * @brief Positive test case 01: try to invoke EventReceivedSignal, should be no exception
 *
 * @procedure
 * @step 1 get ImfManager
 * @step 2 invoke EventReceivedSignal
 *
 * @Passcondition: function executes successfully with no exception
 * @id DA-097
 */
int UtcDaliImfManagerEventReceivedSignal_p_01(void)
{
    tet_printf("EventReceivedSignal \n");

    ImfManager imfmanager = ImfManager::Get();
    DALI_TEST_CHECK(imfmanager);

    try
    {
        imfmanager.EventReceivedSignal();
        tet_result(TET_PASS);
    }
    catch(Dali::DaliException& e)
    {
        tet_result(TET_FAIL);
    }
    END_TEST;
}

/**
 * @brief Positive test case 02: check to invoke EventReceivedSignal.  Keyboard event must call callback event received signal(receivedEvent)
 *        and receivedEvent will set flag is_event_called to true.
 *
 * @procedure
 * @step 1 get ImfManager
 * @step 2 connect EventReceivedSignal to function receivedEvent
 * @step 3 invoke callback ECORE_IMF_CALLBACK_COMMIT
 * @step 4 callback must invoke receivedEvent, and receivedEvent changes flag is_event_called to true
 * @step 5 check flag is_event_called
 *
 * @Passcondition: flag is_event_called must be true
 * @id DA-097
*/
int UtcDaliImfManagerEventReceivedSignal_p_02(void)
{
    tet_printf("imfmanager ActivatedSignal \n");

    ImfManager imfmanager = ImfManager::Get();
    DALI_TEST_CHECK(imfmanager);

    imfmanager.EventReceivedSignal().Connect(&receivedEvent);

    char* event_info = "event";
    imfmanager_callback_func( const_cast<void*>(imfmanager_callback_data), imfmanager_callback_context, static_cast<void*>(event_info));

    DALI_TEST_CHECK( is_event_called );

    END_TEST;
}

/**
 * @brief Negative test, invoke EventReceivedSignal from ImfManager with uninitialized handle
 *
 * @procedure
 * @step 1 create new imfmanager
 * @step 2 invoke EventReceivedSignal
 * @step 3 catch exception
 *
 * @Passcondition: function musts throw exception
 * @id DA-097
 */
int UtcDaliImfManagerEventReceivedSignal_n_01(void)
{
    tet_printf("EventReceivedSignal negative\n");

    ImfManager* imfmanager = new ImfManager();

    try
    {
        imfmanager->EventReceivedSignal();
    }
    catch (Dali::DaliException& e)
    {
        DALI_TEST_ASSERT(e, "orientation", TEST_LOCATION);
    }

    delete imfmanager;

    END_TEST;
}