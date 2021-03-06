/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#ifndef DALI_PROFILE_UBUNTU
#include <system_settings.h>
#endif // DALI_PROFILE_UBUNTU
#include <Elementary.h>

// INTERNAL INCLUDES
#include "system-settings.h"

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

int GetElmAccessActionOver()
{
#ifndef DALI_PROFILE_UBUNTU
  // ELM_ACCESS_ACTION_OVER not available in common profile
  return ELM_ACCESS_ACTION_LAST;
#else // DALI_PROFILE_UBUNTU
  return 0;
#endif // DALI_PROFILE_UBUNTU
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
