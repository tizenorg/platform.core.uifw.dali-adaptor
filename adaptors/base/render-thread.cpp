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

// CLASS HEADER
#include "render-thread.h"

// EXTERNAL INCLUDES
#include <cstdlib>
#include <vector>
#include <sys/time.h>
#include <unistd.h>
#include <sys/prctl.h>

#include <dali/integration-api/debug.h>
#include <base/interfaces/adaptor-internal-services.h>
#include <base/update-render-synchronization.h>
#include <base/log-options.h>
#include <dali/integration-api/gl-abstraction.h>

// INTERNAL INCLUDES
#include <internal/common/render-surface-impl.h>

#include <base/interfaces/egl-factory-interface.h>
#include <base/update-render-synchronization.h>
#include "android-jobs.h"
#include <nativeLogging.h>


namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace
{

const unsigned int TIME_PER_FRAME_IN_MICROSECONDS = 16667;


class RequestReloadJob : public Jobs::Job
{
public:
  RequestReloadJob(Dali::Integration::Core& core) : mCore(core)
  {
  }

  void Execute(void* context)
  {
    mCore.RequestReloadResources();
  }

protected:
  Dali::Integration::Core& mCore;
};

} // unnamed namespace

RenderThread::RenderThread( UpdateRenderSynchronization& sync,
                            AdaptorInternalServices& adaptorInterfaces,
                            const LogOptions& logOptions )
: mUpdateRenderSync( sync ),
  mCore( adaptorInterfaces.GetCore() ),
  mGLES( adaptorInterfaces.GetGlesInterface() ),
  mEglFactory( &adaptorInterfaces.GetEGLFactoryInterface() ),
  mEGL( NULL ),
  mThread( NULL ),
  mUsingPixmap( false ),
  mLogOptions (logOptions),
  mNewDataAvailable( false ),
  mPixmapSyncReceived( false ),
  mPixmapSyncRunning( false ),
  mCurrentMicroSec(0),
  mSurfaceReplaceCompleted( false )
{
  // set the initial values before render thread starts
  mCurrent.surface = adaptorInterfaces.GetRenderSurfaceInterface();

  // set the pixmap flag, if we're using one
  if( (mCurrent.surface->GetType() == Dali::RenderSurface::PIXMAP ) ||
      (mCurrent.surface->GetType() == Dali::RenderSurface::NATIVE_BUFFER ) )
  {
    mUsingPixmap = true;
  }
}

RenderThread::~RenderThread()
{
  DALI_ASSERT_ALWAYS( mThread == NULL && "RenderThread is still alive");
  mEglFactory->Destroy();
}

void RenderThread::Start()
{
  // initialise GL and kick off render thread
  DALI_ASSERT_ALWAYS( !mEGL && "Egl already initialized" );

  // Tell core what the minimum frame interval is
  mCore.SetMinimumFrameTimeInterval( mCurrent.syncMode * TIME_PER_FRAME_IN_MICROSECONDS );

  // create the render thread, initially we are rendering
  mThread = new boost::thread(boost::bind(&RenderThread::Run, this));

  // Inform render thread to block waiting for PixmapSync
  if( mUsingPixmap )
  {
    boost::unique_lock< boost::mutex > lock( mPixmapSyncMutex );
    mPixmapSyncRunning = true;
  }
 }

void RenderThread::Stop()
{
  ACORE_PROLOG;
  // shutdown the render thread and destroy the opengl context
  if( mThread )
  {
    // Inform render thread not to block waiting for PixmapSync
    if( mUsingPixmap )
    {
      boost::unique_lock< boost::mutex > lock( mPixmapSyncMutex );
      mPixmapSyncRunning = false;
    }

    mCurrent.surface->StopRender();

    // need to simulate a RenderSync to get render-thread to finish
    RenderSync();

     // Request one update
    mUpdateRenderSync.UpdateRequested();

    //mUpdateRenderSync.SetRunning(false);
    mUpdateRenderSync.SetRenderRunning(false);

#ifdef ACORE_DEBUG_ENABLED
    LOGD("Stopping renderthread\n");
#endif

    // wait for the thread to finish
    mThread->join();

#ifdef ACORE_DEBUG_ENABLED
    LOGD("Renderthread terminated.\n");
#endif

    delete mThread;
    mThread = NULL;
  }
}

