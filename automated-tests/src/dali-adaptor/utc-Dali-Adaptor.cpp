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

void utc_dali_adaptor_startup(void)
{
    ecore_x_init(NULL);
    test_return_value = TET_UNDEF;
}

void utc_dali_adaptor_cleanup(void)
{
    test_return_value = TET_PASS;
    ecore_x_shutdown();
}

//A callback function for AddIdle method
void callFunction(void) { return; }


/**
* @brief create a New Adaptor
*
* @procedure
* @step 1 set window position
* @step 2 create window
* @step 3 create an adaptor which uses that window
* @step 4 checks whether the adaptor is available
*
* @Passcondition: adaptor is created
* @id DA-034
*/
int UtcDaliAdaptorNew_p_01(void)
{
    tet_printf("Adaptor creation - static Adaptor& New( Window window )\n");
    Dali::PositionSize positionSize(0, 0, 800, 1280);
    Dali::Window window = Dali::Window::New( positionSize, "My Application" );

    Dali::Adaptor* adaptor = &Dali::Adaptor::New( window );

    DALI_TEST_CHECK( adaptor->IsAvailable() );
    delete adaptor;

    END_TEST;
}

/**
* @brief create a New Adaptor with base layout
*
* @procedure
* @step 1 set window position
* @step 2 create window
* @step 3 create base layout
* @step 4 create an adaptor which uses that window
* @step 5 checks whether the adaptor is available
*
* @Passcondition: adaptor is created
* @id DA-035
*/
int UtcDaliAdaptorNew_p_02(void)
{
    tet_printf("Adaptor creation - static Adaptor& New( Window window, const DeviceLayout& baseLayout )\n");
    Dali::PositionSize positionSize(0, 0, 800, 1280);
    Dali::Window window = Dali::Window::New( positionSize, "My Application" );
    DeviceLayout layout(Vector2(600.0f, 800.0f), 3.65f, Vector2(316.0f, 316.0f), 20.0f);

    Dali::Adaptor* adaptor = &Dali::Adaptor::New( window, layout );

    DALI_TEST_CHECK( adaptor->IsAvailable() );
    delete adaptor;

    END_TEST;
}

/**
* @brief start the Adaptor
*
* @procedure
* @step 1 set window position
* @step 2 create window
* @step 3 create an adaptor which uses that window
* @step 4 starts the adaptor
* @step 5 checks whether the adaptor is available
*
* @Passcondition: API executes successfully
* @id DA-036
*/
int UtcDaliAdaptorStart_p_01(void)
{
    tet_printf("Adaptor starting - void Start()\n");
    Dali::PositionSize positionSize(0, 0, 800, 1280);
    Dali::Window window = Dali::Window::New( positionSize, "My Application" );
    Dali::Adaptor* adaptor = &Dali::Adaptor::New( window );
    adaptor->Start();

    DALI_TEST_CHECK( adaptor->IsAvailable() );
    delete adaptor;

    END_TEST;
}

/**
* @brief pause the Adaptor
*
* @procedure
* @step 1 set window position
* @step 2 create window
* @step 3 create an adaptor which uses that window
* @step 4 pause the adaptor
* @step 5 checks whether the adaptor is available
*
* @Passcondition: API executes successfully
* @id DA-037
*/
int UtcDaliAdaptorPause_p_01(void)
{
    tet_printf("Adaptor pausing - void Pause()\n");
    Dali::PositionSize positionSize(0, 0, 800, 1280);
    Dali::Window window = Dali::Window::New( positionSize, "My Application" );
    Dali::Adaptor* adaptor = &Dali::Adaptor::New( window );
    adaptor->Start();
    adaptor->Pause();

    DALI_TEST_CHECK( adaptor->IsAvailable() );
    delete adaptor;

    END_TEST;
}

/**
* @brief resume the Adaptor
*
* @procedure
* @step 1 set window position
* @step 2 create window
* @step 3 create an adaptor which uses that window
* @step 4 resume the adaptor
* @step 5 checks whether the adaptor is available
*
* @Passcondition: API executes successfully
* @id DA-038
*/
int UtcDaliAdaptorResume_p_01(void)
{
    tet_printf("Adaptor resuming - void Resume()\n");
    Dali::PositionSize positionSize(0, 0, 800, 1280);
    Dali::Window window = Dali::Window::New( positionSize, "My Application" );
    Dali::Adaptor* adaptor = &Dali::Adaptor::New( window );
    adaptor->Start();
    adaptor->Pause();

    // Resumes the Adaptor, if previously paused, otherwise nothing happens
    adaptor->Resume();

    DALI_TEST_CHECK( adaptor->IsAvailable() );
    delete adaptor;

    END_TEST;
}

/**
* @brief stop the Adaptor
*
* @procedure
* @step 1 set window position
* @step 2 create window
* @step 3 create an adaptor which uses that window
* @step 4 stop the adaptor
* @step 5 checks whether the adaptor is available
*
* @Passcondition: API executes successfully
* @id DA-039
*/
int UtcDaliAdaptorStop_p_01(void)
{
    tet_printf("Adaptor stoping - void Stop()\n");
    Dali::PositionSize positionSize(0, 0, 800, 1280);
    Dali::Window window = Dali::Window::New( positionSize, "My Application" );
    Dali::Adaptor* adaptor = &Dali::Adaptor::New( window );
    adaptor->Start();
    adaptor->Stop();

    DALI_TEST_CHECK( adaptor->IsAvailable() );
    delete adaptor;

    END_TEST;
}

/**
* @brief addition a callback function to the adaptor
*
* @procedure
* @step 1 set window position
* @step 2 create window
* @step 3 create an adaptor which uses that window
* @step 4 start adaptor (callback function only added if the Adaptor is actually running)
* @step 5 add callback function
*
* @Passcondition: function is added successfully
* @id DA-040
*/
int UtcDaliAdaptorAddIdle_p_01(void)
{
    tet_printf("Adaptor AddIdle - bool AddIdle( boost::function<void(void)> callBack )\n");
    Dali::PositionSize positionSize(0, 0, 800, 1280);
    Dali::Window window = Dali::Window::New( positionSize, "My Application" );
    Dali::Adaptor* adaptor = &Dali::Adaptor::New( window );
    boost::function<void(void)> callBack = &callFunction;
    adaptor->Start();

    DALI_TEST_CHECK( adaptor->AddIdle(callBack) );
    delete adaptor;

    END_TEST;
}

/**
* @brief try to add a callback function to the adaptor without it's running
*
* @procedure
* @step 1 set window position
* @step 2 create window
* @step 3 create an adaptor which uses that window
* @step 4 add callback function
*
* @Passcondition: function isn't added
* @id DA-040
*/
int UtcDaliAdaptorAddIdle_n_01(void)
{
    tet_printf("Adaptor AddIdle Negative- bool AddIdle( boost::function<void(void)> callBack )\n");
    Dali::PositionSize positionSize(0, 0, 800, 1280);
    Dali::Window window = Dali::Window::New( positionSize, "My Application" );
    Dali::Adaptor* adaptor = &Dali::Adaptor::New( window );
    boost::function<void(void)> callBack = &callFunction;

    DALI_TEST_CHECK( !adaptor->AddIdle(callBack) );
    delete adaptor;

    END_TEST;
}