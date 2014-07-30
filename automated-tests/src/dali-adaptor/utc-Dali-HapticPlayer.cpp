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
#include <sstream>
#include <stdlib.h>
#include <dali/dali.h>
#include <dali-test-suite-utils.h>
#include <Ecore_X.h>

using namespace Dali;

static Dali::Adaptor* adaptor;

void utc_dali_haptic_player_startup(void)
{
    ecore_x_init(NULL);
    Dali::PositionSize positionSize(0, 0, 800, 1280);
    Dali::Window window = Dali::Window::New(positionSize, "Haptic Application");
    adaptor = &Dali::Adaptor::New(window);

    test_return_value = TET_UNDEF;
}

void utc_dali_haptic_player_cleanup(void)
{
    delete adaptor;
    ecore_x_shutdown();

    test_return_value = TET_PASS;
}

/**
 * @brief HapticPlayer default constructor. Handle will be uninitialized.
 *
 * @procedure
 * @step 1 create instance of HapticPlayer using default constructor
 * @step 2 check the pointer to the instance
 *
 * @Passcondition: Handle is initialized.
 * @id DA-078
 */
int UtcDaliHapticPlayerHapticPlayer_p_01(void)
{
    tet_printf("HapticPlayer constructor called\n");
    HapticPlayer * hp = new HapticPlayer();
    DALI_TEST_CHECK(!*hp);
    delete hp;
    END_TEST;
}

/**
 * @brief Create an initialized handle to the HapticPlayer.
 *
 * @procedure
 * @step 1 get initialized handle
 * @step 2 check the handle
 *
 * @Passcondition: Handle is correct.
 * @id DA-079
 */
int UtcDaliHapticPlayerGet_p_01(void)
{
    tet_printf("HapticPlayer Get called\n");
    HapticPlayer hp = HapticPlayer::Get();
    DALI_TEST_CHECK(hp);
    END_TEST;
}

/**
 * @brief Negative test case.
 *
 * @procedure
 * @step 1 delete adaptor
 * @step 2 try to get an initialized handler
 * @step 3 catch an exception, if rise
 *
 * @Passcondition: Exception is generated.
 * @id DA-079
 */
int UtcDaliHapticPlayerGet_n_01(void)
{
    tet_printf("HapticPlayer Get called\n");
    HapticPlayer hp;
    bool testPass = false;
    delete adaptor;
    try
    {
        hp = HapticPlayer::Get();
        testPass = false;
    }
    catch(DaliException ex)
    {
        testPass = true;
    }
    DALI_TEST_CHECK( testPass );

    Dali::PositionSize positionSize(0, 0, 800, 1280);
    Dali::Window window = Dali::Window::New(positionSize, "Haptic Application 2");
    adaptor = &Dali::Adaptor::New(window);

    END_TEST;
}

/**
 * @brief Plays a monotone vibration.
 *
 * @procedure
 * @step 1 get an initialized handler for HapticPlayer
 * @step 2 call PlayMonotone function with valid param
 * @step 3 check if exception was rised
 *
 * @Passcondition: No exception rised.
 * @id DA-080
 */
int UtcDaliHapticPlayerPlayMonotone_p_01(void)
{
    tet_printf("HapticPlayer PlayMonotone called\n");
    bool testPass = false;
    HapticPlayer hp = HapticPlayer::Get();
    try
    {
        hp.PlayMonotone(2);
        /*TODO Write a method that will help to check haptic player PlayMonotone function */
        testPass = true;
    }
    catch(...)
    {
        testPass = false;
    }
    DALI_TEST_CHECK( testPass );
    END_TEST;
}

/**
 * @brief Negative test case for PlayMonotone. Unexpected input.
 *
 * @procedure
 * @step 1 get an initialized handler for HapticPlayer
 * @step 2 call PlayMonotone with invalid param
 * @step 3 check if exception was rised
 *
 * @Passcondition: No exception rised.
 * @id DA-080
 */
int UtcDaliHapticPlayerPlayMonotone_n_01(void)
{
    tet_printf("HapticPlayer PlayMonotone called\n");
    bool testPass = false;
    HapticPlayer hp = HapticPlayer::Get();
    try
    {
        hp.PlayMonotone(0);
        testPass = true;
    }
    catch(...)
    {
        testPass = false;
    }
    DALI_TEST_CHECK( testPass );
    END_TEST;
}

/**
 * @brief Negative test case for PlayMonotone. Uninitialized handler.
 *
 * @procedure
 * @step 1 get an uninitialized handler for HapticPlayer
 * @step 2 call PlayMonotone with valid param
 * @step 3 check if exception was rised
 *
 * @Passcondition: Exception rised.
 * @id DA-080
 */
