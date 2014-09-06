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

// CLASS HEADER
#include <dali/public-api/adaptor-framework/common/device-layout.h>

namespace Dali
{

const DeviceLayout DeviceLayout::DEFAULT_BASE_LAYOUT
(
  Vector2(720.0f, 1280.0f), // The resolution of the screen
  4.65f,                    // The screen size
  316.0f,                   // The DPI
  30.0f                     // The Viewing Distance
);

DeviceLayout::DeviceLayout()
: screenSize(0.0f),
  dpi(0.0f),
  viewingDistance(0.0f)
{
}

DeviceLayout::DeviceLayout(Vector2 resolution, float screenSize, float dpi, float viewingDistance)
: resolution(resolution),
  screenSize(screenSize),
  dpi(dpi),
  viewingDistance(viewingDistance)
{
}

DeviceLayout::~DeviceLayout()
{
}

} // namespace Dali
