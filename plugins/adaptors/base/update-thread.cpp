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

// CLASS HEADER
#include "update-thread.h"

// EXTERNAL INCLUDES
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <boost/thread.hpp>
#include <sys/prctl.h>

#include <dali/integration-api/debug.h>
#include <base/interfaces/adaptor-internal-services.h>
#include <base/update-render-synchronization.h>
#include <base/log-options.h>

#ifdef ACORE_DEBUG_ENABLED
#include "nativeLogging.h"
#endif

// INTERNAL INCLUDES
#include "internal/common/trigger-event.h"

#include <base/update-render-synchronization.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace
{

const unsigned int MICROSECONDS_PER_MILLISECOND( 1000 );

} // unnamed namespace

UpdateThread::UpdateThread(  UpdateRenderSynchronization& sync,
                            AdaptorInternalServices& adaptorInterfaces,
                            const LogOptions& logOptions )
: mUpdateRenderSync( sync ),
  mCore( adaptorInterfaces.GetCore() ),
  mFpsTrackingSeconds( logOptions.GetFrameRateLoggingFrequency() ),
  mElapsedTime( 0.0f ),
  mElapsedSeconds( 0u ),
  mStatusLogInterval( logOptions.GetUpdateStatusLoggingFrequency() ),
  mStatusLogCount( 0u ),
  mNotificationTrigger( adaptorInterfaces.GetTriggerEventInterface() ),
  mThread( NULL ),
  mLogOptions( logOptions )
{
  if( mFpsTrackingSeconds > 0 )
  {
    mFpsRecord.resize( mFpsTrackingSeconds, 0.0f );
  }
}

UpdateThread::~UpdateThread()
{
  if(mFpsTrackingSeconds > 0)
  {
    OutputFPSRecord();
  }
  Stop();
}

void UpdateThread::Start()
{
  if ( !mThread )
  {
    // Create and run the update-thread
    mThread = new boost::thread( boost::bind( &UpdateThread::Run, this ) );
  }
}

void UpdateThread::Stop()
{
  if( mThread )
  {
    // wait for the thread to finish
    mThread->join();

    delete mThread;
    mThread = NULL;
  }
}

bool UpdateThread::Run()
{
  prctl(PR_SET_NAME, "update_thread");
  nice(-18);
#ifdef ACORE_DEBUG_ENABLED
  LOGI("UpdateThread starting up\n");
#endif

  Integration::UpdateStatus status;

  // install a function for logging
  mLogOptions.InstallLogFunction();

  bool running( true );

  // Update loop, we stay inside here while the update-thread is running
  while ( running )
  {
    // Inform synchronization object update is ready to run, this will pause update thread if required.
    mUpdateRenderSync.UpdateReadyToRun();

    // Do the update
    mCore.Update( status );

    if( mFpsTrackingSeconds > 0 )
    {
      FPSTracking(status.SecondsFromLastFrame());
    }

    // Do the notifications first so the actor-thread can start processing them
    if( status.NeedsNotification() )
    {
      // Tell the event-thread to wake up (if asleep) and send a notification event to Core
      mNotificationTrigger.Trigger();
    }

    bool renderNeedsUpdate;

    // tell the synchronisation class that a buffer has been written to,
    // and to wait until there is a free buffer to write to
    running = mUpdateRenderSync.UpdateSyncWithRender( renderNeedsUpdate );

    if( running )
    {
      unsigned int keepUpdatingStatus = status.KeepUpdating();

      // Optional logging of update/render status
      if ( mStatusLogInterval )
      {
        UpdateStatusLogging( keepUpdatingStatus, renderNeedsUpdate );
      }

      //  2 things can keep update running.
      // - The status of the last update
      // - The status of the last render
      bool runUpdate = (Integration::KeepUpdating::NOT_REQUESTED != keepUpdatingStatus) || renderNeedsUpdate;

      if( !runUpdate )
      {
        running = mUpdateRenderSync.UpdateTryToSleep();
      }
    }
  }

  // uninstall a function for logging
  mLogOptions.UnInstallLogFunction();
  
#ifdef ACORE_DEBUG_ENABLED
  LOGI("UpdateThread shutting down.\n");
#endif

  return true;
}

void UpdateThread::FPSTracking(float secondsFromLastFrame)
{
  if (mElapsedSeconds < mFpsTrackingSeconds)
  {
    mElapsedTime += secondsFromLastFrame;
    if( secondsFromLastFrame  > 1.0 )
    {
      int seconds = floor(mElapsedTime);
      mElapsedSeconds += seconds;
      mElapsedTime -= static_cast<float>(seconds);
    }
    else
    {
      if( mElapsedTime>=1.0f )
      {
        mElapsedTime -= 1.0f;
        mFpsRecord[mElapsedSeconds] += 1.0f - mElapsedTime/secondsFromLastFrame;
        mElapsedSeconds++;
        mFpsRecord[mElapsedSeconds] += mElapsedTime/secondsFromLastFrame;
      }
      else
      {
        mFpsRecord[mElapsedSeconds] += 1.0f;
      }
    }
  }
  else
  {
    OutputFPSRecord();
    mFpsRecord.clear();
    mFpsTrackingSeconds = 0;
  }
}

void UpdateThread::OutputFPSRecord()
{
  for(unsigned int i = 0; i < mElapsedSeconds; i++)
  {
    DALI_LOG_FPS("fps( %d ):%f\n",i ,mFpsRecord[i]);
  }
  std::ofstream outFile("/tmp/dalifps.txt");
  if(outFile.is_open())
  {
    for(unsigned int i = 0; i < mElapsedSeconds; i++)
    {
      outFile << mFpsRecord[i]<<std::endl;
    }
    outFile.close();
  }
}

void UpdateThread::UpdateStatusLogging( unsigned int keepUpdatingStatus, bool renderNeedsUpdate )
{
  DALI_ASSERT_ALWAYS( mStatusLogInterval );

  std::ostringstream oss;

  if ( !(++mStatusLogCount % mStatusLogInterval) )
  {
    oss << "UpdateStatusLogging keepUpdating: " << (bool)keepUpdatingStatus << " ";

    if ( keepUpdatingStatus )
    {
      oss << "because: ";
    }

    if ( keepUpdatingStatus & Integration::KeepUpdating::STAGE_KEEP_RENDERING )
    {
      oss << "<Stage::KeepRendering() used> ";
    }

    if ( keepUpdatingStatus & Integration::KeepUpdating::INCOMING_MESSAGES )
    {
      oss << "<Messages sent to Update> ";
    }

    if ( keepUpdatingStatus & Integration::KeepUpdating::ANIMATIONS_RUNNING )
    {
      oss << "<Animations running> ";
    }

    if ( keepUpdatingStatus & Integration::KeepUpdating::DYNAMICS_CHANGED )
    {
      oss << "<Dynamics running> ";
    }

    if ( keepUpdatingStatus & Integration::KeepUpdating::LOADING_RESOURCES )
    {
      oss << "<Resources loading> ";
    }

    if ( keepUpdatingStatus & Integration::KeepUpdating::NOTIFICATIONS_PENDING )
    {
      oss << "<Notifications pending> ";
    }

    if ( renderNeedsUpdate )
    {
      oss << "<Render needs Update> ";
    }

    DALI_LOG_UPDATE_STATUS( "%s\n", oss.str().c_str() );
  }
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
