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
 *
 */

// CLASS HEADER
#include "update-render-thread.h"

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <base/interfaces/adaptor-internal-services.h>
#include <base/basic-thread-synchronization.h>
#include <base/environment-options.h>
#include <base/display-connection.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace
{
#if defined(DEBUG_ENABLED)
Integration::Log::Filter* gRenderLogFilter = Integration::Log::Filter::New(Debug::NoLogging, false, "LOG_RENDER_THREAD");
#endif
}

UpdateRenderThread::UpdateRenderThread( BasicThreadSynchronization& sync,
                                        AdaptorInternalServices& adaptorInterfaces,
                                        const EnvironmentOptions& environmentOptions )
: mThreadSynchronization( sync ),
  mCore( adaptorInterfaces.GetCore() ),
  mFpsTracker( environmentOptions ),
  mUpdateStatusLogger( environmentOptions ),
  mGLES( adaptorInterfaces.GetGlesInterface() ),
  mEglFactory( &adaptorInterfaces.GetEGLFactoryInterface()),
  mEGL( NULL ),
  mThread( NULL ),
  mEnvironmentOptions( environmentOptions ),
  mSurfaceReplaced(false)
{
  // set the initial values before render thread starts
  mSurface = adaptorInterfaces.GetRenderSurfaceInterface();

  mDisplayConnection = Dali::DisplayConnection::New();
}

UpdateRenderThread::~UpdateRenderThread()
{
  Stop();

  if (mDisplayConnection)
  {
    delete mDisplayConnection;
    mDisplayConnection = NULL;
  }

  DALI_ASSERT_ALWAYS( mThread == NULL && "UpdateRenderThread is still alive");
  mEglFactory->Destroy();
}

void UpdateRenderThread::Start()
{
  DALI_LOG_INFO( gRenderLogFilter, Debug::Verbose, "UpdateRenderThread::Start()\n");

  // initialise GL and kick off render thread
  DALI_ASSERT_ALWAYS( !mEGL && "Egl already initialized" );

  // create the update-render thread, initially we are rendering
  DALI_ASSERT_ALWAYS( !mThread );

  mThread = new pthread_t();
  int error = pthread_create( mThread, NULL, InternalThreadEntryFunc, this );
  DALI_ASSERT_ALWAYS( !error && "Return code from pthread_create() in UpdateRenderThread" );

  if( mSurface )
  {
    mSurface->StartRender();
  }
}

