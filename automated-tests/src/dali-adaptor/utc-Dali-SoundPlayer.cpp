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

#include <string>
#include <dali/dali.h>
#include <Ecore_X.h>
#include <dali-test-suite-utils.h>

void utc_dali_soundplayer_startup(void)
{
    test_return_value = TET_UNDEF;

    //Initialize the X display connection to the given display.
    ecore_x_init(NULL);
}

void utc_dali_soundplayer_cleanup(void)
{
    //Shut down the Ecore X library.
    ecore_x_shutdown();

    test_return_value = TET_PASS;
}

/**
 * @brief Test default constructor for class SoundPlayer.
 *
 * @procedure
 * @step 1 create default instance of class SoundPlayer
 * @step 2 check if an instance of class SoundPlayer is empty
 *
 * @Passcondition: an empty instance of class SoundPlayer.
 * @id DA-119
 */
int UtcDaliSoundPlayerSoundPlayer_p_01(void)
{
    Dali::SoundPlayer soundPlayer;
    DALI_TEST_CHECK(!soundPlayer);

    END_TEST;
}


#define CREATE_STUB_DALI_ADAPTOR                                                    \
    PositionSize windowPosition = PositionSize(0, 0, 640, 480);                     \
    const std::string name = "windowName";                                          \
    const bool isTransparent = false;                                               \
    Dali::Window window = Dali::Window::New(windowPosition, name, isTransparent);   \
    DALI_TEST_CHECK(window);                                                        \
    Dali::Adaptor::New(window); /*create dali adaptor*/                             \
    DALI_TEST_CHECK(Dali::Adaptor::IsAvailable() == true);

#define DELETE_STUB_DALI_ADAPTOR                                                    \
    if (Dali::Adaptor::IsAvailable() == true)                                       \
    {                                                                               \
        Dali::Adaptor& adaptor = Dali::Adaptor::Get();                              \
        delete &adaptor;                                                            \
    }

/**
 * @brief Positive test of method Get of class SoundPlayer.
 *
 * @procedure
 * @step 1 create stub adaptor
 * @step 2 get an instance of SoundPlayer by calling static
 * method Get of singleton class SoundPlayer.
 * @step 3 check if instance is initialized
 * @step 4 delete stub adaptor
 *
 * @Passcondition: an initialized instance of class SoundPlayer.
 * @id DA-120
 */
int UtcDaliSoundPlayerGet_p_01(void)
{
    CREATE_STUB_DALI_ADAPTOR;

    try
    {
        Dali::SoundPlayer soundPlayer = Dali::SoundPlayer::Get();
        DALI_TEST_CHECK(soundPlayer);
    }
    catch(...)
    {
        DALI_TEST_CHECK(false);
    }

    DELETE_STUB_DALI_ADAPTOR;

    END_TEST;
}

/**
 * @brief Negative test of method Get of class SoundPlayer when adaptor
 * is not created.
 *
 * @procedure
 * @step 1 get an instance of SoundPlayer by calling static
 * method Get of singleton class SoundPlayer
 * @step 2 check if instance is uninitialized
 *
 * @Passcondition: an uninitialized instance of class SoundPlayer.
 * @id DA-120
 */
int UtcDaliSoundPlayerGet_n_01(void)
{
    try
    {
        //if adaptor is not created
        Dali::SoundPlayer soundPlayer = Dali::SoundPlayer::Get();
        DALI_TEST_CHECK(!soundPlayer);
    }
    catch(...)
    {
        DALI_TEST_CHECK(true);
    }

    END_TEST;
}

/**
 * @brief Positive testing method PlaySound of class SoundPlayer.
 *
 * @procedure
 * @step 1 create stub adaptor
 * @step 2 get an initialized instance of SoundPlayer by calling static
 * method Get of singleton class SoundPlayer
 * @step 3 call method PlaySound
 * @step 4 check if return code of calling method PlaySound is not an error code
 * @step 5 delete stub adaptor
 *
 * @Passcondition: return code of method PlaySound is not an error code.
 * @id DA-121
 */
