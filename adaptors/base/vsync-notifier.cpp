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
#include <xf86drm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <boost/thread.hpp>

#include <dali/integration-api/core.h>
#include <dali/integration-api/platform-abstraction.h>

// INTERNAL INCLUDES
#include "vsync-notifier.h"
#include "vsync-monitor-interface.h"
#include <base/update-render-synchronization.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace
{

const unsigned int MICROSECONDS_PER_SECOND( 100000u );
const unsigned int TIME_PER_FRAME_IN_MICROSECONDS( 16667u );

#if defined(DEBUG_ENABLED)
Integration::Log::Filter* gLogFilter = Integration::Log::Filter::New(Debug::Concise, false, "LOG_VSYNC_NOTIFIER");
#endif

} // unnamed namespace

VSyncNotifier::VSyncNotifier( UpdateRenderSynchronization& sync,
                              Dali::Integration::Core& core,
                              Dali::Integration::PlatformAbstraction& platformAbstraction,
                              VSyncMonitorInterface* vSyncMonitor,
                              Dali::Integration::Log::LogFunction& logFunction,
                              int logOpts )
: mUpdateRenderSync( sync ),
  mCore( core ),
  mPlatformAbstraction( platformAbstraction ),
  mVSyncMonitor( vSyncMonitor ),
  mLogFunction( logFunction ),
  mLogOpts( logOpts ),
  mThread( NULL )
{
}

VSyncNotifier::~VSyncNotifier()
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "%s\n", __func__ );

  Stop();
}

void VSyncNotifier::Start()
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "%s\n", __func__ );

  if ( !mThread )
  {
    mVSyncMonitor->Initialize();

    mThread = new boost::thread( boost::bind( &VSyncNotifier::Run, this ) );
  }
}

void VSyncNotifier::Stop()
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "%s\n", __func__ );

  if( mThread )
  {
    // wait for the thread to finish
    mThread->join();

    delete mThread;
    mThread = NULL;
  }

  mVSyncMonitor->Terminate();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The following is executed inside the notifier thread !!!
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void VSyncNotifier::Run()
{
  if( mLogOpts & Debug::LogRenderThread )
  {
    // install a function for logging
    Dali::Integration::Log::InstallLogFunction( mLogFunction, mLogOpts );
  }

  unsigned int frameNumber( 0u );             // frameCount, updated when the thread is paused
  unsigned int currentSequenceNumber( 0u );   // platform specific vsync sequence number (increments with each vsync)
  unsigned int currentSeconds( 0u );          // timestamp at latest vsync
  unsigned int currentMicroseconds( 0u );     // timestamp at latest vsync
  unsigned int seconds( 0u );
  unsigned int microseconds( 0u );

  bool validSync( false );
  bool running( true );
  while( running )
  {
    validSync = false;

    // Hardware VSyncs available?
    if( mVSyncMonitor->UseHardware() )
    {
      // Yes..wait for hardware VSync
      validSync = mVSyncMonitor->DoSync( currentSequenceNumber, currentSeconds, currentMicroseconds );
    }
    else
    {
      // No..use software timer
      mPlatformAbstraction.GetTimeMicroseconds( seconds, microseconds );

      unsigned int timeDelta( MICROSECONDS_PER_SECOND * (seconds - currentSeconds) );
      if( microseconds < currentMicroseconds)
      {
        timeDelta += (microseconds + MICROSECONDS_PER_SECOND) - currentMicroseconds;
      }
      else
      {
        timeDelta += microseconds - currentMicroseconds;
      }

      if( timeDelta < TIME_PER_FRAME_IN_MICROSECONDS )
      {
          usleep( TIME_PER_FRAME_IN_MICROSECONDS - timeDelta );
      }
      else
      {
        usleep( TIME_PER_FRAME_IN_MICROSECONDS );
      }

      validSync = true;
    }

    if( validSync )
    {
      // call Core::VSync with frame number and time stamp
      mCore.VSync( ++frameNumber, currentSeconds, currentMicroseconds );
    }

    running = mUpdateRenderSync.VSyncNotifierSyncWithUpdateAndRender( frameNumber, currentSeconds, currentMicroseconds );
  }

  if( mLogOpts & Debug::LogRenderThread )
  {
    // uninstall a function for logging
    Dali::Integration::Log::UninstallLogFunction();
  }
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
