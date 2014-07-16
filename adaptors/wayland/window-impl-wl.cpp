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
#include <Ecore.h>
#include <Ecore_Wayland.h>

#include <dali/integration-api/core.h>
#include <dali/integration-api/system-overlay.h>
#include <dali/public-api/render-tasks/render-task.h>
#include <dali/public-api/render-tasks/render-task-list.h>
#include <orientation.h>

// INTERNAL HEADERS
#include <window-render-surface.h>
#include <drag-and-drop-detector-impl.h>
#include <indicator-impl.h>
#include <window-visibility-observer.h>
#include <orientation-impl.h>

namespace
{
const float INDICATOR_ANIMATION_DURATION( 0.18f ); // 180 milli seconds
const float INDICATOR_SHOW_Y_POSITION( 0.0f );
const float INDICATOR_HIDE_Y_POSITION( -52.0f );
}

namespace Dali
{
namespace Internal
{
namespace Adaptor
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gWindowLogFilter = Debug::Filter::New(Debug::Concise, false, "LOG_WINDOW");
#endif

struct Window::Impl
{
  typedef std::vector<Indicator*> DiscardedIndicators;

  RenderSurface*                   mSurface;
  Dali::Window::IndicatorStyle     mIndicatorStyle;     ///< indicator style
  Dali::Window::IndicatorVisibleMode mIndicatorVisible; ///< public state
  bool                             mIndicatorIsShown:1; ///< private state
  bool                             mShowRotatedIndicatorOnClose:1;
  bool                             mStarted:1;
  bool                             mIsTransparent:1;
  bool                             mWMRotationAppSet:1;
  Indicator*                       mIndicator;
  Dali::Window::WindowOrientation  mIndicatorOrientation;
  Dali::Window::WindowOrientation  mNextIndicatorOrientation;
  Dali::Window::IndicatorBgOpacity mIndicatorOpacityMode;
  Integration::SystemOverlay*      mOverlay;
  Adaptor*                         mAdaptor;
  Dali::DragAndDropDetector        mDragAndDropDetector;

  struct EventHandler;
  EventHandler*                    mEventHandler;

  OrientationPtr                               mOrientation;
  std::vector<Dali::Window::WindowOrientation> mAvailableOrientations;
  Dali::Window::WindowOrientation              mPreferredOrientation;


  /**
   * @copydoc Dali::Internal::Window::SetAdaptor()
   */
  void SetAdaptor(Dali::Adaptor& adaptor);

  /**
   * @copydoc Dali::Internal::Window::GetSurface()
   */
  RenderSurface* GetSurface();

  /**
   * @copydoc Dali::Window::SetIndicatorStyle()
   */
  void SetIndicatorStyle( Dali::Window::IndicatorStyle style );

  /**
   * @copydoc Dali::Window::ShowIndicator()
   */
  void ShowIndicator( bool show );

  /**
   * @copydoc Dali::Window::ShowIndicator()
   */
  void ShowIndicator( Dali::Window::IndicatorVisibleMode visibleMode );

  /**
   * @copydoc Dali::Window::SetIndicatorBgOpacity()
   */
  void SetIndicatorBgOpacity( Dali::Window::IndicatorBgOpacity opacity );

  /**
   * @copydoc Dali::Window::RotateIndicator()
   */
  void RotateIndicator( Dali::Window::WindowOrientation orientation );

  /**
   * @copydoc Dali::Window::SetClass()
   */
  void SetClass( std::string name, std::string klass );

  /**
   * @copydoc Dali::Window::Raise()
   */
  void Raise();

  /**
   * @copydoc Dali::Window::Lower()
   */
  void Lower();

  /**
   * @copydoc Dali::Window::Activate()
   */
  void Activate();

  /**
   * @copydoc Dali::Window::GetOrientation()
   */
  OrientationPtr GetOrientation();

  /**
   * @copydoc Dali::Window::AddAvailableOrientation()
   */
  void AddAvailableOrientation(Dali::Window::WindowOrientation orientation);

  /**
   * @copydoc Dali::Window::RemoveAvailableOrientation()
   */
  void RemoveAvailableOrientation(Dali::Window::WindowOrientation orientation);

  /**
   * @copydoc Dali::Window::SetAvailableOrientations()
   */
  void SetAvailableOrientations(const std::vector<Dali::Window::WindowOrientation>& orientations);

  /**
   * @copydoc Dali::Window::GetAvailableOrientations()
   */
  const std::vector<Dali::Window::WindowOrientation>& GetAvailableOrientations();