int UtcDaliSoundPlayerPlaySound_p_01(void)
{
    CREATE_STUB_DALI_ADAPTOR;

    Dali::SoundPlayer soundPlayer = Dali::SoundPlayer::Get();
    DALI_TEST_CHECK(soundPlayer);

    const std::string correctFileName = "audio/ailen_wind.wav";
    const int handle = soundPlayer.PlaySound(correctFileName);
    DALI_TEST_CHECK(handle != -1);

    DELETE_STUB_DALI_ADAPTOR;

    END_TEST;
}

/**
 * @brief Negative testing of method PlaySound of class SoundPlayer
 * by its calling with wrong file name.
 *
 * @procedure
 * @step 1 create stub adaptor
 * @step 2 get an initialized instance of SoundPlayer by calling static
 * method Get of singleton class SoundPlayer
 * @step 3 call method PlaySound with wrong file name
 * @step 4 check if return code of calling method PlaySound is an error code
 * @step 5 delete stub adaptor
 *
 * @Passcondition: return code of method PlaySound is an error code.
 * @id DA-121
 */
int UtcDaliSoundPlayerPlaySound_n_01(void)
{
    CREATE_STUB_DALI_ADAPTOR;

    Dali::SoundPlayer soundPlayer = Dali::SoundPlayer::Get();
    DALI_TEST_CHECK(soundPlayer);

    try
    {
        const std::string wrongFileName = "thisIsWrongFileName";

        int handle = soundPlayer.PlaySound(wrongFileName);
        //I suppose that PlaySound return -1 if file name is wrong.
        //This information I found in appropriate library on tizen.org.
        DALI_TEST_CHECK(handle == -1);
    }
    catch(...)
    {
        DALI_TEST_CHECK(false);
    }

    DELETE_STUB_DALI_ADAPTOR;

    END_TEST;
}

/**
 * @brief Negative test for method PlaySound with uninitialized
 * instance of class SoundPlayer.
 *
 * @procedure
 * @step 1 create an uninitialized instance of SoundPlayer
 * @step 2 call method PlaySound with correct file name that has to generate an exception
 * @step 3 an exception generated should be caught by try/catch block
 *
 * @Passcondition: calling method PlaySound should throw an exception
 * that will catch in ty/catch block.
 * @id DA-121
 */
int UtcDaliSoundPlayerPlaySound_n_02(void)
{
    //If SoundPlayer is unitilized..
    Dali::SoundPlayer soundPlayer;
    DALI_TEST_CHECK(!soundPlayer);

    try
    {
        const std::string correctFileName = "audio/ailen_wind.wav";
        (void)soundPlayer.PlaySound(correctFileName);
        tet_printf( "SoundPlayer is uninitialized. Calling method SoundPlayer.PlaySound should throw an exception." );
        DALI_TEST_CHECK(false);
    }
    catch(...)
    {
        DALI_TEST_CHECK(true);
    }

    END_TEST;
}

/**
 * @brief Positive testing method Stop of class SoundPlayer.
 *
 * @procedure
 * @step 1 create stub adaptor
 * @step 2 get an initialized instance of SoundPlayer by calling static
 * method Get of singleton class SoundPlayer
 * @step 3 call method PlaySound with correct file name
 * @step 4 check if return code of calling method PlaySound is not an error code
 * @step 5 call method Stop that should normal finish
 * @step 6 delete stub adaptor
 *
 * @Passcondition: calling method Stop that should normal finish
 * @id DA-122
 */
int UtcDaliSoundPlayerStop_p_01(void)
{
    CREATE_STUB_DALI_ADAPTOR;

    Dali::SoundPlayer soundPlayer = Dali::SoundPlayer::Get();
    DALI_TEST_CHECK(soundPlayer);

    try
    {
        const std::string correctFileName = "audio/ailen_wind.wav";
        const int handle = soundPlayer.PlaySound(correctFileName);
        DALI_TEST_CHECK(handle != -1);

        soundPlayer.Stop(handle);
    }
    catch(...)
    {
        DALI_TEST_CHECK(false);
    }

    DELETE_STUB_DALI_ADAPTOR;

    END_TEST;
}

/**
 * @brief Negative test for method Stop with
 * uninitialized instance of class SoundPlayer.
 *
 * @procedure
 * @step 1 create an unitilized instance of class SoundPlayer
 * @step 2 call method Stop of unitilized instance of class SoundPlayer
 * that has to generate an exception
 * @step 3 cath generated exception
 *
 * @Passcondition: calling method Stop of unitilized instance
 * has to generate an exception
 * @id DA-122
 */
