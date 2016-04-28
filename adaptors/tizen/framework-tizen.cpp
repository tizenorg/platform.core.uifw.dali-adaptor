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
 *
 */

// CLASS HEADER
#include "framework.h"

// EXTERNAL INCLUDES
#include <app.h>
#include <bundle.h>
#include <Ecore.h>

#include <system_info.h>
#include <app_control_internal.h>
#include <bundle_internal.h>
#include <appcore-watch/watch_app.h>

#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <callback-manager.h>


namespace Dali
{

namespace Internal
{

namespace Adaptor
{

/**
 * Impl to hide EFL data members
 */
struct Framework::Impl
{
// Constructor
  Impl(void* data, ApplicationType type )
  : mAbortCallBack( NULL ),
    mCallbackManager( NULL )
  {
    mFramework = static_cast<Framework*>(data);

    mApplicationType = type;
    mCallbackManager = CallbackManager::New();
  }

  ~Impl()
  {
    delete mAbortCallBack;

    // we're quiting the main loop so
    // mCallbackManager->RemoveAllCallBacks() does not need to be called
    // to delete our abort handler
    delete mCallbackManager;
  }

  int AppMain()
  {
    int ret;

    if (mApplicationType == NORMAL)
    {
      ret = AppNormalMain();
    }
    else
    {
      ret = AppWatchMain();
    }
    return ret;
  }

  void AppExit()
  {
    if (mApplicationType == NORMAL)
    {
      AppNormalExit();
    }
    else
    {
      AppWatchExit();
    }
  }


  // Data
  ApplicationType mApplicationType;
  CallbackBase* mAbortCallBack;
  CallbackManager *mCallbackManager;

  Framework* mFramework;
  app_event_handler_h handlers[5];
  ui_app_lifecycle_callback_s mEventCallback;
  watch_app_lifecycle_callback_s mWatchCallback;

  static bool AppCreate(void *data)
  {
    return static_cast<Framework*>(data)->Create();
  }

  static void AppTerminate(void *data)
  {
    Observer *observer = &static_cast<Framework*>(data)->mObserver;

    observer->OnTerminate();
  }

  static void AppPause(void *data)
  {
    Observer *observer = &static_cast<Framework*>(data)->mObserver;

    observer->OnPause();
  }

  static void AppResume(void *data)
  {
    Observer *observer = &static_cast<Framework*>(data)->mObserver;

    observer->OnResume();
  }

  static void ProcessBundle(Framework* framework, bundle *bundleData)
  {
    if(bundleData == NULL)
    {
      return;
    }

    // get bundle name
    char* bundleName = const_cast<char*>(bundle_get_val(bundleData, "name"));
    if(bundleName != NULL)
    {
      framework->SetBundleName(bundleName);
    }

    // get bundle id
    char* bundleId = const_cast<char*>(bundle_get_val(bundleData, "id"));
    if(bundleId != NULL)
    {
      framework->SetBundleId(bundleId);
    }
  }

  /**
   * Called by AppCore when the application is launched from another module (e.g. homescreen).
   * @param[in] b the bundle data which the launcher module sent
   */
  static void AppControl(app_control_h app_control, void *data)
  {
    Framework* framework = static_cast<Framework*>(data);
    Observer *observer = &framework->mObserver;
    bundle *bundleData = NULL;

    app_control_to_bundle(app_control, &bundleData);
    ProcessBundle(framework, bundleData);

    observer->OnReset();
    observer->OnAppControl(app_control);
  }

