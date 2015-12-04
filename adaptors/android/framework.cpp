/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
 */

// CLASS HEADER
#include "framework.h"

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <jni.h>

// INTERNAL INCLUDES
#include <callback-manager.h>
#include <adaptors/android/android-framework-timer-factory.h>
#include <adaptors/integration-api/adaptor.h>

namespace Dali
{
namespace Integration
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFWFilter = Debug::Filter::New( Debug::Concise, false, "LOG_FRAMEWORK");
#endif

/**
 * Impl to hide android specific code
 */
struct Framework::Impl
{
  // Constructor

  Impl(void* data)
  : mIdleHandler( NULL ),
    mNativeWindow( NULL ),
    mJVM( NULL ),
    isSurfaceCreated( false ),
    isDaliSurfaceCreated( false ),
    isAppActive( false ),
    isResumeRequested( false )
  {
  }

  ~Impl()
  {
  }

public:
  void Quit()
  {
    // @todo Communicate upwards to Java layer

    mTimerFactory.Reset();
    mIdleHandler  = NULL;
    mNativeWindow = NULL;
    gAssetManager = NULL;
    mCacheDir.clear();

    isSurfaceCreated = false;
    isDaliSurfaceCreated = false;
    isAppActive = false;
    isResumeRequested = false;
  }

  // Data
  TimerFactory   mTimerFactory;
  Idler*         mIdleHandler;
  ANativeWindow* mNativeWindow;
  std::string    mCacheDir;

  JavaVM* mJVM;

  Dali::Vector2 mDpi;
  Dali::Rect<int> mScreen;

  bool isSurfaceCreated;
  bool isDaliSurfaceCreated;
  bool isAppActive;
  bool isResumeRequested;

