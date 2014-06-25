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
#ifdef WAYLAND
#include <Ecore_Wayland.h>
#else
#include <Ecore_X.h>
#endif

#include <dali/integration-api/core.h>
#include <dali/integration-api/system-overlay.h>
#include <dali/public-api/render-tasks/render-task.h>
#include <dali/public-api/render-tasks/render-task-list.h>
#include <orientation.h>

// INTERNAL HEADERS
#ifdef WAYLAND
#include <ecore-wl/wl-window-render-surface.h>
#else
#include <ecore-x/window-render-surface.h>
#endif
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
#ifndef WAYLAND
    mWindowPropertyHandler( ecore_event_handler_add( ECORE_X_EVENT_WINDOW_PROPERTY,  EcoreEventWindowPropertyChanged, this ) ),
    mClientMessagehandler( ecore_event_handler_add( ECORE_X_EVENT_CLIENT_MESSAGE,  EcoreEventClientMessage, this ) ),
#endif
    mEcoreWindow( 0 )
  {
#ifndef WAYLAND
    // store ecore window handle
    ECoreX::WindowRenderSurface* x11Window( dynamic_cast< ECoreX::WindowRenderSurface * >( mWindow->mSurface ) );
    if( x11Window )
    {
      mEcoreWindow = x11Window->GetXWindow();
    }
    DALI_ASSERT_ALWAYS( mEcoreWindow != 0 && "There is no ecore x window");

    // set property on window to get deiconify approve client message
    unsigned int tmp = 1;
    ecore_x_window_prop_card32_set(mEcoreWindow,
                             ECORE_X_ATOM_E_DEICONIFY_APPROVE,
                             &tmp, 1);
#endif
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
#ifdef WAYLAND
    return EINA_FALSE;
#else
    Ecore_X_Event_Window_Property* propertyChangedEvent( (Ecore_X_Event_Window_Property*)event );
    EventHandler* handler( (EventHandler*)data );
    Eina_Bool handled( ECORE_CALLBACK_PASS_ON );

    if ( handler && handler->mWindow )
    {
      WindowVisibilityObserver* observer( handler->mWindow->mAdaptor );
      if ( observer && ( propertyChangedEvent->win == handler->mEcoreWindow ) )
      {
        Ecore_X_Window_State_Hint state( ecore_x_icccm_state_get( propertyChangedEvent->win ) );

        switch ( state )
        {
          case ECORE_X_WINDOW_STATE_HINT_WITHDRAWN:
          {
            // Window was hidden.
            observer->OnWindowHidden();
            DALI_LOG_INFO( gWindowLogFilter, Debug::General, "Window (%d) Withdrawn\n", handler->mEcoreWindow );
            handled = ECORE_CALLBACK_DONE;
          }
          break;

          case ECORE_X_WINDOW_STATE_HINT_ICONIC:
          {
            // Window was iconified (minimised).
            observer->OnWindowHidden();
            DALI_LOG_INFO( gWindowLogFilter, Debug::General, "Window (%d) Iconfied\n", handler->mEcoreWindow );
            handled = ECORE_CALLBACK_DONE;
          }
          break;

          case ECORE_X_WINDOW_STATE_HINT_NORMAL:
          {
            // Window was shown.
            observer->OnWindowShown();
            DALI_LOG_INFO( gWindowLogFilter, Debug::General, "Window (%d) Shown\n", handler->mEcoreWindow );
            handled = ECORE_CALLBACK_DONE;
          }
          break;

          default:
            // Ignore
            break;
        }
      }
    }

    return handled;
#endif
  }

  /// Called when the window properties are changed.
  static Eina_Bool EcoreEventClientMessage( void* data, int type, void* event )
  {
#ifdef WAYLAND
    return EINA_FALSE;
#else
    Eina_Bool handled( ECORE_CALLBACK_PASS_ON );
    Ecore_X_Event_Client_Message* clientMessageEvent( (Ecore_X_Event_Client_Message*)event );
    EventHandler* handler( (EventHandler*)data );

    if (clientMessageEvent->message_type == ECORE_X_ATOM_E_DEICONIFY_APPROVE)
    {
      ECoreX::WindowRenderSurface* x11Window( dynamic_cast< ECoreX::WindowRenderSurface * >( handler->mWindow->mSurface ) );
      WindowVisibilityObserver* observer( handler->mWindow->mAdaptor );

      if ( observer && ( (unsigned int)clientMessageEvent->data.l[0] == handler->mEcoreWindow ) )
      {
        if (clientMessageEvent->data.l[1] == 0) //wm sends request message using value 0
        {
          observer->OnWindowShown();

          // request to approve the deiconify. render-surface should send proper event after real rendering
          if(x11Window)
          {
            x11Window->RequestToApproveDeiconify();
          }

          handled = ECORE_CALLBACK_DONE;
        }
      }
    }

    return handled;
#endif
  }

  // Data
  Window* mWindow;
  Ecore_Event_Handler* mWindowPropertyHandler;
  Ecore_Event_Handler* mClientMessagehandler;
