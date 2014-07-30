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
#include <Ecore_X.h>
#include <dali-test-suite-utils.h>

#include "math.h"

extern int ECORE_X_EVENT_CLIENT_MESSAGE;

using namespace Dali ;

static Window* window = NULL;
static Adaptor* adaptor= NULL;

void utc_dali_orientation_startup(void)
{
    test_return_value = TET_UNDEF;
    ecore_x_init(NULL);

    Rect<int> positionSize(0, 0, 800, 1280);

    window = new Window(Window::New(positionSize, "orientation"));
    adaptor = &Adaptor::New(*window);
}

void utc_dali_orientation_cleanup(void)
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

static bool test_orient = false;
static Ecore_Event_Handler_Cb orientation_callback_func = NULL;
static const void* orientation_callback_data = NULL;
static int orientationId = 0;

extern "C"
{
Ecore_Event_Handler* ecore_event_handler_add( int type, Ecore_Event_Handler_Cb func, const void *data)
{
    if ( type == ECORE_X_EVENT_CLIENT_MESSAGE )
    {
        orientation_callback_func = func;
        orientation_callback_data = data;
    }


    orientationId++;
    return (Ecore_Event_Handler*)orientationId;
}

void* ecore_event_handler_del( Ecore_Event_Handler* event_handler)
{
    orientation_callback_func = NULL;
    orientation_callback_data = NULL;
    return NULL;
}
}

void OrientationChanged(Orientation orientation)
{
    test_orient = true;
}

void rotate(int degree)
{
    Ecore_X_Event_Client_Message clientMessageEvent;
    clientMessageEvent.message_type = ECORE_X_ATOM_E_WINDOW_ROTATION_CHANGE_PREPARE;
    clientMessageEvent.data.l[1] = degree;
    clientMessageEvent.data.l[2] = 0;
    clientMessageEvent.data.l[3] = 200;
    clientMessageEvent.data.l[4] = 100;

    orientation_callback_func(const_cast<void*>(orientation_callback_data), 0, &clientMessageEvent);

    clientMessageEvent.message_type = ECORE_X_ATOM_E_WINDOW_ROTATION_CHANGE_REQUEST;
    orientation_callback_func(const_cast<void*>(orientation_callback_data), 0, &clientMessageEvent);
}

/**
 * @brief Positive test case 01: creating Orientation via constructor.
 *
 * @procedure
 * @step 1 create new Orientation
 * @step 2 check handler uninitialized
 *
 * @Passcondition: method successfully creates new Orientation with uninitialized handler
 * @id DA-101
 */
int UtcDaliOrientationOrientation_p_01(void)
{
    tet_printf("orientation creation \n");
    Orientation* orient = new Orientation();

    DALI_TEST_CHECK( !(*orient) );

    delete orient;

    END_TEST;
}

/**
 * @brief Positive test case 01: try to invoke GetDegrees, should be no exception
 *
 * @procedure
 * @step 1 get Orientation from window
 * @step 2 invoke GetDegrees
 *
 * @Passcondition: function executes successfully with no exception
 * @id DA-102
 */
int UtcDaliOrientationGetDegrees_p_01(void)
{
    tet_printf("get degrees \n");

    Orientation orient = window->GetOrientation();
    DALI_TEST_CHECK(orient);

    try
    {
        orient.GetDegrees();
        tet_result(TET_PASS);
    }
    catch (Dali::DaliException& e)
    {
        tet_result(TET_FAIL);
    }
    END_TEST;
}

/**
 * @brief Positive test case 02: check GetDegrees for return correct value
 *
 * @procedure
 * @step 1 get Orientation from window
 * @step 2 check current degree (0)
 * @step 3 rotate window 90 degree
 * @step 4 check current degree (90)
 *
 * @Passcondition: function must return value same set value from callback
 * @id DA-102
 */
int UtcDaliOrientationGetDegrees_p_02(void)
{
    tet_printf("orientation get degrees \n");

    Orientation orient = window->GetOrientation();

    DALI_TEST_CHECK( orient.GetDegrees() == 0);

    rotate(90);

    DALI_TEST_CHECK( orient.GetDegrees() == 90);

    END_TEST;
}

/**
 * @brief Negative test case 01: invoke GetDegrees from Orientation with uninitialized handler
 *
 * @procedure
 * @step 1 create new Orientation
 * @step 2 invoke GetDegrees
 * @step 3 catch exception
 *
 * @Passcondition: function musts throw exception
 * @id DA-102
 */