void RenderThread::ReplaceSurface( RenderSurface* surface )
{
  // Make sure it's a new surface. Note! we are reading the current value of render thread here, but reading is ok
  DALI_ASSERT_ALWAYS( surface != mCurrent.surface && "Trying to replace surface with itself" );

  // lock and set to false
  {
    boost::unique_lock<boost::mutex> lock( mSurfaceChangedMutex );
    mSurfaceReplaceCompleted = false;
  }

  // lock cache and set update flag at the end of function
  {
    SendMessageGuard msg( *this );
    // set new values to cache
    mNewValues.replaceSurface = true;
    mNewValues.surface = surface;
  }

  /*
   * Reset the mPixmapFlushed condition if surface was changed.
   * : in this case, client can not handle the previous damage because surface was changed.
   */
  RenderSync();
}

void RenderThread::WaitForSurfaceReplaceComplete()
{
  boost::unique_lock<boost::mutex> lock( mSurfaceChangedMutex );

  // if already completed no need to wait
  while( !mSurfaceReplaceCompleted )
  {
    mSurfaceChangedNotify.wait( lock ); // Block the main thread here and releases mSurfaceChangedMutex so the render-thread can notify us
  }
}

void RenderThread::SetVSyncMode( EglInterface::SyncMode syncMode )
{
  // lock cache and set update flag at the end of function
  SendMessageGuard msg( *this );
  // set new values to cache
  mNewValues.syncMode = syncMode;
}

