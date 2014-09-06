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
#include "update-render-controller.h"

// INTERNAL INCLUDES
#include <base/update-thread.h>
#include <base/render-thread.h>
#include <base/update-render-synchronization.h>
#include <base/vsync-notifier.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

UpdateRenderController::UpdateRenderController( AdaptorInternalServices& adaptorInterfaces,
                                                const LogOptions& logOptions )  : mUpdateThread( NULL ),
                                                                                  mRenderThread( NULL ),
                                                                                  mVSyncNotifier( NULL ),
                                                                                  mUpdateRenderSync( NULL )
{
  mUpdateRenderSync = new UpdateRenderSynchronization( adaptorInterfaces );

  mUpdateThread = new UpdateThread( *mUpdateRenderSync, adaptorInterfaces, logOptions );

  mRenderThread = new RenderThread( *mUpdateRenderSync, adaptorInterfaces, logOptions );

  mVSyncNotifier = new VSyncNotifier( *mUpdateRenderSync, adaptorInterfaces, logOptions );
}

UpdateRenderController::~UpdateRenderController()
{
  delete mVSyncNotifier;
  delete mRenderThread;
  delete mUpdateThread;
  delete mUpdateRenderSync;
}

void UpdateRenderController::Start()
{
  // Notify the synchronization object before starting the threads
  mUpdateRenderSync->Start();

  mUpdateThread->Start();
  mRenderThread->Start();
  mVSyncNotifier->Start();
}

void UpdateRenderController::Pause()
{
  mUpdateRenderSync->Pause();
  // if update thread is napping, wake it up to get it to pause in correct place
  mUpdateRenderSync->UpdateRequested();
}

void UpdateRenderController::Resume()
{
  mUpdateRenderSync->AltSetRunning(true);
  mUpdateRenderSync->Resume();
  mUpdateRenderSync->UpdateRequested();
}

void UpdateRenderController::Stop()
{
  // Notify the synchronization object before stopping the threads
  mUpdateRenderSync->Stop();

  mVSyncNotifier->Stop();
  mUpdateThread->Stop();
  mRenderThread->Stop();
}

void UpdateRenderController::RequestUpdate()
{
  mUpdateRenderSync->UpdateRequested();
}

void UpdateRenderController::RequestUpdateOnce()
{
  // we may be sleeping
  mUpdateRenderSync->UpdateRequested();
  // if we are paused, need to allow one update
  mUpdateRenderSync->UpdateWhilePaused();
}

void UpdateRenderController::ReplaceSurface( RenderSurface* surface )
{
  // tell render thread to start the replace
  mRenderThread->ReplaceSurface(surface);

  // make sure that a frame gets  processed and render thread runs at least once
  mUpdateRenderSync->UpdateRequested();

  // block here until replace complete
  mRenderThread->WaitForSurfaceReplaceComplete();
}

void UpdateRenderController::TerminateRenderThread()
{
  // Tell render thread to quit at the end of the rendering loop
  mUpdateRenderSync->SetRenderRunning(false);
  mRenderThread->Stop();
}

void UpdateRenderController::StartRenderThread()
{
  mUpdateRenderSync->SetRenderRunning(true);
  mRenderThread->Start();
}

void UpdateRenderController::RenderSync()
{
  mRenderThread->RenderSync();
}

void UpdateRenderController::DisableVSync()
{
  mRenderThread->SetVSyncMode( EglInterface::NO_SYNC );
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
