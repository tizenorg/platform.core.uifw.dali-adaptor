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

// EXTERNAL INCLUDES
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <dali/integration-api/debug.h>

    // INTERNAL INCLUDES
#include "vsync-monitor.h"
#include "vsync/vsyncDll.h"

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

VSyncMonitor::VSyncMonitor()
: mVSync( NULL )
{
}

VSyncMonitor::~VSyncMonitor()
{
  Terminate();
}

bool VSyncMonitor::Initialize()
{
  mVSync = new AndroidVSync;
  return true;
}

void VSyncMonitor::Terminate()
{
  delete mVSync;
  mVSync = NULL;
}

/**
 * copydoc Dali::Internal::Adaptor::VSyncMonitorInterface::UseHardware
 */
bool VSyncMonitor::UseHardware() {
  return true;
}

bool VSyncMonitor::DoSync( unsigned int& frameNumber, unsigned int& seconds, unsigned int& microseconds )
{
  bool res = mVSync->DoSyncAndroid();
  frameNumber = mVSync->GetFrameNumber();
  seconds = mVSync->GetSeconds();
  microseconds = mVSync->GetMicroseconds();
  return res;
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
