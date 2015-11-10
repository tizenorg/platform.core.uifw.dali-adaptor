#ifndef __DALI_INTERNAL_TIME_SERVICE_H__
#define __DALI_INTERNAL_TIME_SERVICE_H__

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

#include <stdint.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace TimeService
{

/**
 * @brief Get the monotonic time since some unspecified starting point (usually the boot time).
 *
 * @param[out]  nanoseconds  The time in nanoseconds since the reference point.
 *
 * @note The maximum value this can hold is 0xFFFFFFFFFFFFFFFF which is 1.844674407e+19. Therefore, this can overflow after approximately 584 years.
 */
void GetNanoseconds( uint64_t& time );

} // namespace TimeService

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_TIME_SERVICE_H__
