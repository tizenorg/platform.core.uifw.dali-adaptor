#ifndef __DALI_INTERNAL_GESTURE_MANAGER_H__
#define __DALI_INTERNAL_GESTURE_MANAGER_H__

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

// EXTERNAL INCLUDES
#include <vector>

#include <dali/public-api/math/vector2.h>
#include <dali/integration-api/gesture-manager.h>

// INTERNAL INCLUDES
#include <internal/common/events/gesture-detector.h>

namespace Dali
{

namespace Integration
{
class Core;
struct TouchEvent;
}

namespace Internal
{

namespace Adaptor
{

class CallbackManager;

/**
 * Implementation of the Integration::GestureManager.
 *
 * Contains a list of adaptor gesture detectors. It passes touch events to each required detector which
 * in turn process them to determine if their corresponding gesture has occurred.
 */
class GestureManager : public Integration::GestureManager
{
public:

  /**
   * Constructor.
   * @param[in]  screenSize  The size of the screen.
   * @param[in]  callbackManager used to install callbacks
   */
  GestureManager(Vector2 screenSize, CallbackManager* callbackManager);

  /**
   * The destructor
   */
  virtual ~GestureManager();

public:

  /**
   * Used by the event handler to send touch events to the Gesture Manager.
   * @param[in]  event  The latest touch event.
   */
  void SendEvent(const Integration::TouchEvent& event);

  /**
   * Sets the core object so that the gesture manager can send gesture events to it.
   */
  void SetCore(Integration::Core& core);

  /**
   * Used by the event handler to starts the GestureManager detection.
   */
  void Start();

  /**
   * Used by the event handler to stop the GestureManager detection.
   */
  void Stop();

public: // GestureManager overrides

  /**
   * copydoc Dali::Integration::GestureManager::Register(const Integration::GestureRequest&)
   */
  virtual void Register(const Integration::GestureRequest& request);

  /**
   * copydoc Dali::Integration::GestureManager::Unregister(const Integration::GestureRequest&)
   */
  virtual void Unregister(const Integration::GestureRequest& request);

  /**
   * copydoc Dali::Integration::GestureManager::Unregister(const Integration::GestureRequest&)
   */
  virtual void Update(const Integration::GestureRequest& request);

private:

  /**
   * Used to delete the gesture detector of the given type.
   */
  void DeleteGestureDetector( Gesture::Type type );

private:

  typedef std::vector<GestureDetectorPtr> GestureDetectorContainer;

  Integration::Core* mCore;
  GestureDetectorContainer mGestureDetectors;
  Vector2 mScreenSize;
  CallbackManager* mCallbackManager;
  bool mRunning; ///< States whether the GestureManager is running or not.
};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_GESTURE_MANAGER_H__
