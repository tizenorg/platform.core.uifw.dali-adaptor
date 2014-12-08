// EXTERNAL INCLUDES
#include "sdl-application.h"

#include <SDL.h>
#include <SDL_image.h>
#include <X11/Xlib.h>
#include <EGL/egl.h>
#include <SDL_syswm.h>
#include <stdio.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <stropts.h>
#include <unistd.h>

#include <dali/public-api/dali-core.h>
#include <dali/integration-api/events/touch-event-integ.h>

// INTERNAL INCLUDES
#include "base/interfaces/egl-interface.h"
#include "base/frame-time.h"
#include <gl/egl-implementation.h>

#include "platform-abstractions/emscripten/emscripten-callbacks.h"

// emscripten are using SDL version 1.3 that isnt available for download
// but version 2.0 is close to version 1.3 but version 1.2 support removed.
// This is one way it isnt.
#if SDL_MAJOR_VERSION == 2
# define VIDEO_FLAGS SDL_WINDOW_OPENGL
#else
# define VIDEO_FLAGS SDL_OPENGL
#endif

namespace
{

int _kbhit()
{
  static const int STDIN = 0;
  static bool initialized = false;

  if (! initialized)
  {
    // Use termios to turn off line buffering
    termios term;
    tcgetattr(STDIN, &term);
    term.c_lflag &= ~ICANON;
    tcsetattr(STDIN, TCSANOW, &term);
    setbuf(stdin, NULL);
    initialized = true;
  }

  int bytesWaiting;
  ioctl(STDIN, FIONREAD, &bytesWaiting);
  return bytesWaiting;
}


void LogFunction(Dali::Integration::Log::DebugPriority priority, std::string& message)
{
  printf("%s", message.c_str());
}

}

