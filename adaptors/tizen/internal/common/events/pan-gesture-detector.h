#ifndef __DALI_INTERNAL_PAN_GESTURE_DETECTOR_H__
#define __DALI_INTERNAL_PAN_GESTURE_DETECTOR_H__

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

// INTERNAL INCLUDES
#include <internal/common/events/pan-gesture-detector-base.h>
#include <dali/integration-api/events/pan-gesture-event.h>

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
 * Detects a pan gesture and sends it to core.
 */
class PanGestureDetector : public PanGestureDetectorBase
{
public:

  /**
   * Constructor
   * @param[in]  core        The core object.
   * @param[in]  screenSize  The size of the screen.
   * @param[in]  request     The details of the request.
   */
  PanGestureDetector(Integration::Core& core, Vector2 screenSize, const Integration::PanGestureRequest& request);

  /**
   * Virtual destructor.
   */
  virtual ~PanGestureDetector();

private:

  /**
   * Emits the pan gesture event to the core.
   * @param[in] gesture The pan gesture event.
   */
  virtual void EmitPan(const Integration::PanGestureEvent gesture);

private:

  Integration::Core& mCore; ///< The core
};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_PAN_GESTURE_DETECTOR_H__
