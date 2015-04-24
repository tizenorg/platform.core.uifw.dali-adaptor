/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

#include <dali/dali.h>
#include <dali-test-suite-utils.h>

extern int gArgc;
extern char ** gArgv;

using namespace Dali;

const float DEFAULT_STEREO_BASE( 65.0f );


enum TEST_TYPE
{
  NEW = 0,
  VIEW_MODE = 1,
  STEREO_BASE = 2
};

void utc_dali_application_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_application_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{

struct MyTestApp : public ConnectionTracker
{
  MyTestApp( Application& app)
    :MyTestApp(app, NEW)
  {
  }

  MyTestApp( Application& app, int type )
  : initCalled( false ),
    tickCalled( false ),
    application( app ),
    mTestType( type )
  {
    application.InitSignal().Connect( this, &MyTestApp::Create );
  }

  void Create(Application& app)
  {
    initCalled = true;

    timer = Timer::New(500);
    timer.TickSignal().Connect(this, &MyTestApp::Tick);
    timer.Start();

    excuteTest();
  }

  bool Tick()
  {
    tickCalled = true;
    timer.Stop();
    application.Quit();
    return true;
  }

  bool IsDoneSuccessfully()
  {
    return (initCalled && tickCalled);
  }

  void excuteTest()
  {
    switch (mTestType)
    {
      case VIEW_MODE:
        testViewMode();
        break;
      case STEREO_BASE:
        testStereoBase();
        break;
    }
  }

  void testViewMode()
  {
    application.SetViewMode(STEREO_HORIZONTAL);
    DALI_TEST_CHECK( application.GetViewMode() ==  STEREO_HORIZONTAL );

    application.SetViewMode(STEREO_VERTICAL);
    DALI_TEST_CHECK( application.GetViewMode() ==  STEREO_VERTICAL );

    application.SetViewMode(STEREO_INTERLACED);
    DALI_TEST_CHECK( application.GetViewMode() ==  STEREO_INTERLACED );
  }

  void testStereoBase()
  {
    application.SetStereoBase(DEFAULT_STEREO_BASE * 2);
    DALI_TEST_CHECK( application.GetStereoBase() ==  DEFAULT_STEREO_BASE * 2 );

    application.SetStereoBase(DEFAULT_STEREO_BASE);
    DALI_TEST_CHECK( application.GetStereoBase() ==  DEFAULT_STEREO_BASE);
  }

  // Data
  bool initCalled;
  bool tickCalled;
  Application& application;
  int mTestType;
  Timer timer;
};

} // unnamed namespace

int UtcDaliApplicationNew(void)
{
  Application application = Application::New();

  MyTestApp testApp( application );

  DALI_TEST_CHECK( application );

  END_TEST;
}

int UtcDaliApplicationInitSignal(void)
{
  Application application = Application::New(&gArgc, &gArgv);

  MyTestApp testApp( application );

  application.MainLoop();

  DALI_TEST_CHECK( testApp.IsDoneSuccessfully() );

  END_TEST;
}

int UtcDaliApplicationViewModeSetAndGet(void)
{
  Application application = Application::New(&gArgc, &gArgv);

  MyTestApp testApp( application, VIEW_MODE );

  application.MainLoop();

  DALI_TEST_CHECK( testApp.IsDoneSuccessfully() );

  END_TEST;
}

int UtcDaliApplicationStereoBaseSetAndGet(void)
{
  Application application = Application::New(&gArgc, &gArgv);

  MyTestApp testApp( application, STEREO_BASE );

  application.MainLoop();

  DALI_TEST_CHECK( testApp.IsDoneSuccessfully() );

  END_TEST;
}
