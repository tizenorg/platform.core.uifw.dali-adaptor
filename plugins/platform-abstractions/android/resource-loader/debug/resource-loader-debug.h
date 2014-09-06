#ifndef __DALI_SLP_PLATFORM_RESOURCE_LOADER_DEBUG_H__
#define __DALI_SLP_PLATFORM_RESOURCE_LOADER_DEBUG_H__

/*
Copyright (c) 2000-2013 Samsung Electronics Co., Ltd All Rights Reserved

This file is part of Dali

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

#include <dali/integration-api/debug.h>
#if defined(DEBUG_ENABLED)

#include <platform-abstractions/android/resource-loader/resource-loader.h>
#include <dali/integration-api/resource-cache.h>


namespace Dali
{
namespace SlpPlatform
{

extern Debug::Filter* gLoaderFilter;

std::string DebugRequestList(Integration::TextResourceType::CharacterList& chars);
std::string DebugResourceList(LoadedResource& partialResource);

} // SlpPlatform
} // Dali

#endif // defined(DEBUG_ENABLED)
#endif //__DALI_SLP_PLATFORM_RESOURCE_LOADER_DEBUG_H__
