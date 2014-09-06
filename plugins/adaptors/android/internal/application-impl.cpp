/*
Copyright (c) 2000-2012 Samsung Electronics Co., Ltd All Rights Reserved

This file is part of Dali Adaptor

PROPRIETARY/CONFIDENTIAL

This software is the confidential and proprietary information of
SAMSUNG ELECTRONICS ("Confidential Information"). You shall not
disclose such Confidential Information and shall use it only in
accordance with the terms of the license agreement you entered
into with SAMSUNG ELECTRONICS.

SAMSUNG make no representations or warranties about the suitability
of the software, either express or implied, including but not limited
to the implied warranties of merchantability, fitness for a particular
purpose, or non-infringement. SAMSUNG shall not be liable for any
damages suffered by licensee as a result of using, modifying or
distributing this software or its derivatives.
*/

// CLASS HEADER
#include "application-impl.h"

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/adaptor-framework/common/orientation.h>

// INTERNAL INCLUDES
#include <internal/common/command-line-options.h>
#include <internal/common/adaptor-impl.h>
#include <internal/common/android-render-surface.h>
#include <internal/common/orientation-impl.h>
#include <android-interface.h>

extern bool keepRunningMainLoop;
extern void quit_looper();

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

boost::thread_specific_ptr<Application> gThreadLocalApplication;
}

ApplicationPtr Application::New(
  int* argc,
  char **argv[],
  const std::string& name,
  const DeviceLayout& baseLayout )
{
  ApplicationPtr application ( new Application (argc, argv, name, baseLayout ) );
  return application;
}

Application::Application(
  int* argc,
  char** argv[],
  const std::string& name,
  const DeviceLayout& baseLayout )
: mFramework(NULL),
  mAdaptor(NULL),
  mOrientation(NULL),
  mWindow(),
  mName(name),
  mInitialized(false),
  mBaseLayout(baseLayout),
  mAdaptorStarted(false),
  mSlotDelegate(this)
{
  // make sure we don't create the local thread application instance twice
  DALI_ASSERT_ALWAYS(gThreadLocalApplication.get() == NULL && "Cannot create more than one Application per thread" );

  // reset is used to store a new value associated with this thread
  gThreadLocalApplication.reset(this);

  mFramework = new Framework(*this, argc, argv, name);
}

Application::~Application()
{
  delete mFramework;
  delete mAdaptor;
  mWindow.Reset();
  gThreadLocalApplication.release();
}

void Application::CreateWindow()
{
#ifndef __arm__
   PositionSize windowPosition(0, 0, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
#else
   PositionSize windowPosition(0, 0, 0, 0);  // this will use full screen
#endif

  mWindow = Window::New( windowPosition, mName );
}

void Application::CreateAdaptor()
{
  DALI_ASSERT_ALWAYS( mWindow != NULL && "Window required to create adaptor" );
  Dali::Window window( mWindow.Get() );

  mAdaptor = &Dali::Adaptor::New( window, mBaseLayout);

  // Allow DPI to be overridden from command line.
  Adaptor& adaptorImpl = Internal::Adaptor::Adaptor::GetImplementation(*mAdaptor);
  unsigned int hDPI=DEFAULT_HORIZONTAL_DPI;
  unsigned int vDPI=DEFAULT_VERTICAL_DPI;
  adaptorImpl.SetDpi(hDPI, vDPI);

  mAdaptor->ResizedSignal().Connect(mSlotDelegate, &Application::OnResize);
}

void Application::MainLoop()
{
  // Run the application
  mFramework->Run();
}

void Application::Quit()
{
  // AddIdle(boost::bind(&Application::QuitFromMainLoop, this));

  keepRunningMainLoop = false;
}

void Application::QuitFromMainLoop()
{
  mAdaptor->Stop();
  mAdaptorStarted = false;

  Dali::Application application(this);
  mTerminateSignalV2.Emit( application );

  mFramework->Quit();

  // delete the window as ecore_x will be destroyed by AppCore
  mWindow.Reset();
  mInitialized = false;
}

Dali::Orientation& Application::GetOrientation()
{
  return *mOrientation;
}

void Application::OnInit()
{
  mFramework->AddAbortCallback(boost::bind(&Application::QuitFromMainLoop, this));
}

void Application::OnTerminate()
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

void Application::OnSurfaceCreated()
{
  if (!mAdaptorStarted)
  {
    if (!mInitialized)
    {  
      CreateWindow();
      CreateAdaptor();

      mInitialized = true;

      Dali::Application application(this);
      mInitSignalV2.Emit( application );
    }
    
    mAdaptor->Start();
    mAdaptorStarted = true;
  }
  else
  {
    CreateWindow();
    mAdaptor->SurfaceCreated();
  }

  Dali::Application application(this);
  mSurfaceCreatedSignalV2.Emit(application);
}

void Application::OnSurfaceDestroyed()
{
  mAdaptor->Pause();
  RenderSurface* surface = mWindow->GetSurface();
  surface->SurfaceLost();
  mAdaptor->SurfaceLost();

  Dali::Application application(this);
  mSurfaceDestroyedSignalV2.Emit(application);
}

void Application::OnReset()
{
  /*
   * usually, reset callback was called when a caller request to launch this application via aul.
   * because Application class already handled initialization in OnInit(), OnReset do nothing.
   */
  Dali::Application application(this);
  mResetSignalV2.Emit( application );

  Dali::Window window( mWindow.Get() );
  window.Raise();
}

void Application::OnLanguageChanged()
{
  //mAdaptor->NotifyLanguageChanged();
}

void Application::OnNotificationRequested()
{
  mAdaptor->RequestNotification();
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

Dali::Internal::Adaptor::WindowPtr Application::GetWindow()
{
  return mWindow;
}

Dali::Application Application::Get()
{
  DALI_ASSERT_ALWAYS( gThreadLocalApplication.get() != NULL && "Application not instantiated" );

  Dali::Application application(gThreadLocalApplication.get());

  return application;
}


} // namespace Adaptor

} // namespace Internal

} // namespace Dali