  /**
   * @copydoc Dali::Window::SetPreferredOrientation()
   */
  void SetPreferredOrientation(Dali::Window::WindowOrientation orientation);

  /**
   * @copydoc Dali::Window::GetPreferredOrientation()
   */
  Dali::Window::WindowOrientation GetPreferredOrientation();

  /**
   * @copydoc Dali::Window::GetDragAndDropDetector() const
   */
  Dali::DragAndDropDetector GetDragAndDropDetector() const;

  /**
   * Called from Orientation after the Change signal has been sent
   */
  void RotationDone( int orientation, int width, int height );

  /**
   * constructor.
   */
  Impl();

  /**
   * Destructor
   */
  ~Impl();

  /**
   * Second stage initialization
   */
  void Initialize(const PositionSize& posSize, const std::string& name);

  /**
   * Shows / hides the indicator bar.
   * Handles close/open if rotation changes whilst hidden
   */
  void DoShowIndicator( Dali::Window::WindowOrientation lastOrientation );

  /**
   * Close current indicator and open a connection onto the new indicator service.
   * Effect may not be synchronous if waiting for an indicator update on existing connection.
   */
  void DoRotateIndicator( Dali::Window::WindowOrientation orientation );

  /**
   * Change the indicator actor's rotation to match the current orientation
   */
  void SetIndicatorActorRotation();

  /**
   * Set the indicator properties on the window
   */
  void SetIndicatorProperties( bool isShown, Dali::Window::WindowOrientation lastOrientation );

};


/**
 * TODO: Abstract Window class out and move this into a window implementation for Ecore
 */
struct Window::EventHandler
{
  /**
   * Constructor
   * @param[in]  window  A pointer to the window class.
   */
  EventHandler( Window* window )
  : mWindow( window ),
    mEcoreWindow( 0 )
  {
  }

  /**
   * Destructor
   */
  ~EventHandler()
  {
    if ( mWindowPropertyHandler )
    {
      ecore_event_handler_del( mWindowPropertyHandler );
    }
    if ( mClientMessagehandler )
    {
      ecore_event_handler_del( mClientMessagehandler );
    }
  }

  // Static methods

  /// Called when the window properties are changed.
  static Eina_Bool EcoreEventWindowPropertyChanged( void* data, int type, void* event )
  {
    return EINA_FALSE;
  }

  /// Called when the window properties are changed.
  static Eina_Bool EcoreEventClientMessage( void* data, int type, void* event )
  {
    return EINA_FALSE;
  }

  // Data
  Window* mWindow;
  Ecore_Event_Handler* mWindowPropertyHandler;
  Ecore_Event_Handler* mClientMessagehandler;
  Ecore_Wl_Window* mEcoreWindow;
};


void Window::Impl::SetAdaptor(Dali::Adaptor& adaptor)
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

  if( mOrientation )
  {
    mOrientation->SetAdaptor(adaptor);
  }

  if( mIndicator != NULL )
  {
    mIndicator->SetAdaptor(mAdaptor);
  }
}

RenderSurface* Window::Impl::GetSurface()
{
  return mSurface;
}

void Window::Impl::SetIndicatorStyle( Dali::Window::IndicatorStyle style )
{
  mIndicatorStyle = style;
}

void Window::ShowIndicator( Dali::Window::IndicatorVisibleMode visibleMode )
{
  DALI_LOG_TRACE_METHOD_FMT( gWindowLogFilter, "visible : %d\n", visibleMode );
  DALI_ASSERT_DEBUG(mOverlay);

  DoShowIndicator( mIndicatorOrientation );
}

void Window::Impl::RotateIndicator(Dali::Window::WindowOrientation orientation)
{
  DALI_LOG_TRACE_METHOD_FMT( gWindowLogFilter, "Orientation: %d\n", orientation );

  DoRotateIndicator( orientation );
}

void Window::Impl::SetIndicatorBgOpacity( Dali::Window::IndicatorBgOpacity opacityMode )
{
  mIndicatorOpacityMode = opacityMode;

  if( mIndicator != NULL )
  {
    mIndicator->SetOpacityMode( opacityMode );
  }
}

void Window::Impl::SetClass(std::string name, std::string klass)
{
}

Window::Impl::Window()
: mSurface(NULL),
  mIndicatorStyle(Dali::Window::CHANGEABLE_COLOR),
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
}

Window::Impl::~Window()
{
  delete mEventHandler;

  if ( mAdaptor )
  {
    mAdaptor->RemoveObserver( *this );
    mAdaptor->SetDragAndDropDetector( NULL );
    mAdaptor = NULL;
  }

  delete mSurface;
}