namespace Dali
{

typedef ::Pixmap XPixmap;
typedef ::Window XWindow;
typedef ::Display XDisplay;
typedef ::Screen XScreen;

const unsigned int SdlApplication::DEFAULT_SURFACE_WIDTH = 600;
const unsigned int SdlApplication::DEFAULT_SURFACE_HEIGHT= 480;


SdlApplication::SdlApplication( size_t surfaceWidth,
                                size_t surfaceHeight,
                                float  horizontalDpi,
                                float  verticalDpi )
  : mCore( NULL ),
    mSurfaceWidth( surfaceWidth ),
    mSurfaceHeight( surfaceHeight ),
    mFrame( 0u ),
    mSeconds(0),
    mMicroSeconds(0)
{

  EGLNativeDisplayType  display        = (EGLNativeDisplayType)XOpenDisplay(NULL);
  bool                  isOwnSurface   = true;
  mEglImplementation.InitializeGles( display, isOwnSurface );

  SdlCreateWindow(surfaceWidth, surfaceHeight, "Dali");

  bool                              isWindowType      = true;
  Internal::Adaptor::ColorDepth     depth             = Internal::Adaptor::COLOR_DEPTH_32;
  mEglImplementation.ChooseConfig( isWindowType, depth );

#if defined(EMSCRIPTEN)
  EGLNativeWindowType window = NULL;
#else
  SDL_SysWMinfo info;
  SDL_VERSION(&info.version);
  SDL_GetWMInfo(&info);

  EGLNativeWindowType window = (EGLNativeWindowType)info.info.x11.window;
#endif

  mEglImplementation.CreateSurfaceWindow( window, depth );

  mEglImplementation.CreateContext();

  mEglImplementation.MakeContextCurrent();

  //
  // SDL/EGL setup, now create core
  //
  mCore = Dali::Integration::Core::New(
    mRenderController,
    mPlatformAbstraction,
    mGlAbstraction,
    mGlSyncAbstraction,
    mGestureManager,
    ResourcePolicy::DataRetention::DALI_RETAINS_ALL_DATA);

  mCore->ContextCreated();
  mCore->SurfaceResized( mSurfaceWidth, mSurfaceHeight );
  mCore->SetDpi( horizontalDpi, verticalDpi );

  Dali::Integration::Log::InstallLogFunction( LogFunction );

}

SdlApplication::~SdlApplication()
{
  // Dali::Integration::Log::UninstallLogFunction();
  delete mCore;

  SDL_Quit();

}

void SdlApplication::SdlCreateWindow(size_t surfaceWidth,
                                     size_t surfaceHeight,
                                     const std::string &title)
{
  if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
  {
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

bool SdlApplication::DoUpdate( void )
{

  // pump events
  mCore->ProcessEvents();

  // Update Time values
  static Internal::Adaptor::FrameTime frameTime(mPlatformAbstraction);
  static bool init = false;
  if( !init )
  {
    frameTime.SetMinimumFrameTimeInterval( 16667 );
    init = true;
  }

  static unsigned int frameNo = 0;
  frameNo++;
  frameTime.SetSyncTime(frameNo);

  float lastFrameDelta( 0.0f );
  unsigned int lastSyncTime( 0 );
  unsigned int nextSyncTime( 0 );
  frameTime.PredictNextSyncTime( lastFrameDelta, lastSyncTime, nextSyncTime );

  Integration::UpdateStatus status;

  mCore->Update( lastFrameDelta, lastSyncTime, nextSyncTime, status );

  Dali::Internal::Emscripten::stats.lastFrameDeltaSeconds = lastFrameDelta;
  Dali::Internal::Emscripten::stats.lastSyncTimeMilliseconds = lastSyncTime;
  Dali::Internal::Emscripten::stats.nextSyncTimeMilliseconds = nextSyncTime;

  Dali::Internal::Emscripten::stats.keepUpdating = status.keepUpdating;
  Dali::Internal::Emscripten::stats.needsNotification = status.needsNotification;
  Dali::Internal::Emscripten::stats.secondsFromLastFrame = status.secondsFromLastFrame;

  return true;
}


void SdlApplication::DoRender()
{
  // render
  mCore->Render( mRenderStatus );

  mFrame++;

  Dali::Internal::Emscripten::stats.frameCount = mFrame;

  mEglImplementation.SwapBuffers();

}

void SdlApplication::MainLoop()
{
  while(!_kbhit())
  {
    DoUpdate();
    DoRender();
    sleep(1); // to let kbhit in
  }
}

void SdlApplication::SendTouchEvent(double x, double y, int downUpMotion)
{
  // send dali touch events?
  // enum TouchPoint::State
  // {
  //   Down,        /**< Screen touched */
  //   Up,          /**< Touch stopped */
  //   Motion,      /**< Finger dragged */
  //   Leave,       /**< Leave the boundary of an actor */
  //   Stationary,  /**< No change from last event.  Useful when a multi-touch event occurs where
  //                     all points are sent but indicates that this particular point has not changed
  //                     since the last time */
  //   Interrupted, /**< A system event has occurred which has interrupted the touch event sequence. */
  //   Last         /**< Number of states. */
  // };

  TouchPoint::State state = TouchPoint::Up;
  if( 0 == downUpMotion )
  {
    state = TouchPoint::Down;
  }
  else if( 2 == downUpMotion )
  {
    state = TouchPoint::Motion;
  }
  // else // if( 1 == downUpMotion )
  // {
  //   state = TouchPoint::Up;
  // }

  Dali::Integration::TouchEvent e; // (unsigned long time);
  e.AddPoint( TouchPoint( 0,                        // int id, for device
                          state,                   // TouchPoint::State state,
                          static_cast<float>(x),    //float screenX,
                          static_cast<float>(y) ) );//float screenX,

  mCore->QueueEvent(e);
}

void SdlApplication::SetSurfaceWidth( unsigned int width, unsigned height )
{
  mSurfaceWidth = width;
  mSurfaceHeight = height;

  mCore->SurfaceResized( mSurfaceWidth, mSurfaceHeight );
}

}
