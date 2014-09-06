#ifndef __DALI_INTERNAL_APPLICATION_H__
#define __DALI_INTERNAL_APPLICATION_H__

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

// EXTERNAL INCLUDES
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/thread.hpp>

#include <dali/public-api/math/rect.h>
#include <dali/public-api/object/base-object.h>

// INTERNAL INCLUDES
#include <public-api/adaptor-framework/application.h>

#include <internal/framework.h>
#include <internal/common/window-impl.h>

namespace Dali
{

class Adaptor;

namespace Internal
{

namespace Adaptor
{
class Window;
class CommandLineOptions;
class EventLoop;

typedef Dali::Rect<int> PositionSize;

class Application;
typedef Dali::IntrusivePtr<Application> ApplicationPtr;

/**
 * Implementation of the Application class.
 */
class Application : public BaseObject, public Framework::Observer
{
public:

  typedef Dali::Application::AppSignalV2 AppSignalV2;

  /**
   * Constructor
   * @param[in]  app         The public instance of the Application
   * @param[in]  argc        A pointer to the number of arguments
   * @param[in]  argv        A pointer to the argument list
   * @param[in]  name        A name of application
   * @param[in]  baseLayout  The base layout that the application has been written for
   */
  static ApplicationPtr New(int* argc, char **argv[], const std::string& name, const DeviceLayout& baseLayout);

  Application(int* argc, char **argv[], const std::string& name, const DeviceLayout& baseLayout);

  /**
   * Destructor
   */
  virtual ~Application();

public:

  /**
   * @copydoc Dali::Application::MainLoop()
   */
  void MainLoop();

  /**
   * @copydoc Dali::Application::Lower()
   */
  void Lower();

  /**
   * @copydoc Dali::Application::Quit()
   */
  void Quit();

  /**
   * @copydoc Dali::Application::GetOrientation()
   */
  Dali::Orientation& GetOrientation();

  /**
   * @copydoc Dali::Application::AddIdle()
   */
  bool AddIdle(boost::function<void(void)> callBack);

  /**
   * @copydoc Dali::Application::GetAdaptor();
   */
  Dali::Adaptor& GetAdaptor();

  /**
   * @copydoc Dali::Application::Ticker();
   */
  void Ticker();

  /**
   * @copydoc Dali::Application::GetWindow();
   */
  Dali::Internal::Adaptor::WindowPtr GetWindow();

  /**
   * @copydoc Dali::Application::Get();
   */
  static Dali::Application Get();


public: // From Framework::Observer

  /**
   * Called when the framework is initialised.
   */
  virtual void OnInit();

  /**
   * Called when the framework is terminated.
   */
  virtual void OnTerminate();

  /**
   * Called when the framework is paused.
   */
  virtual void OnPause();

  /**
   * Called when the framework resumes from a paused state.
   */
  virtual void OnResume();

  /**
   * Called when surface created.
   */
  virtual void OnSurfaceCreated();

  /**
   * Called when surface destroyed.
   */
  virtual void OnSurfaceDestroyed();

  /**
   * Called when the framework informs the application that it should reset itself.
   */
  virtual void OnReset();

  /**
   * Called when the framework informs the application that the language of the device has changed.
   */
  virtual void OnLanguageChanged();

  /**
   * Called when the framework requests an update.
   */
  virtual void OnNotificationRequested();

public:

  /**
   * Signal handler when the adaptor's window resizes itself.
   * @param[in]  adaptor  The adaptor
   */
  void OnResize(Dali::Adaptor& adaptor);

public:  // Signals

  /**
   * @copydoc Dali::Application::InitSignal()
   */
  Dali::Application::AppSignalV2& InitSignal() { return mInitSignalV2; }

  /**
   * @copydoc Dali::Application::TerminateSignal()
   */
  Dali::Application::AppSignalV2& TerminateSignal() { return mTerminateSignalV2; }

  /**
   * @copydoc Dali::Application::PauseSignal()
   */
  Dali::Application::AppSignalV2& PauseSignal() { return mPauseSignalV2; }

  /**
   * @copydoc Dali::Application::ResumeSignal()
   */
  Dali::Application::AppSignalV2& ResumeSignal() { return mResumeSignalV2; }

  /**
   * @copydoc Dali::Application::ResetSignal()
   */
  Dali::Application::AppSignalV2& ResetSignal() { return mResetSignalV2; }

  /**
   * @copydoc Dali::Application::ResizeSignal()
   */
  Dali::Application::AppSignalV2& ResizeSignal() { return mResizeSignalV2; }

  /**
   * @copydoc Dali::Application::LanguageChangedSignal()
   */
  Dali::Application::AppSignalV2& LanguageChangedSignal() { return mLanguageChangedSignalV2; }

  Dali::Application::AppSignalV2& SurfaceCreatedSignal() { return mSurfaceCreatedSignalV2; }

  Dali::Application::AppSignalV2& SurfaceDestroyedSignal() { return mSurfaceDestroyedSignalV2; }

private:

  // Undefined
  Application(const Application&);
  Application& operator=(Application&);

private:
  /**
   * Creates the window
   */
  void CreateWindow();

  /**
   * Creates the adaptor
   */
  void CreateAdaptor();

  /**
   * Quits from the main loop
   */
  void QuitFromMainLoop();

private:

  AppSignalV2                           mInitSignalV2;
  AppSignalV2                           mTerminateSignalV2;
  AppSignalV2                           mPauseSignalV2;
  AppSignalV2                           mResumeSignalV2;
  AppSignalV2                           mResetSignalV2;
  AppSignalV2                           mResizeSignalV2;
  AppSignalV2                           mLanguageChangedSignalV2;
  AppSignalV2							              mSurfaceCreatedSignalV2;
  AppSignalV2							              mSurfaceDestroyedSignalV2;

  Dali::Orientation*                    mOrientation;

  EventLoop*                            mEventLoop;
  Framework*                            mFramework;

  Dali::Adaptor*                        mAdaptor;
  WindowPtr                             mWindow;
  std::string                           mName;

  bool                                  mInitialized;
  DeviceLayout                          mBaseLayout;
  bool                                  mAdaptorStarted;

  SlotDelegate< Application >           mSlotDelegate;
};

inline Application& GetImplementation(Dali::Application& application)
{
  DALI_ASSERT_ALWAYS(application && "application handle is empty");

  BaseObject& handle = application.GetBaseObject();

  return static_cast<Internal::Adaptor::Application&>(handle);
}

inline const Application& GetImplementation(const Dali::Application& application)
{
  DALI_ASSERT_ALWAYS(application && "Timre handle is empty");

  const BaseObject& handle = application.GetBaseObject();

  return static_cast<const Internal::Adaptor::Application&>(handle);
}


} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_APPLICATION_H__
