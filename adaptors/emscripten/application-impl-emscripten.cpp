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

// CLASS HEADER
#include "application-impl.h"

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <style-monitor.h>

// INTERNAL INCLUDES
#include <command-line-options.h>
#include <common/adaptor-impl.h>

namespace Dali
{

namespace SlpPlatform
{
class SlpPlatformAbstraction;
}

namespace Integration
{
class Core;
}

namespace Internal
{

namespace Adaptor
{

namespace
{
// Defaults taken from H2 device
const unsigned int DEFAULT_WINDOW_WIDTH   = 480;
const unsigned int DEFAULT_WINDOW_HEIGHT  = 800;
const float        DEFAULT_HORIZONTAL_DPI = 220;
const float        DEFAULT_VERTICAL_DPI   = 217;

}

ApplicationPtr Application::New(
  int* argc,
  char **argv[],
  const std::string& name,
  const DeviceLayout& baseLayout,
  Dali::Application::WINDOW_MODE windowMode)
{
  ApplicationPtr application ( new Application (argc, argv, name, baseLayout, windowMode ) );
  return application;
}

Application::Application(
  int* argc,
  char** argv[],
  const std::string& name,
  const DeviceLayout& baseLayout,
  Dali::Application::WINDOW_MODE windowMode)
: mFramework(NULL),
  mCommandLineOptions(NULL),
  mAdaptor(NULL),
  mWindow(),
  mWindowMode( windowMode ),
  mName(name),
  mInitialized(false),
  mBaseLayout(baseLayout),
  mSlotDelegate( this )
{
  mCommandLineOptions = new CommandLineOptions(argc, argv);

  mFramework = new Framework(*this, argc, argv, name);
}

Application::~Application()
{
  delete mFramework;
  delete mCommandLineOptions;
  delete mAdaptor;
  mWindow.Reset();
}

void Application::CreateWindow()
{
  // window position checked at run time
  PositionSize windowPosition(0, 0, 0, 0);

  mWindow = Dali::Window::New( windowPosition, mName, mWindowMode == Dali::Application::TRANSPARENT );
}

void Application::CreateAdaptor()
{
  DALI_ASSERT_ALWAYS( mWindow && "Window required to create adaptor" );

  mAdaptor = &Dali::Adaptor::New( mWindow, mBaseLayout );

  // Allow DPI to be overridden from command line.
  unsigned int hDPI=DEFAULT_HORIZONTAL_DPI;
  unsigned int vDPI=DEFAULT_VERTICAL_DPI;

  Internal::Adaptor::Adaptor::GetImplementation( *mAdaptor ).SetDpi(hDPI, vDPI);

  mAdaptor->ResizedSignal().Connect( mSlotDelegate, &Application::OnResize );
}

void Application::MainLoop()
{
  //
  // We actually don't loop here for emscripten.
  // MainLoop() is called from a browser Javascript idle type function
  // and needs to return to single threaded Javascript so as not to block
  //

  // Run the application
  mFramework->Run();

  //
  // Update Once
  //
  unsigned int intervalMilliseconds = 16; // DEFAULT_RENDER_INTERVAL

  // pump events
  // Integration::NotificationEvent event;
  // // mCore->SendEvent(event);
  // mCore->QueueEvent(event);
  mCore->ProcessEvents();

  // Update Time values
  mMicroSeconds += intervalMilliseconds * 1000u;
  unsigned int additionalSeconds = mMicroSeconds / 1000000u;

  mSeconds += additionalSeconds;
  mMicroSeconds -= additionalSeconds * 1000000u;

  mPlatformAbstraction.IncrementGetTimeResult( intervalMilliseconds ) ; // size_t milliseconds)

  mCore->VSync( mFrame, mSeconds, mMicroSeconds );

  mCore->Update( mStatus );

  //
  // Render Once
  //
  mCore->Render( mRenderStatus );

  mFrame++;

  mEglImplementation.SwapBuffers();


}

void Application::Lower()
{
  // Lower the application without quitting it.
  mWindow.Lower();
}

void Application::Quit()
{
  // Actually quit the application.
  AddIdle(boost::bind(&Application::QuitFromMainLoop, this));
}

void Application::QuitFromMainLoop()
{
  if( mAdaptor )
  {
    mAdaptor->Stop();

    Dali::Application application(this);
    mTerminateSignalV2.Emit( application );

  }

  if(mFramework)
  {
    mFramework->Quit();
  }

  // This will trigger OnTerminate(), below, after the main loop has completed.
  mInitialized = false;
}

void Application::OnInit()
{
  CreateWindow();
  CreateAdaptor();

  // Run the adaptor
  mAdaptor->Start();

  mInitialized = true;

  // // in default, auto hide indicator mode
  // mWindow.ShowIndicator(Dali::Window::AUTO);

  Dali::Application application(this);
  mInitSignalV2.Emit( application );
}

void Application::OnTerminate()
{
  mWindow.Reset();
  mInitialized = false;
}

void Application::OnAbort()
{
  QuitFromMainLoop();
}

void Application::OnPause()
{
  mAdaptor->Pause();
  Dali::Application application(this);
  mPauseSignalV2.Emit( application );
}

void Application::OnResume()
{
  mAdaptor->Resume();
  Dali::Application application(this);
  mResumeSignalV2.Emit( application );
}

void Application::OnReset()
{
  /*
   * usually, reset callback was called when a caller request to launch this application via aul.
   * because Application class already handled initialization in OnInit(), OnReset do nothing.
   */
  Dali::Application application(this);
  mResetSignalV2.Emit( application );

  mWindow.Raise();
}

void Application::OnLanguageChanged()
{
  mAdaptor->NotifyLanguageChanged();
}

void Application::OnResize(Dali::Adaptor& adaptor)
{
  Dali::Application application(this);
  mResizeSignalV2.Emit( application );
}

bool Application::AddIdle(boost::function<void(void)> callBack)
{
  return mAdaptor->AddIdle(callBack);
}

Dali::Adaptor& Application::GetAdaptor()
{
  return *mAdaptor;
}

Dali::Window Application::GetWindow()
{
  return mWindow;
}

Dali::Application Application::Get()
{
  Dali::Application application(this);

  return application;
}

const std::string& Application::GetTheme()
{
  return Dali::StyleMonitor::Get().GetTheme();
}

void Application::SetTheme(const std::string& themeFilePath)
{
  return Dali::StyleMonitor::Get().SetTheme(themeFilePath);
}

// Stereoscopy

void Application::SetViewMode( ViewMode viewMode )
{
  Internal::Adaptor::Adaptor::GetImplementation( *mAdaptor ).SetViewMode( viewMode );
}

ViewMode Application::GetViewMode() const
{
  return Internal::Adaptor::Adaptor::GetImplementation( *mAdaptor ).GetViewMode();
}

void Application::SetStereoBase( float stereoBase )
{
  Internal::Adaptor::Adaptor::GetImplementation( *mAdaptor ).SetStereoBase( stereoBase );
}

float Application::GetStereoBase() const
{
  return Internal::Adaptor::Adaptor::GetImplementation( *mAdaptor ).GetStereoBase();
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
