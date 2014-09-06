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
#include <xf86drm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <boost/thread.hpp>

#include <dali/integration-api/core.h>
#include <dali/integration-api/platform-abstraction.h>

// INTERNAL INCLUDES
#include "vsync-notifier.h"
#include <base/interfaces/adaptor-internal-services.h>
#include <base/update-render-synchronization.h>
#include <base/environment-options.h>

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
                              AdaptorInternalServices& adaptorInterfaces,
                              const LogOptions& logOptions )
: mUpdateRenderSync( sync ),
  mCore( adaptorInterfaces.GetCore() ),
  mPlatformAbstraction( adaptorInterfaces.GetPlatformAbstractionInterface() ),
  mVSyncMonitor( adaptorInterfaces.GetVSyncMonitorInterface() ),
  mThread( NULL ),
  mLogOptions( logOptions )
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
    if( mVSyncMonitor->Initialize() )
    {
      // Create and run the vsync monitoring thread
      mThread = new boost::thread( boost::bind( &VSyncNotifier::Run, this ) );
    }
    else
    {
      DALI_LOG_WARNING( "using fallback timed thread.\n");
      // Create and run fallback thread timed to give vsyncs at 60fps
      mThread = new boost::thread( boost::bind( &VSyncNotifier::RunTimed, this ) );
    }
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
  prctl(PR_SET_NAME, "vsync_thread");
  nice(-19);
  // install a function for logging
  mEnvironmentOptions.InstallLogFunction();

  unsigned int frameNumber( 0u );             // frameCount, updated when the thread is paused
  unsigned int currentSequenceNumber( 0u );   // platform specific vsync sequence number (increments with each vsync)
  unsigned int currentSeconds( 0u );          // timestamp at latest vsync
  unsigned int currentMicroseconds( 0u );     // timestamp at latest vsync

  bool running( true );
  while( running )
  {
    if( mVSyncMonitor->DoSync( currentSequenceNumber, currentSeconds, currentMicroseconds ) )
    {
      // call Core::VSync with frame number and time stamp
      mCore.VSync( ++frameNumber, currentSeconds, currentMicroseconds );
    }

    running = mUpdateRenderSync.VSyncNotifierSyncWithUpdateAndRender( frameNumber, currentSeconds, currentMicroseconds );
  }
  
   // uninstall a function for logging
  mLogOptions.UnInstallLogFunction();
}

void VSyncNotifier::RunTimed()
{
  // install a function for logging
  mLogOptions.InstallLogFunction();


  unsigned int frameNumber( 0u );
  unsigned int currentSeconds( 0u );
  unsigned int currentMicroseconds( 0u );
  unsigned int seconds( 0u );
  unsigned int microseconds( 0u );
  bool running( true );

  while( running )
  {
    mPlatformAbstraction.GetTimeMicroseconds( currentSeconds, currentMicroseconds );

    // call Core::VSync with frame number and time stamp
    mCore.VSync( ++frameNumber, currentSeconds, currentMicroseconds );

    running = mUpdateRenderSync.VSyncNotifierSyncWithUpdateAndRender( frameNumber, currentSeconds, currentMicroseconds );

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
  }
  // uninstall a function for logging
  mLogOptions.UnInstallLogFunction();
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
