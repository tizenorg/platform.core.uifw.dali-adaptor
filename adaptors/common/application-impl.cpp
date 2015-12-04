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

// INTERNAL INCLUDES
#include <style-monitor.h>
#include <command-line-options.h>
#include <common/adaptor-impl.h>
#include <singleton-service-impl.h>

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

ApplicationPtr Application::New(
  Integration::Framework* framework,
  int* argc,
  char **argv[],
  const std::string& stylesheet,
  Dali::Application::WINDOW_MODE windowMode)
{
  ApplicationPtr application ( new Application ( framework, argc, argv, stylesheet, windowMode ) );
  return application;
}

Application::Application( Integration::Framework* framework, int* argc, char** argv[], const std::string& stylesheet, Dali::Application::WINDOW_MODE windowMode )
: mInitSignal(),
  mTerminateSignal(),
  mPauseSignal(),
  mResumeSignal(),
  mResetSignal(),
  mResizeSignal(),
  mLanguageChangedSignal(),
  mOrientation( NULL ),
  mEventLoop( NULL ),
  mFramework( NULL ),
  mContextLossConfiguration(Dali::Configuration::APPLICATION_DOES_NOT_HANDLE_CONTEXT_LOSS),
  mCommandLineOptions( NULL ),
  mSingletonService( SingletonService::New() ),
  mAdaptor( NULL ),
  mWindow(),
  mSurface( NULL ),
  mWindowMode( windowMode ),
  mName(),
  mStylesheet( stylesheet ),
  mEnvironmentOptions(),
  mAdaptorStarted(false),
  mSlotDelegate( this )
{
  mCommandLineOptions = new CommandLineOptions(argc, argv);

  if( framework != NULL )
  {
    mFramework = framework;
  }
  else
  {
    mFramework = new Integration::Framework();
  }

  mFramework->Initialize( *this, argc, argv );
}

Application::~Application()
{
  mSingletonService.UnregisterAll();

  delete mFramework;
  delete mCommandLineOptions;
  delete mAdaptor;
  mWindow.Reset();
}

void Application::CreateWindow()
{
  PositionSize windowPosition(0, 0, 0, 0);  // this will use full screen

  if( mCommandLineOptions->stageWidth > 0 && mCommandLineOptions->stageHeight > 0 )
  {
    // let the command line options over ride
    windowPosition = PositionSize(0,0,mCommandLineOptions->stageWidth,mCommandLineOptions->stageHeight);
  }

  Dali::Window window( Internal::Adaptor::Window::New( mFramework, windowPosition, mName, "", mWindowMode == Dali::Application::TRANSPARENT ) );

  mWindow = window;
}

void Application::CreateAdaptor()
{
  DALI_ASSERT_ALWAYS( mWindow && "Window required to create adaptor" );

  Internal::Adaptor::Window& windowImpl = GetImplementation( mWindow );

  mAdaptor = Dali::Internal::Adaptor::Adaptor::New( windowImpl.GetSurface(), mContextLossConfiguration, &mEnvironmentOptions, mFramework );

  mAdaptor->ResizedSignal().Connect( mSlotDelegate, &Application::OnResize );
}

void Application::MainLoop(Dali::Configuration::ContextLoss configuration)
{
  DALI_ASSERT_ALWAYS( 0 && "Cannot use Application::MainLoop with this adaptor");

  mContextLossConfiguration = configuration;

  // Run the application
  mFramework->Run();
}

void Application::Start(Dali::Configuration::ContextLoss configuration)
{
  mContextLossConfiguration = configuration;
  mFramework->Start();
}

void Application::Lower()
{
  // Lower the application without quitting it.
  mWindow.Lower();
}

void Application::Quit()
{
  // Actually quit the application.
  AddIdle( MakeCallback( this, &Application::QuitFromMainLoop ) );
}

void Application::QuitFromMainLoop()
{
  mAdaptor->Stop();
  mAdaptorStarted = false;

  Dali::Application application(this);
  mTerminateSignal.Emit( application );

  mFramework->Quit();
  // This will trigger OnTerminate(), below, after the main loop has completed.
}

Dali::Orientation& Application::GetOrientation()
{
  return *mOrientation;
}