void UpdateRenderThread::Stop()
{
  DALI_LOG_INFO( gRenderLogFilter, Debug::Verbose, "UpdateRenderThread::Stop()\n");

  if( mSurface )
  {
    // Tell surface we have stopped rendering
    mSurface->StopRender();

    // The surface will be destroyed soon; this pointer will become invalid
    mSurface = NULL;
  }

  // shutdown the render thread and destroy the opengl context
  if( mThread )
  {
    // wait for the thread to finish
    pthread_join(*mThread, NULL);

    delete mThread;
    mThread = NULL;
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The following methods are all executed inside render thread !!!
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool UpdateRenderThread::Run()
{
  DALI_LOG_INFO( gRenderLogFilter, Debug::Verbose, "UpdateRenderThread::Run\n");

  // Install a function for logging
  mEnvironmentOptions.InstallLogFunction();

  InitializeEgl();

  Integration::UpdateStatus status;
  bool runUpdate = true;
  float lastFrameDelta( 0.0f );
  unsigned int lastSyncTime( 0 );
  unsigned int nextSyncTime( 0 );

  Dali::Integration::RenderStatus renderStatus;
  RenderRequest* request = NULL;

  // Render loop, we stay inside here when rendering
  while( mThreadSynchronization.UpdateRenderReady( status.NeedsNotification(), runUpdate, lastFrameDelta, lastSyncTime, nextSyncTime, request ) )
  {
    DALI_LOG_INFO( gRenderLogFilter, Debug::Verbose, "UpdateRenderThread::Run. 1 - UpdateReady(delta:%f, lastSync:%u, nextSync:%u)\n", lastFrameDelta, lastSyncTime, nextSyncTime);

    DALI_LOG_INFO( gRenderLogFilter, Debug::Verbose, "UpdateRenderThread::Run. 2 - Core.Update()\n");

    mThreadSynchronization.AddPerformanceMarker( PerformanceInterface::UPDATE_START );
    mCore.Update( lastFrameDelta, lastSyncTime, nextSyncTime, status );
    mThreadSynchronization.AddPerformanceMarker( PerformanceInterface::UPDATE_END );

    mFpsTracker.Track( status.SecondsFromLastFrame() );

    unsigned int keepUpdatingStatus = status.KeepUpdating();

    // Optional logging of update/render status
    mUpdateStatusLogger.Log( keepUpdatingStatus );

    //  2 things can keep update running.
    // - The status of the last update
    // - The status of the last render
    runUpdate = (Integration::KeepUpdating::NOT_REQUESTED != keepUpdatingStatus);

    DALI_LOG_INFO( gRenderLogFilter, Debug::Verbose, "UpdateRenderThread::Run. 3 - runUpdate(%d)\n", runUpdate );

    // Consume any pending events to avoid memory leaks
    DALI_LOG_INFO( gRenderLogFilter, Debug::Verbose, "UpdateRenderThread::Run. 4 - ConsumeEvents\n");
    mDisplayConnection->ConsumeEvents();

    // Check if we've got a request from the main thread (e.g. replace surface)
    if( request )
    {
      DALI_LOG_INFO( gRenderLogFilter, Debug::Verbose, "UpdateRenderThread::Run. 5 - Process requests\n");
      ProcessRequest( request );
    }

    if( PreRender() ) // Returns false if no surface onto which to render
    {
      // Render
      DALI_LOG_INFO( gRenderLogFilter, Debug::Verbose, "UpdateRenderThread::Run. 6 - Core.Render()\n");

      mThreadSynchronization.AddPerformanceMarker( PerformanceInterface::RENDER_START );
      mCore.Render( renderStatus );
      mThreadSynchronization.AddPerformanceMarker( PerformanceInterface::RENDER_END );

      // Perform any post-render operations
      if ( renderStatus.HasRendered() )
      {
        DALI_LOG_INFO( gRenderLogFilter, Debug::Verbose, "UpdateRenderThread::Run. 7 - PostRender()\n");
        PostRender();
      }
    }

    // Reset time variables
    lastFrameDelta = 0.0f;
    lastSyncTime = 0;
    nextSyncTime = 0;

    request = NULL; // Clear the request if it was set, no need to release memory
  }

  // Shut down EGL
  ShutdownEgl();

  // Uninstall the logging function
  mEnvironmentOptions.UnInstallLogFunction();

  return true;
}

void UpdateRenderThread::InitializeEgl()
{
  mEGL = mEglFactory->Create();

  DALI_ASSERT_ALWAYS( mSurface && "NULL surface" );

  // initialize egl & OpenGL
  mDisplayConnection->InitializeEgl( *mEGL );
  mSurface->InitializeEgl( *mEGL );

  // create the OpenGL context
  mEGL->CreateContext();

  // create the OpenGL surface
  mSurface->CreateEglSurface(*mEGL);

  // Make it current
  mEGL->MakeContextCurrent();

  // set the initial sync mode

  // tell core it has a context
  mCore.ContextCreated();

}

void UpdateRenderThread::ProcessRequest( RenderRequest* request )
{
  if( request != NULL )
  {
    switch(request->GetType())
    {
      case RenderRequest::REPLACE_SURFACE:
      {
        // change the surface
        ReplaceSurfaceRequest* replaceSurfaceRequest = static_cast<ReplaceSurfaceRequest*>(request);
        ReplaceSurface( replaceSurfaceRequest->GetSurface() );
        replaceSurfaceRequest->ReplaceCompleted();
        mThreadSynchronization.UpdateRenderInformSurfaceReplaced();
        break;
      }
    }
  }
}

void UpdateRenderThread::ReplaceSurface( RenderSurface* newSurface )
{
  // This is designed for replacing pixmap surfaces, but should work for window as well
  // we need to delete the egl surface and renderable (pixmap / window)
  // Then create a new pixmap/window and new egl surface
  // If the new surface has a different display connection, then the context will be lost
  DALI_ASSERT_ALWAYS(newSurface && "NULL surface");

  mDisplayConnection->InitializeEgl(*mEGL);

  newSurface->ReplaceEGLSurface(*mEGL);

  // use the new surface from now on
  mSurface = newSurface;
  mSurfaceReplaced = true;
}

void UpdateRenderThread::ShutdownEgl()
{
  // inform core of context destruction
  mCore.ContextDestroyed();

  if( mSurface )
  {
    // give a chance to destroy the OpenGL surface that created externally
    mSurface->DestroyEglSurface( *mEGL );
  }

  // delete the GL context / egl surface
  mEGL->TerminateGles();
}

bool UpdateRenderThread::PreRender()
{
  bool success( false );
  if( mSurface )
  {
    success = mSurface->PreRender( *mEGL, mGLES );
  }

  if( success )
  {
    mGLES.PreRender();
  }
  return success;
}

void UpdateRenderThread::PostRender()
{
  // Inform the gl implementation that rendering has finished before informing the surface
  mGLES.PostRender();

  if( mSurface )
  {
    // Inform the surface that rendering this frame has finished.
    mSurface->PostRender( *mEGL, mGLES, mDisplayConnection, mSurfaceReplaced );
  }
  mSurfaceReplaced = false;
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
