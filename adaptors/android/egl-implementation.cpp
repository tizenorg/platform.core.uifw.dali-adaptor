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

// EXTERNAL INCLUDES
#include <xf86drm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <boost/thread.hpp>

#include <dali/integration-api/core.h>
#include <dali/integration-api/platform-abstraction.h>

// INTERNAL INCLUDES
#include "vsync-notifier.h"
#include <base/interfaces/adaptor-internal-services.h>
#include <base/update-render-synchronization.h>
#include <base/environment-options.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace
{

const unsigned int MICROSECONDS_PER_SECOND( 100000u );
const unsigned int TIME_PER_FRAME_IN_MICROSECONDS( 16667u );

#if defined(DEBUG_ENABLED)
Integration::Log::Filter* gLogFilter = Integration::Log::Filter::New(Debug::Concise, false, "LOG_VSYNC_NOTIFIER");
#endif

} // unnamed namespace

VSyncNotifier::VSyncNotifier( UpdateRenderSynchronization& sync,
                              AdaptorInternalServices& adaptorInterfaces,
                              const EnvironmentOptions& environmentOptions )
: mUpdateRenderSync( sync ),
  mCore( adaptorInterfaces.GetCore() ),
  mPlatformAbstraction( adaptorInterfaces.GetPlatformAbstractionInterface() ),
  mVSyncMonitor( adaptorInterfaces.GetVSyncMonitorInterface() ),
  mThread( NULL ),
  mEnvironmentOptions( environmentOptions )
{
}

VSyncNotifier::~VSyncNotifier()
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "%s\n", __func__ );

  Stop();
}

void VSyncNotifier::Start()
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "%s\n", __func__ );

  if ( !mThread )
  {
    mVSyncMonitor->Initialize();

    mThread = new boost::thread( boost::bind( &VSyncNotifier::Run, this ) );
  }
}

void VSyncNotifier::Stop()
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "%s\n", __func__ );

  if( mThread )
  {
    // wait for the thread to finish
    mThread->join();

    delete mThread;
    mThread = NULL;
  }

  mVSyncMonitor->Terminate();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The following is executed inside the notifier thread !!!
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void VSyncNotifier::Run()
{
  // install a function for logging
  mEnvironmentOptions.InstallLogFunction();

  unsigned int frameNumber( 0u );             // frameCount, updated when the thread is paused
  unsigned int currentSequenceNumber( 0u );   // platform specific vsync sequence number (increments with each vsync)
  unsigned int currentSeconds( 0u );          // timestamp at latest vsync
  unsigned int currentMicroseconds( 0u );     // timestamp at latest vsync
  unsigned int seconds( 0u );
  unsigned int microseconds( 0u );

  bool running( true );
  while( running )
  {
    bool validSync( true );

    // Hardware VSyncs available?
    if( mVSyncMonitor->UseHardware() )
    {
      // Yes..wait for hardware VSync
      validSync = mVSyncMonitor->DoSync( currentSequenceNumber, currentSeconds, currentMicroseconds );
    }
    else
    {
      // No..use software timer
      mPlatformAbstraction.GetTimeMicroseconds( seconds, microseconds );

      unsigned int timeDelta( MICROSECONDS_PER_SECOND * (seconds - currentSeconds) );
      if( microseconds < currentMicroseconds)
      {
        timeDelta += (microseconds + MICROSECONDS_PER_SECOND) - currentMicroseconds;
      }
      else
      {
        timeDelta += microseconds - currentMicroseconds;
      }

      if( timeDelta < TIME_PER_FRAME_IN_MICROSECONDS )
      {
          usleep( TIME_PER_FRAME_IN_MICROSECONDS - timeDelta );
      }
      else
      {
        usleep( TIME_PER_FRAME_IN_MICROSECONDS );
      }
    }

    running = mUpdateRenderSync.VSyncNotifierSyncWithUpdateAndRender( validSync, ++frameNumber, currentSeconds, currentMicroseconds );
  }

  // uninstall a function for logging
  mEnvironmentOptions.UnInstallLogFunction();

}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali


// CLASS HEADER
#include "egl-implementation.h"

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/common/dali-common.h>

// INTERNAL INCLUDES
#include <internal/common/android-render-surface.h>

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
    mEglSurface(0),
    mGlesInitialized(false),
    mIsOwnSurface(true),
    mSyncMode(FULL_SYNC),
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

void EglImplementation::DestroyContext()
{
  DALI_ASSERT_ALWAYS( mEglContext && "no EGL context" );

  eglDestroyContext( mEglDisplay, mEglContext );
  mEglContext = 0;
}

void EglImplementation::DestroySurface()
{
  DALI_ASSERT_ALWAYS( mEglSurface && "no EGL surface" );

  if(mIsOwnSurface)
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

    if(mIsOwnSurface)
    {
      eglDestroySurface(mEglDisplay, mEglSurface);
    }
    eglDestroyContext(mEglDisplay, mEglContext);

    // DO NOT use eglTerminate for Pixmap surface : it doesn't consider other egl context
    if( mIsWindow )
    {
      eglTerminate(mEglDisplay);
    }

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

