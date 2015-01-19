#ifndef __DALI_APPLICATION_H__
#define __DALI_APPLICATION_H__

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
#include <boost/function.hpp>
#include <string>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/common/view-mode.h>

#include "application-configuration.h"
#include "style-monitor.h"
#include "device-layout.h"
#include "window.h"

namespace Dali
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
 * Applications should follow the example below:
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
 *   app.InitSignal().Connect(&CreateProgram);
 *   app.MainLoop();
 * }
 * @endcode
 *
 * If required, you can also connect class member functions to a signal:
 *
 * @code
 * MyApplication app;
 * app.ResumeSignal().Connect(&app, &MyApplication::Resume);
 * @endcode
 *
 * This class accepts command line arguments as well. The following options are supported:
 *
 * @code
 *     --no-vsync       Disable VSync on Render
 *  -w|--width          Stage Width
 *  -h|--height         Stage Height
 *  -d|--dpi            Emulated DPI
 *     --help           Help
 * @endcode
 *
 * When the above options are found, they are stripped from argv, and argc is updated appropriately.
 */
class DALI_IMPORT_API Application : public BaseHandle
{
public:

  typedef SignalV2< void (Application&) > AppSignalV2;
  typedef SignalV2< void (Application&, void *) > ControlSignalV2;

  /**
   * Decides whether a Dali application window is opaque or transparent.
   */
  enum WINDOW_MODE
  {
    OPAQUE = 0,       ///< The window will be opaque
    TRANSPARENT = 1   ///< The window transparency will match the alpha value set in Dali::Stage::SetBackgroundcolor()
  };

public:

  /**
   * This is the constructor for applications without an argument list.
   *
   * @note The default base layout (DeviceLayout::DEFAULT_BASE_LAYOUT) will be used with this constructor.
   */
  static Application New();

  /**
   * This is the constructor for applications.
   *
   * @param[in,out]  argc        A pointer to the number of arguments
   * @param[in,out]  argv        A pointer the the argument list
   *
   * @note The default base layout (DeviceLayout::DEFAULT_BASE_LAYOUT) will be used with this constructor.
   * @note Supported options are stripped from argv, and argc is updated appropriately.
   */
  static Application New( int* argc, char **argv[] );

  /**
   * This is the constructor for applications with a name
   *
   * @param[in,out]  argc  A pointer to the number of arguments
   * @param[in,out]  argv  A pointer the the argument list
   * @param[in]  name  A name of application
   *
   * @note The default base layout (DeviceLayout::DEFAULT_BASE_LAYOUT) will be used with this constructor.
   * @note Supported options are stripped from argv, and argc is updated appropriately.
   */
  static Application New( int* argc, char **argv[], const std::string& name );

  /**
   * This is the constructor for applications with a name, and also require a
   * transparent top-level window
   *
   * @param[in,out]  argc  A pointer to the number of arguments
   * @param[in,out]  argv  A pointer the the argument list
   * @param[in]  name  A name of application
   * @param[in]  windowMode A member of WINDOW_MODE
   *
   * @note The default base layout (DeviceLayout::DEFAULT_BASE_LAYOUT) will be used with this constructor.
   * @note Supported options are stripped from argv, and argc is updated appropriately.
   */
  static Application New( int* argc, char **argv[], const std::string& name, WINDOW_MODE windowMode );

  /**
   * This is the constructor for applications when a layout for the application is specified.
   *
   * @param[in,out]  argc        A pointer to the number of arguments
   * @param[in,out]  argv        A pointer the the argument list
   * @param[in]  baseLayout  The base layout that the application has been written for
   *
   * @note Supported options are stripped from argv, and argc is updated appropriately.
   */
  static Application New( int* argc, char **argv[], const DeviceLayout& baseLayout );

