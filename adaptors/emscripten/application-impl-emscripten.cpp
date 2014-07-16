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
#include <boost/bind.hpp>

// INTERNAL INCLUDES
#include <style-monitor.h>
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

struct Application::Impl
{
  AppSignalV2                           mInitSignalV2;
  AppSignalV2                           mTerminateSignalV2;
  AppSignalV2                           mPauseSignalV2;
  AppSignalV2                           mResumeSignalV2;
  AppSignalV2                           mResetSignalV2;
  AppSignalV2                           mResizeSignalV2;
  AppSignalV2                           mLanguageChangedSignalV2;

  EventLoop*                            mEventLoop;
  Framework*                            mFramework;

  CommandLineOptions*                   mCommandLineOptions;

  Dali::Adaptor*                        mAdaptor;
  Dali::Window                          mWindow;
  Dali::Application::WINDOW_MODE        mWindowMode;
  std::string                           mName;

  bool                                  mInitialized;
  DeviceLayout                          mBaseLayout;

  SlotDelegate< Application::Impl >     mSlotDelegate;

  Impl( int* argc,
        char** argv[],
        const std::string& name,
        const DeviceLayout& baseLayout,
        Dali::Application::WINDOW_MODE windowMode,
        Application* app);
  ~Impl();
  void CreateWindow();
  void CreateAdaptor();
  void MainLoop();
  void Lower();
  void Quit(Application* app);
  void QuitFromMainLoop(Application* app);
  void OnInit(Application* app);
  void OnTerminate();
  void OnAbort(Application* app);
  void OnPause(Application* app);
  void OnResume(Application* app);
  void OnReset(Application* app);
  void OnLanguageChanged();
  void OnResize(Dali::Adaptor& adaptor, Application* app);
  bool AddIdle(boost::function<void(void)> callBack);
  Dali::Adaptor& GetAdaptor();
  Dali::Window GetWindow();
  Dali::Application Get();
  const std::string& GetTheme();
  void SetTheme(const std::string& themeFilePath);
  void SetViewMode( ViewMode viewMode );
  ViewMode GetViewMode() const;
  void SetStereoBase( float stereoBase );
  float GetStereoBase() const;
  Dali::Application::AppSignalV2& InitSignal();
  Dali::Application::AppSignalV2& TerminateSignal();
  Dali::Application::AppSignalV2& PauseSignal();
  Dali::Application::AppSignalV2& ResumeSignal();
  Dali::Application::AppSignalV2& ResetSignal();
  Dali::Application::AppSignalV2& ResizeSignal();
  Dali::Application::AppSignalV2& LanguageChangedSignal();

};


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

Application::Impl::Impl(int* argc,
                        char** argv[],
                        const std::string& name,
                        const DeviceLayout& baseLayout,
                        Dali::Application::WINDOW_MODE windowMode,
                        Application* app)
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

  mFramework = new Framework(*app, argc, argv, name);
}

Application::Impl::~Impl()
{
  delete mFramework;
  delete mCommandLineOptions;
  delete mAdaptor;
  mWindow.Reset();
}

void Application::Impl::CreateWindow()
{
  // window position checked at run time
  PositionSize windowPosition(0, 0, 0, 0);

  mWindow = Dali::Window::New( windowPosition, mName, mWindowMode == Dali::Application::TRANSPARENT );
}

void Application::Impl::CreateAdaptor()
{
  DALI_ASSERT_ALWAYS( mWindow && "Window required to create adaptor" );

  mAdaptor = &Dali::Adaptor::New( mWindow, mBaseLayout );

}

void Application::Impl::MainLoop()
{
  //
  // We actually don't loop here for emscripten.
  // MainLoop() is called from a browser Javascript idle type function
  // and needs to return to single threaded Javascript so as not to block
  //
  Dali::Internal::Adaptor::Adaptor& adaptor = Internal::Adaptor::Adaptor::GetImplementation(*mAdaptor);
  adaptor.RequestUpdateOnce();

}

void Application::Impl::Lower()
{
  // Lower the application without quitting it.
  mWindow.Lower();
}

void Application::Impl::Quit(Application* app)
{
  // Actually quit the application.
  AddIdle(boost::bind(&Application::Impl::QuitFromMainLoop, this, app));
}

void Application::Impl::QuitFromMainLoop(Application* app)
{
  if( mAdaptor )
  {
    mAdaptor->Stop();

    Dali::Application application(app);
    mTerminateSignalV2.Emit( application );

  }

  if(mFramework)
  {
    mFramework->Quit();
  }

  // This will trigger OnTerminate(), below, after the main loop has completed.
  mInitialized = false;
}

void Application::Impl::OnInit(Application* app)
{
  CreateWindow();
  CreateAdaptor();

  // Run the adaptor
  mAdaptor->Start();

  mInitialized = true;

  // // in default, auto hide indicator mode
  // mWindow.ShowIndicator(Dali::Window::AUTO);

  Dali::Application application(app);
  mInitSignalV2.Emit( application );
}

void Application::Impl::OnTerminate()
{
  mWindow.Reset();
  mInitialized = false;
}

void Application::Impl::OnAbort(Application* app)
{
  QuitFromMainLoop(app);
}

void Application::Impl::OnPause(Application* app)
{
  mAdaptor->Pause();
  Dali::Application application(app);
  mPauseSignalV2.Emit( application );
}

void Application::Impl::OnResume(Application* app)
{
  mAdaptor->Resume();
  Dali::Application application(app);
  mResumeSignalV2.Emit( application );
}

