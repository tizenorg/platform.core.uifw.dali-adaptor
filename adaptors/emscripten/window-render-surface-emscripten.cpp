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
#include "window-render-surface.h"

// EXTERNAL INCLUDES
#include <dali/integration-api/gl-abstraction.h>
#include <dali/integration-api/debug.h>
#include <gl/egl-implementation.h>

// INTERNAL INCLUDES

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

#if defined(DEBUG_ENABLED)
extern Debug::Filter* gRenderSurfaceLogFilter;
#endif

namespace ECore
{

namespace
{

} // unnamed namespace

WindowRenderSurface::WindowRenderSurface()
{
  DALI_LOG_INFO( gRenderSurfaceLogFilter, Debug::Verbose, "Creating Window\n" );
}

WindowRenderSurface::~WindowRenderSurface()
{
}

Dali::RenderSurface::SurfaceType WindowRenderSurface::GetType()
{
  return Dali::RenderSurface::WINDOW;
}

Any WindowRenderSurface::GetSurface()
{
  // already an e-core type
  return Any();
}

void WindowRenderSurface::InitializeEgl( EglInterface& eglIf )
{
  DALI_LOG_TRACE_METHOD( gRenderSurfaceLogFilter );

  EglImplementation& eglImpl = static_cast<EglImplementation&>( eglIf );
  // eglImpl.InitializeGles( reinterpret_cast< EGLNativeDisplayType >( mMainDisplay ) );

  eglImpl.ChooseConfig(true, COLOR_DEPTH_32);
}

void WindowRenderSurface::CreateEglSurface( EglInterface& eglIf )
{
  DALI_LOG_TRACE_METHOD( gRenderSurfaceLogFilter );
}

void WindowRenderSurface::DestroyEglSurface( EglInterface& eglIf )
{
  DALI_LOG_TRACE_METHOD( gRenderSurfaceLogFilter );
}

bool WindowRenderSurface::ReplaceEGLSurface( EglInterface& eglIf )
{
  DALI_LOG_TRACE_METHOD( gRenderSurfaceLogFilter );
  return true;
}

void WindowRenderSurface::MoveResize( Dali::PositionSize /* positionSize */ )
{
}

void WindowRenderSurface::Map()
{
}

bool WindowRenderSurface::PreRender( EglInterface&, Integration::GlAbstraction& )
{
  // nothing to do for windows
  return true;
}

void WindowRenderSurface::PostRender( EglInterface& egl, Integration::GlAbstraction& glAbstraction, unsigned int, SyncMode )
{
  EglImplementation& eglImpl = static_cast<EglImplementation&>( egl );
  eglImpl.SwapBuffers();
}

void WindowRenderSurface::SetViewMode( ViewMode /* viewMode */ )
{
}

} // namespace ECore

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
