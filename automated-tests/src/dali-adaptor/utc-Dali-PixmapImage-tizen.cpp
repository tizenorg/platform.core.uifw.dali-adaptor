/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

#include <Ecore_X.h>

extern int gArgc;
extern char ** gArgv;

using namespace Dali;

void utc_dali_pixmap_image_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_pixmap_image_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{

enum TEST_TYPE
{
  NEW,
  NEW_ANY,
  GET_PIXMAP,
  GET_PIXELS,
  ENCODE_TO_FILE
};

struct MyTestApp : public ConnectionTracker
{
  MyTestApp( Application& app, int type )
  : mApplication( app ),
    mTestType( type )
  {
    mApplication.InitSignal().Connect( this, &MyTestApp::OnInit );
  }

  void OnInit(Application& app)
  {
    mTimer = Timer::New( 500 );
    mTimer.TickSignal().Connect( this, &MyTestApp::Tick );
    mTimer.Start();
  }

  bool Tick()
  {
    ExcuteTest();

    mTimer.Stop();
    mApplication.Quit();
    return true;
  }

  void ExcuteTest()
  {
    PixmapImagePtr pixmapImage;
    switch (mTestType)
    {
      case NEW:
        pixmapImage = CreatePixmap();
        break;
      case NEW_ANY:
        TestNewAny();
        break;
      case GET_PIXMAP:
        TestGetPixmap();
        break;
      case GET_PIXELS:
        TestGetPixels();
        break;
      case ENCODE_TO_FILE:
        TestEncodeToFile();
        break;
    }
  }

  PixmapImagePtr CreatePixmap()
  {
    unsigned int width = 256u;
    unsigned int height = 256u;

    PixmapImagePtr pixmapImage = PixmapImage::New( width, height, PixmapImage::COLOR_DEPTH_DEFAULT );
    DALI_TEST_CHECK( pixmapImage );

    return pixmapImage;
  }

  void TestNewAny()
  {
    unsigned int width = 256u;
    unsigned int height = 256u;
    Ecore_X_Window window = ecore_x_window_root_first_get();

    Ecore_X_Pixmap pixmap = ecore_x_pixmap_new( window, width, height, 24 );

    PixmapImagePtr pixmapImage = PixmapImage::New( pixmap );
    DALI_TEST_CHECK( pixmapImage );
  }

  void TestGetPixmap()
  {
    PixmapImagePtr pixmapImage = CreatePixmap();
    Any pixmap = pixmapImage->GetPixmap();
    DALI_TEST_CHECK( !pixmap.Empty() );
  }

  void TestGetPixels()
  {
    PixmapImagePtr pixmapImage = CreatePixmap();
    std::vector<unsigned char> pixbuf;
    unsigned int width, height;
    Pixel::Format pixelFormat;

    pixmapImage->GetPixels( pixbuf, width, height, pixelFormat );

    DALI_TEST_CHECK( width == 256 && height == 256 );
  }

  void TestEncodeToFile()
  {
    PixmapImagePtr pixmapImage = CreatePixmap();
    bool ret = pixmapImage->EncodeToFile("PixmapImage.png");

    DALI_TEST_CHECK( !ret );
  }

  // Data
  Application& mApplication;
  int mTestType;
  Timer mTimer;
};

} // unnamed namespace

int UtcDaliPixmapImageNew(void)
{
  Application application = Application::New( &gArgc, &gArgv );

  MyTestApp testApp( application, NEW );

  application.MainLoop();

  END_TEST;
}

int UtcDaliPixmapImageNewAny(void)
{
  Application application = Application::New( &gArgc, &gArgv );

  MyTestApp testApp( application, NEW_ANY );

  application.MainLoop();

  END_TEST;
}

int UtcDaliPixmapImageGetPixmap(void)
{
  Application application = Application::New( &gArgc, &gArgv );

  MyTestApp testApp( application, GET_PIXMAP );

  application.MainLoop();

  END_TEST;
}

int UtcDaliPixmapImageGetPixels(void)
{
  Application application = Application::New( &gArgc, &gArgv );

  MyTestApp testApp( application, GET_PIXELS );

  application.MainLoop();

  END_TEST;
}

int UtcDaliPixmapImageEncodeToFile(void)
{
  Application application = Application::New( &gArgc, &gArgv );

  MyTestApp testApp( application, ENCODE_TO_FILE );

  application.MainLoop();

  END_TEST;
}
