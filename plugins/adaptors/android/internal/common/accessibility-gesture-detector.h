#ifndef __DALI_INTERNAL_ACCESSIBILITY_GESTURE_DETECTOR_H__
#define __DALI_INTERNAL_ACCESSIBILITY_GESTURE_DETECTOR_H__

/*
Copyright (c) 2000-2013 Samsung Electronics Co., Ltd All Rights Reserved

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

// INTERNAL INCLUDES
#include <internal/common/events/pan-gesture-detector-base.h>
#include <internal/common/adaptor-impl.h>
#include <public-api/adaptor-framework/common/accessibility-gesture-handler.h>

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
 * Detects an accessibility pan gesture and sends it to the gesture handler.
 */
class AccessibilityGestureDetector : public PanGestureDetectorBase
{
public:

  /**
   * Constructor
   */
  AccessibilityGestureDetector();

  /**
   * Virtual destructor.
   */
  virtual ~AccessibilityGestureDetector();

  /**
   * Set the handler to handle accessibility gestures.
   * @param[in] handler The Accessibility gesture handler.
   * @note Handlers should remove themselves when they are destroyed.
   */
  void SetGestureHandler(AccessibilityGestureHandler& handler);

  /**
   * Set the PPD scale that should be applied to when processing accessibility gesture.
   * @param[in]  ppdScale  The PPD Scale.
   */
  void SetPpdScale(float ppdScale);

private:

  /**
   * Emits the pan gesture event to the gesture handler.
   * @param[in] gesture The pan gesture event.
   */
  virtual void EmitPan(const Integration::PanGestureEvent gesture);

private:

  AccessibilityGestureHandler* mGestureHandler; ///< The pointer of accessibility gesture handler
  float mPpdScale;  ///< The PPD Scale that should be applied to gesture event when passing values back to the application.
  bool mPanning;    ///< Keep track of panning state, when panning is occuring, this is true.
};

typedef Dali::IntrusivePtr<AccessibilityGestureDetector> AccessibilityGestureDetectorPtr;

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_ACCESSIBILITY_GESTURE_DETECTOR_H__
