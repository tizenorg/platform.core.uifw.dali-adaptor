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
#include <SDL.h>
#include <SDL_image.h>
#include <X11/Xlib.h>
#include <EGL/egl.h>
#include "SDL_syswm.h"

// emscripten are using SDL version 1.3 that isnt available for download
// but version 2.0 is close to version 1.3 but version 1.2 support removed.
// This is one way it isnt.
#if SDL_MAJOR_VERSION == 2
# define VIDEO_FLAGS SDL_WINDOW_OPENGL
#else
# define VIDEO_FLAGS SDL_OPENGL
#endif

// EXTERNAL HEADERS

#include <dali/integration-api/core.h>
#include <dali/integration-api/system-overlay.h>
#include <dali/public-api/render-tasks/render-task.h>
#include <dali/public-api/render-tasks/render-task-list.h>
#include <orientation.h>

// INTERNAL HEADERS
#include <drag-and-drop-detector-impl.h>
#include <indicator-impl.h>
#include <window-visibility-observer.h>
#include <orientation-impl.h>

// // namespace
// {
// const float INDICATOR_ANIMATION_DURATION( 0.18f ); // 180 milli seconds
// const float INDICATOR_SHOW_Y_POSITION( 0.0f );
// const float INDICATOR_HIDE_Y_POSITION( -52.0f );
// }

namespace Dali
{
namespace Internal
{
namespace Adaptor
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gWindowLogFilter = Debug::Filter::New(Debug::Concise, false, "LOG_WINDOW");
#endif

struct Window::Impl : public OrientationObserver
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
   * @copydoc Dali::Window::SetIndicatorStyle()
   */
  void SetIndicatorStyle( Dali::Window::IndicatorStyle style );

  /**
   * @copydoc Dali::Window::IndicatorTypeChanged()
   */
  void IndicatorTypeChanged( Indicator::Type type );

  /**
   * @copydoc Dali::Window::IndicatorClosed()
   */
  void IndicatorClosed(Indicator* indicator);

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

  void SdlCreateWindow(size_t surfaceWidth,
                       size_t surfaceHeight,
                       const std::string &title);

  // LifeCycleObserver
  virtual void OnStart();

  /**
   * Called when the adaptor is about to pause.
   */
  virtual void OnPause();

  /**
   * Called when the adaptor is about to resume.
   */
  virtual void OnResume();

  /**
   * Called when the adaptor is about to stop.
   */
  virtual void OnStop();

  /**
   * Called when the adaptor is about to be destroyed.
   */
  virtual void OnDestroy();

};

void Window::Impl::SdlCreateWindow(size_t surfaceWidth,
                                   size_t surfaceHeight,
                                   const std::string &title)
{
  if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
    std::cout << "SDL_Init Err" << SDL_GetError() << std::endl;
    DALI_ASSERT_ALWAYS(!"Couldn't initialize SDL");
  }

  // load support for the JPG and PNG image formats
  int flags    = IMG_INIT_JPG | IMG_INIT_PNG;
  int initted  = IMG_Init(flags);
  if( (initted & flags) != flags)
  {
    std::cout << "IMG_Init Err:" << IMG_GetError() << std::endl;
    DALI_ASSERT_ALWAYS("!IMG_Init: Failed to init required jpg and png support!\n");
  }

  // bool fsaa        = true;
  // bool sync        = true;
  float gamma      = 0.0;
  int bpp          = 0; // 0 means current display bpp

  Uint32 video_flags = VIDEO_FLAGS; // /* SDL_ANYFORMAT | */ SDL_OPENGL; // | SDL_FULLSCREEN | SDL_NOFRAME

#ifndef EMSCRIPTEN
  SDL_GL_SetAttribute( SDL_GL_RED_SIZE,   8 ); // DESKTOP (non gles) only
  SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
  SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE,  8 );
  SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );
  SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );
  SDL_GL_SetAttribute( SDL_GL_BUFFER_SIZE, 32 );

  // SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