  int AppNormalMain()
  {
    int ret;

    mEventCallback.create = AppCreate;
    mEventCallback.terminate = AppTerminate;
    mEventCallback.pause = AppPause;
    mEventCallback.resume = AppResume;
    mEventCallback.app_control = AppControl;

    ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, AppBatteryLow, mFramework);
    ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, AppMemoryLow, mFramework);
    ui_app_add_event_handler(&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED], APP_EVENT_DEVICE_ORIENTATION_CHANGED, AppDeviceRotated, mFramework);
    ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, AppLanguageChanged, mFramework);
    ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED, AppRegionChanged, mFramework);

    ret = ui_app_main(*mFramework->mArgc, *mFramework->mArgv, &mEventCallback, mFramework);

    return ret;
  }

  void AppNormalExit()
  {
      ui_app_exit();
  }

  static bool AppCreateWatch(int width, int height, void *data)
  {
    return static_cast<Framework*>(data)->Create();
  }

  static void AppTimeTick(watch_time_h time, void *data)
  {
    Observer *observer = &static_cast<Framework*>(data)->mObserver;

    observer->OnTimeTick(time);
  }

  static void AppAmbientTick(watch_time_h time, void *data)
  {
    Observer *observer = &static_cast<Framework*>(data)->mObserver;

    observer->OnAmbientTick(time);
  }

  static void AppAmbientChanged(bool ambient, void *data)
  {
    Observer *observer = &static_cast<Framework*>(data)->mObserver;

    observer->OnAmbientChanged(ambient);
  }

  int AppWatchMain()
  {
    int ret = true;

    mWatchCallback.create = AppCreateWatch;
    mWatchCallback.app_control = AppControl;
    mWatchCallback.terminate = AppTerminate;
    mWatchCallback.pause = AppPause;
    mWatchCallback.resume = AppResume;
    mWatchCallback.time_tick = AppTimeTick;
    mWatchCallback.ambient_tick = AppAmbientTick;
    mWatchCallback.ambient_changed = AppAmbientChanged;

    watch_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, AppBatteryLow, mFramework);
    watch_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, AppMemoryLow, mFramework);
    watch_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, AppLanguageChanged, mFramework);
    watch_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED, AppRegionChanged, mFramework);

    ret = watch_app_main(*mFramework->mArgc, *mFramework->mArgv, &mWatchCallback, mFramework);
    return ret;
  }

  void AppWatchExit()
  {
    watch_app_exit();
  }

  static void AppLanguageChanged(app_event_info_h event, void *data)
  {
    Observer *observer = &static_cast<Framework*>(data)->mObserver;

    observer->OnLanguageChanged();
  }

  static void AppDeviceRotated(app_event_info_h event_info, void *data)
  {
  }

  static void AppRegionChanged(app_event_info_h event, void *data)
  {
    Observer *observer = &static_cast<Framework*>(data)->mObserver;

    observer->OnRegionChanged();
  }

  static void AppBatteryLow(app_event_info_h event, void *data)
  {
    Observer *observer = &static_cast<Framework*>(data)->mObserver;

    observer->OnBatteryLow();
  }

  static void AppMemoryLow(app_event_info_h event, void *data)
  {
    Observer *observer = &static_cast<Framework*>(data)->mObserver;

    observer->OnMemoryLow();
  }

private:
  // Undefined
  Impl( const Impl& impl );

  // Undefined
  Impl& operator=( const Impl& impl );
};

Framework::Framework( Framework::Observer& observer, int *argc, char ***argv, ApplicationType type )
: mObserver(observer),
  mInitialised(false),
  mRunning(false),
  mArgc(argc),
  mArgv(argv),
  mBundleName(""),
  mBundleId(""),
  mAbortHandler( MakeCallback( this, &Framework::AbortCallback ) ),
  mImpl(NULL)
{
  bool featureFlag = true;
  system_info_get_platform_bool( "tizen.org/feature/opengles.version.2_0", &featureFlag );

  if( featureFlag == false )
  {
    set_last_result( TIZEN_ERROR_NOT_SUPPORTED );
    throw Dali::DaliException( "", "OpenGL ES 2.0 is not supported." );
  }
  InitThreads();

  mImpl = new Impl(this, type);
}

Framework::~Framework()
{
  if (mRunning)
  {
    Quit();
  }

  delete mImpl;
}

bool Framework::Create()
{
  mInitialised = true;

  // Connect to abnormal exit signals
  mAbortHandler.AbortOnSignal( SIGINT );
  mAbortHandler.AbortOnSignal( SIGQUIT );
  mAbortHandler.AbortOnSignal( SIGKILL );
  mAbortHandler.AbortOnSignal( SIGTERM );
  mAbortHandler.AbortOnSignal( SIGHUP );

  mObserver.OnInit();
  return true;
}

void Framework::Run()
{
  mRunning = true;
  int ret;

  ret = mImpl->AppMain();
  if (ret != APP_ERROR_NONE)
  {
    DALI_LOG_ERROR("Framework::Run(), ui_app_main() is failed. err = %d", ret);
  }
  mRunning = false;
}

void Framework::Quit()
{
  mImpl->AppExit();
}

bool Framework::IsMainLoopRunning()
{
  return mRunning;
}

void Framework::AddAbortCallback( CallbackBase* callback )
{
  mImpl->mAbortCallBack = callback;
}

std::string Framework::GetBundleName() const
{
  return mBundleName;
}

void Framework::SetBundleName(const std::string& name)
{
  mBundleName = name;
}

std::string Framework::GetBundleId() const
{
  return mBundleId;
}

void Framework::SetBundleId(const std::string& id)
{
  mBundleId = id;
}

void Framework::AbortCallback( )
{
  // if an abort call back has been installed run it.
  if (mImpl->mAbortCallBack)
  {
    CallbackBase::Execute( *mImpl->mAbortCallBack );
  }
  else
  {
    Quit();
  }
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
