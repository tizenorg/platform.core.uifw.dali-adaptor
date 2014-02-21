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

// CLASS HEADER
#include "pan-gesture-detector.h"

// EXTERNAL INCLUDES
#include <dali/integration-api/events/gesture-requests.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/integration-api/core.h>

// INTERNAL INCLUDES

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

PanGestureDetector::PanGestureDetector(Integration::Core& core, Vector2 screenSize, const Integration::PanGestureRequest& request)
: PanGestureDetectorBase(screenSize, request),
  mCore(core)
{
}

PanGestureDetector::~PanGestureDetector()
{
}

void PanGestureDetector::EmitPan(const Integration::PanGestureEvent gesture)
{
  mCore.SendEvent(gesture);
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
