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
#include <android/native_window_jni.h>
#include <android-interface.h>

#include <dali/public-api/adaptor-framework/common/orientation.h>

// INTERNAL HEADERS
#include "internal/common/android-render-surface.h"
#include "internal/common/drag-and-drop-detector-impl.h"
#include <internal/common/orientation-impl.h>

namespace Dali
{
namespace Internal
{
namespace Adaptor
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gWindowLogFilter = Debug::Filter::New(Debug::Concise, false, "LOG_WINDOW");
#endif


Window* Window::New(const PositionSize& posSize, const std::string& name, bool isTransparent)
{
  Window* window = new Window();
  window->mIsTransparent = isTransparent;
  window->Initialize(posSize, name);
  return window;
}

void Window::SetAdaptor(Dali::Adaptor& adaptor)
{
//  DALI_ASSERT_ALWAYS( !mStarted && "Adaptor already started" );
//  mStarted = true;

//  // Only create one overlay per window
//  Internal::Adaptor::Adaptor& adaptorImpl = Internal::Adaptor::Adaptor::GetImplementation(adaptor);
//  Integration::Core& core = adaptorImpl.GetCore();
//  mOverlay = &core.GetSystemOverlay();

//  Dali::RenderTaskList taskList = mOverlay->GetOverlayRenderTasks();
//  taskList.CreateTask();

//  mAdaptor = &adaptorImpl;
//  mAdaptor->AddObserver( *this );

//  // Can only create the detector when we know the Core has been instantiated.
//  // mDragAndDropDetector = DragAndDropDetector::New();
//  // mAdaptor->SetDragAndDropDetector( &GetImplementation( mDragAndDropDetector ) );

//  if( mIndicator != NULL )
//  {
//    mIndicator->SetAdaptor(mAdaptor);
//  }
}

RenderSurface* Window::GetSurface()
{
  return mSurface;
}

void Window::ShowIndicator( bool show )
{
  DALI_LOG_TRACE_METHOD_FMT( gWindowLogFilter, "%s\n", show?"SHOW":"HIDE" );

  // DALI_ASSERT_DEBUG(mOverlay);

  // mShowIndicator = show;
  // DoShowIndicator( mShowIndicator );
}

void Window::RotateIndicator(Dali::Window::WindowOrientation orientation)
{
  DALI_LOG_TRACE_METHOD_FMT( gWindowLogFilter, "Orientation: %d\n", orientation );

  DoRotateIndicator( orientation );
}

void Window::SetIndicatorBgOpacity( Dali::Window::IndicatorBgOpacity opacity )
{
//  switch(opacity)
//  {
//    case Dali::Window::TRANSLUCENT :
//      mIndicatorBgOpacity = 0.5f;
//      break;

//    case Dali::Window::TRANSPARENT :
//      mIndicatorBgOpacity = 0.0f;
//      break;

//    case Dali::Window::OPAQUE :
//    default :
//      mIndicatorBgOpacity = 1.0f;
//      break;
//  }

//  if( mIndicator != NULL )
//  {
//    mIndicator->SetOpacity(mIndicatorBgOpacity);
//  }
}

void Window::SetClass(std::string name, std::string klass)
{
  /*
  // Get render surface's x11 window
  if( mSurface )
  {
    X11::RenderSurface* x11Window = dynamic_cast< X11::RenderSurface * >( mSurface );
    if( x11Window )
    {
      ecore_x_icccm_name_class_set( x11Window->GetXWindow(), name.c_str(), klass.c_str() );
    }
  }
  */
}

Window::Window()
: mSurface(NULL),
  mShowIndicator(false),
  mIndicatorIsShown(false),
  mShowRotatedIndicatorOnClose(false),
  // mIndicator(NULL),
  mIndicatorOrientation(Dali::Window::PORTRAIT),
  mNextIndicatorOrientation(Dali::Window::PORTRAIT),
  mOverlay(NULL),
  mStarted(false),
  mIsTransparent(false),
  mWMRotationAppSet(false),
  mIndicatorOpacityMode(Dali::Window::OPAQUE),
  mAdaptor(NULL)
  // mEventHandler( new EventHandler( this ) )
{
}

Window::~Window()
{
//  delete mEventHandler;

//  if ( mAdaptor )
//  {
//    mAdaptor->RemoveObserver( *this );
//    // mAdaptor->SetDragAndDropDetector( NULL );
//    mAdaptor = NULL;
//  }

//  delete mSurface;
}

void Window::Initialize(const PositionSize& windowPosition, const std::string& name)
{
  // create an X11 window by default
  boost::any surface = boost::any(androidcore_GetPrimaryWindow());
  boost::any display;
  mIsTransparent = true;
  mSurface = new Android::RenderSurface( Dali::RenderSurface::WINDOW, windowPosition, surface, display, name, mIsTransparent );
}

void Window::DoShowIndicator( bool show, Dali::Window::WindowOrientation lastOrientation )
{
  mIndicatorIsShown = false;
  SetIndicatorProperties( false, lastOrientation );
}

void Window::DoRotateIndicator( Dali::Window::WindowOrientation orientation )
{
//  if( mIndicatorIsShown )
//  {
//    mShowRotatedIndicatorOnClose = true;
//    mNextIndicatorOrientation = orientation;
//    mIndicator->Close(); // May synchronously call IndicatorClosed() callback
//  }
//  else
//  {
//    // Save orientation for when the indicator is next shown
//    mShowRotatedIndicatorOnClose = false;
//    mNextIndicatorOrientation = orientation;
//  }
}

void Window::SetIndicatorActorRotation()
{
#if 0
  DALI_LOG_TRACE_METHOD( gWindowLogFilter );
  DALI_ASSERT_DEBUG( mIndicator != NULL );

  Dali::ImageActor actor = mIndicator->GetActor();
  switch( mIndicatorOrientation )
  {
    case Dali::Window::PORTRAIT:
      actor.SetParentOrigin( ParentOrigin::TOP_CENTER );
      actor.SetAnchorPoint(  AnchorPoint::TOP_CENTER );
      actor.SetRotation( Degree(0), Vector3::ZAXIS );
      break;
    case Dali::Window::PORTRAIT_INVERSE:
      actor.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
      actor.SetAnchorPoint(  AnchorPoint::TOP_CENTER );
      actor.SetRotation( Degree(180), Vector3::ZAXIS );
      break;
    case Dali::Window::LANDSCAPE:
      actor.SetParentOrigin( ParentOrigin::CENTER_LEFT );
      actor.SetAnchorPoint(  AnchorPoint::TOP_CENTER );
      actor.SetRotation( Degree(270), Vector3::ZAXIS );
      break;
    case Dali::Window::LANDSCAPE_INVERSE:
      actor.SetParentOrigin( ParentOrigin::CENTER_RIGHT );
      actor.SetAnchorPoint(  AnchorPoint::TOP_CENTER );
      actor.SetRotation( Degree(90), Vector3::ZAXIS );
      break;
  }
#endif
}

void Window::Raise()
{
  // X11::RenderSurface* x11Window = dynamic_cast< X11::RenderSurface * >( mSurface );
  // if( x11Window )
  //{
    // Ecore_X_Window win = x11Window->GetXWindow();
    // ecore_x_window_raise(win);
  //}
}

void Window::Lower()
{
  // X11::RenderSurface* x11Window = dynamic_cast< X11::RenderSurface * >( mSurface );
  // if( x11Window )
  //{
    // Ecore_X_Window win = x11Window->GetXWindow();
    // ecore_x_window_lower(win);
  //}
}

Dali::DragAndDropDetector Window::GetDragAndDropDetector() const
{
  return mDragAndDropDetector;
}

void Window::OnStart()
{
  // DoShowIndicator( mShowIndicator );
}

void Window::OnPause()
{
//  // If this is during a BitmapImage update, the upload complete signal may not arrive until resumed.
//  if( mIndicator != NULL )
//  {
//    mIndicator->IgnoreBitmapUpdates( true );
//  }
}

void Window::OnResume()
{
  // resume indicator status
//  if( mIndicator != NULL )
//  {
//    mIndicator->IgnoreBitmapUpdates( false );
//  }
}

void Window::OnStop()
{
//  if( mIndicator )
//  {
//    mIndicator->Close();
//  }

//  delete mIndicator;
//  mIndicator = NULL;
}

OrientationPtr Window::GetOrientation()
{
  return mOrientation;
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
#if 0
  DALI_ASSERT_ALWAYS( mAvailableOrientations.size() <= 4 && "Incorrect number of available orientations" );

  mAvailableOrientations = orientations;
  ECoreX::WindowRenderSurface* x11Window = dynamic_cast< ECoreX::WindowRenderSurface * >( mSurface );
  if( x11Window )
  {
    Ecore_X_Window ecoreWindow = x11Window->GetXWindow();
    if( ! mWMRotationAppSet )
    {
      mWMRotationAppSet = true;
      ecore_x_e_window_rotation_app_set(ecoreWindow, EINA_TRUE);
    }

    int rotations[4];
    for( std::size_t i=0; i<mAvailableOrientations.size(); i++ )
    {
      rotations[i] = static_cast<int>(mAvailableOrientations[i]);
    }
    ecore_x_e_window_rotation_available_rotations_set(ecoreWindow, rotations, mAvailableOrientations.size() );

  }
#endif
}

const std::vector<Dali::Window::WindowOrientation>& Window::GetAvailableOrientations()
{
  return mAvailableOrientations;
}

void Window::SetPreferredOrientation(Dali::Window::WindowOrientation orientation)
{
  mPreferredOrientation = orientation;

#if 0
  ECoreX::WindowRenderSurface* x11Window = dynamic_cast< ECoreX::WindowRenderSurface * >( mSurface );
  if( x11Window )
  {
    Ecore_X_Window ecoreWindow = x11Window->GetXWindow();

    if( ! mWMRotationAppSet )
    {
      mWMRotationAppSet = true;
      ecore_x_e_window_rotation_app_set(ecoreWindow, EINA_TRUE);
    }

    ecore_x_e_window_rotation_preferred_rotation_set(ecoreWindow, orientation);
  }
#endif
}

Dali::Window::WindowOrientation Window::GetPreferredOrientation()
{
  return mPreferredOrientation;
}

void Window::RotationDone( int orientation, int width, int height )
{
#if 0
  // Tell window manager we're done
  ECoreX::WindowRenderSurface* x11Window = dynamic_cast< ECoreX::WindowRenderSurface * >( mSurface );
  if( x11Window )
  {
    Ecore_X_Window ecoreWindow = x11Window->GetXWindow();
    Ecore_X_Window root = ecore_x_window_root_get(ecoreWindow);

    /**
     * send rotation done message to wm, even if window is already rotated.
     * that's why wm must be wait for comming rotation done message
     * after sending rotation request.
     */
    ecore_x_e_window_rotation_change_done_send(root, ecoreWindow, orientation, width, height);

    /**
     * set rotate window property
     */
    int angles[2] = { orientation, orientation };
    ecore_x_window_prop_property_set( ecoreWindow,
                                     ECORE_X_ATOM_E_ILLUME_ROTATE_WINDOW_ANGLE,
                                     ECORE_X_ATOM_CARDINAL, 32, &angles, 2 );
  }
#endif
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

