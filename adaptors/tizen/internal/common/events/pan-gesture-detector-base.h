#ifndef __DALI_INTERNAL_PAN_GESTURE_DETECTOR_BASE_H__
#define __DALI_INTERNAL_PAN_GESTURE_DETECTOR_BASE_H__

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
#include <dali/integration-api/events/pan-gesture-event.h>

// INTERNAL INCLUDES
#include <internal/common/events/gesture-detector.h>

namespace Dali
{

namespace Integration
{
class Core;
struct TouchEvent;
struct PanGestureRequest;
}

namespace Internal
{

namespace Adaptor
{

/**
 * When given a set of touch events, this detector attempts to determine if a pan gesture has taken place.
 */
class PanGestureDetectorBase : public GestureDetector
{
public:

  /**
   * Virtual destructor.
   */
  virtual ~PanGestureDetectorBase();

public:

  /**
   * @copydoc Dali::Internal::GestureDetector::SendEvent(const Integration::TouchEvent&)
   */
  virtual void SendEvent(const Integration::TouchEvent& event);

  /**
   * @copydoc Dali::Internal::GestureDetector::Update(const Integration::GestureRequest&)
   */
  virtual void Update(const Integration::GestureRequest& request);

protected:

  /**
   * Constructor
   * @param[in]  screenSize  The size of the screen.
   * @param[in]  request     The details of the request.
   */
  PanGestureDetectorBase(Vector2 screenSize, const Integration::PanGestureRequest& request);

private:

  /**
   * Emits the pan gesture event (performs some smoothing operation).
   * @param[in]  state         The state of the pan.
   * @param[in]  currentEvent  The latest touch event.
   */
  void SendPan(Gesture::State state, const Integration::TouchEvent& currentEvent);

  /**
   * Emits the pan gesture event to the core.
   * @param[in] gesture The pan gesture event.
   */
  virtual void EmitPan(const Integration::PanGestureEvent gesture) = 0;

private:

  /**
   * Internal state machine.
   */
  enum State
  {
    Clear,    ///< No gesture detected.
    Possible, ///< The current touch event data suggests that a gesture is possible.
    Started,  ///< A gesture has been detected.
    Finished, ///< A previously started pan gesture has finished.
    Failed,   ///< Current touch event data suggests a pan gesture is not possible.
  };

  State mState; ///< The current state of the detector.
  std::vector<Integration::TouchEvent> mTouchEvents;     ///< A container of all touch events after an initial down event.

  Vector2 mPrimaryTouchDownLocation;    ///< The initial touch down point.
  unsigned long mPrimaryTouchDownTime;  ///< The initial touch down time.
  unsigned int mMinimumTouchesRequired; ///< The minimum touches required before a pan should be emitted.
  unsigned int mMaximumTouchesRequired; ///< The maximum touches after which a pan should not be emitted.
};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_PAN_GESTURE_DETECTOR_BASE_H__
