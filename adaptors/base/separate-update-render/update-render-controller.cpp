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
#include "update-render-controller.h"

// INTERNAL INCLUDES
#include <base/separate-update-render/update-thread.h>
#include <base/separate-update-render/render-thread.h>
#include <base/separate-update-render/update-render-synchronization.h>
#include <base/separate-update-render/vsync-notifier.h>
#include <base/interfaces/adaptor-internal-services.h>
#include <base/environment-options.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

UpdateRenderController::UpdateRenderController( AdaptorInternalServices& adaptorInterfaces,
                                                const EnvironmentOptions& environmentOptions )
: mAdaptorInterfaces( adaptorInterfaces ),
  mUpdateThread( NULL ),
  mRenderThread( NULL ),
  mVSyncNotifier( NULL ),
  mUpdateRenderSync( NULL ),
  mNumberOfVSyncsPerRender( 1 )
{
  mUpdateRenderSync = new UpdateRenderSynchronization( adaptorInterfaces, mNumberOfVSyncsPerRender );

  mUpdateThread = new UpdateThread( *mUpdateRenderSync, adaptorInterfaces, environmentOptions );

  mRenderThread = new RenderThread( *mUpdateRenderSync, adaptorInterfaces, environmentOptions );

  mVSyncNotifier = new VSyncNotifier( *mUpdateRenderSync, adaptorInterfaces, environmentOptions );
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

void UpdateRenderController::ResumeFrameTime()
{
  mUpdateRenderSync->ResumeFrameTime();
}

void UpdateRenderController::Resume()
{
  mUpdateRenderSync->Resume();
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

void UpdateRenderController::SurfaceLost()
{
  mUpdateRenderSync->SurfaceLost();
}

void UpdateRenderController::ReplaceSurface( RenderSurface* newSurface )
{
  mUpdateRenderSync->ReplaceSurface( newSurface );
}

void UpdateRenderController::SetRenderRefreshRate(unsigned int numberOfVSyncsPerRender )
{
  mNumberOfVSyncsPerRender = numberOfVSyncsPerRender;
  mUpdateRenderSync->SetRenderRefreshRate(numberOfVSyncsPerRender);
}


} // namespace Adaptor

} // namespace Internal

} // namespace Dali
