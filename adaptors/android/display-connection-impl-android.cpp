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
#include <dali/integration-api/debug.h>


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
}

Any DisplayConnection::GetDisplay()
{
  return Any();
}

void DisplayConnection::ConsumeEvents()
{
  // Do nothing
}

bool DisplayConnection::InitializeEgl(EglInterface& egl)
{
  EglImplementation& eglImpl = static_cast<EglImplementation&>(egl);

  if( !eglImpl.InitializeGles( EGL_DEFAULT_DISPLAY ))
  {
    DALI_LOG_ERROR("Failed to initialize GLES.");
    return false;
  }

  return true;
}

void DisplayConnection::GetDpi(unsigned int& dpiHorizontal, unsigned int& dpiVertical)
{
  // calculate DPI
  float xres, yres;

  // 1 inch = 25.4 millimeters
  xres = 0;
  yres = 0;

  dpiHorizontal = int(xres + 0.5f);  // rounding
  dpiVertical   = int(yres + 0.5f);
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
