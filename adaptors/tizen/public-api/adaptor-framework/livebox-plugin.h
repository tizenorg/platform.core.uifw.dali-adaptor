#ifndef __DALI_LIVEBOX_H__
#define __DALI_LIVEBOX_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// EXTERNAL INCLUDS
#include <boost/function.hpp>
#include <string>
#include <dali/public-api/math/rect.h>

// INTERNAL INCLUDES
#include "common/device-layout.h"
#include "common/style-monitor.h"


namespace Dali DALI_IMPORT_API
{
typedef Dali::Rect<int> PositionSize;

/**
* Livebox's size types.
* It refers "livebox-service.h"
*/
enum LiveboxSizeType
{
  LIVEBOX_SIZE_TYPE_1x1 = 0x0001,
  LIVEBOX_SIZE_TYPE_2x1 = 0x0002,
  LIVEBOX_SIZE_TYPE_2x2 = 0x0004,
  LIVEBOX_SIZE_TYPE_4x1 = 0x0008,
  LIVEBOX_SIZE_TYPE_4x2 = 0x0010,
  LIVEBOX_SIZE_TYPE_4x3 = 0x0020,
  LIVEBOX_SIZE_TYPE_4x4 = 0x0040,
  LIVEBOX_SIZE_TYPE_EASY_1x1 = 0x0100,
  LIVEBOX_SIZE_TYPE_EASY_3x1 = 0x0200,
  LIVEBOX_SIZE_TYPE_EASY_3x3 = 0x0400,
  LIVEBOX_SIZE_TYPE_UNKNOWN = 0xFFFF,
};

struct GlanceBarEventInfo
{
  std::string emission;
  std::string source;

  struct
  {
    double x;
    double y;
    int down;
  } pointer; ///< touch information for script

  struct
  {
    double sx;
    double sy;
    double ex;
    double ey;
  } part; ///<part information for script
};

namespace Internal DALI_INTERNAL
{
namespace Adaptor
{

class LiveboxPlugin;
}
}

/**
 * @brief A native Dali application class that draws to a livebox.
 *
 * A LiveboxPlugin class object should be created by every livebox.
 * that wishes to use Dali.  It provides a means for initialising the
 * resources required by the Dali::Core.
 *
 * The LiveboxPlugin class emits several signals which the user can
 * connect to.  The user should not create any Dali objects in the main
 * function and instead should connect to the Init signal of the
 * LiveboxPlugin and create the Dali objects in the connected callback.
 *
 * SLP and Linux Liveboxs should follow the example below:
 *
 * @code
 * void CreateProgram(LiveboxPlugin& livebox)
 * {
 *   // Create Dali components...
 *   // Can instantiate here, if required
 * }
 *
 * int main (int argc, char **argv)
 * {
 *   LiveboxPlugin livebox(&argc, &argv);
 *   livebox.InitSignal().Connect(&CreateProgram);
 *   livebox.Run();
 * }
 * @endcode
 *
 * If required, you can also connect class member functions to a signal:
 *
 * @code
 * MyLivebox livebox;
 * livebox.SignalResumed().Connect(&app, &MyLivebox::OnResumed);
 * @endcode
 */
class LiveboxPlugin
{
public:

  typedef SignalV2< void (LiveboxPlugin&) > LiveboxPluginSignalV2;

public:

  /**
   * @brief This is the constructor for Linux & SLP liveboxs.
   *
   * @param  argc        A pointer to the number of arguments
   * @param  argv        A pointer the the argument list
   * @note The default base layout (DeviceLayout::DEFAULT_BASE_LAYOUT) will be used with this constructor.
   */
  LiveboxPlugin( int* argc, char **argv[] );

  /**
   * @brief This is the constructor for Linux & SLP liveboxs with a name.
   *
   * @param  argc  A pointer to the number of arguments
   * @param  argv  A pointer the the argument list
   * @param  name  A name of livebox
   * @note The default base layout (DeviceLayout::DEFAULT_BASE_LAYOUT) will be used with this constructor.
   */
  LiveboxPlugin( int* argc, char **argv[], const std::string& name );

  /**
   * @brief This is the constructor for Linux & SLP liveboxs when a layout for the livebox is specified.
   *
   * @param  argc        A pointer to the number of arguments
   * @param  argv        A pointer the the argument list
   * @param  baseLayout  The base layout that the livebox has been written for
   */
  LiveboxPlugin( int* argc, char **argv[], const DeviceLayout& baseLayout );

