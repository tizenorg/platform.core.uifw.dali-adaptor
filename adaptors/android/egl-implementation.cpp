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
#include <dali/integration-api/debug.h>
#include <dali/public-api/common/dali-common.h>

// INTERNAL INCLUDES
#include <android-render-surface.h>

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
    DALI_LOG_ERROR("EGL error after %s code=%x\n", lastCommand,err); \
    DALI_ASSERT_ALWAYS(0 && "EGL error");                            \
  } \
}

EglImplementation::EglImplementation()
  : mEglNativeDisplay(0),
    mEglNativeWindow(0),
    mEglNativePixmap(0),
    mEglDisplay(0),
    mEglConfig(0),
    mEglContext(0),
    mEglSecondaryContext(0),
    mEglSurface(0),
    mGlesInitialized(false),
    mIsOwnSurface(true),
    mContextCurrent(false),
    mIsWindow(true)
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

    mContextAttribs[0] = EGL_CONTEXT_CLIENT_VERSION;
    mContextAttribs[1] = 2;
    mContextAttribs[2] = EGL_NONE;

    mGlesInitialized = true;
    mIsOwnSurface = isOwnSurface;
  }

  return mGlesInitialized;
}

bool EglImplementation::CreateContext()
{
  // make sure a context isn't created twice
  DALI_ASSERT_ALWAYS( (mEglContext == 0) && "EGL context recreated" );

  mEglContext = eglCreateContext(mEglDisplay, mEglConfig, NULL, mContextAttribs);
  TEST_EGL_ERROR("eglCreateContext render thread");

  DALI_ASSERT_ALWAYS( EGL_NO_CONTEXT != mEglContext && "EGL context not created" );

  return true;
}

bool EglImplementation::CreateSecondaryContext()
{
  // make sure a context isn't created twice
  if (!mEglSecondaryContext)
  {
    mEglSecondaryContext = eglCreateContext(mEglDisplay, mEglConfig, NULL, mContextAttribs);
    TEST_EGL_ERROR("eglCreateContext render thread");

    DALI_ASSERT_ALWAYS( EGL_NO_CONTEXT != mEglSecondaryContext && "EGL secondary context not created" );
  }
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

  if ( eglGetError() != EGL_SUCCESS )
  {
    DALI_LOG_ERROR("make current failed!\n");
    exit(0);
  }

  DALI_LOG_WARNING("- EGL Information\nVendor: %s\nVersion: %s\nClient APIs: %s\nExtensions: %s\n",
                   eglQueryString(mEglDisplay, EGL_VENDOR),
                   eglQueryString(mEglDisplay, EGL_VERSION),
                   eglQueryString(mEglDisplay, EGL_CLIENT_APIS),
                   eglQueryString(mEglDisplay, EGL_EXTENSIONS));

  if ( mIsWindow )
  {
    //SetRefreshSync( mSyncMode );
  }
}

void EglImplementation::MakeSecondaryContextCurrent()
{
  CreateSecondaryContext();

  mContextCurrent = false;

  eglMakeCurrent( mEglDisplay, 0, 0, mEglSecondaryContext );

  if ( eglGetError() != EGL_SUCCESS )
  {
    DALI_LOG_ERROR("make current failed!\n");
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

    if(mIsOwnSurface)
    {
      eglDestroySurface(mEglDisplay, mEglSurface);
    }
    eglDestroyContext(mEglDisplay, mEglContext);
    eglDestroyContext(mEglDisplay, mEglSecondaryContext);

    // DO NOT use eglTerminate for Pixmap surface : it doesn't consider other egl context
    if( mIsWindow )
    {
      eglTerminate(mEglDisplay);
    }

    mEglDisplay = NULL;
    mEglConfig  = NULL;
    mEglContext = NULL;
    mEglSecondaryContext = NULL;
    mEglSurface = NULL;

    mGlesInitialized = false;
  }
}

bool EglImplementation::IsGlesInitialized() const
{
  return mGlesInitialized;
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
  if(mEglConfig && isWindowType == mIsWindow && mColorDepth == depth)
  {
    return;
  }

  EGLint numConfigs;
  EGLint configAttribs[40];
  int n = 0;

  if(isWindowType)
  {
    configAttribs[n++] = EGL_SURFACE_TYPE;
    configAttribs[n++] = EGL_WINDOW_BIT;
  }
  else
  {
    configAttribs[n++] = EGL_SURFACE_TYPE;
    configAttribs[n++] = EGL_PIXMAP_BIT;
  }

  configAttribs[n++] = EGL_RENDERABLE_TYPE;
  configAttribs[n++] = EGL_OPENGL_ES2_BIT;

  configAttribs[n++] = EGL_RED_SIZE;
  configAttribs[n++] = 8;
  configAttribs[n++] = EGL_GREEN_SIZE;
  configAttribs[n++] = 8;
  configAttribs[n++] = EGL_BLUE_SIZE;
  configAttribs[n++] = 8;

  if(depth == COLOR_DEPTH_32)
  {
    configAttribs[n++] = EGL_ALPHA_SIZE;
    configAttribs[n++] = 8;
  }
  else
  {
    configAttribs[n++] = EGL_ALPHA_SIZE;
    configAttribs[n++] = 0;
  }

  configAttribs[n++] = EGL_DEPTH_SIZE;
  configAttribs[n++] = 24;
  configAttribs[n++] = EGL_STENCIL_SIZE;
  configAttribs[n++] = 8;
#if 0
  configAttribs[n++] = EGL_SAMPLES;
  configAttribs[n++] = 4;
  configAttribs[n++] = EGL_SAMPLE_BUFFERS;
  configAttribs[n++] = 1;
#endif
  configAttribs[n++] = EGL_NONE;

  if ( !eglChooseConfig( mEglDisplay, configAttribs, &mEglConfig, 1, &numConfigs ) || numConfigs != 1 )
  {
    TEST_EGL_ERROR("eglChooseConfig");
    return;
  }
}


void EglImplementation::CreateSurfaceWindow( EGLNativeWindowType window, ColorDepth depth )
{
  DALI_ASSERT_ALWAYS( ( mEglSurface == 0 ) && "EGL surface already exists" );

  mEglNativeWindow = window;
  mColorDepth = depth;
  mIsWindow = true;

  // egl choose config
  ChooseConfig(mIsWindow, mColorDepth);

  mEglSurface = eglCreateWindowSurface( mEglDisplay, mEglConfig, mEglNativeWindow, NULL );
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

    // context is lost, need to create new
    CreateContext();

    // set the context to be current with the new surface
    MakeContextCurrent();

    // let the adaptor know that all resources have been lost
    contextLost = true;
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
    InitializeGles( display );

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
    InitializeGles( display );

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
