#ifndef __DALI_INTERNAL_X_INPUT2_DEVICE_H__
#define __DALI_INTERNAL_X_INPUT2_DEVICE_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <X11/extensions/XInput2.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

/**
 *
 * @brief struct used to encpasulate use XIDeviceInfo information.
 * Kept as a POD so it can be used in a Dali::Vector
 */
struct XInput2Device
{
  XInput2Device()
  : deviceId(0),
  attachment(0),
  use(0),
  keyClass(false),
  touchClass(false),
  buttonClass(false),
  valuatorClass(false), // e.g. mouse axis, tablet pen tilt angle..
  scrollClass(false)
  {}

  void AssignDeviceInfo( const XIDeviceInfo* device );

  int deviceId;
  int attachment;
  int use;
  bool keyClass:1;
  bool touchClass:1;
  bool buttonClass:1;
  bool valuatorClass:1; // e.g. mouse axis, tablet pen tilt angle..
  bool scrollClass:1;

};


} // namespace Adaptor
} // namespace Internal
} // namespace Dali

#endif
