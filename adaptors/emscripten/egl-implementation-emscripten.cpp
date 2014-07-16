/*
Copyright (c) 2000-2013 Samsung Electronics Co., Ltd All Rights Reserved

This file is part of Dali Adaptor

PROPRIETARY/CONFIDENTIAL

This software is the confidential and proprietary information of
SAMSUNG ELECTRONICS ("Confidential Information"). You shall not
disclose such Confidential Information and shall use it only in
accordance with the terms of the license agreement you entered
into with SAMSUNG ELECTRONICS.

SAMSUNG make no representations or warranties about the suitability
of the software, either express or implied, including but not limited
to the implied warranties of merchantability, fitness for a particular
purpose, or non-infringement. SAMSUNG shall not be liable for any
damages suffered by licensee as a result of using, modifying or
distributing this software or its derivatives.
*/


// CLASS HEADER
#include <gl/egl-implementation.h>

// EXTERNAL INCLUDES
#include <iostream>
#include <dali/integration-api/debug.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/dali-vector.h>

// INTERNAL INCLUDES
namespace
{


void PrintConfigs(EGLDisplay d)
{
  EGLint numConfigs;

  eglGetConfigs(d, NULL, 0, &numConfigs);
  EGLConfig *configs = new EGLConfig[numConfigs];

  eglGetConfigs(d, configs, numConfigs, &numConfigs);

  printf("Configurations: N=%d\n", numConfigs);
  printf(" - config id\n");
  printf(" - buffer size\n");
  printf(" - level\n");
  printf(" - double buffer\n");
  printf(" - stereo\n");
  printf(" - r, g, b\n");
  printf(" - depth\n");
  printf(" - stencil\n");

  printf("     bf lv d st colorbuffer dp st   supported \n");
  printf("  id sz  l b ro  r  g  b  a th cl   surfaces  \n");
  printf("----------------------------------------------\n");
  for (EGLint i = 0; i < numConfigs; i++) {
    EGLint id, size, level;
    EGLint red, green, blue, alpha;
    EGLint depth, stencil;
    EGLint surfaces;
    EGLint doubleBuf = 1, stereo = 0;
    char surfString[100] = "";

    eglGetConfigAttrib(d, configs[i], EGL_CONFIG_ID, &id);
    eglGetConfigAttrib(d, configs[i], EGL_BUFFER_SIZE, &size);
    eglGetConfigAttrib(d, configs[i], EGL_LEVEL, &level);

    eglGetConfigAttrib(d, configs[i], EGL_RED_SIZE, &red);
    eglGetConfigAttrib(d, configs[i], EGL_GREEN_SIZE, &green);
    eglGetConfigAttrib(d, configs[i], EGL_BLUE_SIZE, &blue);
    eglGetConfigAttrib(d, configs[i], EGL_ALPHA_SIZE, &alpha);
    eglGetConfigAttrib(d, configs[i], EGL_DEPTH_SIZE, &depth);
    eglGetConfigAttrib(d, configs[i], EGL_STENCIL_SIZE, &stencil);
    eglGetConfigAttrib(d, configs[i], EGL_SURFACE_TYPE, &surfaces);

    if (surfaces & EGL_WINDOW_BIT)
      strcat(surfString, "win,");
    if (surfaces & EGL_PBUFFER_BIT)
      strcat(surfString, "pb,");
    if (surfaces & EGL_PIXMAP_BIT)
      strcat(surfString, "pix,");
    if (strlen(surfString) > 0)
      surfString[strlen(surfString) - 1] = 0;

    printf("0x%02x %2d %2d %c  %c %2d %2d %2d %2d %2d %2d   %-12s\n",
           id, size, level,
           doubleBuf ? 'y' : '.',
           stereo ? 'y' : '.',
           red, green, blue, alpha,
           depth, stencil, surfString);
  }

  delete [] configs;
}



}



