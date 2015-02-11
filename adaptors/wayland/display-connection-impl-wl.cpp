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
#include <pixmap-render-surface.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

DisplayConnection* DisplayConnection::New()
{
  return new DisplayConnection();
}

DisplayConnection::DisplayConnection()
{
}

DisplayConnection::~DisplayConnection()
{
}

void DisplayConnection::ConsumeEvents()
{
}

bool DisplayConnection::InitializeEgl(EglInterface& egl)
{
  EglImplementation& eglImpl = static_cast<EglImplementation&>(egl);

  if (!eglImpl.InitializeGles(reinterpret_cast<EGLNativeDisplayType>(mDisplay)))
  {
    DALI_LOG_ERROR("Failed to initialize GLES.");
    return false;
  }

  //TODO: change constant to mColorDepth
  eglImpl.ChooseConfig(false, COLOR_DEPTH_32/*mColorDepth*/);

  return true;
}

bool DisplayConnection::ReplaceEGL(EglInterface& egl, RenderSurface* renderSurface)
{
  Internal::Adaptor::EglImplementation& eglImpl = static_cast<Internal::Adaptor::EglImplementation&>(egl);
  eglImpl.InitializeGles(reinterpret_cast<EGLNativeDisplayType>(mDisplay));

  ECore::PixmapRenderSurface* pixmapRenderSurface = dynamic_cast<ECore::PixmapRenderSurface *>(renderSurface);
  if (!pixmapRenderSurface)
  {
    DALI_LOG_ERROR("Failed to get pixmap render surface");
    return false;
  }

  // a new surface for the new pixmap
  // need to cast to X handle as in 64bit system ECore handle is 32 bit whereas EGLnative and XWindow are 64 bit
  XPixmap pixmap = static_cast<XPixmap>(AnyCast<Ecore_X_Pixmap>(pixmapRenderSurface->GetSurface()));
  return eglImpl.ReplaceSurfacePixmap((EGLNativePixmapType)pixmap, // reinterpret_cast does not compile
                                      reinterpret_cast<EGLNativeDisplayType>(mDisplay));
}

void DisplayConnection::GetDpi(unsigned int& dpiHorizontal, unsigned int& dpiVertical) const
{
  // calculate DPI
  float xres, yres;

  // 1 inch = 25.4 millimeters
  xres = ecore_x_dpi_get();
  yres = ecore_x_dpi_get();

  dpiHorizontal = int(xres + 0.5f);  // rounding
  dpiVertical   = int(yres + 0.5f);
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
