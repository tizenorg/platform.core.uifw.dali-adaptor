#ifndef __DALI_APPLICATION_H__
#define __DALI_APPLICATION_H__

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
#include <boost/function.hpp>
#include <string>
#include <dali/public-api/signals/dali-signal-v2.h>
#include <dali/public-api/object/base-handle.h>

// INTERNAL INCLUDES
#include <dali/public-api/adaptor-framework/common/style-monitor.h>
#include <dali/public-api/adaptor-framework/common/device-layout.h>
#include <dali/public-api/adaptor-framework/common/window.h>
#include <dali/public-api/adaptor-framework/common/adaptor.h>

namespace Dali DALI_IMPORT_API
{

namespace Internal DALI_INTERNAL
{
namespace Adaptor
{
class Application;
}
}

/**
 * An Application class object should be created by every application
 * that wishes to use Dali.  It provides a means for initialising the
 * resources required by the Dali::Core.
 *
 * The Application class emits several signals which the user can
 * connect to.  The user should not create any Dali objects in the main
 * function and instead should connect to the Init signal of the
 * Application and create the Dali objects in the connected callback.
 *
 * Linux Applications should follow the example below:
 *
 * @code
 * void CreateProgram(Application app)
 * {
 *   // Create Dali components...
 *   // Can instantiate here, if required
 * }
 *
 * int main (int argc, char **argv)
 * {
 *   Application app = Application::New(&argc, &argv);
 *   app.SignalInit().Connect(&CreateProgram);
 *   app.MainLoop();
 * }
 * @endcode
 *
 * If required, you can also connect class member functions to a signal:
 *
 * @code
 * MyApplication app;
 * app.SignalResume().Connect(&app, &MyApplication::Resume);
 * @endcode
 */
class Application : public BaseHandle
{
public:

  typedef SignalV2< void (Application&) > AppSignalV2;

  //typedef boost::signals2::signal_type< void (Application&), DummyMutex >::type AppSignalType;

  //typedef boost::function<void (Application&)> AppCallbackType;

  //typedef Signal<AppSignalType, AppCallbackType> AppSignal;

public:

  /**
   * This is the constructor for Linux & SLP applications.
   * @param  argc        A pointer to the number of arguments
   * @param  argv        A pointer the the argument list
   * @note The default base layout (DeviceLayout::DEFAULT_BASE_LAYOUT) will be used with this constructor.
   */
  static Application New( int* argc, char **argv[] );

  /**
   * This is the constructor for Linux & SLP applications.
   * @note The default base layout (DeviceLayout::DEFAULT_BASE_LAYOUT) will be used with this constructor.
   */
  static Application New();

  /**
   * This is the constructor for Linux & SLP applications with a name
   * @param  argc  A pointer to the number of arguments
   * @param  argv  A pointer the the argument list
   * @param  name  A name of application
   * @note The default base layout (DeviceLayout::DEFAULT_BASE_LAYOUT) will be used with this constructor.
   */
  static Application New( int* argc, char **argv[], const std::string& name );

  /**
   * This is the constructor for Linux & SLP applications when a layout for the application is specified.
   * @param  argc        A pointer to the number of arguments
   * @param  argv        A pointer the the argument list
   * @param  baseLayout  The base layout that the application has been written for
   */
  static Application New( int* argc, char **argv[], const DeviceLayout& baseLayout );

  /**
   * This is the constructor for Linux & SLP applications with a name and when a layout for the application is specified.
   * @param  argc  A pointer to the number of arguments
   * @param  argv  A pointer the the argument list
   * @param  name  A name of application
   * @param  baseLayout  The base layout that the application has been written for
   */
  static Application New( int* argc, char **argv[], const std::string& name, const DeviceLayout& baseLayout );

  /**
   * Construct an empty handle
   */
  Application();

  /**
   * Copy Constructor
   */
  Application( const Application& application );

  /**
   * Assignment operator
   */
  Application& operator=( const Application& applicaton );

  /**
   * Virtual destructor
   */
  virtual ~Application();

public:
  /**
   * This starts the application.
   */
  void MainLoop();

  /**
   * This quits the application.
   */
  void Quit();

  /**
   * This returns the Orientation object used by Application which allows
   * the user to determine the orientation of the device and connect to a
   * signal emitted whenever the orientation changes.
   * @return A reference to the Orientation object used by the Application
   */
  Orientation& GetOrientation();

  /**
   * Ensures that the function passed in is called from the main loop when it is idle.
   *
   * A callback of the following type may be used:
   * @code
   *   void MyFunction();
   * @endcode
   *
   * @param[in]  callBack  The function to call.
   * @return true if added successfully, false otherwise
   */
  bool AddIdle(boost::function<void(void)> callBack);

 
  /**
   * Retrieves the window used by the Application class.
   * The application writer can use the window to change indicator and orientation
   * properties.
   * @return A handle to the window
   */
  Window GetWindow();

  /**
   * Returns the local thread's instance of the Application class.
   * @return A reference to the local thread's Application class instance.
   * @pre The Application class has been initialised.
   * @note This is only valid in the main thread.
   */
  static Application Get();

  Adaptor& GetAdaptor();

  void Ticker();

public:  // Signals

  /**
   * The user should connect to this signal to determine when they should initialise
   * their application
   */
  //AppSignal SignalInit();
  AppSignalV2& InitSignal();

  /**
   * The user should connect to this signal to determine when they should terminate
   * their application
   */
  //AppSignal SignalTerminate();
  AppSignalV2& TerminateSignal();

  /**
   * The user should connect to this signal if they need to perform any special
   * activities when the application is about to be paused.
   */
  //AppSignal SignalPause();
  AppSignalV2& PauseSignal();

  /**
   * The user should connect to this signal if they need to perform any special
   * activities when the application has resumed.
   */
  //AppSignal SignalResume();
  AppSignalV2& ResumeSignal();

  /**
   * This signal is sent when the system requires the user to reinitialise itself.
   */
  //AppSignal SignalReset();
  AppSignalV2& ResetSignal();

  /**
   * This signal is emitted when the window the application is rendering on is resized.
   */
  //AppSignal SignalResize();
  AppSignalV2& ResizeSignal();

  /**
   * This signal is emitted when the language is changed on the device.
   */
  //AppSignal SignalLanguageChanged();
  AppSignalV2& LanguageChangedSignal();

  /**
   * This signal is emitted when the surface is created on the device.
   */
  //AppSignal SignalSurfaceCreated();
  AppSignalV2& SurfaceCreatedSignal();

  /**
   * This signal is emitted when the surface is created on the device.
   */
  //AppSignal SignalSurfaceCreated();
  AppSignalV2& SurfaceDestroyedSignal();

public: // Not intended for application developers
  /**
   * Internal constructor
   */
  explicit DALI_INTERNAL Application(Internal::Adaptor::Application* application);
};

} // namespace Dali

#endif // __DALI_APPLICATION_H__
