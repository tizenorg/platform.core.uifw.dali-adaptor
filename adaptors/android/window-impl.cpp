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
#include "window-impl.h"

// EXTERNAL HEADERS

#include <dali/integration-api/core.h>
#include <dali/integration-api/system-overlay.h>
#include <dali/public-api/render-tasks/render-task.h>
#include <dali/public-api/render-tasks/render-task-list.h>

// INTERNAL HEADERS
#include <adaptors/integration-api/framework.h>
#include <android/native_window_jni.h>
#include "android-render-surface.h"
#include "drag-and-drop-detector-impl.h"
#include <window-visibility-observer.h>
#include <orientation.h>
#include <orientation-impl.h>

namespace Dali
{
namespace Internal
{
namespace Adaptor
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gWindowLogFilter = Debug::Filter::New(Debug::Concise, false, "LOG_WINDOW");
#endif


Window* Window::New( Integration::Framework* framework, const PositionSize& posSize, const std::string& name, const std::string& className, bool isTransparent)
{
  Window* window = new Window();
  window->mIsTransparent = isTransparent;
  window->Initialize( framework, posSize, name, className);
  return window;
}

void Window::SetAdaptor(Dali::Adaptor& adaptor)
{
}

RenderSurface* Window::GetSurface()
{
  return mSurface;
}

void Window::SetClass(std::string name, std::string klass)
{
}

Window::Window()
: mSurface(NULL),
  mStarted(false),
  mIsTransparent(false),
  mWMRotationAppSet(false),
  mOverlay(NULL),
  mAdaptor(NULL)
{
}

Window::~Window()
{
  if(mSurface)
  {
    delete mSurface;
  }
}

void Window::Initialize( Integration::Framework* framework, const PositionSize& windowPosition, const std::string& name, const std::string& className )
{
  Any surface = Any( framework->GetWindow() );
  Any display;
  mIsTransparent = true;
  mSurface = new Android::AndroidRenderSurface( framework, Dali::RenderSurface::WINDOW, windowPosition, surface, display, name, mIsTransparent );

  DALI_LOG_INFO(gWindowLogFilter, Debug::General, "Window::Initialize() android_surface=%p", framework->GetWindow());
}

void Window::DoShowIndicator(Dali::Window::WindowOrientation lastOrientation )
{
  mIndicatorIsShown = false;
  SetIndicatorProperties( false, lastOrientation );
}


void Window::DoRotateIndicator( Dali::Window::WindowOrientation orientation )
{
}

void Window::SetIndicatorActorRotation()
{
}

void Window::Raise()
{
}

void Window::Lower()
{
}

void Window::Activate()
{
}

Dali::DragAndDropDetector Window::GetDragAndDropDetector() const
{
  return mDragAndDropDetector;
}

void Window::OnStart()
{
}

void Window::OnPause()
{
}

void Window::OnResume()
{
}

void Window::OnStop()
{
}

void Window::AddAvailableOrientation(Dali::Window::WindowOrientation orientation)
{
  bool found = false;

  for( std::size_t i=0; i<mAvailableOrientations.size(); i++ )
  {
    if(mAvailableOrientations[i] == orientation)
    {
      found = true;
      break;
    }
  }

  if( ! found )
  {
    mAvailableOrientations.push_back(orientation);
    SetAvailableOrientations( mAvailableOrientations );
  }
}

void Window::RemoveAvailableOrientation(Dali::Window::WindowOrientation orientation)
{
  for( std::vector<Dali::Window::WindowOrientation>::iterator iter = mAvailableOrientations.begin();
       iter != mAvailableOrientations.end(); ++iter )
  {
    if( *iter == orientation )
    {
      mAvailableOrientations.erase( iter );
      break;
    }
  }

  SetAvailableOrientations( mAvailableOrientations );
}

void Window::SetAvailableOrientations(const std::vector<Dali::Window::WindowOrientation>& orientations)
{
}

const std::vector<Dali::Window::WindowOrientation>& Window::GetAvailableOrientations()
{
  return mAvailableOrientations;
}

void Window::SetPreferredOrientation(Dali::Window::WindowOrientation orientation)
{
  mPreferredOrientation = orientation;

}

Dali::Window::WindowOrientation Window::GetPreferredOrientation()
{
  return mPreferredOrientation;
}

void Window::RotationDone( int orientation, int width, int height )
{
}

void Window::SetIndicatorProperties( bool isShow, Dali::Window::WindowOrientation lastOrientation )
{
}

void Window::OnDestroy()
{
  mAdaptor = NULL;
}

} // Adaptor
} // Internal
} // Dali
