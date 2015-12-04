
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
: mVSync( NULL ),
  mUseHardwareVSync( 0 ),
  mHardwareVSyncAvailable( 0 )
{
}

VSyncMonitor::~VSyncMonitor()
{
  Terminate();
}

void VSyncMonitor::SetUseHardwareVSync( bool useHardware )
{
  mUseHardwareVSync = useHardware;
}

void VSyncMonitor::SetHardwareVSyncAvailable( bool hardwareVSyncAvailable )
{
  mHardwareVSyncAvailable = hardwareVSyncAvailable;
}

void VSyncMonitor::Initialize()
{
  mVSync = new AndroidVSync;
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