#ifdef WAYLAND
  Ecore_Wl_Window* mEcoreWindow;
#else
  Ecore_X_Window mEcoreWindow;
#endif
};


Window* Window::New(const PositionSize& posSize, const std::string& name, bool isTransparent)
{
  Window* window = new Window();
  window->mIsTransparent = isTransparent;
  window->Initialize(posSize, name);
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

  if( mOrientation )
  {
    mOrientation->SetAdaptor(adaptor);
  }

  if( mIndicator != NULL )
  {
    mIndicator->SetAdaptor(mAdaptor);
  }
}

RenderSurface* Window::GetSurface()
{
  return mSurface;
}

void Window::SetIndicatorStyle( Dali::Window::IndicatorStyle style )
{
  mIndicatorStyle = style;
}

void Window::ShowIndicator( bool show )
{
  DALI_LOG_TRACE_METHOD_FMT( gWindowLogFilter, "%s\n", show?"SHOW":"HIDE" );
  DALI_ASSERT_DEBUG(mOverlay);

  if(show)
  {
    mIndicatorVisible = Dali::Window::VISIBLE;
  }
  else
  {
    mIndicatorVisible = Dali::Window::INVISIBLE;
  }

  DoShowIndicator( mIndicatorOrientation );
}

void Window::ShowIndicator( Dali::Window::IndicatorVisibleMode visibleMode )
{
  DALI_LOG_TRACE_METHOD_FMT( gWindowLogFilter, "visible : %d\n", visibleMode );
  DALI_ASSERT_DEBUG(mOverlay);

#ifndef WAYLAND
  ECoreX::WindowRenderSurface* x11Window = dynamic_cast< ECoreX::WindowRenderSurface * >( mSurface );
  DALI_ASSERT_DEBUG(x11Window);
  Ecore_X_Window xWinId = x11Window->GetXWindow();

  mIndicatorVisible = visibleMode;

  if ( mIndicatorVisible == Dali::Window::VISIBLE )
  {
    // when the indicator is visible, set proper mode for indicator server according to bg mode
    if ( mIndicatorOpacityMode == Dali::Window::OPAQUE )
    {
      ecore_x_e_illume_indicator_opacity_set(xWinId, ECORE_X_ILLUME_INDICATOR_OPAQUE);
    }
    else if ( mIndicatorOpacityMode == Dali::Window::TRANSLUCENT )
    {
      ecore_x_e_illume_indicator_opacity_set(xWinId, ECORE_X_ILLUME_INDICATOR_TRANSLUCENT);
    }
#if defined(DALI_PROFILE_MOBILE)
    else if ( mIndicatorOpacityMode == Dali::Window::TRANSPARENT )
    {
      ecore_x_e_illume_indicator_opacity_set(xWinId, ECORE_X_ILLUME_INDICATOR_BG_TRANSPARENT);
    }
#endif
  }
  else
  {
    // when the indicator is not visible, set TRANSPARENT mode for indicator server
    ecore_x_e_illume_indicator_opacity_set(xWinId, ECORE_X_ILLUME_INDICATOR_TRANSPARENT); // it means hidden indicator
  }
#endif

  DoShowIndicator( mIndicatorOrientation );
}

void Window::RotateIndicator(Dali::Window::WindowOrientation orientation)
{
  DALI_LOG_TRACE_METHOD_FMT( gWindowLogFilter, "Orientation: %d\n", orientation );

  DoRotateIndicator( orientation );
}

void Window::SetIndicatorBgOpacity( Dali::Window::IndicatorBgOpacity opacityMode )
{
  mIndicatorOpacityMode = opacityMode;

  if( mIndicator != NULL )
  {
    mIndicator->SetOpacityMode( opacityMode );
  }
}

void Window::SetClass(std::string name, std::string klass)
{
  // Get render surface's x11 window
  if( mSurface )
  {
#ifndef WAYLAND
    ECoreX::WindowRenderSurface* x11Window = dynamic_cast< ECoreX::WindowRenderSurface * >( mSurface );
    if( x11Window )
    {
      ecore_x_icccm_name_class_set( x11Window->GetXWindow(), name.c_str(), klass.c_str() );
    }
#endif
  }
}

