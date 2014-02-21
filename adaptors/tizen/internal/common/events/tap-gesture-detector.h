#ifndef __DALI_INTERNAL_TAP_GESTURE_DETECTOR_H__
#define __DALI_INTERNAL_TAP_GESTURE_DETECTOR_H__

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
#include <dali/public-api/adaptor-framework/common/timer.h>

// INTERNAL INCLUDES
#include <internal/common/events/gesture-detector.h>

namespace Dali
{

namespace Integration
{
class Core;
struct TouchEvent;
struct TapGestureRequest;
}

namespace Internal
{

namespace Adaptor
{

/**
 * When given a set of touch events, this detector attempts to determine if a tap gesture has taken place.
 */
class TapGestureDetector : public GestureDetector
{
public:

  /**
   * Constructor
   * @param[in]  core        The core object.
   * @param[in]  screenSize  The size of the screen.
   * @param[in]  request     The tap gesture request.
   */
  TapGestureDetector(Integration::Core& core, Vector2 screenSize, const Integration::TapGestureRequest& request);

  /**
   * Virtual destructor.
   */
  virtual ~TapGestureDetector();

public:

  /**
   * @copydoc Dali::Internal::GestureDetector::SendEvent(const Integration::TouchEvent&)
   */
  virtual void SendEvent(const Integration::TouchEvent& event);

  /**
   * @copydoc Dali::Internal::GestureDetector::Update(const Integration::GestureRequest&)
   */
  virtual void Update(const Integration::GestureRequest& request);

private:

  /**
   * Timer Callback
   * @return will return false; one-shot timer.
   */
  bool TimerCallback();

  /**
   * Checks if registered taps are within required bounds and emits tap gesture if they are.
   */
  void EmitGesture( Gesture::State state, unsigned int time );

private:

  /**
   * Internal state machine.
   */
  enum State
  {
    Clear,      ///< No gesture detected.
    Touched,    ///< User is touching the screen.
    Registered, ///< At least one tap has been registered.
    Failed,     ///< Gesture has failed.
  };

  Integration::Core& mCore; ///< The core
  State mState; ///< Current state of the detector.

  int mMinimumTapsRequired; ///< Minimum number of taps required.
  int mMaximumTapsRequired; ///< Maximum number of taps required.
  int mTapsRegistered;      ///< In current detection, the number of taps registered.

  Vector2 mTouchPosition;   ///< The initial touch down position.
  unsigned long mTouchTime; ///< The initial touch down time.

  Dali::Timer mTimer;       ///< The timer to start when we have registered the tap. We have to register all taps within a certain time frame.
  SlotDelegate< TapGestureDetector > mTimerSlot;
};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_TAP_GESTURE_DETECTOR_H__
