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
#include "display-connection-impl.h"

// EXTERNAL_HEADERS
#include <Ecore_Wayland.h>
#include <dali/integration-api/debug.h>

// INTERNAL HEADERS
#include <native-source-render-surface.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

DisplayConnection* DisplayConnection::New()
{
  DisplayConnection* pDisplayConnection(new DisplayConnection());

  return pDisplayConnection;
}

DisplayConnection::DisplayConnection()
{
}

DisplayConnection::~DisplayConnection()
{
  if( mSurfaceType == RenderSurface::NATIVE_SOURCE_RENDER_SURFACE )
  {
    ReleaseNativeSourceDisplay();
  }
}

Any DisplayConnection::GetDisplay()
{
  return Any( mDisplay );
}

void DisplayConnection::ConsumeEvents()
{
}

bool DisplayConnection::InitializeEgl(EglInterface& egl)
{
  EglImplementation& eglImpl = static_cast<EglImplementation&>(egl);

  if( !eglImpl.InitializeGles( mDisplay ) )
  {
    DALI_LOG_ERROR("Failed to initialize GLES.");
    return false;
  }

  return true;
}

void DisplayConnection::SetSurfaceType( RenderSurface::Type type )
{
  mSurfaceType = type;

  if( mSurfaceType == RenderSurface::NATIVE_SOURCE_RENDER_SURFACE )
  {
    mDisplay = GetNativeSourceDisplay();
  }
  else
  {
    mDisplay = reinterpret_cast< EGLNativeDisplayType >( ecore_wl_display_get() );
  }
}

void DisplayConnection::GetDpi(unsigned int& dpiHorizontal, unsigned int& dpiVertical)
{
  // calculate DPI
  float xres, yres;

  // 1 inch = 25.4 millimeters
  xres = ecore_wl_dpi_get();
  yres = ecore_wl_dpi_get();

  dpiHorizontal = int(xres + 0.5f);  // rounding
  dpiVertical   = int(yres + 0.5f);
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
