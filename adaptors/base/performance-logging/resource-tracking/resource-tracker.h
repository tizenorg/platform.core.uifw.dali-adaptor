#ifndef __DALI_INTERNAL_ADAPTOR_RESOURCE_TRACKER_H__
#define __DALI_INTERNAL_ADAPTOR_RESOURCE_TRACKER_H__

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

#include <dlog.h>

// INTERNAL INCLUDES

#include <base/performance-logging/resource-tracking/resource-tracking-manager.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

/**
 * @brief Logging function to output tracked resources
 *
 * @param rtm pointer to the Resource Tracking Manager
 */
void ResourceLogger();

} // namespace Internal

} // namespace Adaptor

} // namespace Dali

#endif
