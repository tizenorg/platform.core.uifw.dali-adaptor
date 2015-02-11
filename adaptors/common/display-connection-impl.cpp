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
#include <Ecore_X.h>

#include <dali/integration-api/debug.h>

// INTERNAL HEADERS

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

DisplayConnection* DisplayConnection::New(/*Any display*/)
{
  return new DisplayConnection();
}

DisplayConnection::DisplayConnection(/*Any display*/)
: mDisplay(NULL)
{
  Any display;

  // display can be passed either EFL e-core types, or x11 types
  // we use DAli::Any to determine at run time which type
  if (display.Empty() == false)
  {
    // check we have a valid type
    DALI_ASSERT_ALWAYS(((display.GetType() == typeid (Ecore_X_Display *)) ||
                          (display.GetType() == typeid (XDisplay *)))
                        &&
                        "Display type is invalid");

    // display may point to EcoreXDisplay so may need to cast
    if (display.GetType() == typeid (Ecore_X_Display*))
    {
      mDisplay = static_cast< XDisplay* >(AnyCast< Ecore_X_Display* >( display ));
    }
    else
    {
      mDisplay = AnyCast< XDisplay* >(display);
    }
  }
  else
  {
    // mDisplay = (XDisplay*)ecore_x_display_get();
    // Because of DDK issue, we need to use separated x display instead of ecore default display
    mDisplay = XOpenDisplay(0);
  }
}

DisplayConnection::~DisplayConnection()
{
  if(mDisplay)
  {
    XCloseDisplay(mDisplay);
  }
}

XDisplay* DisplayConnection::GetDisplay()
{
  return mDisplay;
}

void DisplayConnection::ConsumeEvents()
{
  // check events so that we can flush the queue and avoid any potential memory leaks in X
  // looping if events remain
  int events(0);
  do
  {
    // Check if there are any events in the queue
    events = XEventsQueued(mDisplay, QueuedAfterFlush);

    if (events > 0)
    {
      // Just flush event to prevent memory leak from event queue as the events get built up in
      // memory but are only deleted when we retrieve them
      XEvent ev;
      XNextEvent(mDisplay, &ev);
    }
  }
  while (events > 0);
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

void DisplayConnection::GetDpi( unsigned int& dpiHorizontal, unsigned int& dpiVertical ) const
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