#endif

  // if ( fsaa ) {
  //   SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 1 );
  //   SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, fsaa );
  // }

  // SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1 );

  // if ( sync ) {
  //   SDL_GL_SetAttribute( SDL_GL_SWAP_CONTROL, 1 );
  // } else {
  //   SDL_GL_SetAttribute( SDL_GL_SWAP_CONTROL, 0 );
  // }

  SDL_Surface *surface = SDL_SetVideoMode( static_cast<int>(surfaceWidth),
                                           static_cast<int>(surfaceHeight),
                                           bpp,
                                           video_flags );

  if ( surface == NULL )
  {
    fprintf(stderr, "Couldn't set GL mode: %s\n", SDL_GetError());
    DALI_ASSERT_ALWAYS("Couldn't set GL mode");
    SDL_Quit();
    exit(1);
  }

  /* Set the window manager title bar */
  SDL_WM_SetCaption( title.c_str(), "daliweb" );

  /* Set the gamma for the window */
  if ( gamma != 0.0 )
  {
    SDL_SetGamma(gamma, gamma, gamma);
  }

}

void Window::Impl::SetAdaptor(Dali::Adaptor& adaptor)
{
  DALI_ASSERT_ALWAYS( !mStarted && "Adaptor already started" );
  mStarted = true;

  // // Only create one overlay per window
  // Internal::Adaptor::Adaptor& adaptorImpl = Internal::Adaptor::Adaptor::GetImplementation(adaptor);
  // Integration::Core& core = adaptorImpl.GetCore();
  // mOverlay = &core.GetSystemOverlay();

  // Dali::RenderTaskList taskList = mOverlay->GetOverlayRenderTasks();
  // taskList.CreateTask();

  mAdaptor = &adaptorImpl;
  mAdaptor->AddObserver( *this );

  // // Can only create the detector when we know the Core has been instantiated.
  // mDragAndDropDetector = DragAndDropDetector::New();
  // mAdaptor->SetDragAndDropDetector( &GetImplementation( mDragAndDropDetector ) );

  if( mOrientation )
  {
    mOrientation->SetAdaptor(adaptor);
  }

}

RenderSurface* Window::Impl::GetSurface()
{
  return mSurface;
}


void Window::Impl::SetIndicatorStyle( Dali::Window::IndicatorStyle /* style */ )
{
}

void Window::Impl::ShowIndicator( bool /*show*/ )
{
}

void Window::Impl::ShowIndicator( Dali::Window::IndicatorVisibleMode /*visibleMode*/ )
{
}

void Window::Impl::RotateIndicator(Dali::Window::WindowOrientation /*orientation*/)
{
}

void Window::Impl::SetIndicatorBgOpacity( Dali::Window::IndicatorBgOpacity /*opacityMode*/ )
{
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
  mAdaptor(NULL),
  mEventHandler(NULL)
{
}

Window::Impl::~Window()
{
  SDL_Quit();
}

void Window::Impl::Initialize(const PositionSize& windowPosition, const std::string& name)
{
  SdlCreateWindow(windowPosition.width, windowPosition.height, name);
  mOrientation = Orientation::New(this);
}

void Window::Impl::DoShowIndicator( Dali::Window::WindowOrientation /*lastOrientation*/ )
{
}

void Window::Impl::DoRotateIndicator( Dali::Window::WindowOrientation /*orientation*/ )
{
}

void Window::Impl::SetIndicatorProperties( bool isShow, Dali::Window::WindowOrientation /*lastOrientation*/ )
{
}

void Window::Impl::IndicatorTypeChanged(Indicator::Type /*type*/)
{
}

void Window::Impl::IndicatorClosed( Indicator* /*indicator*/ )
{
}

void Window::Impl::SetIndicatorActorRotation()
{
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
}

void Window::Impl::OnPause()
{
}

void Window::Impl::OnResume()
{
}

void Window::Impl::OnStop()
{
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

Dali::Window::WindowOrientation Window::Impl::GetPreferredOrientation()
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

void Window::Activate()
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