void RenderThread::RenderSync()
{
  if( !mUsingPixmap )
  {
    return;
  }
  {
    boost::unique_lock< boost::mutex > lock( mPixmapSyncMutex );
    mPixmapSyncReceived = true;
  }

  // awake render thread if it was waiting for the notify
  mPixmapSyncNotify.notify_all();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The following methods are all executed inside render thread !!!
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool RenderThread::Run()
{
  prctl(PR_SET_NAME, "render_thread");
  nice(-18);
  // install a function for logging
  mLogOptions.InstallLogFunction();

  InitializeEgl();

  bool running( true );

  // Wait for first update
  mUpdateRenderSync.RenderSyncWithUpdate();

  Dali::Integration::RenderStatus renderStatus;

  uint64_t currentTime( 0 );

  // render loop, we stay inside here when rendering
  while( running )
  {
    // Consume any pending events
    ConsumeEvents();

    // Check if we've got updates from the main thread
    CheckForUpdates();

    // perform any pre-render operations
    if(PreRender() == true)
    {
      uint32_t xy = static_cast<uint32_t>(currentTime);
      // Render
      // Trying to prevent the renderer from rendering when running is set to false - PG
      if ( mUpdateRenderSync.GetRenderRunning() )
      {
        mCore.Render( renderStatus );

        // Notify the update-thread that a render has completed
        mUpdateRenderSync.RenderFinished( renderStatus.NeedsUpdate() );

        uint64_t newTime( mUpdateRenderSync.GetTimeMicroseconds() );

        // perform any post-render operations
        if ( renderStatus.HasRendered() )
        {
          PostRender( static_cast< unsigned int >(newTime - currentTime) );
        }

        currentTime = newTime;
      }
    }

    // Wait until another frame has been updated
    running = mUpdateRenderSync.RenderSyncWithUpdate();
  }
  mUpdateRenderSync.SetRenderRunning(false);
  // shut down egl
  ShutdownEgl();
  DALI_LOG_INFO(Debug::Filter::gShader, Debug::General, "Renderthread terminated\n");
  // install a function for logging
  mEnvironmentOptions.UnInstallLogFunction();

  return true;
}

void RenderThread::InitializeEgl()
{
  mEGL = mEglFactory->Create();

  DALI_ASSERT_ALWAYS( mCurrent.surface && "NULL surface" );

  // initialize egl & OpenGL
  mCurrent.surface->InitializeEgl( *mEGL );

  // create the OpenGL context
  mEGL->CreateContext();

  // create the OpenGL surface
  mCurrent.surface->CreateEglSurface( *mEGL );

  // Make it current
  mEGL->MakeContextCurrent();

  if( !mUsingPixmap )
  {
    // set the initial sync mode
    mEGL->SetRefreshSync( mCurrent.syncMode );
  }

  // tell core it has a context
  mCore.ContextCreated();

  Jobs::JobHandle requestJob = new RequestReloadJob(mCore);
  Jobs::add_job(Jobs::get_main_queue(), requestJob);

  DALI_LOG_INFO(Debug::Filter::gShader, Debug::General, "*** GL_VENDOR : %s ***\n", mGLES.GetString(GL_VENDOR));
  DALI_LOG_INFO(Debug::Filter::gShader, Debug::General, "*** GL_RENDERER : %s ***\n", mGLES.GetString(GL_RENDERER));
  DALI_LOG_INFO(Debug::Filter::gShader, Debug::General, "*** GL_VERSION : %s ***\n", mGLES.GetString(GL_VERSION));
  DALI_LOG_INFO(Debug::Filter::gShader, Debug::General, "*** GL_SHADING_LANGUAGE_VERSION : %s***\n", mGLES.GetString(GL_SHADING_LANGUAGE_VERSION));
  DALI_LOG_INFO(Debug::Filter::gShader, Debug::General, "*** Supported Extensions ***\n%s\n\n", mGLES.GetString(GL_EXTENSIONS));
}

void RenderThread::ConsumeEvents()
{
  // tell surface to consume any events to avoid memory leaks
  mCurrent.surface->ConsumeEvents();
}

void RenderThread::CheckForUpdates()
{
  // atomic check to see if we've got updates, resets the flag int
  if( __sync_fetch_and_and( &mNewDataAvailable, 0 ) )
  {
    // scope for lock
    // NOTE! This block is the only place in render side where mNewValues can be used inside render thread !!!
    {
      // need to lock to access new values
      boost::unique_lock< boost::mutex > lock( mThreadDataLock );

      // did the sync mode change
      if( mCurrent.syncMode != mNewValues.syncMode )
      {
        mCurrent.syncMode = mNewValues.syncMode;
        mEGL->SetRefreshSync( mCurrent.syncMode );
      }

      // check if the surface needs replacing
      if( mNewValues.replaceSurface )
      {
        mNewValues.replaceSurface = false; // reset the flag
        // change the surface
        ChangeSurface( mNewValues.surface );
        mNewValues.surface = NULL;
      }
    }
  }
}

void RenderThread::ChangeSurface( RenderSurface* newSurface )
{
  // This is designed for replacing pixmap surfaces, but should work for window as well
  // we need to delete the egl surface and renderable (pixmap / window)
  // Then create a new pixmap/window and new egl surface
  // If the new surface has a different display connection, then the context will be lost
  DALI_ASSERT_ALWAYS( newSurface && "NULL surface" )
  bool contextLost = newSurface->ReplaceEGLSurface( *mEGL );

  // if( contextLost )
  {
    DALI_LOG_WARNING("Context lost\n");
    mCore.ContextToBeDestroyed();
    mCore.ContextCreated();
  }

  // if both new and old surface are using the same display, and the display
  // connection was created by Dali, then transfer
  // display owner ship to the new surface.
  mCurrent.surface->TransferDisplayOwner( *newSurface );

  // use the new surface from now on
  mCurrent.surface = newSurface;

  // Notify main thread that surface was changed so it can release the old one
  NotifySurfaceChangeCompleted();
}

void RenderThread::NotifySurfaceChangeCompleted()
{
  {
    boost::unique_lock< boost::mutex > lock( mSurfaceChangedMutex );
    mSurfaceReplaceCompleted = true;
  }
  // notify main thread
  mSurfaceChangedNotify.notify_all();
}

void RenderThread::ShutdownEgl()
{
  // inform core the context is about to be destroyed,
  mCore.ContextToBeDestroyed();

  //  delete the GL context / egl surface
  mEGL->TerminateGles();
  mEGL = NULL; // needed for restart
}

bool RenderThread::PreRender()
{
  return mCurrent.surface->PreRender( *mEGL, mGLES );
}

void RenderThread::PostRender( unsigned int timeDelta )
{
  const bool waitForSync = mCurrent.surface->PostRender( *mEGL, mGLES, timeDelta );

  if( waitForSync )
  {
    boost::unique_lock< boost::mutex > lock( mPixmapSyncMutex );

    // wait until synced,
    // this blocks the thread here and releases the mPixmapSyncMutex (so the main thread can use it)
    if( mPixmapSyncRunning && !mPixmapSyncReceived )
    {
      mPixmapSyncNotify.wait( lock );
    }
    mPixmapSyncReceived = false;
  }
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