int UtcDaliHapticPlayerPlayMonotone_n_02(void)
{
    tet_printf("HapticPlayer PlayMonotone called\n");
    bool testPass = false;
    HapticPlayer hp;
    try
    {
        hp.PlayMonotone(3);
        testPass = false;
    }
    catch(...)
    {
        testPass = true;
    }
    DALI_TEST_CHECK( testPass );
    END_TEST;
}

/**
 * @brief Plays vibration in predefined patterns.
 *
 * @procedure
 * @step 1 get initialized handler for HapticPlayer
 * @step 2 call PlayFile function
 * @step 3 return TET_UNDEF because correct file does not exist right now
 *
 * @Passcondition: Function vibrates in predefined patterns.
 * @id DA-081
 */
int UtcDaliHapticPlayerPlayFile_p_01(void)
{
    tet_printf("HapticPlayer PlayFile called\n");
    bool testPass = false;
    HapticPlayer hp = HapticPlayer::Get();
    /*TODO Create correct file to check this function. */
    hp.PlayFile("someFile");
    return TET_UNDEF;
}

/**
 * @brief Negative test case. Input file does not exist.
 *
 * @procedure
 * @step 1 get an initialized handler for HapticPlayer
 * @step 2 call PlayFile function with invalid param
 * @step 3 check for exception
 *
 * @Passcondition: No exception rised.
 * @id DA-081
 */
int UtcDaliHapticPlayerPlayFile_n_01(void)
{
    tet_printf("HapticPlayer PlayFile called\n");
    bool testPass = false;
    HapticPlayer hp = HapticPlayer::Get();
    try
    {
        hp.PlayFile("unexisting.doc.mp3.avi");
        testPass = true;
    }
    catch(...)
    {
        testPass = false;
    }
    DALI_TEST_CHECK( testPass );
    END_TEST;
}

/**
 * @brief Negative test case. Uninitialized HapticPlayer.
 *
 * @procedure
 * @step 1 get an unitialized handler for HapticPlayer
 * @step 2 call PlayFile
 * @step 3 check if exception rise
 *
 * @Passcondition: Exception rise.
 * @id DA-081
 */
int UtcDaliHapticPlayerPlayFile_n_02(void)
{
    tet_printf("HapticPlayer PlayFile called\n");
    bool testPass = false;
    HapticPlayer hp;
    try
    {
        hp.PlayFile("unexisting.doc.mp3.avi");
        testPass = false;
    }
    catch(...)
    {
        testPass = true;
    }
    DALI_TEST_CHECK( testPass );
    END_TEST;
}

/**
 * @brief Stops the currently playing vibration effects.
 *
 * @procedure
 * @step 1 get an initialized HapticPlayer handle
 * @step 2 start vibration
 * @step 3 call stop function
 * @step 4 check if exception rised
 *
 * @Passcondition: No exception rised.
 * @id DA-082
 */
int UtcDaliHapticPlayerStop_p_01(void)
{
    tet_printf("HapticPlayer Stop called\n");
    bool testPass = false;
    HapticPlayer hp = HapticPlayer::Get();
    try
    {
        hp.PlayMonotone(3);
        hp.Stop();
        /*TODO Write a method that will help to check haptic player Stop function */
        testPass = true;
    }
    catch(...)
    {
        testPass = false;
    }
    DALI_TEST_CHECK( testPass );
    END_TEST;
}

/**
 * @brief Negative test case. Vibration has not started before stop.
 *
 * @procedure
 * @step 1 get an initialized HapticPlayer handle
 * @step 2 call stop function
 * @step 3 check if exception rised
 *
 * @Passcondition: No exception rised.
 * @id DA-082
 */
int UtcDaliHapticPlayerStop_n_01(void)
{
    tet_printf("HapticPlayer Stop called\n");
    bool testPass = false;
    HapticPlayer hp = HapticPlayer::Get();
    try
    {
        hp.Stop();
        /*TODO Write a method that will help to check haptic player Stop function */
        testPass = true;
    }
    catch(...)
    {
        testPass = false;
    }
    DALI_TEST_CHECK( testPass );
    END_TEST;
}

/**
 * @brief Negative test case. HapticPlayer was not initialized.
 *
 * @procedure
 * @step 1 get an uninitialized HapticPlayer handle
 * @step 2 call stop function
 * @step 3 check if exception rised
 *
 * @Passcondition: An exception rised.
 * @id DA-082
 */
int UtcDaliHapticPlayerStop_n_02(void)
{
    tet_printf("HapticPlayer Stop called\n");
    bool testPass = false;
    HapticPlayer hp;
    try
    {
        hp.Stop();
        testPass = false;
    }
    catch(...)
    {
        testPass = true;
    }
    DALI_TEST_CHECK( testPass );
    END_TEST;
}
