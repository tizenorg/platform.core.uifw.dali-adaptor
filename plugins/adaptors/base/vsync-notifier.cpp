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
// #include <xf86drm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <boost/thread.hpp>

#include <dali/integration-api/core.h>
#include <dali/integration-api/platform-abstraction.h>

// INTERNAL INCLUDES
#include "vsync-notifier.h"
#include <base/interfaces/adaptor-internal-services.h>
#include <base/interfaces/vsync-monitor-interface.h>
#include <base/update-render-synchronization.h>
#include <base/log-options.h>
#include <sys/prctl.h>

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
  mLogOptions.InstallLogFunction();


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
