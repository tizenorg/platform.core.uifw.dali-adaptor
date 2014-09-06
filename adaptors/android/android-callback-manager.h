#ifndef __DALI_ANDROID_CALLBACK_MANAGER_H__
#define __DALI_ANDROID_CALLBACK_MANAGER_H__

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
                              const EnvironmentOptions& environmentOptions )
: mUpdateRenderSync( sync ),
  mCore( adaptorInterfaces.GetCore() ),
  mPlatformAbstraction( adaptorInterfaces.GetPlatformAbstractionInterface() ),
  mVSyncMonitor( adaptorInterfaces.GetVSyncMonitorInterface() ),
  mThread( NULL ),
  mEnvironmentOptions( environmentOptions )
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
  // install a function for logging
  mEnvironmentOptions.InstallLogFunction();

  unsigned int frameNumber( 0u );             // frameCount, updated when the thread is paused
  unsigned int currentSequenceNumber( 0u );   // platform specific vsync sequence number (increments with each vsync)
  unsigned int currentSeconds( 0u );          // timestamp at latest vsync
  unsigned int currentMicroseconds( 0u );     // timestamp at latest vsync
  unsigned int seconds( 0u );
  unsigned int microseconds( 0u );

  bool running( true );
  while( running )
  {
    bool validSync( true );

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
    }

    running = mUpdateRenderSync.VSyncNotifierSyncWithUpdateAndRender( validSync, ++frameNumber, currentSeconds, currentMicroseconds );
  }

  // uninstall a function for logging
  mEnvironmentOptions.UnInstallLogFunction();

}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali


// INTERNAL INCLUDES
#include "callback-manager.h"

// EXTERNAL INCLUDES
#include <boost/thread.hpp>
#include <list>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

struct CallbackData;

/**
 * Ecore interface to install call backs in the applications main loop.
 */
class AndroidCallbackManager : public CallbackManager
{

public:

     /**
     * constructor
     */
    AndroidCallbackManager();

    /**
     * destructor
     */
    ~AndroidCallbackManager()
    {
    }

    /**
     * @copydoc CallbackManager::AddCallback()
     */
    virtual bool AddCallback(Callback callback, Priority priority);

    /**
     * @copydoc CallbackManager::AddEventCallback()
     */
    virtual bool AddEventCallback(Callback callback, int type, EventControl control);

    /**
     * @copydoc CallbackManager::Start()
     */
    virtual void Start();

    /**
     * @copydoc CallbackManager::Stop()
     */
    virtual void Stop();

    /**
     * Remove all call backs
     * Always called from the main thread
     */
    void RemoveAllCallbacksFromMainThread();

private:

    /**
     * Deletes any expired callbacks in the callback container
     */
    void RefreshContainer();

    /**
     * Removes a single call back from the container
     * Always called from main thread
     * @param callbackData callback data
     */
    void RemoveCallbackFromContainer(CallbackData *callbackData);

    /**
     * Remove a standard call back from ecore
     * Always called from main thread
     * @param callbackData callback data
     */
    void RemoveStandardCallback(CallbackData *callbackData);

    /**
     * Remove an event handler from ecore
     * Always called from main thread
     * @param callbackData callback data
     */
    void RemoveEventCallback(CallbackData *callbackData);



    typedef std::list<CallbackData *>  CallbackList;

    bool                           mRunning;            ///< flag is set to true if when running
    CallbackList                   mCallbackContainer;  ///< container of live callbacks
    boost::mutex                   mMutex;              ///< protect access to shared data
};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_ECORE_CALLBACK_MANAGER_H__