void Application::Impl::OnReset(Application* app)
{
  /*
   * usually, reset callback was called when a caller request to launch this application via aul.
   * because Application class already handled initialization in OnInit(), OnReset do nothing.
   */
  Dali::Application application(app);
  mResetSignalV2.Emit( application );

  mWindow.Raise();
}

void Application::Impl::OnLanguageChanged()
{
  mAdaptor->NotifyLanguageChanged();
}

void Application::Impl::OnResize(Dali::Adaptor& adaptor, Application* app)
{
  Dali::Application application(app);
  mResizeSignalV2.Emit( application );
}

bool Application::Impl::AddIdle(boost::function<void(void)> callBack)
{
  return mAdaptor->AddIdle(callBack);
}

Dali::Adaptor& Application::Impl::GetAdaptor()
{
  return *mAdaptor;
}

Dali::Window Application::Impl::GetWindow()
{
  return mWindow;
}

Dali::Application Application::Impl::Get()
{
  static int argc = 0;
  static Dali::Application application = Dali::Application::New(&argc, NULL);

  return application;
}

const std::string& Application::Impl::GetTheme()
{
  return Dali::StyleMonitor::Get().GetTheme();
}

void Application::Impl::SetTheme(const std::string& themeFilePath)
{
  return Dali::StyleMonitor::Get().SetTheme(themeFilePath);
}

// Stereoscopy

void Application::Impl::SetViewMode( ViewMode viewMode )
{
  Internal::Adaptor::Adaptor::GetImplementation( *mAdaptor ).SetViewMode( viewMode );
}

ViewMode Application::Impl::GetViewMode() const
{
  return Internal::Adaptor::Adaptor::GetImplementation( *mAdaptor ).GetViewMode();
}

void Application::Impl::SetStereoBase( float stereoBase )
{
  Internal::Adaptor::Adaptor::GetImplementation( *mAdaptor ).SetStereoBase( stereoBase );
}

float Application::Impl::GetStereoBase() const
{
  return Internal::Adaptor::Adaptor::GetImplementation( *mAdaptor ).GetStereoBase();
}


//////////
Application::Application(
  int* argc,
  char** argv[],
  const std::string& name,
  const DeviceLayout& baseLayout,
  Dali::Application::WINDOW_MODE windowMode)
{
  mImpl = new Impl(argc, argv, name, baseLayout, windowMode, this);
}

Application::~Application()
{
  delete mImpl;
}

void Application::CreateWindow()
{
  mImpl->CreateWindow();
}

void Application::CreateAdaptor()
{
  mImpl->CreateAdaptor();
}

void Application::MainLoop()
{
  mImpl->MainLoop();
}

void Application::Lower()
{
  mImpl->Lower();
}

void Application::Quit()
{
  mImpl->Quit(this);
}

void Application::QuitFromMainLoop()
{
  mImpl->QuitFromMainLoop(this);
}

void Application::OnInit()
{
  mImpl->OnInit(this);
}

void Application::OnTerminate()
{
  mImpl->OnTerminate();
}

void Application::OnAbort()
{
  mImpl->OnAbort(this);
}

void Application::OnPause()
{
  mImpl->OnPause(this);
}

void Application::OnResume()
{
  mImpl->OnResume(this);
}

void Application::OnReset()
{
  mImpl->OnReset(this);
}

void Application::OnLanguageChanged()
{
  mImpl->OnLanguageChanged();
}

void Application::OnResize(Dali::Adaptor& adaptor)
{
  mImpl->OnResize(adaptor, this);
}

bool Application::AddIdle(boost::function<void(void)> callBack)
{
  return mImpl->AddIdle(callBack);
}

Dali::Adaptor& Application::GetAdaptor()
{
  return mImpl->GetAdaptor();
}

Dali::Window Application::GetWindow()
{
  return mImpl->GetWindow();
}

// Dali::Application Application::Get()
// {
//   return mImpl->Get();
// }

const std::string& Application::GetTheme()
{
  return mImpl->GetTheme();
}

void Application::SetTheme(const std::string& themeFilePath)
{
  return mImpl->SetTheme(themeFilePath);
}

void Application::SetViewMode( ViewMode viewMode )
{
  mImpl->SetViewMode(viewMode);
}

ViewMode Application::GetViewMode() const
{
  return mImpl->GetViewMode();
}

void Application::SetStereoBase( float stereoBase )
{
  mImpl->SetStereoBase(stereoBase);
}

float Application::GetStereoBase() const
{
  return mImpl->GetStereoBase();
}

Dali::Application::AppSignalV2& Application::InitSignal()
{
  return mImpl->mInitSignalV2;
}

Dali::Application::AppSignalV2& Application::TerminateSignal()
{
  return mImpl->mTerminateSignalV2;
}

Dali::Application::AppSignalV2& Application::PauseSignal()
{
  return mImpl->mPauseSignalV2;
}

Dali::Application::AppSignalV2& Application::ResumeSignal()
{
  return mImpl->mResumeSignalV2;
}

Dali::Application::AppSignalV2& Application::ResetSignal()
{
  return mImpl->mResetSignalV2;
}

Dali::Application::AppSignalV2& Application::ResizeSignal()
{
  return mImpl->mResizeSignalV2;
}

Dali::Application::AppSignalV2& Application::LanguageChangedSignal()
{
  return mImpl->mLanguageChangedSignalV2;
}


} // namespace Adaptor

} // namespace Internal

} // namespace Dali