int UtcDaliSoundPlayerStop_n_01(void)
{
    Dali::SoundPlayer soundPlayer;
    DALI_TEST_CHECK(!soundPlayer);

    const int handle = 0; //any integer number
    try
    {
        soundPlayer.Stop(handle);
        tet_printf( "SoundPlayer is uninitialized. Calling method SoundPlayer.Stop should throw an exception." );
        DALI_TEST_CHECK(false);
    }
    catch(...)
    {
        DALI_TEST_CHECK(true);
    }

    END_TEST;
}

namespace
{
    void emptyConnectingFunc(Dali::SoundPlayer& /*soundPlayer*/)
    {
        //do nothing
    }
}

/**
 * @brief Positive testing method SoundPlayFinishedSignal of class SoundPlayer.
 *
 * @procedure
 * @step 1 create stub adaptor
 * @step 2 get an empty instance of SoundPlayer by calling static
 * method Get of singleton class SoundPlayer
 * @step 3 call method PlaySound with correct file name
 * @step 4 get reference to SoundPlayFinishedSignalV2 of SoundPlayer
 * @step 5 check if there are no signals and no connections
 * @step 6 add one callback function to SoundPlayer
 * @step 7 check if it was added
 * @step 8 delete stub adaptor
 *
 * @Passcondition: calling method SoundPlayFinishedSignal of SoundPlayer
 * should normal finish
 * @id DA-123
 */
int UtcDaliSoundPlayerSoundPlayFinishedSignal_p_01(void)
{
    CREATE_STUB_DALI_ADAPTOR;

    Dali::SoundPlayer soundPlayer = Dali::SoundPlayer::Get();
    DALI_TEST_CHECK(soundPlayer);

    try
    {
        const std::string correctFileName = "audio/ailen_wind.wav";
        const int handle = soundPlayer.PlaySound(correctFileName);
        DALI_TEST_CHECK(handle != -1);

        Dali::SoundPlayer::SoundPlayFinishedSignalV2& soundPlayFinishedSignalV2 = soundPlayer.SoundPlayFinishedSignal();
        DALI_TEST_CHECK(soundPlayFinishedSignalV2.Empty());
        DALI_TEST_CHECK(soundPlayFinishedSignalV2.GetConnectionCount() == 0);

        soundPlayFinishedSignalV2.Connect(emptyConnectingFunc);
        DALI_TEST_CHECK(!soundPlayFinishedSignalV2.Empty());
        DALI_TEST_CHECK(soundPlayFinishedSignalV2.GetConnectionCount() == 1);

        if (!soundPlayFinishedSignalV2.Empty())
        {
            soundPlayFinishedSignalV2.Disconnect(emptyConnectingFunc);
            DALI_TEST_CHECK(soundPlayFinishedSignalV2.Empty());
            DALI_TEST_CHECK(soundPlayFinishedSignalV2.GetConnectionCount() == 0);
        }
    }
    catch(...)
    {
        DALI_TEST_CHECK(false);
    }

    DELETE_STUB_DALI_ADAPTOR;

    END_TEST;
}

/**
 * @brief Negative test for method SoundPlayFinishedSignal
 * with uninitialized instance of class SoundPlayer.
 *
 * @procedure
 * @step 1 create an empty instance of SoundPlayer
 * @step 2 calling method of empty instance of SoundPlayer should generate an exception
 *
 * @Passcondition: generated exception should caught by try/catch block
 * @id DA-123
 */
int UtcDaliSoundPlayerSoundPlayFinishedSignal_n_01(void)
{
    Dali::SoundPlayer soundPlayer;
    DALI_TEST_CHECK(!soundPlayer);

    try
    {
        Dali::SoundPlayer::SoundPlayFinishedSignalV2& soundPlayFinishedSignalV2 = soundPlayer.SoundPlayFinishedSignal();
        tet_printf( "SoundPlayer is uninitialized. Calling method SoundPlayer.SoundPlayFinishedSignal should throw an exception." );
        DALI_TEST_CHECK(false);
    }
    catch(...)
    {
        DALI_TEST_CHECK(true);
    }

    END_TEST;
}
