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

using namespace Dali;

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
  MyTestApp()
  : initCalled( false )
  {
  }

  void InitMethod( Application& app )
  {
    initCalled = true;
    app.Quit();
  }

  // Data
  bool initCalled;
};

} // unnamed namespace
int UtcDaliApplicationInitCallback(void)
{
  MyTestApp testApp;
  Application application = Application::New();
  application.InitSignal().Connect( &testApp, &MyTestApp::InitMethod );
  application.MainLoop();

  DALI_TEST_CHECK( testApp.initCalled );

  END_TEST;
}
