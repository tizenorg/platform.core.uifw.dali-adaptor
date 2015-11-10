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

// HEADER
#include "base/time-service.h"

// EXTERNAL INCLUDES
#include <ctime>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace TimeService
{

namespace
{
const uint64_t NANOSECONDS_PER_SECOND = 1e+9;
}

void GetNanoseconds( uint64_t& time )
{
  timespec timeSpec;
  clock_gettime( CLOCK_MONOTONIC, &timeSpec );
  time = ( timeSpec.tv_sec * NANOSECONDS_PER_SECOND ) + timeSpec.tv_nsec;
}

} // namespace TimeService

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