  /**
   * @brief This is the constructor for Linux & SLP liveboxs with a name and when a layout for the livebox is specified.
   *
   * @param  argc  A pointer to the number of arguments
   * @param  argv  A pointer the the argument list
   * @param  name  A name of livebox
   * @param  baseLayout  The base layout that the livebox has been written for
   */
  LiveboxPlugin( int* argc, char **argv[], const std::string& name, const DeviceLayout& baseLayout );

  /**
   * @brief Virtual destructor.
   */
  virtual ~LiveboxPlugin();

public:

  /**
   * @brief Set title string of the livebox.
   *
   * @param[in] title title string
   */
  void SetTitle(const std::string& title);

  /**
   * @brief Set content string of the livebox.
   *
   * @param[in] content content string
   */
  void SetContent(const std::string& content);

  /**
   * @brief Get glance bar's geometry information.
   *
   * In the return value,
   *   x, y mean arrow position
   *   w, h mean glance size
   * User can use this method in the glance-created signal handler
   * @return PositionSize structure for glance bar information. {-1, -1, -1, -1} means invalid status for glance
   */
  const PositionSize& GetGlanceBarGeometry() const;

  /**
   * @brief Get glance bar's event information.
   *
   * @return GlanceBarEventInfo structure for glance bar event
   */
  const GlanceBarEventInfo& GetGlanceBarEventInfo() const;

  /**
   * @brief Get current size type of livebox.
   */
  LiveboxSizeType GetLiveboxSizeType() const;

  /**
   * @brief This starts the livebox providing.
   */
  void Run();

  /**
   * @brief This quits the livebox providing.
   */
  void Quit();

  /**
   * @brief Ensures that the function passed in is called from the main loop when it is idle.
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
   * @brief Returns the local thread's instance of the LiveboxPlugin class.
   *
   * @return A reference to the local thread's LiveboxPlugin class instance.
   * @pre The LiveboxPlugin class has been initialised.
   * @note This is only valid in the main thread.
   */
  static LiveboxPlugin& Get();

public:  // Signals

  /**
   * @brief The user should connect to this signal to determine when they should initialise
   * their livebox.
   */
  LiveboxPluginSignalV2& InitializedSignal();

  /**
   * @brief The user should connect to this signal to determine when they should terminate
   * their livebox.
   */
  LiveboxPluginSignalV2& TerminatedSignal();

  /**
   * @brief The user should connect to this signal if they need to perform any special
   * activities when the livebox is about to be paused.
   */
  LiveboxPluginSignalV2& PausedSignal();

  /**
   * @brief The user should connect to this signal if they need to perform any special
   * activities when the livebox has resumed.
   */
  LiveboxPluginSignalV2& ResumedSignal();

  /**
   * @brief This signal is emitted when the surface the livebox is rendering on is resized.
   */
  LiveboxPluginSignalV2& ResizedSignal();

  /**
   * @brief This signal is emitted when the glance bar popup was created.
   */
  LiveboxPluginSignalV2& GlanceCreatedSignal();

  /**
   * @brief This signal is emitted when the glance bar popup was destroyed.
   */
  LiveboxPluginSignalV2& GlanceDestroyedSignal();

  /**
   * @brief This signal is emitted when the glance bar popup was touched.
   */
  LiveboxPluginSignalV2& GlanceTouchedSignal();

  /**
   * @brief This signal is emitted when the glance bar popup was moved.
   */
  LiveboxPluginSignalV2& GlanceMovedSignal();

  /**
   * @brief This signal is emitted when the glance bar popup got the script event callback.
   *
   * If application registered the edje file for glance bar,
   * this signal will be emitted instead of SignalGlanceTouched.
   * Application can get the event information by using GetGlanceBarEventInfo()
   */
  LiveboxPluginSignalV2& GlanceScriptEventSignal();

  /**
   * @brief This signal is emitted when the language is changed on the device.
   */
  LiveboxPluginSignalV2& LanguageChangedSignal();

private:

  // Undefined
  LiveboxPlugin(const LiveboxPlugin&);
  LiveboxPlugin& operator=(const LiveboxPlugin&);

private:

  Internal::Adaptor::LiveboxPlugin *mImpl;
  friend class Internal::Adaptor::LiveboxPlugin;
};

} // namespace Dali

#endif // __DALI_LIVEBOX_H__
