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

// CLASS HEADER
#include "update-render-controller.h"

// INTERNAL INCLUDES
#include <base/update-thread.h>
#include <base/render-thread.h>
#include <base/update-render-synchronization.h>
#include <base/vsync-notifier.h>
#include <base/performance-logging/performance-interface-factory.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

UpdateRenderController::UpdateRenderController( Dali::Integration::Core& core,
                                                Dali::Integration::Log::LogFunction& logFunction,
                                                int logOpts,
                                                unsigned int fpsTrackingSeconds,
                                                unsigned int updateStatusLogInterval,
                                                unsigned int performanceLogging,
                                                TriggerEvent& notificationTrigger,
                                                Integration::GlAbstraction& gles,
                                                EglFactoryInterface* eglFactory,
                                                RenderSurface* surface,
                                                Dali::Integration::PlatformAbstraction& platformAbstraction,
                                                VSyncMonitorInterface* vSyncMonitor )
: mUpdateThread( NULL ),
  mRenderThread( NULL ),
  mVSyncNotifier( NULL ),
  mUpdateRenderSync( NULL ),
  mPerformanceInterface( NULL )
{

  if( performanceLogging )
  {
    mPerformanceInterface = PerformanceInterfaceFactory::CreateInterface( platformAbstraction, logFunction, logOpts );
    mPerformanceInterface->SetLogging( performanceLogging, updateStatusLogInterval );
  }

  mUpdateRenderSync = new UpdateRenderSynchronization( core.GetMaximumUpdateCount(), core, platformAbstraction, mPerformanceInterface);

  mUpdateThread = new UpdateThread( *mUpdateRenderSync, core, logFunction, logOpts, fpsTrackingSeconds, updateStatusLogInterval, notificationTrigger );

  mRenderThread = new RenderThread( *mUpdateRenderSync, core, logFunction, logOpts, gles, eglFactory, surface );

  mVSyncNotifier = new VSyncNotifier( *mUpdateRenderSync, core, platformAbstraction, vSyncMonitor, logFunction, logOpts );
}

UpdateRenderController::~UpdateRenderController()
{
  delete mVSyncNotifier;
  delete mRenderThread;
  delete mUpdateThread;
  delete mUpdateRenderSync;
  delete mPerformanceInterface;
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

void UpdateRenderController::ReplaceSurface( RenderSurface* surface )
{
  // tell render thread to start the replace
  mRenderThread->ReplaceSurface(surface);

  // Ensure that a frame gets processed and render thread runs at least once
  // Note: ReplaceSurface may be called while threads are paused so call
  //       RequestUpdateOnce to ensure we do an update/render pass even if paused
  RequestUpdateOnce();

  // block here until replace complete
  mRenderThread->WaitForSurfaceReplaceComplete();
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