namespace Dali
{

namespace Internal
{

namespace Adaptor
{

#define TEST_EGL_ERROR(lastCommand) \
{ \
  EGLint err = eglGetError(); \
  if (err != EGL_SUCCESS) \
  { \
    printf("EGL error after %s code=%x\n", lastCommand, err); \
    DALI_LOG_ERROR("EGL error after %s code=%x\n", lastCommand,err); \
    DALI_ASSERT_ALWAYS(0 && "EGL error");                            \
  } \
}

EglImplementation::EglImplementation()
  : mEglNativeDisplay(0),
#ifndef EMSCRIPTEN
    mEglNativeWindow(0),
#endif
    mEglNativePixmap(0),
    mEglDisplay(0),
    mEglConfig(0),
    mEglContext(0),
    mEglSurface(0),
    mGlesInitialized(false),
    mIsOwnSurface(true),
    mSyncMode(FULL_SYNC),
    mContextCurrent(false),
    mIsWindow(true),
    mColorDepth(COLOR_DEPTH_24)
{
}

EglImplementation::~EglImplementation()
{
  TerminateGles();
}

bool EglImplementation::InitializeGles( EGLNativeDisplayType display, bool isOwnSurface )
{
  if ( !mGlesInitialized )
  {
    mEglNativeDisplay = display;

    //@todo see if we can just EGL_DEFAULT_DISPLAY instead
    mEglDisplay = eglGetDisplay(mEglNativeDisplay);

    EGLint majorVersion = 0;
    EGLint minorVersion = 0;
    if ( !eglInitialize( mEglDisplay, &majorVersion, &minorVersion ) )
    {
      return false;
    }
    eglBindAPI(EGL_OPENGL_ES_API);

    mContextAttribs.Clear();

#if DALI_GLES_VERSION >= 30

    mContextAttribs.Reserve(5);
    mContextAttribs.PushBack( EGL_CONTEXT_MAJOR_VERSION_KHR );
    mContextAttribs.PushBack( 3 );
    mContextAttribs.PushBack( EGL_CONTEXT_MINOR_VERSION_KHR );
    mContextAttribs.PushBack( 0 );

#else // DALI_GLES_VERSION >= 30

    mContextAttribs.Reserve(3);
    mContextAttribs.PushBack( EGL_CONTEXT_CLIENT_VERSION );
    mContextAttribs.PushBack( 2 );

#endif // DALI_GLES_VERSION >= 30

    mContextAttribs.PushBack( EGL_NONE );

    mGlesInitialized = true;
    mIsOwnSurface = isOwnSurface;
  }

  return mGlesInitialized;
}

bool EglImplementation::CreateContext()
{
  // make sure a context isn't created twice
  DALI_ASSERT_ALWAYS( (mEglContext == 0) && "EGL context recreated" );
  DALI_ASSERT_ALWAYS( mGlesInitialized );

  printf("----eglQueryAPI: %d", eglQueryAPI());

  mEglContext = eglCreateContext(mEglDisplay, mEglConfig, NULL, &(mContextAttribs[0]));

  //if emscripten ignore this (egl spec says non gles2 implementation must return EGL_BAD_MATCH if it doesnt support gles2)
  // so just ignore error for now....
  // TEST_EGL_ERROR("eglCreateContext render thread");
  // DALI_ASSERT_ALWAYS( EGL_NO_CONTEXT != mEglContext && "EGL context not created" );

  return true;
}

void EglImplementation::DestroyContext()
{
  DALI_ASSERT_ALWAYS( mEglContext && "no EGL context" );

  eglDestroyContext( mEglDisplay, mEglContext );
  mEglContext = 0;
}

void EglImplementation::DestroySurface()
{
  if(mIsOwnSurface && mEglSurface)
  {
    eglDestroySurface( mEglDisplay, mEglSurface );
    mEglSurface = 0;
  }
}

void EglImplementation::MakeContextCurrent()
{
  mContextCurrent = true;

  if(mIsOwnSurface)
  {
    eglMakeCurrent( mEglDisplay, mEglSurface, mEglSurface, mEglContext );
  }

  EGLint error = eglGetError();

  if ( error != EGL_SUCCESS )
  {
    switch (error)
    {
      case EGL_BAD_DISPLAY:
      {
        DALI_LOG_ERROR("EGL_BAD_DISPLAY : Display is not an EGL display connection");
        break;
      }
      case EGL_NOT_INITIALIZED:
      {
        DALI_LOG_ERROR("EGL_NOT_INITIALIZED : Display has not been initialized");
        break;
      }
      case EGL_BAD_SURFACE:
      {
        DALI_LOG_ERROR("EGL_BAD_SURFACE : Draw or read is not an EGL surface");
        break;
      }
      case EGL_BAD_CONTEXT:
      {
        DALI_LOG_ERROR("EGL_BAD_CONTEXT : Context is not an EGL rendering context");
        break;
      }
      case EGL_BAD_MATCH:
      {
        DALI_LOG_ERROR("EGL_BAD_MATCH : Draw or read are not compatible with context, or if context is set to EGL_NO_CONTEXT and draw or read are not set to EGL_NO_SURFACE, or if draw or read are set to EGL_NO_SURFACE and context is not set to EGL_NO_CONTEXT");
        break;
      }
      case EGL_BAD_ACCESS:
      {
        DALI_LOG_ERROR("EGL_BAD_ACCESS : Context is current to some other thread");
        break;
      }
      case EGL_BAD_NATIVE_PIXMAP:
      {
        DALI_LOG_ERROR("EGL_BAD_NATIVE_PIXMAP : A native pixmap underlying either draw or read is no longer valid.");
        break;
      }
      case EGL_BAD_NATIVE_WINDOW:
      {
        DALI_LOG_ERROR("EGL_BAD_NATIVE_WINDOW : A native window underlying either draw or read is no longer valid.");
        break;
      }
      case EGL_BAD_CURRENT_SURFACE:
      {
        DALI_LOG_ERROR("EGL_BAD_CURRENT_SURFACE : The previous context has unflushed commands and the previous surface is no longer valid.");
        break;
      }
      case EGL_BAD_ALLOC:
      {
        DALI_LOG_ERROR("EGL_BAD_ALLOC : Allocation of ancillary buffers for draw or read were delayed until eglMakeCurrent is called, and there are not enough resources to allocate them");
        break;
      }
      case EGL_CONTEXT_LOST:
      {
        DALI_LOG_ERROR("EGL_CONTEXT_LOST : If a power management event has occurred. The application must destroy all contexts and reinitialise OpenGL ES state and objects to continue rendering");
        break;
      }
      default:
      {
        DALI_LOG_ERROR("Unknown error");
        break;
      }
    }
    DALI_ASSERT_ALWAYS(false && "MakeContextCurrent failed!");
  }

  DALI_LOG_WARNING("- EGL Information\nVendor: %s\nVersion: %s\nClient APIs: %s\nExtensions: %s\n",
                   eglQueryString(mEglDisplay, EGL_VENDOR),
                   eglQueryString(mEglDisplay, EGL_VERSION),
                   eglQueryString(mEglDisplay, EGL_CLIENT_APIS),
                   eglQueryString(mEglDisplay, EGL_EXTENSIONS));

  if ( mIsWindow )
  {
    SetRefreshSync( mSyncMode );
  }
}

void EglImplementation::MakeContextNull()
{
  mContextCurrent = false;
  // clear the current context
  eglMakeCurrent( mEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
}

void EglImplementation::TerminateGles()
{
  if ( mGlesInitialized )
  {
    // in latest Mali DDK (r2p3 ~ r3p0 in April, 2012),
    // MakeContextNull should be called before eglDestroy surface
    // to prevent crash in _mali_surface_destroy_callback
    MakeContextNull();

    if(mIsOwnSurface && mEglSurface)
    {
      eglDestroySurface(mEglDisplay, mEglSurface);
    }
    eglDestroyContext(mEglDisplay, mEglContext);

    eglTerminate(mEglDisplay);

    mEglDisplay = NULL;
    mEglConfig  = NULL;
    mEglContext = NULL;
    mEglSurface = NULL;

    mGlesInitialized = false;
  }
}

bool EglImplementation::IsGlesInitialized() const
{
  return mGlesInitialized;
}

bool EglImplementation::SetRefreshSync( SyncMode mode )
{
  if ( mIsWindow == false )
  {
    return false;
  }
  mSyncMode = mode;

  // eglSwapInterval specifies the minimum number of video frame periods
  // per buffer swap for the window associated with the current context.

  if ( !mContextCurrent )
  {
    return true;
  }

  EGLBoolean ok = eglSwapInterval( mEglDisplay, mode );
  if ( !ok )
  {
    TEST_EGL_ERROR("eglSwapInterval");
    return false;
  }

  return true;
}

void EglImplementation::SwapBuffers()
{
  eglSwapBuffers( mEglDisplay, mEglSurface );
}

void EglImplementation::CopyBuffers()
{
  eglCopyBuffers( mEglDisplay, mEglSurface, mEglNativePixmap );
}

void EglImplementation::WaitGL()
{
  eglWaitGL();
}

void EglImplementation::ChooseConfig( bool isWindowType, ColorDepth depth )
{
  std::cout << "DISPLAY CONFIGS" << std::endl;
  PrintConfigs( mEglDisplay );

  if(mEglConfig && isWindowType == mIsWindow && mColorDepth == depth)
  {
    return;
  }

  mIsWindow = isWindowType;

  EGLint numConfigs;
  Vector<EGLint> configAttribs;
  configAttribs.Reserve(31);

  if(isWindowType)
  {
    configAttribs.PushBack( EGL_SURFACE_TYPE );
    configAttribs.PushBack( EGL_WINDOW_BIT );
  }
  else
  {
    DALI_ASSERT_ALWAYS(!"uninplemented");
    configAttribs.PushBack( EGL_SURFACE_TYPE );
    configAttribs.PushBack( EGL_PIXMAP_BIT );
  }

  configAttribs.PushBack( EGL_RENDERABLE_TYPE );

#if DALI_GLES_VERSION >= 30
  DALI_ASSERT_ALWAYS(!"uninplemented");

#ifdef _ARCH_ARM_
  configAttribs.PushBack( EGL_OPENGL_ES3_BIT_KHR );
#else
  // There is a bug in the desktop emulator
  // Requesting for ES3 causes eglCreateContext even though it allows to ask
  // for a configuration that supports GLES 3.0
  configAttribs.PushBack( EGL_OPENGL_ES2_BIT );
#endif // _ARCH_ARM_

#else // DALI_GLES_VERSION >= 30

  configAttribs.PushBack( EGL_OPENGL_ES2_BIT );

#endif //DALI_GLES_VERSION >= 30

#if DALI_GLES_VERSION >= 30
// TODO: enable this flag when it becomes supported
//  configAttribs.PushBack( EGL_CONTEXT_FLAGS_KHR );
//  configAttribs.PushBack( EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE_BIT_KHR );
#endif //DALI_GLES_VERSION >= 30

  configAttribs.PushBack( EGL_RED_SIZE );
  configAttribs.PushBack( 8 );
  configAttribs.PushBack( EGL_GREEN_SIZE );
  configAttribs.PushBack( 8 );
  configAttribs.PushBack( EGL_BLUE_SIZE );
  configAttribs.PushBack( 8 );

  //
  // Setting the alpha crashed .... need SDL_SetVideo(...) with alpha somehow??
  //

  // if(depth == COLOR_DEPTH_32)
  // {
  configAttribs.PushBack( EGL_ALPHA_SIZE );
  configAttribs.PushBack( 8 );
  // }
  // else
  // {
  //   configAttribs.PushBack( EGL_ALPHA_SIZE );
  //   configAttribs.PushBack( 0 );
  // }

  configAttribs.PushBack( EGL_DEPTH_SIZE );
  configAttribs.PushBack( 24 );
  // configAttribs.PushBack( EGL_STENCIL_SIZE );
  // configAttribs.PushBack( 8 );
  // configAttribs.PushBack( EGL_SAMPLES );
  // configAttribs.PushBack( 4 );
  // configAttribs.PushBack( EGL_SAMPLE_BUFFERS );
  // configAttribs.PushBack( 1 );

  configAttribs.PushBack( EGL_NONE );

  std::cout << "CHOSEN CONFIGS" << std::endl;

  PrintConfigs( mEglDisplay );

  int newline = 0;
  for(Vector<EGLint>::Iterator iter = configAttribs.Begin(); iter != configAttribs.End(); ++iter)
  {
    std::cout << *iter << " " << std::endl;
    if(newline % 2)
    {
      std::cout << std::endl;
    }
    newline++;
  }

  if ( eglChooseConfig( mEglDisplay, &(configAttribs[0]), &mEglConfig, 1, &numConfigs ) != EGL_TRUE )
  {
    EGLint error = eglGetError();
    switch (error)
    {
      case EGL_BAD_DISPLAY:
      {
        DALI_LOG_ERROR("Display is not an EGL display connection");
        break;
      }
      case EGL_BAD_ATTRIBUTE:
      {
        DALI_LOG_ERROR("The parameter confirAttribs contains an invalid frame buffer configuration attribute or an attribute value that is unrecognized or out of range");
        break;
      }
      case EGL_NOT_INITIALIZED:
      {
        DALI_LOG_ERROR("Display has not been initialized");
        break;
      }
      case EGL_BAD_PARAMETER:
      {
        DALI_LOG_ERROR("The parameter numConfig is NULL");
        break;
      }
      default:
      {
        DALI_LOG_ERROR("Unknown error");
      }
    }
    DALI_ASSERT_ALWAYS(false && "eglChooseConfig failed!");
  }

  if ( numConfigs != 1 )
  {
    DALI_LOG_ERROR("No configurations found.");

    TEST_EGL_ERROR("eglChooseConfig");
    // DALI_ASSERT_ALWAYS(!"no configs found");
  }
}


void EglImplementation::CreateSurfaceWindow( EGLNativeWindowType window, ColorDepth depth )
{
  DALI_ASSERT_ALWAYS( ( mEglSurface == 0 ) && "EGL surface already exists" );

  mColorDepth = depth;
  mIsWindow = true;

  // egl choose config

#ifdef EMSCRIPTEN

  static_cast<void>(window);
  EGLNativeWindowType dummyWindow = NULL;

  mEglSurface = eglCreateWindowSurface( mEglDisplay, mEglConfig, dummyWindow, NULL );

#else

  mEglNativeWindow = window;

  ChooseConfig(mIsWindow, mColorDepth);

  mEglSurface = eglCreateWindowSurface( mEglDisplay, mEglConfig, mEglNativeWindow, NULL );

#endif

  TEST_EGL_ERROR("eglCreateWindowSurface");

  DALI_ASSERT_ALWAYS( mEglSurface && "Create window surface failed" );
}

void EglImplementation::CreateSurfacePixmap( EGLNativePixmapType pixmap, ColorDepth depth )
{
  DALI_ASSERT_ALWAYS( mEglSurface == 0 && "Cannot create more than one instance of surface pixmap" );

  mEglNativePixmap = pixmap;
  mColorDepth = depth;
  mIsWindow = false;

  // egl choose config
  ChooseConfig(mIsWindow, mColorDepth);

  mEglSurface = eglCreatePixmapSurface( mEglDisplay, mEglConfig, mEglNativePixmap, NULL );
  TEST_EGL_ERROR("eglCreatePixmapSurface");

  DALI_ASSERT_ALWAYS( mEglSurface && "Create pixmap surface failed" );
}

bool EglImplementation::ReplaceSurfaceWindow( EGLNativeWindowType window, EGLNativeDisplayType display )
{
  DALI_ASSERT_ALWAYS(!"Unimplemented");

  bool contextLost = false;

  //  the surface is bound to the context, so set the context to null
  MakeContextNull();

  // destroy the surface
  DestroySurface();

  // If the display has not changed, then we can just create a new surface
  if ( display == mEglNativeDisplay )
  {
    // create the EGL surface
    CreateSurfaceWindow( window, mColorDepth );

    // set the context to be current with the new surface
    MakeContextCurrent();
  }
  else  // the display has changed, we need to start egl with a new x-connection
  {
    // Note! this code path is untested

    // this will release all EGL specific resources
    eglTerminate( mEglDisplay );

    mGlesInitialized = false;

    // let the adaptor know that all resources have been lost
    contextLost = true;

    // re-initialise GLES with the new connection
    InitializeGles( display, true );

    // create the EGL surface
    CreateSurfaceWindow( window, mColorDepth );

    // create the OpenGL context
    CreateContext();

    // Make it current
    MakeContextCurrent();
  }

  return contextLost;
}

bool EglImplementation::ReplaceSurfacePixmap( EGLNativePixmapType pixmap, EGLNativeDisplayType display )
{
  bool contextLost = false;

  //  the surface is bound to the context, so set the context to null
  MakeContextNull();

  // destroy the surface
  DestroySurface();

  // If the display has not changed, then we can just create a new surface
  if ( display == mEglNativeDisplay )
  {
    // create the EGL surface
    CreateSurfacePixmap( pixmap, mColorDepth );

    // set the context to be current with the new surface
    MakeContextCurrent();
  }
  else  // the display has changed, we need to start egl with a new x-connection
  {
    // Note! this code path is untested

    // this will release all EGL specific resources
    eglTerminate( mEglDisplay );

    mGlesInitialized = false;

    // let the adaptor know that all resources have been lost
    contextLost = true;

    // re-initialise GLES with the new connection
    InitializeGles( display, true );

    // create the EGL surface
    CreateSurfacePixmap( pixmap, mColorDepth );

    // create the OpenGL context
    CreateContext();

    // Make it current
    MakeContextCurrent();
  }
  return contextLost;
}

EGLDisplay EglImplementation::GetDisplay() const
{
  return mEglDisplay;
}

EGLDisplay EglImplementation::GetContext() const
{
  return mEglContext;
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

