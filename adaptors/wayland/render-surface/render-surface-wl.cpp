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
#include "render-surface-wl.h"

// EXTERNAL INCLUDES
#include <stdio.h>
#include <dali/integration-api/gl-abstraction.h>
#include <dali/integration-api/debug.h>
#include <cstring>
#include <unistd.h>

// INTERNAL INCLUDES
#include <base/separate-update-render/thread-synchronization.h>
#include <gl/egl-implementation.h>
#include <trigger-event.h>
#include <base/interfaces/window-event-interface.h>
#include <wayland-manager.h>


namespace Dali
{

namespace Wayland
{

RenderSurface::RenderSurface(Dali::PositionSize positionSize,
                                           Any surface,
                                           const std::string& name,
                                           bool isTransparent)
: mRenderNotification(NULL),
  mColorDepth(isTransparent ? COLOR_DEPTH_32 : COLOR_DEPTH_24)
{
  mWindow.mPosition = positionSize;
  mWindow.mTitle = name;
  mWaylandClient = new  Dali::Internal::Adaptor::WaylandManager;
  mWaylandClient->Initialise();
}

RenderSurface::~RenderSurface()
{
  delete mWaylandClient;
}

void RenderSurface::CreateSurface()
{
  mWaylandClient->CreateSurface( mWindow );
}

Window* RenderSurface::GetWindow()
{
  return &mWindow;
}


void RenderSurface::AssignWindowEventInterface( Dali::Internal::Adaptor::WindowEventInterface* eventInterface)
{
  mWaylandClient->AssignWindowEventInterface( eventInterface );
}

PositionSize RenderSurface::GetPositionSize() const
{
  return mWindow.mPosition;
}

void RenderSurface::InitializeEgl( EglInterface& egl )
{
  Internal::Adaptor::EglImplementation& eglImpl = static_cast<Internal::Adaptor::EglImplementation&>( egl );

  if (!eglImpl.InitializeGles(static_cast<EGLNativeDisplayType>( mWaylandClient->mDisplay )))
  {
    DALI_LOG_ERROR("Failed to initialize GLES.");
  }

  eglImpl.ChooseConfig( true, mColorDepth );
}

void RenderSurface::CreateEglSurface( EglInterface& egl )
{
  Internal::Adaptor::EglImplementation& eglImpl = static_cast<Internal::Adaptor::EglImplementation&>( egl );

  CreateSurface();

  mEglWindow = wl_egl_window_create( mWaylandClient->GetSurface(), mWindow.mPosition.width, mWindow.mPosition.height);

  eglImpl.CreateSurfaceWindow( (EGLNativeWindowType)mEglWindow, mColorDepth ); // reinterpret_cast does not compile

}

void RenderSurface::DestroyEglSurface( EglInterface& eglIf )
{
  Internal::Adaptor::EglImplementation& eglImpl = static_cast<Internal::Adaptor::EglImplementation&>( eglIf );
  eglImpl.DestroySurface();
  wl_egl_window_destroy( mEglWindow );
  mEglWindow = NULL;
}

bool RenderSurface::ReplaceEGLSurface( EglInterface& egl )
{
  return true;
}

void RenderSurface::MoveResize( Dali::PositionSize positionSize )
{
}

void RenderSurface::SetViewMode( ViewMode viewMode )
{
}

void RenderSurface::StartRender()
{
}

bool RenderSurface::PreRender( EglInterface&, Integration::GlAbstraction& )
{
  return true;
}

void RenderSurface::PostRender( EglInterface& egl, Integration::GlAbstraction& glAbstraction, DisplayConnection* displayConnection, bool replacingSurface )
{
  Internal::Adaptor::EglImplementation& eglImpl = static_cast<Internal::Adaptor::EglImplementation&>( egl );
  eglImpl.SwapBuffers();
}

void RenderSurface::StopRender()
{
}

void RenderSurface::ReleaseLock()
{
}
void RenderSurface::SetThreadSynchronization( ThreadSynchronizationInterface& threadSynchronization )
{
}



} // namespace Wayland

} // namespace Dali
