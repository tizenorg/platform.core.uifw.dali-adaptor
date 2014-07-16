#include "sdl-application.h"
#include <SDL.h>
#include <SDL_image.h>
#include <X11/Xlib.h>
#include <EGL/egl.h>
#include "base/interfaces/egl-interface.h"
#include "base/frame-time.h"
#include "SDL_syswm.h"
#include <gl/egl-implementation.h>
#include <dali/public-api/dali-core.h>

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
    mGestureManager );

  mCore->ContextCreated();
  mCore->SurfaceResized( mSurfaceWidth, mSurfaceHeight );
  mCore->SetDpi( horizontalDpi, verticalDpi );

  // Dali::Integration::Log::LogFunction logFunction(&SdlApplication::LogMessage);
  // unsigned int logOpts = Dali::Integration::Log::ParseLogOptions("");
  // Dali::Integration::Log::InstallLogFunction(logFunction, logOpts);

  Dali::Integration::Log::InstallLogFunction( LogFunction );

//   //
//   // Add some stuff
//   //
// #ifdef EMSCRIPTEN
//   std::string filename("moon-on-a-stick.png");
// #else
//   std::string filename("/home/likewise-open/SERILOCAL/lee.morgan/work-3/emscripten-adaptor-as-executable/moon-on-a-stick.png");
// #endif

  Dali::Stage stage = Dali::Stage::GetCurrent();

//   Dali::Image img = Image::New(filename);

//   // Dali::ImageActor ia = Dali::ImageActor::New(img);


//   // stage.Add(ia);

//   // stage.SetBackgroundColor(Stage::DEBUG_BACKGROUND_COLOR);

//   for( int i =0; i < 100; ++i)
//   {

//     Dali::ImageActor ia = Dali::ImageActor::New(img);

//     ia.SetParentOrigin(Dali::ParentOrigin::CENTER);
//     ia.SetAnchorPoint(Dali::AnchorPoint::CENTER);
//     ia.SetSize( Dali::Vector2(50,50) );

//     int x = rand() % 100 + 1;
//     int y = rand() % 100 + 1;
//     int z = rand() % 100 + 1;

//     ia.SetPosition( Vector3(x, y, z) );

//     Dali::Animation anim = Dali::Animation::New(8.f);

//     anim.RotateBy(ia, Degree(360), Vector3::ZAXIS);
//     anim.SetLooping(true);
//     anim.Play();

//     stage.Add(ia);

//   }
  stage.SetBackgroundColor(Dali::Stage::DEBUG_BACKGROUND_COLOR);


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


// void IncrementGetTimeResult(size_t milliseconds)
// {
//   mMicroSeconds += milliseconds * 1000u;
//   unsigned int additionalSeconds = mMicroSeconds / 1000000u;

//   mSeconds += additionalSeconds;
//   mMicroSeconds -= additionalSeconds * 1000000u;
// }

#include <stdio.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <stropts.h>

int _kbhit() {
  static const int STDIN = 0;
  static bool initialized = false;

  if (! initialized) {
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


#include <unistd.h>

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

  // int x = EM_ASM_INT({
  //     console.log('nextSyncTime: ' + [$0,$1,$2]);
  //   }, lastFrameDelta, lastSyncTime, nextSyncTime);

  mCore->Update( lastFrameDelta, lastSyncTime, nextSyncTime, status );
  return true;
}


void SdlApplication::DoRender()
{
  // render
  mCore->Render( mRenderStatus );

  mFrame++;

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


  // static void SdlApplication::LogMessage(Dali::Integration::Log::DebugPriority level, std::string& message)
  // {
  //   switch(level)
  //   {
  //     case Dali::Integration::Log::DebugInfo:
  //       tet_printf("INFO: %s", message.c_str());
  //       break;
  //     case Dali::Integration::Log::DebugWarning:
  //       tet_printf("WARN: %s", message.c_str());
  //       break;
  //     case Dali::Integration::Log::DebugError:
  //       tet_printf("ERROR: %s", message.c_str());
  //       break;
  //     case Dali::Integration::Log::DebugResources:
  //       tet_printf("INFO: %s", message.c_str());
  //       break;
  //     default:
  //       tet_printf("DEFAULT: %s", message.c_str());
  //       break;
  //   }
  // }

  // Dali::Integration::Core& SdlApplication::GetCore()
  // {
  //   return *mCore;
  // }

  // SdlPlatformAbstraction& SdlApplication::GetPlatform()
  // {
  //   return mPlatformAbstraction;
  // }

  // SdlRenderController& SdlApplication::GetRenderController()
  // {
  //   return mRenderController;
  // }

  // SdlGlAbstraction& SdlApplication::GetGlAbstraction()
  // {
  //   return mGlAbstraction;
  // }

  // SdlGestureManager& SdlApplication::GetGestureManager()
  // {
  //   return mGestureManager;
  // }

  // void SdlApplication::SendNotification()
  // {
  //   Integration::NotificationEvent event;
  //   mCore->SendEvent(event);
  // }

  void SdlApplication::SetSurfaceWidth( unsigned int width, unsigned height )
  {
    mSurfaceWidth = width;
    mSurfaceHeight = height;

    mCore->SurfaceResized( mSurfaceWidth, mSurfaceHeight );
  }

  // bool SdlApplication::Render( unsigned int intervalMilliseconds = DEFAULT_RENDER_INTERVAL )
  // {
  //   // Update Time values
  //   mPlatformAbstraction.IncrementGetTimeResult( intervalMilliseconds );
  //   unsigned int seconds(0u), microseconds(0u);
  //   mPlatformAbstraction.GetTimeMicroseconds( seconds, microseconds );

  //   mCore->VSync( mFrame, seconds, microseconds );
  //   mCore->Update( mStatus );
  //   mCore->Render( mRenderStatus );

  //   mFrame++;

  //   return mStatus.KeepUpdating() || mRenderStatus.NeedsUpdate();
  // }

  // bool SdlApplication::UpdateOnly( unsigned int intervalMilliseconds = DEFAULT_RENDER_INTERVAL )
  // {
  //   // Update Time values
  //   mPlatformAbstraction.IncrementGetTimeResult( intervalMilliseconds );
  //   unsigned int seconds(0u), microseconds(0u);
  //   mPlatformAbstraction.GetTimeMicroseconds( seconds, microseconds );

  //   mCore->VSync( mFrame, seconds, microseconds );
  //   mCore->Update( mStatus );

  //   return mStatus.KeepUpdating();
  // }

  // bool SdlApplication::RenderOnly( )
  // {
  //   // Update Time values
  //   mCore->Render( mRenderStatus );

  //   mFrame++;

  //   return mRenderStatus.NeedsUpdate();
  // }

  // void SdlApplication::ResetContext()
  // {
  //   mCore->ContextToBeDestroyed();
  //   mCore->ContextCreated();
  // }

}
