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
#include "ecore-wl-render-surface.h"

// EXTERNAL INCLUDES
#include <dali/integration-api/gl-abstraction.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <ecore-wl-types.h>
#include <trigger-event.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

#if defined(DEBUG_ENABLED)
Debug::Filter* gRenderSurfaceLogFilter = Debug::Filter::New(Debug::Verbose, false, "LOG_ECORE_X_RENDER_SURFACE");
#endif

namespace ECore
{

namespace
{

static bool gWlInitThreadsCalled = false;     ///< global to say whether WlInitThreads has been called in this process

} // unnamed namespace

EcoreWlRenderSurface::EcoreWlRenderSurface(Dali::PositionSize positionSize,
                                           Any surface,
                                           Any display,
                                           const std::string& name,
                                           bool isTransparent)
: mMainDisplay(NULL),
  mType(type),
  mPosition(positionSize),
  mTitle(name),
  mColorDepth(isTransparent ? COLOR_DEPTH_32 : COLOR_DEPTH_24),
  mOwnSurface(false),
  mOwnDisplay(false)
{
  // see if there is a display in Any display
  SetDisplay( display );
}

void EcoreWlRenderSurface::Init( Any surface )
{
  // see if there is a surface in Any surface
  unsigned int surfaceId  = GetSurfaceId( surface );

  // if the surface is empty, create a new one.
  if ( surfaceId == 0 )
  {
    if ( !gWlInitThreadsCalled )
    {
      ecore_wl_init(NULL);
      mMainDisplay = ecore_wl_display_get();
      mOwnDisplay = true;
      gWlInitThreadsCalled = true;
    }

    // we own the surface about to created
    mOwnSurface = true;
    CreateWlRenderable();
  }
  else
  {
    // XLib should already be initialized so no point in calling XInitThreads
    UseExistingRenderable( surfaceId );
  }

#ifdef DEBUG_ENABLED
  // prints out 'INFO: DALI: new EcoreWlRenderSurface, created display xx, used existing surface xx
  // we can not use LOG_INFO because the surface can be created before Dali Core is created.
  printf(  "INFO: DALI: new EcoreWlRenderSurface, %s display %p, %s %s surface %X \n",
             mOwnDisplay?"created":"used existing",mMainDisplay,
             mOwnSurface?"created":"used existing",
             Dali::RenderSurface::PIXMAP==mType?" pixmap" :"window",
             GetDrawable() );
#endif
}

EcoreWlRenderSurface::~EcoreWlRenderSurface()
{
  // release the display connection if we use our own
  if( mOwnDisplay )
  {
    if( mMainDisplay )
    {
      // NOTE, on 64bit desktop with some NVidia driver versions this crashes
    }
  }
}

Ecore_Wl_Window* EcoreWlRenderSurface::GetWlWindow()
{
  return 0;
}

Ecore_Wl_Window* EcoreWlRenderSurface::GetDrawable()
{
  return 0;
}

Any EcoreWlRenderSurface::GetDisplay()
{
  // this getter is used by main thread so we need to return the main thread version of the display
  return Any( ecore_wl_display_get() );
}

PositionSize EcoreWlRenderSurface::GetPositionSize() const
{
  return mPosition;
}

void EcoreWlRenderSurface::MoveResize( Dali::PositionSize positionSize )
{
  // nothing to do in base class
}

void EcoreWlRenderSurface::GetDpi( unsigned int& dpiHorizontal, unsigned int& dpiVertical ) const
{
  // calculate DPI
  float xres, yres;

  // 1 inch = 25.4 millimeters
  xres = ecore_wl_dpi_get();
  yres = ecore_wl_dpi_get();

  dpiHorizontal = int(xres + 0.5f);  // rounding
  dpiVertical   = int(yres + 0.5f);
}

void EcoreWlRenderSurface::Map()
{
}

void EcoreWlRenderSurface::ConsumeEvents()
{
}

void EcoreWlRenderSurface::SetViewMode( ViewMode )
{
}

void EcoreWlRenderSurface::SetDisplay( Any display )
{
  // the render surface can be passed either EFL e-core types, or x11 types
  // we use boost any to determine at run time which type

  if ( display.Empty() == false )
  {
    // check we have a valid type
    DALI_ASSERT_ALWAYS( ( display.GetType() == typeid (WlDisplay *) )
                        &&
                        "Display type is invalid" );

    mOwnDisplay = false;

    mMainDisplay = AnyCast< WlDisplay* >( display );
  }
}

unsigned int EcoreWlRenderSurface::GetSurfaceId( Any surface ) const
{
  unsigned int surfaceId = 0;

  if ( surface.Empty() == false )
  {
    // check we have a valid type
    DALI_ASSERT_ALWAYS( ( (surface.GetType() == typeid (Ecore_Wl_Window *) ) )
                        && "Surface type is invalid" );

    surfaceId = AnyCast<unsigned int>( surface );
  }
  return surfaceId;
}

} // namespace ECore

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