void Application::OnInit()
{
  mFramework->AddAbortCallback( MakeCallback( this, &Application::QuitFromMainLoop ) );

  CreateWindow();
  CreateAdaptor();

  // Run the adaptor
  mAdaptor->Start();
  mAdaptorStarted = true;

  // Check if user requires no vsyncing and set on X11 Adaptor
  if (mCommandLineOptions->noVSyncOnRender)
  {
    mAdaptor->SetUseHardwareVSync(false);
  }

  Internal::Adaptor::Adaptor::GetImplementation( *mAdaptor ).SetStereoBase( mCommandLineOptions->stereoBase );
  if( mCommandLineOptions->viewMode != 0 )
  {
    ViewMode viewMode = MONO;
    if( mCommandLineOptions->viewMode <= STEREO_INTERLACED )
    {
      viewMode = static_cast<ViewMode>( mCommandLineOptions->viewMode );
    }
    Internal::Adaptor::Adaptor::GetImplementation( *mAdaptor ).SetViewMode( viewMode );
  }

  if( ! mStylesheet.empty() )
  {
    Dali::StyleMonitor::Get().SetTheme( mStylesheet );
  }

  // in default, auto hide indicator mode
  mWindow.ShowIndicator(Dali::Window::AUTO);

  Dali::Application application(this);
  mInitSignal.Emit( application );

  mAdaptor->NotifySceneCreated();
}

void Application::OnTerminate()
{
}

void Application::OnPause()
{
  mAdaptor->Pause();
  Dali::Application application(this);
  mPauseSignal.Emit( application );
}

void Application::OnResume()
{
  // Emit the signal first so the application can queue any messages before we do an update/render
  // This ensures we do not just redraw the last frame before pausing if that's not required
  Dali::Application application(this);
  mResumeSignal.Emit( application );
  mAdaptor->Resume();
}

void Application::OnSurfaceCreated()
{
  if (!mAdaptorStarted)

  {
    mAdaptor->Start();
    mAdaptorStarted = true;
  }

  mAdaptor->SurfaceCreated();

  void *surface = mFramework->GetWindow();

  if (mSurface != NULL && mSurface != surface)
  {
    PositionSize sz(0,0,0,0);
    ReplaceWindow(sz, mName);
  }

  Dali::Application application(this);
  mSurfaceCreatedSignal.Emit(application);
}

void Application::OnSurfaceDestroyed()
{
  mAdaptor->Pause();
  Window& windowImpl = GetImplementation(mWindow);
  RenderSurface* surface = windowImpl.GetSurface();
  surface->SurfaceLost();
  mAdaptor->SurfaceLost();

  Dali::Application application(this);
  mSurfaceDestroyedSignal.Emit(application);
}

void Application::OnReset()
{
  Dali::Application application(this);
  mResetSignal.Emit( application );

  mWindow.Raise();
}

void Application::OnLanguageChanged()
{
  //mAdaptor->NotifyLanguageChanged();
}

void Application::OnNotificationRequested()
{
  //mAdaptor->RequestNotification();
}

void Application::OnResize(Dali::Adaptor& adaptor)
{
  Dali::Application application(this);
  mResizeSignal.Emit( application );
}

bool Application::AddIdle( CallbackBase* callback )
{
  return mAdaptor->AddIdle( callback );
}

Dali::Adaptor& Application::GetAdaptor()
{
  return *mAdaptor;
}

Dali::Window Application::GetWindow()
{
  return mWindow;
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

void Application::ReplaceWindow(PositionSize windowPosition, const std::string& name)
{
  Dali::Window newWindow( Internal::Adaptor::Window::New( mFramework, windowPosition, name, "", mWindowMode == Dali::Application::TRANSPARENT ) );

  Window& windowImpl = GetImplementation(newWindow);
  windowImpl.SetAdaptor(*mAdaptor);
  newWindow.ShowIndicator(Dali::Window::INVISIBLE);
  Dali::RenderSurface* renderSurface = windowImpl.GetSurface();
  Internal::Adaptor::Adaptor::GetImplementation( *mAdaptor ).ReplaceSurface(*renderSurface);
  mWindow = newWindow;

  mSurface = mFramework->GetWindow();
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
