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
//#include <Ecore.h>
//#include <Ecore_Wayland.h>

#include <dali/integration-api/core.h>
#include <dali/integration-api/system-overlay.h>
#include <dali/public-api/render-tasks/render-task.h>
#include <dali/public-api/render-tasks/render-task-list.h>
#include <orientation.h>

// INTERNAL HEADERS
#include "pure-window-render-surface.h"
#include <drag-and-drop-detector-impl.h>
#include <indicator-impl.h>
#include <window-visibility-observer.h>
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

struct Window::EventHandler
{
  // place holder
};

Window* Window::New(const PositionSize& posSize, const std::string& name, const std::string& className, bool isTransparent)
{
  Window* window = new Window();
  window->mIsTransparent = isTransparent;
  window->Initialize(posSize, name, className);
  return window;
}

void Window::SetAdaptor(Dali::Adaptor& adaptor)
{
  DALI_ASSERT_ALWAYS( !mStarted && "Adaptor already started" );
  mStarted = true;

  // Only create one overlay per window
  Internal::Adaptor::Adaptor& adaptorImpl = Internal::Adaptor::Adaptor::GetImplementation(adaptor);
  Integration::Core& core = adaptorImpl.GetCore();
  mOverlay = &core.GetSystemOverlay();

  Dali::RenderTaskList taskList = mOverlay->GetOverlayRenderTasks();
  taskList.CreateTask();

  mAdaptor = &adaptorImpl;
  mAdaptor->AddObserver( *this );

  // Can only create the detector when we know the Core has been instantiated.
  mDragAndDropDetector = DragAndDropDetector::New();
  mAdaptor->SetDragAndDropDetector( &GetImplementation( mDragAndDropDetector ) );


}

RenderSurface* Window::GetSurface()
{
  return mSurface;
}

void Window::ShowIndicator( Dali::Window::IndicatorVisibleMode visibleMode )
{

}

void Window::RotateIndicator(Dali::Window::WindowOrientation orientation)
{
}

void Window::SetIndicatorBgOpacity( Dali::Window::IndicatorBgOpacity opacityMode )
{
}

void Window::SetClass(std::string name, std::string klass)
{
}

Window::Window()
: mSurface(NULL),
  mIndicatorVisible(Dali::Window::VISIBLE),
  mIndicatorIsShown(false),
  mShowRotatedIndicatorOnClose(false),
  mStarted(false),
  mIsTransparent(false),
  mWMRotationAppSet(false),
  mIndicator(NULL),
  mIndicatorOrientation(Dali::Window::PORTRAIT),
  mNextIndicatorOrientation(Dali::Window::PORTRAIT),
  mIndicatorOpacityMode(Dali::Window::OPAQUE),
  mOverlay(NULL),
  mAdaptor(NULL)
{
  mEventHandler = NULL;
}

Window::~Window()
{
  delete mEventHandler;

  if( mIndicator )
  {
    mIndicator->Close();
    delete mIndicator;
  }

  if ( mAdaptor )
  {
    mAdaptor->RemoveObserver( *this );
    mAdaptor->SetDragAndDropDetector( NULL );
    mAdaptor = NULL;
  }

  delete mSurface;
}

void Window::Initialize(const PositionSize& windowPosition, const std::string& name, const std::string& className)
{
  // create an Wayland window by default
  Any surface;
  Wayland::WindowRenderSurface* windowSurface = new Wayland::WindowRenderSurface( windowPosition, surface, name, mIsTransparent );

  windowSurface->Map();

  mSurface = windowSurface;

  mOrientation = Orientation::New(this);

  // create event handler for Wayland window
 // mEventHandler = new EventHandler( this );
}

void Window::DoShowIndicator( Dali::Window::WindowOrientation lastOrientation )
{

}

void Window::DoRotateIndicator( Dali::Window::WindowOrientation orientation )
{

}

void Window::SetIndicatorProperties( bool isShow, Dali::Window::WindowOrientation lastOrientation )
{
}

void Window::IndicatorTypeChanged(Indicator::Type type)
{
}

void Window::IndicatorClosed( Indicator* indicator )
{

}

void Window::IndicatorVisibilityChanged(bool isVisible)
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

Dali::Any Window::GetNativeHandle() const
{
  Wayland::WindowRenderSurface* surface = static_cast<  Wayland::WindowRenderSurface* >( mSurface );

  return surface->GetWlWindow();
}

void Window::OnStart()
{
  DoShowIndicator( mIndicatorOrientation );
}

void Window::OnPause()
{
}

void Window::OnResume()
{
  // resume indicator status
  if( mIndicator != NULL )
  {
    // Restore own indicator opacity
    // Send opacity mode to indicator service when app resumed
    mIndicator->SetOpacityMode( mIndicatorOpacityMode );
  }
}

void Window::OnStop()
{
  if( mIndicator )
  {
    mIndicator->Close();
  }

  delete mIndicator;
  mIndicator = NULL;
}

void Window::OnDestroy()
{
  mAdaptor = NULL;
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
  DALI_ASSERT_ALWAYS( mAvailableOrientations.size() <= 4 && "Incorrect number of available orientations" );
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

//} // Wayland
} // Adaptor
} // Internal
} // Dali