  /**
   * This is the constructor for applications with a name and when a layout for the application is specified.
   *
   * @param[in,out]  argc  A pointer to the number of arguments
   * @param[in,out]  argv  A pointer the the argument list
   * @param[in]  name  A name of application
   * @param[in]  baseLayout  The base layout that the application has been written for
   *
   * @note Supported options are stripped from argv, and argc is updated appropriately.
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
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~Application();

public:
  /**
   * This starts the application. Choosing this form of main loop indicates that the default
   * application configuration of APPLICATION_HANDLES_CONTEXT_LOSS is used. On platforms where
   * context loss can occur, the application is responsible for tearing down and re-loading UI.
   * The application should listen to Stage::ContextLostSignal and Stage::ContextRegainedSignal.
   */
  void MainLoop();

  /**
   * This starts the application, and allows the app to choose a different configuration.
   * If the application plans on using the ReplaceSurface or ReplaceWindow API, then this will
   * trigger context loss & regain.
   * The application should listen to Stage::ContextLostSignal and Stage::ContextRegainedSignal.
   */
  void MainLoop(Configuration::ContextLoss configuration);

  /**
   * This lowers the application to bottom without actually quitting it
   */
  void Lower();

  /**
   * This quits the application.  Tizen applications should use Lower to improve re-start performance unless they need to Quit completely.
   */
  void Quit();

  /**
   * This returns a handle to the Orientation object used by Application which allows
   * the user to determine the orientation of the device and connect to a
   * signal emitted whenever the orientation changes.
   * @return A handle to the Orientation object used by the Application
   */
  Orientation GetOrientation();

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
   * Replace the current window.
   * This will force context loss.
   * If you plan on using this API in your application, then you should configure
   * it to prevent discard behaviour when handling the Init signal.
   * @param[in] windowPosition The position and size parameters of the new window
   * @param[in] name The name of the new window
   */
  void ReplaceWindow(PositionSize windowPosition, const std::string& name);

public: // Stereoscopy

  /**
   * Set the viewing mode for the application.
   * @param[in] viewMode The new viewing mode.
   */
  void SetViewMode( ViewMode viewMode );

  /**
   * Get the current viewing mode.
   * @return The current viewing mode.
   */
  ViewMode GetViewMode() const;

  /**
   * Set the stereo base (eye seperation) for stereoscopic 3D
   * @param[in] stereoBase The stereo base (eye seperation) for stereoscopic 3D
   */
  void SetStereoBase( float stereoBase );

  /**
   * Get the stereo base (eye seperation) for stereoscopic 3D
   * @return The stereo base (eye seperation) for stereoscopic 3D
   */
  float GetStereoBase() const;

public:  // Signals

  /**
   * The user should connect to this signal to determine when they should initialise
   * their application.
   */
  AppSignalV2& InitSignal();

  /**
   * The user should connect to this signal to determine when they should terminate
   * their application
   */
  AppSignalV2& TerminateSignal();

  /**
   * The user should connect to this signal if they need to perform any special
   * activities when the application is about to be paused.
   */
  AppSignalV2& PauseSignal();

  /**
   * The user should connect to this signal if they need to perform any special
   * activities when the application has resumed.
   */
  AppSignalV2& ResumeSignal();

  /**
   * This signal is sent when the system requires the user to reinitialise itself.
   */
  AppSignalV2& ResetSignal();

  /**
   * This signal is emitted when the window the application is rendering on is resized.
   */
  AppSignalV2& ResizeSignal();

  /**
   * This signal is emitted when there is control signal from the framework.
   */
  ControlSignalV2& ControlSignal();

  /**
   * This signal is emitted when the language is changed on the device.
   * This callback function is passed to the app_control containing the reason due to which the application is launched
   */
  AppSignalV2& LanguageChangedSignal();

  /**
   * This signal is emitted when the region of the device is changed.
   */
  AppSignalV2& RegionChangedSignal();

  /**
   * This signal is emitted when the battery level of the device is low.
   */
  AppSignalV2& BatteryLowSignal();

  /**
   * This signal is emitted when the memory level of the device is low.
   */
  AppSignalV2& MemoryLowSignal();

public: // Not intended for application developers
  /**
   * Internal constructor
   */
  explicit DALI_INTERNAL Application(Internal::Adaptor::Application* application);
};

} // namespace Dali

#endif // __DALI_APPLICATION_H__
