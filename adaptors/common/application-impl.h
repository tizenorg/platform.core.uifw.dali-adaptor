#ifndef __DALI_INTERNAL_APPLICATION_H__
#define __DALI_INTERNAL_APPLICATION_H__

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

// EXTERNAL INCLUDES
#include <dali/public-api/math/rect.h>
#include <dali/public-api/object/base-object.h>

// INTERNAL INCLUDES
#include <application.h>
#include <singleton-service.h>

#include <framework.h>
#include <window-impl.h>
#include <base/environment-options.h>
#include <adaptors/integration-api/framework.h>

namespace Dali
{
class Adaptor;
class Window;

namespace Internal
{

namespace Adaptor
{
class CommandLineOptions;
class EventLoop;

typedef Dali::Rect<int> PositionSize;

class Application;
typedef IntrusivePtr<Application> ApplicationPtr;

/**
 * Implementation of the Application class.
 */
class Application : public BaseObject, public Integration::Framework::Observer
{
public:

  typedef Dali::Application::AppSignalType AppSignalType;
  typedef Dali::Application::AppControlSignalType AppControlSignalType;
  typedef Dali::Application::WINDOW_MODE WINDOW_MODE;

  /**
   * Create a new application
   * @param[in]  framework   A pointer to the application framework
   * @param[in]  argc        A pointer to the number of arguments
   * @param[in]  argv        A pointer to the argument list
   * @param[in]  stylesheet  The path to user defined theme file
   * @param[in]  windowMode  A member of Dali::Application::WINDOW_MODE
   */
  static ApplicationPtr New( Integration::Framework* framework, int* argc, char **argv[], const std::string& stylesheet, WINDOW_MODE windowMode );

public:

  /**
   * @copydoc Dali::Application::MainLoop()
   */
  void Start(Dali::Configuration::ContextLoss configuration);

  /**
   * @copydoc Dali::Application::MainLoop()
   */
  void MainLoop(Dali::Configuration::ContextLoss configuration);

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
  bool AddIdle( CallbackBase* callback );

  /**
   * @copydoc Dali::Application::GetAdaptor();
   */
  Dali::Adaptor& GetAdaptor();

  /**
   * @copydoc Dali::Application::GetWindow();
   */
  Dali::Window GetWindow();

  /**
   * @copydoc Dali::Application::ReplaceWindow();
   */
  void ReplaceWindow(PositionSize windowPosition, const std::string& name);

public: // Stereoscopy

  /**
   * @copydoc Dali::Application::SetViewMode()
   */
  void SetViewMode( ViewMode viewMode );

  /**
   * @copydoc Dali::Application::GetViewMode()
   */
  ViewMode GetViewMode() const;

  /**
   * @copydoc Dali::Application::SetStereoBase()
   */
  void SetStereoBase( float stereoBase );

  /**
   * @copydoc Dali::Application::GetStereoBase()
   */
  float GetStereoBase() const;

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
  Dali::Application::AppSignalType& InitSignal() { return mInitSignal; }

  /**
   * @copydoc Dali::Application::TerminateSignal()
   */
  Dali::Application::AppSignalType& TerminateSignal() { return mTerminateSignal; }

  /**
   * @copydoc Dali::Application::PauseSignal()
   */
  Dali::Application::AppSignalType& PauseSignal() { return mPauseSignal; }

  /**
   * @copydoc Dali::Application::ResumeSignal()
   */
  Dali::Application::AppSignalType& ResumeSignal() { return mResumeSignal; }

  /**
   * @copydoc Dali::Application::ResetSignal()
   */
  Dali::Application::AppSignalType& ResetSignal() { return mResetSignal; }

  /**
   * @copydoc Dali::Application::ResizeSignal()
   */
  Dali::Application::AppSignalType& ResizeSignal() { return mResizeSignal; }

  /**
   * @copydoc Dali::Application::LanguageChangedSignal()
   */
  Dali::Application::AppSignalType& LanguageChangedSignal() { return mLanguageChangedSignal; }

  /**
   * @copydoc Dali::Application::SurfaceCreatedSignal()
   */
  Dali::Application::AppSignalType& SurfaceCreatedSignal() { return mSurfaceCreatedSignal; }

  /**
   * @copydoc Dali::Application::SurfaceDestroyedSignal()
   */
  Dali::Application::AppSignalType& SurfaceDestroyedSignal() { return mSurfaceDestroyedSignal; }

private:

  /**
   * Private Constructor
   * @param[in]  framework   A pointer to the application framework
   * @param[in]  argc        A pointer to the number of arguments
   * @param[in]  argv        A pointer to the argument list
   * @param[in]  stylesheet  The path to user defined theme file
   * @param[in]  windowMode  A member of Dali::Application::WINDOW_MODE
   */
  Application( Integration::Framework* framework, int* argc, char **argv[], const std::string& stylesheet, WINDOW_MODE windowMode );

  /**
   * Destructor
   */
  virtual ~Application();

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

  AppSignalType                         mInitSignal;
  AppSignalType                         mTerminateSignal;
  AppSignalType                         mPauseSignal;
  AppSignalType                         mResumeSignal;
  AppSignalType                         mResetSignal;
  AppSignalType                         mResizeSignal;
  AppSignalType                         mLanguageChangedSignal;
  AppSignalType                         mSurfaceCreatedSignal;
  AppSignalType                         mSurfaceDestroyedSignal;

  Dali::Orientation*                    mOrientation;
  EventLoop*                            mEventLoop;
  Integration::Framework*               mFramework;

  Dali::Configuration::ContextLoss      mContextLossConfiguration;
  CommandLineOptions*                   mCommandLineOptions;

  Dali::SingletonService                mSingletonService;
  Dali::Adaptor*                        mAdaptor;
  Dali::Window                          mWindow;
  void*                                 mSurface;

  Dali::Application::WINDOW_MODE        mWindowMode;
  std::string                           mName;
  std::string                           mStylesheet;
  EnvironmentOptions                    mEnvironmentOptions;
  bool                                  mInitialized;
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