Window::Window()
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

Window::~Window()
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

void Window::Initialize(const PositionSize& windowPosition, const std::string& name)
{
  // create an X11 window by default
  Any surface;
  Any display;
#ifdef WAYLAND
  mSurface = new ECoreWayland::WindowRenderSurface( windowPosition, surface, display, name, mIsTransparent );
#else
  mSurface = new ECoreX::WindowRenderSurface( windowPosition, surface, display, name, mIsTransparent );
#endif
  mOrientation = Orientation::New(this);

  // create event handler for X11 window
  mEventHandler = new EventHandler( this );
}

void Window::DoShowIndicator( Dali::Window::WindowOrientation lastOrientation )
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

void Window::DoRotateIndicator( Dali::Window::WindowOrientation orientation )
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

void Window::SetIndicatorProperties( bool isShow, Dali::Window::WindowOrientation lastOrientation )
{
#ifndef WAYLAND
  ECoreX::WindowRenderSurface* x11Window = dynamic_cast< ECoreX::WindowRenderSurface * >( mSurface );
  if( x11Window )
  {
    Ecore_X_Window win = x11Window->GetXWindow();

    int show_state = static_cast<int>( isShow );
    ecore_x_window_prop_property_set( win,
                                      ECORE_X_ATOM_E_ILLUME_INDICATOR_STATE,
                                      ECORE_X_ATOM_CARDINAL, 32, &show_state, 1);

    if ( isShow )
    {
      ecore_x_e_illume_indicator_state_set(win, ECORE_X_ILLUME_INDICATOR_STATE_ON);
    }
    else
    {
      ecore_x_e_illume_indicator_state_set(win, ECORE_X_ILLUME_INDICATOR_STATE_OFF);
    }
  }
#endif
}

void Window::IndicatorTypeChanged(Indicator::Type type)
{
#ifndef WAYLAND
  ECoreX::WindowRenderSurface* x11Window = dynamic_cast< ECoreX::WindowRenderSurface * >( mSurface );
  if( x11Window )
  {
    Ecore_X_Window win = x11Window->GetXWindow();
    switch(type)
    {
      case Indicator::INDICATOR_TYPE_1:
        ecore_x_e_illume_indicator_type_set( win, ECORE_X_ILLUME_INDICATOR_TYPE_1 );
        break;

      case Indicator::INDICATOR_TYPE_2:
        ecore_x_e_illume_indicator_type_set( win, ECORE_X_ILLUME_INDICATOR_TYPE_2 );
        break;

      case Indicator::INDICATOR_TYPE_UNKNOWN:
      default:
        break;
    }
  }
#endif
}

void Window::IndicatorClosed( Indicator* indicator )
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

void Window::SetIndicatorActorRotation()
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

void Window::Raise()
{
#ifndef WAYLAND
  ECoreX::WindowRenderSurface* x11Window = dynamic_cast< ECoreX::WindowRenderSurface * >( mSurface );
  if( x11Window )
  {
    Ecore_X_Window win = x11Window->GetXWindow();
    ecore_x_window_raise(win);
  }
#endif
}

void Window::Lower()
{
#ifndef WAYLAND
  ECoreX::WindowRenderSurface* x11Window = dynamic_cast< ECoreX::WindowRenderSurface * >( mSurface );
  if( x11Window )
  {
    Ecore_X_Window win = x11Window->GetXWindow();
    ecore_x_window_lower(win);
  }
#endif
}

void Window::Activate()
{
#ifndef WAYLAND
  ECoreX::WindowRenderSurface* x11Window = dynamic_cast< ECoreX::WindowRenderSurface * >( mSurface );
  if( x11Window )
  {
    Ecore_X_Window win = x11Window->GetXWindow();
    ecore_x_netwm_client_active_request(ecore_x_window_root_get(win), win, 1 /* request type, 1:application, 2:pager */, 0);
  }
#endif
}

Dali::DragAndDropDetector Window::GetDragAndDropDetector() const
{
  return mDragAndDropDetector;
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
  DALI_ASSERT_ALWAYS( mAvailableOrientations.size() <= 4 && "Incorrect number of available orientations" );

#ifndef WAYLAND
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

#ifndef WAYLAND
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
#ifndef WAYLAND
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


} // Adaptor
} // Internal
} // Dali