  // @todo Make this non-static. It needs to be accessed from multiple threads
  // (i.e. any thread that can perform image loading or call LoadResourceSynchronously)
  static AAssetManager* gAssetManager;
};

AAssetManager* Framework::Impl::gAssetManager = NULL;


Framework::Framework( Framework::Observer& observer, int *argc, char ***argv )
: mObserver( &observer ),
  mInitialised(false),
  mRunning(false),
  mArgc(argc),
  mArgv(argv),
  mImpl(NULL)
{
  mImpl = new Impl(this);
}

Framework::Framework()
: mObserver( NULL ),
  mInitialised(false),
  mRunning(false),
  mArgc(0),
  mArgv(NULL),
  mImpl(NULL)
{
  mImpl = new Impl(this);
}

void Framework::Initialize( Framework::Observer& observer, int *argc, char ***argv )
{
  mObserver = &observer;
  mArgc = argc;
  mArgv = argv;
}

Framework::~Framework()
{
  if( mRunning )
  {
    Quit();
  }

  delete mImpl;
}

void Framework::Start()
{
  DALI_LOG_INFO( gLogFWFilter, Debug::Concise, "%s\n", __PRETTY_FUNCTION__ );
  mRunning = true;
}

void Framework::Quit()
{
  DALI_LOG_INFO( gLogFWFilter, Debug::Concise, "%s\n", __PRETTY_FUNCTION__ );
  mImpl->Quit();
}

void Framework::AddAbortCallback( CallbackBase* callback )
{
  // @todo Reimplement abort callback?
}

void Framework::SetWindow( ANativeWindow* nativeWindow )
{
  DALI_LOG_INFO( gLogFWFilter, Debug::Concise, "%s\n", __PRETTY_FUNCTION__ );
  mImpl->mNativeWindow = nativeWindow;
}

ANativeWindow*  Framework::GetWindow()
{
  return mImpl->mNativeWindow;
}

void Framework::Create()
{
  DALI_LOG_INFO( gLogFWFilter, Debug::Concise, "%s\n", __PRETTY_FUNCTION__ );
  mInitialised = true;
  if( mObserver )
  {
    mObserver->OnInit();
  }
}

void Framework::SurfaceCreated()
{
  DALI_LOG_INFO( gLogFWFilter, Debug::Concise, "%s\n", __PRETTY_FUNCTION__ );
  mImpl->isSurfaceCreated = true;

  // We dont have a dali-surface yet, but resume was already requested.
  // If we are not active at this point, but received the resume message, activate the app.
  if (!mImpl->isDaliSurfaceCreated && mImpl->isResumeRequested)
  {
    TriggerResume();
    mImpl->isResumeRequested = false;
  }
}

void Framework::SurfaceDestroyed()
{
  DALI_LOG_INFO( gLogFWFilter, Debug::Concise, "%s\n", __PRETTY_FUNCTION__ );
  SetWindow( NULL );

  if( mImpl->isDaliSurfaceCreated )
  {
    if( mObserver )
    {
      mObserver->OnSurfaceDestroyed();
    }

    mImpl->isDaliSurfaceCreated = false;
  }

  mImpl->isSurfaceCreated = false;
}

void Framework::Pause()
{
  DALI_LOG_INFO( gLogFWFilter, Debug::Concise, "%s\n", __PRETTY_FUNCTION__ );
  if( mImpl->isAppActive )
  {
    if( mObserver )
    {
      mObserver->OnPause();
    }
  }

  mImpl->isAppActive = false;
  mImpl->isResumeRequested = false;
}

void Framework::TriggerResume()
{
  // On 4.4 this is to ensure we dont create the surface if we haven't lost it.
  if( false == mImpl->isDaliSurfaceCreated )
  {
    if( mObserver )
    {
      mObserver->OnSurfaceCreated();
    }

    mImpl->isDaliSurfaceCreated = true;
  }
  if( mObserver )
  {
    mObserver->OnResume();
  }

  mImpl->isAppActive = true;
}

void Framework::Resume()
{
  DALI_LOG_INFO( gLogFWFilter, Debug::Concise, "%s\n", __PRETTY_FUNCTION__ );
  if( !mImpl->isAppActive )
  {
    // Did we receive a surface created message at this point?
    if( mImpl->isSurfaceCreated )
    {
      // If yes, trigger the surface creation, and then we are safe to resume.
      TriggerResume();
    }
  }

  // if we did not have a surface when this callback was triggered,
  // flag this for later.
  mImpl->isResumeRequested = !mImpl->isSurfaceCreated;
}

void Framework::Stop()
{
  DALI_LOG_INFO( gLogFWFilter, Debug::Concise, "%s\n", __PRETTY_FUNCTION__ );
  if( mObserver )
  {
    mObserver->OnTerminate();
  }
}

void Framework::ResizeSurface( int width, int height )
{
  DALI_LOG_INFO( gLogFWFilter, Debug::Concise, "%s\n", __PRETTY_FUNCTION__ );
  Dali::Adaptor& adaptor = Dali::Adaptor::Get();
  adaptor.ResizeSurface( width, height);
}

void Framework::RunIdlers()
{
  mImpl->mTimerFactory.RunTimers();

  if( mImpl->mIdleHandler != NULL )
  {
    mImpl->mIdleHandler->Run();
  }
}

void Framework::SetWindowParameters(
  float xdpi, float ydpi,
  int width, int height )
{
  mImpl->mDpi.x = xdpi;
  mImpl->mDpi.y = ydpi;

  mImpl->mScreen.x = 0;
  mImpl->mScreen.y = 0;
  mImpl->mScreen.width = width;
  mImpl->mScreen.height = height;
}

const Vector2& Framework::GetDpi()
{
  return mImpl->mDpi;
}

Rect<int> Framework::GetWidthHeight()
{
  return mImpl->mScreen;
}

void Framework::SetAssetManager( AAssetManager* assetManager )
{
  Framework::Impl::gAssetManager = assetManager;
}

AAssetManager* Framework::GetAssetManager()
{
  return Framework::Impl::gAssetManager;
}

void Framework::SetCacheDir( const std::string& path )
{
  mImpl->mCacheDir = path;
}

std::string Framework::GetCacheDir()
{
  return mImpl->mCacheDir;
}

Framework::Timer* Framework::AddTimer( float seconds, Framework::TimerCallback callback, void* context )
{
  return mImpl->mTimerFactory.Create( seconds, callback, context );
}

void Framework::RemoveTimer( Framework::Timer* timer )
{
  mImpl->mTimerFactory.Destroy( timer );
}

void Framework::SetIdleHandler ( Framework::Idler* idler )
{
  mImpl->mIdleHandler = idler;
}

void Framework::SetJavaVM( JavaVM* jvm )
{
  mImpl->mJVM = jvm;
}

JavaVM* Framework::GetJavaVM()
{
  return mImpl->mJVM;
}

} // namespace Integration

} // namespace Dali