void Window::Impl::Initialize(const PositionSize& windowPosition, const std::string& name)
{
  // create an Wayland window by default
  Any surface;
  Any display;
  mSurface = new ECore::WindowRenderSurface( windowPosition, surface, display, name, mIsTransparent );
  mOrientation = Orientation::New(this);

  // create event handler for Wayland window
  mEventHandler = new EventHandler( this );
}

void Window::Impl::DoShowIndicator( Dali::Window::WindowOrientation lastOrientation )
{
  if( mIndicator == NULL )
  {
    if( mIndicatorVisible != Dali::Window::INVISIBLE )
    {
      mIndicator = new Indicator( mAdaptor, mIndicatorOrientation, mIndicatorStyle, this );
      mIndicator->SetOpacityMode( mIndicatorOpacityMode );
      Dali::Actor actor = mIndicator->GetActor();
      SetIndicatorActorRotation();
      mOverlay->Add(actor);
    }
    // else don't create a hidden indicator
  }
  else // Already have indicator
  {
    if( mIndicatorVisible == Dali::Window::VISIBLE )
    {
      // If we are resuming, and rotation has changed,
      if( mIndicatorIsShown == false && mIndicatorOrientation != mNextIndicatorOrientation )
      {
        // then close current indicator and open new one
        mShowRotatedIndicatorOnClose = true;
        mIndicator->Close(); // May synchronously call IndicatorClosed() callback & 1 level of recursion
        // Don't show actor - will contain indicator for old orientation.
      }
    }
  }

  // set indicator visible mode
  if( mIndicator != NULL )
  {
    mIndicator->SetVisible( mIndicatorVisible );
  }

  bool show = (mIndicatorVisible != Dali::Window::INVISIBLE );
  SetIndicatorProperties( show, lastOrientation );
  mIndicatorIsShown = show;
}

void Window::Impl::DoRotateIndicator( Dali::Window::WindowOrientation orientation )
{
  if( mIndicatorIsShown )
  {
    mShowRotatedIndicatorOnClose = true;
    mNextIndicatorOrientation = orientation;
    mIndicator->Close(); // May synchronously call IndicatorClosed() callback
  }
  else
  {
    // Save orientation for when the indicator is next shown
    mShowRotatedIndicatorOnClose = false;
    mNextIndicatorOrientation = orientation;
  }
}

void Window::Impl::SetIndicatorProperties( bool isShow, Dali::Window::WindowOrientation lastOrientation )
{
}

void Window::Impl::IndicatorTypeChanged(Indicator::Type type)
{
}

void Window::Impl::IndicatorClosed( Indicator* indicator )
{
  DALI_LOG_TRACE_METHOD( gWindowLogFilter );

  if( mShowRotatedIndicatorOnClose )
  {
    Dali::Window::WindowOrientation currentOrientation = mIndicatorOrientation;
    mIndicator->Open(mNextIndicatorOrientation);
    mIndicatorOrientation = mNextIndicatorOrientation;
    SetIndicatorActorRotation();
    DoShowIndicator(currentOrientation);
  }
}

void Window::Impl::SetIndicatorActorRotation()
{
  DALI_LOG_TRACE_METHOD( gWindowLogFilter );
  DALI_ASSERT_DEBUG( mIndicator != NULL );

  Dali::Actor actor = mIndicator->GetActor();
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
}

void Window::Impl::Raise()
{
}

void Window::Impl::Lower()
{
}

void Window::Impl::Activate()
{
}

Dali::DragAndDropDetector Window::Impl::GetDragAndDropDetector() const
{
  return mDragAndDropDetector;
}

void Window::Impl::OnStart()
{
  DoShowIndicator( mIndicatorOrientation );
}

void Window::Impl::OnPause()
{
}

void Window::Impl::OnResume()
{
  // resume indicator status
  if( mIndicator != NULL )
  {
    // Restore own indicator opacity
    // Send opacity mode to indicator service when app resumed
    mIndicator->SetOpacityMode( mIndicatorOpacityMode );
  }
}

void Window::Impl::OnStop()
{
  if( mIndicator )
  {
    mIndicator->Close();
  }

  delete mIndicator;
  mIndicator = NULL;
}

void Window::Impl::OnDestroy()
{
  mAdaptor = NULL;
}

OrientationPtr Window::Impl::GetOrientation()
{
  return mOrientation;
}

