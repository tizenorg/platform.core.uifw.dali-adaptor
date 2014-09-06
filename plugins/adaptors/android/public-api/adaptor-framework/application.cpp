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
#include "application.h"

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <internal/application-impl.h>

namespace Dali
{

Application Application::New( int* argc, char **argv[] )
{
  Internal::Adaptor::ApplicationPtr internal = Internal::Adaptor::Application::New( argc, argv, "Dali Application", DeviceLayout::DEFAULT_BASE_LAYOUT );
  return Application(internal.Get());
}

Application Application::New()
{
  Internal::Adaptor::ApplicationPtr internal = Internal::Adaptor::Application::New( NULL, NULL, "Dali Application", DeviceLayout::DEFAULT_BASE_LAYOUT );
  return Application(internal.Get());
}

Application Application::New( int* argc, char **argv[], const std::string& name )
{
  Internal::Adaptor::ApplicationPtr internal = Internal::Adaptor::Application::New( argc, argv, name, DeviceLayout::DEFAULT_BASE_LAYOUT );
  return Application(internal.Get());
}

Application Application::New(int* argc, char **argv[], const DeviceLayout& baseLayout)
{
  Internal::Adaptor::ApplicationPtr internal = Internal::Adaptor::Application::New( argc, argv, "Dali Application", baseLayout );
  return Application(internal.Get());
}

Application Application::New(int* argc, char **argv[], const std::string& name, const DeviceLayout& baseLayout)
{
  Internal::Adaptor::ApplicationPtr internal = Internal::Adaptor::Application::New( argc, argv, name, baseLayout );
  return Application(internal.Get());
}

Application::~Application()
{
}

Application::Application(const Application& application)
: BaseHandle(application)
{
}

Application& Application::operator=(const Application& application)
{
  if( *this != application )
  {
    BaseHandle::operator=( application );
  }
  return *this;
}

void Application::MainLoop()
{
  Internal::Adaptor::GetImplementation(*this).MainLoop();
}

void Application::Quit()
{
  Internal::Adaptor::GetImplementation(*this).Quit();
}

Orientation& Application::GetOrientation()
{
  return Internal::Adaptor::GetImplementation(*this).GetOrientation();
}
bool Application::AddIdle(boost::function<void(void)> callBack)
{
  return Internal::Adaptor::GetImplementation(*this).AddIdle(callBack);
}

Window Application::GetWindow()
{
  Internal::Adaptor::WindowPtr window = Internal::Adaptor::GetImplementation(*this).GetWindow();
  return Window(window.Get());
}

Application Application::Get()
{
  return Internal::Adaptor::Application::Get();
}

Adaptor& Application::GetAdaptor()
{
  Adaptor &adaptor = Internal::Adaptor::GetImplementation(*this).GetAdaptor();
  return adaptor;
}

//Application::AppSignal Application::SignalInit()
//{
//  return Internal::Adaptor::GetImplementation(*this).SignalInit();
//}

Application::AppSignalV2& Application::InitSignal()
{
  return Internal::Adaptor::GetImplementation(*this).InitSignal();
}

//Application::AppSignal Application::SignalTerminate()
//{
//  return Internal::Adaptor::GetImplementation(*this).SignalTerminate();
//}

Application::AppSignalV2& Application::TerminateSignal()
{
  return Internal::Adaptor::GetImplementation(*this).TerminateSignal();
}

//Application::AppSignal Application::SignalPause()
//{
//  return Internal::Adaptor::GetImplementation(*this).SignalPause();
//}

Application::AppSignalV2& Application::PauseSignal()
{
  return Internal::Adaptor::GetImplementation(*this).PauseSignal();
}

//Application::AppSignal Application::SignalResume()
//{
//  return Internal::Adaptor::GetImplementation(*this).SignalResume();
//}

Application::AppSignalV2& Application::ResumeSignal()
{
  return Internal::Adaptor::GetImplementation(*this).ResumeSignal();
}

//Application::AppSignal Application::SignalReset()
//{
//  return Internal::Adaptor::GetImplementation(*this).SignalReset();
//}

Application::AppSignalV2& Application::ResetSignal()
{
  return Internal::Adaptor::GetImplementation(*this).ResetSignal();
}

//Application::AppSignal Application::SignalResize()
//{
//  return Internal::Adaptor::GetImplementation(*this).SignalResize();
//}

Application::AppSignalV2& Application::ResizeSignal()
{
  return Internal::Adaptor::GetImplementation(*this).ResizeSignal();
}

//Application::AppSignal Application::SignalLanguageChanged()
//{
//  return Internal::Adaptor::GetImplementation(*this).SignalLanguageChanged();
//}

Application::AppSignalV2& Application::LanguageChangedSignal()
{
  return Internal::Adaptor::GetImplementation(*this).LanguageChangedSignal();
}

//Application::AppSignal Application::SignalSurfaceCreated()
//{
//  return Internal::Adaptor::GetImplementation(*this).SignalSurfaceCreated();
//}

Application::AppSignalV2& Application::SurfaceCreatedSignal()
{
  return Internal::Adaptor::GetImplementation(*this).SurfaceCreatedSignal();
}

Application::AppSignalV2& Application::SurfaceDestroyedSignal()
{
  return Internal::Adaptor::GetImplementation(*this).SurfaceDestroyedSignal();
}

Application::Application(Internal::Adaptor::Application* application)
: BaseHandle(application)
{
}


} // namespace Dali
