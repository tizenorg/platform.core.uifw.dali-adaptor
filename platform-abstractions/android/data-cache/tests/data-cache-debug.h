#ifndef __DALI_SLP_PLATFORM_DATA_CACHE_DEBUG_H__
#define __DALI_SLP_PLATFORM_DATA_CACHE_DEBUG_H__

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

//#define DATA_CACHE_DEBUG // un-comment to enable
#ifdef DATA_CACHE_DEBUG

namespace Dali
{
namespace SlpPlatform
{
namespace DataCacheIo
{

/**
 * Threading stress test.
 * Launches a 8 of threads to read / write to the simultaneously to a single data cache.
 * Each thread reads/writes a number of entries, using random keys and random data.
 * When it has completed, a concise check of all data is made to detect any errors.
 *
 */
void ThreadedStressTest();

} // namespace DataCacheIO

} // namespace SlpPlatform

} // namespace Dali

#endif // __DALI_SLP_PLATFORM_DATA_CACHE_DEBUG_H__
#endif // #ifdef DATA_CACHE_DEBUG