void Window::Impl::AddAvailableOrientation(Dali::Window::WindowOrientation orientation)
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

void Window::Impl::RemoveAvailableOrientation(Dali::Window::WindowOrientation orientation)
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

void Window::Impl::SetAvailableOrientations(const std::vector<Dali::Window::WindowOrientation>& orientations)
{
  DALI_ASSERT_ALWAYS( mAvailableOrientations.size() <= 4 && "Incorrect number of available orientations" );
}

const std::vector<Dali::Window::WindowOrientation>& Window::Impl::GetAvailableOrientations()
{
  return mAvailableOrientations;
}

void Window::Impl::SetPreferredOrientation(Dali::Window::WindowOrientation orientation)
{
  mPreferredOrientation = orientation;
}

Dali::Window::Impl::WindowOrientation Window::GetPreferredOrientation()
{
  return mPreferredOrientation;
}

void Window::Impl::RotationDone( int orientation, int width, int height )
{
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////////////

static Window* Window::New(const PositionSize& posSize, const std::string& name, bool isTransparent = false)
{
  Window* window = new Window();
  window->mImpl->mIsTransparent = isTransparent;
  window->mImpl->Initialize(posSize, name);
  return window;
}

Window::Window()
{
  mImpl = new Window::Impl();
}

Window::~Window()
{
  delete mImpl;
}

void Window::SetAdaptor(Dali::Adaptor& adaptor)
{
  mImpl->SetAdaptor(adaptor);
}

RenderSurface* Window::GetSurface()
{
  return mImpl->GetSurface();
}

void Window::SetIndicatorStyle( Dali::Window::IndicatorStyle style )
{
  mImpl->SetIndicatorStyle( style );
}

void Window::ShowIndicator( bool show )
{
  mImpl->ShowIndicator( show );
}

void Window::ShowIndicator( Dali::Window::IndicatorVisibleMode visibleMode )
{
  mImpl->ShowIndicator( visibleMode );
}

void Window::SetIndicatorBgOpacity( Dali::Window::IndicatorBgOpacity opacity )
{
  mImpl->SetIndicatorBgOpacity( opacity );
}

void Window::RotateIndicator( Dali::Window::WindowOrientation orientation )
{
  mImpl->RotateIndicator( orientation );
}

void Window::SetClass( std::string name, std::string klass )
{
  mImpl->SetClass( name, klass );
}

void Window::Raise()
{
  mImpl->Raise();
}

void Window::Lower()
{
  mImpl->Lower();
}

void Activate()
{
  mImpl->Activate();
}

OrientationPtr Window::GetOrientation()
{
  return mImpl->GetOrientation();
}

void Window::AddAvailableOrientation(Dali::Window::WindowOrientation orientation)
{
  mImpl->AddAvailableOrientation( orientation );
}

void Window::RemoveAvailableOrientation(Dali::Window::WindowOrientation orientation)
{
  mImpl->RemoveAvailableOrientation( orientation );
}

void Window::SetAvailableOrientations(const std::vector<Dali::Window::WindowOrientation>& orientations)
{
  mImpl->SetAvailableOrientations( orientations );
}

const std::vector<Dali::Window::WindowOrientation>& Window::GetAvailableOrientations()
{
  return mImpl->GetAvailableOrientations();
}

void Window::SetPreferredOrientation(Dali::Window::WindowOrientation orientation)
{
  return mImpl->SetPreferredOrientation( orientation );
}

Dali::Window::WindowOrientation Window::GetPreferredOrientation()
{
  return mImpl->GetPreferredOrientation();
}

Dali::DragAndDropDetector Window::GetDragAndDropDetector() const
{
  return mImpl->GetDragAndDropDetector();
}

void Window::RotationDone( int orientation, int width, int height )
{
  mImpl->RotationDone( orientation, width, height );
}

Window::Window()
{
  mImpl = new Window::Impl();
}

Window::~Window()
{
  delete mImpl;
}


void Window::IndicatorTypeChanged( Indicator::Type type )
{
  mImpl->IndicatorTypeChanged( type );
}

void Window::IndicatorClosed(Indicator* indicator)
{
  mImpl->IndicatorClosed( indicator );
}

void Window::OnStart()
{
  mImpl->OnStart();
}

void Window::OnPause()
{
  mImpl->OnPause();
}

void Window::OnResume()
{
  mImpl->OnResume();
}

void Window::OnStop()
{
  mImpl->OnResume();
}

void Window::OnDestroy()
{
  mImpl->OnDestroy();
}


} // Adaptor
} // Internal
} // Dali
