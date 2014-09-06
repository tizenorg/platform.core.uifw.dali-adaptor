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

// CLASS HEADER
#include "accessibility-gesture-detector.h"

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include <dali/integration-api/events/gesture-requests.h>
#include <dali/integration-api/core.h>
#include <internal/common/accessibility-manager-impl.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

AccessibilityGestureDetector::AccessibilityGestureDetector()
: PanGestureDetectorBase(Vector2::ZERO, Integration::PanGestureRequest()),
  mGestureHandler(NULL),
  mPpdScale( 1.0f ),
  mPanning(false)
{
}

AccessibilityGestureDetector::~AccessibilityGestureDetector()
{
}

void AccessibilityGestureDetector::SetGestureHandler(AccessibilityGestureHandler& handler)
{
  mGestureHandler = &handler;
}

void AccessibilityGestureDetector::SetPpdScale(float ppdScale)
{
  mPpdScale = ppdScale;
}

void AccessibilityGestureDetector::EmitPan(const Integration::PanGestureEvent gesture)
{
  if( mGestureHandler )
  {
    if(gesture.state == Gesture::Started)
    {
      mPanning = true;
    }

    if( mPanning )
    {
      mGestureHandler->HandlePanGesture(gesture, mPpdScale);

      if( (gesture.state == Gesture::Finished) ||
          (gesture.state == Gesture::Cancelled) )
      {
        mPanning = false;
      }
    }
  }
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
