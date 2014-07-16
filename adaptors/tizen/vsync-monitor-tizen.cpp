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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <vconf.h>
#include <vconf-keys.h>
#include <xf86drm.h>

#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include "vsync-monitor.h"

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace
{

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_VSYNC_MONITOR");
#endif

const char * const DRM_DEVICE( "/dev/dri/card0" );
const int FD_NONE( -1 );

void ScreenStatusChanged(keynode_t* node, void* data)
{
  VSyncMonitor* vsyncMonitor( static_cast< VSyncMonitor* >( data ) );

  int status = 0;
  vconf_get_int( VCONFKEY_PM_STATE, &status );

  // status values
  //  - VCONFKEY_PM_STATE_NORMAL : turn vsync on
  //  - VCONFKEY_PM_STATE_LCDDIM : turn vsync off
  //  - VCONFKEY_PM_STATE_LCDOFF : turn vsync off
  //  - VCONFKEY_PM_STATE_SLEEP : turn vsync off
  const bool screenOn( VCONFKEY_PM_STATE_NORMAL == status );

  vsyncMonitor->SetUseHardware( screenOn );

  DALI_LOG_INFO( gLogFilter, Debug::Concise, "%s, Screen %s.\n", __PRETTY_FUNCTION__, screenOn ? "On" : "Off" );
}

} // unnamed namespace

struct VSyncMonitor::Impl
{
  int       mFileDescriptor;  ///< DRM dev node file descriptor
  drmVBlank mVBlankInfo;
  bool      mUseHardware;     ///< Hardware VSyncs available flag

  Impl()
    : mFileDescriptor( FD_NONE ),
      mUseHardware( true )
  {}
};


VSyncMonitor::VSyncMonitor()
{
  mImpl = new VSyncMonitor::Impl();

  vconf_notify_key_changed( VCONFKEY_PM_STATE, ScreenStatusChanged, this );
}

VSyncMonitor::~VSyncMonitor()
{
  Terminate();

  vconf_ignore_key_changed( VCONFKEY_PM_STATE, ScreenStatusChanged );

  delete mImpl;
}

void VSyncMonitor::SetUseHardware( bool useHardware )
{
  mImpl->mUseHardware = useHardware;
}

void VSyncMonitor::Initialize()
{
  DALI_ASSERT_DEBUG( mImpl->mFileDescriptor == FD_NONE && "VSyncMonitor::Initialize() called twice" );

  // Read initial 'use hardware' status
  ScreenStatusChanged( NULL, this );

  // open /dev node
  mImpl->mFileDescriptor = open( DRM_DEVICE, O_RDWR );

  // setup vblank request - block and wait for next vblank
  mImpl->mVBlankInfo.request.type = DRM_VBLANK_NEXTONMISS;
  mImpl->mVBlankInfo.request.sequence = 0;
  mImpl->mVBlankInfo.request.signal = 0;

  // setup vblank reply - block and wait for next vblank
  mImpl->mVBlankInfo.reply.type = DRM_VBLANK_NEXTONMISS;
  mImpl->mVBlankInfo.reply.sequence = 0;
  mImpl->mVBlankInfo.reply.tval_sec = 0;
  mImpl->mVBlankInfo.reply.tval_usec = 0;
}

void VSyncMonitor::Terminate()
{
  if( mImpl->mFileDescriptor != FD_NONE )
  {
    close( mImpl->mFileDescriptor );
    mImpl->mFileDescriptor = FD_NONE;
  }
}

bool VSyncMonitor::UseHardware()
{
  return mImpl->mUseHardware && (FD_NONE != mImpl->mFileDescriptor );
}

bool VSyncMonitor::DoSync( unsigned int& frameNumber, unsigned int& seconds, unsigned int& microseconds )
{
  DALI_ASSERT_DEBUG( mImpl->mFileDescriptor != FD_NONE && "ECoreX::VSyncMonitor is not initialized" );

  if( 0 == drmWaitVBlank( mImpl->mFileDescriptor, &mImpl->mVBlankInfo ) )
  {
    frameNumber = mImpl->mVBlankInfo.reply.sequence;
    seconds = mImpl->mVBlankInfo.reply.tval_sec;
    microseconds = mImpl->mVBlankInfo.reply.tval_usec;

    return true;
  }

  return false;
}

// drmWaitVBlank is a wrapper around this? so to keep in common build profile (for wayland & tizen) we could just use ioctl?
//
// {
//     int ret;

//     do
//     {
//       ret = ioctl(mImpl->mFileDescriptor, DRM_IOCTL_WAIT_VBLANK, &mImpl->mVBlankInfo);
//       mImpl->mVBlankInfo->request.type &= ~DRM_VBLANK_RELATIVE;
//     } while (ret && errno == EINTR);

//     return ret;
// }


} // namespace Adaptor

} // namespace Internal

} // namespace Dali