int UtcDaliOrientationGetDegrees_n_01(void)
{
    tet_printf("uninitialized get degrees \n");

    Orientation* orient = new Orientation();

    try
    {
        orient->GetDegrees();
        tet_result(TET_FAIL);
    }
    catch (Dali::DaliException& e)
    {
        tet_result(TET_PASS);
    }

    delete orient;

    END_TEST;
}

/**
 * @brief Positive test case 01: try to invoke GetRadians, should be no exception
 *
 * @procedure
 * @step 1 get Orientation from window
 * @step 2 invoke GetRadians
 *
 * @Passcondition: function executes successfully with no exception
 * @id DA-103
 */
int UtcDaliOrientationGetRadians_p_01(void)
{
    tet_printf("get radians \n");

    Orientation orient = window->GetOrientation();
    DALI_TEST_CHECK(orient);

    try
    {
        orient.GetRadians();
        tet_result(TET_PASS);
    }
    catch (Dali::DaliException& e)
    {
        tet_result(TET_FAIL);
    }
    END_TEST;
}

/**
 * @brief Positive test case 02: check GetRadians for return correct value
 *
 * @procedure
 * @step 1 get Orientation from window
 * @step 2 check current degree (0.f)
 * @step 3 rotate window 180 degree (PI)
 * @step 4 check current degree (PI)
 *
 * @Passcondition: function must return value same set value from callback
 * @id DA-103
 */
int UtcDaliOrientationGetRadians_p_02(void)
{
    tet_printf("orientation get degrees \n");

    Orientation orient = window->GetOrientation();

    DALI_TEST_CHECK( orient.GetRadians() == 0.f);

    rotate(180);

    DALI_TEST_CHECK( orient.GetRadians() == M_PI);

    END_TEST;
}

/**
 * @brief Negative test case 01: invoke GetRadians from Orientation with uninitialized handler
 *
 * @procedure
 * @step 1 create new Orientation
 * @step 2 invoke GetRadians
 * @step 3 catch exception
 *
 * @Passcondition:  function musts throw exception
 * @id DA-103
 */
int UtcDaliOrientationGetRadians_n_01(void)
{
    tet_printf("uninitialized get radians \n");

    Orientation* orient = new Orientation();

    try
    {
        orient->GetRadians();
        tet_result(TET_FAIL);
    }
    catch (Dali::DaliException& e)
    {
        tet_result(TET_PASS);
    }

    delete orient;

    END_TEST;
}

/**
 * @brief Positive test case 01: try to invoke ChangedSignal, should be no exception
 *
 * @procedure
 * @step 1 get Orientation from window
 * @step 2 invoke ChangedSignal
 *
 * @Passcondition: function executes successfully with no exception
 * @id DA-104
 */
int UtcDaliOrientationChangedSignal_p_01(void)
{
    tet_printf("ChangedSignal \n");

    Orientation orient = window->GetOrientation();
    DALI_TEST_CHECK(orient);

    try
    {
        orient.ChangedSignal().Connect(&OrientationChanged);
        tet_result(TET_PASS);
    }
    catch (Dali::DaliException& e)
    {
        tet_result(TET_FAIL);
    }
    END_TEST;
}

/**
 * @brief Positive test case 02: check to invoke ChangedSignal. Rotation must call callback changed signal(OrientationChanged)
 *        and OrientationChanged will set flag test_orient to true.
 *
 * @procedure
 * @step 1 get Orientation from window
 * @step 2 connect ChangedSignal to function OrientationChanged
 * @step 3 rotate window
 * @step 4 rotation must invoke OrientationChanged, and OrientationChanged changes flag test_orient to true
 * @step 5 check flag test_orient
 *
 * @Passcondition: flag test_orient must be true
 * @id DA-104
 */
int UtcDaliOrientationChangedSignal_p_02(void)
{
    tet_printf("orientation changed signal \n");

    Orientation orient = window->GetOrientation();

    DALI_TEST_CHECK(orient);

    orient.ChangedSignal().Connect(&OrientationChanged);

    rotate(90);

    DALI_TEST_CHECK( test_orient );

    END_TEST;
}

/**
 * @brief Negative test, invoke ChangedSignal from Orientation with uninitialized handler
 *
 * @procedure
 * @step 1 create new Orientation
 * @step 2 invoke ChangedSignal
 * @step 3 catch exception
 *
 * @Passcondition:  function musts throw exception
 * @id DA-104
 */
int UtcDaliOrientationChangedSignal_n_01(void)
{
    tet_printf("orientation ChangedSignal \n");

    Orientation* orient = new Orientation();

    try
    {
        orient->ChangedSignal();
        tet_result(TET_FAIL);
    }
    catch (Dali::DaliException& e)
    {
        tet_result(TET_PASS);
    }

    delete orient;

    END_TEST;
}