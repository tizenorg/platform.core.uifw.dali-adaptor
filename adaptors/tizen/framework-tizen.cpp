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

#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <callback-manager.h>

#ifdef APPCORE_WATCH_AVAILABLE
#include <appcore-watch/watch_app.h>
#endif

enum
{
  FUNC_MAIN,
  FUNC_EXIT
};

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

/**
 * Impl to hide EFL data members
 */
//struct Framework::Impl
struct Framework::Impl
{
// Constructor
  Impl(void* data, APPLICATION_TYPE type )
  : mAbortCallBack( NULL ),
    mCallbackManager( NULL )
  {
    mFamework = static_cast<Framework*>(data);

#ifdef APPCORE_WATCH_AVAILABLE
    if ( type == WATCH )
      mAppFwFunc = AppFrameworkWatch;
    else
      mAppFwFunc = AppFrameworkNormal;
#else
    mAppFwFunc = AppFrameworkNormal;
#endif
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

  int ProcessAppFramework(int func)
  {
    return (*mAppFwFunc)(this, func);
  }

  // Data
  int (*mAppFwFunc)(struct Impl *, int);
  CallbackBase* mAbortCallBack;
  CallbackManager *mCallbackManager;

  Framework* mFamework;
  app_event_handler_h handlers[5];
  ui_app_lifecycle_callback_s mEventCallback;
#ifdef APPCORE_WATCH_AVAILABLE
  watch_app_lifecycle_callback_s mWatchCallback;
#endif

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

  static int AppFrameworkNormal(Impl *impl, int func)
  {
    int ret = true;

    if (func == FUNC_MAIN)
    {
      ui_app_lifecycle_callback_s *eventCallback = &impl->mEventCallback;
      app_event_handler_h *lhandlers = &impl->handlers[0];
      Framework *framework =  impl->mFamework;

      eventCallback->create = AppCreate;
      eventCallback->terminate = AppTerminate;
      eventCallback->pause = AppPause;
      eventCallback->resume = AppResume;
      eventCallback->app_control = AppControl;

      ui_app_add_event_handler(&lhandlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, AppBatteryLow, framework);
      ui_app_add_event_handler(&lhandlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, AppMemoryLow, framework);
      ui_app_add_event_handler(&lhandlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED], APP_EVENT_DEVICE_ORIENTATION_CHANGED, AppDeviceRotated, framework);
      ui_app_add_event_handler(&lhandlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, AppLanguageChanged, framework);
      ui_app_add_event_handler(&lhandlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED, AppRegionChanged, framework);

     ret = ui_app_main(*framework->mArgc, *framework->mArgv, eventCallback, framework);
    }
    else
    {
      ui_app_exit();
    }
    return ret;
  }

#ifdef APPCORE_WATCH_AVAILABLE
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

  static int AppFrameworkWatch(Impl *impl, int func)
  {
    int ret = true;

    if (func == FUNC_MAIN)
    {
      watch_app_lifecycle_callback_s *eventCallback = &impl->mWatchCallback;
      app_event_handler_h *lhandlers = &impl->handlers[0];
      Framework *framework =  impl->mFamework;

      eventCallback->create = AppCreateWatch;
      eventCallback->app_control = AppControl;
      eventCallback->terminate = AppTerminate;
      eventCallback->pause = AppPause;
      eventCallback->resume = AppResume;
      eventCallback->time_tick = AppTimeTick;
      eventCallback->ambient_tick = AppAmbientTick;
      eventCallback->ambient_changed = AppAmbientChanged;

      watch_app_add_event_handler(&lhandlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, AppBatteryLow, framework);
      watch_app_add_event_handler(&lhandlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, AppMemoryLow, framework);
      watch_app_add_event_handler(&lhandlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, AppLanguageChanged, framework);
      watch_app_add_event_handler(&lhandlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED, AppRegionChanged, framework);

      ret = watch_app_main(*framework->mArgc, *framework->mArgv, eventCallback, framework);
    }
    else
    {
      watch_app_exit();
    }
    return ret;
  }
#endif

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

Framework::Framework( Framework::Observer& observer, int *argc, char ***argv, APPLICATION_TYPE type )
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

  ret = mImpl->ProcessAppFramework(FUNC_MAIN);
  if (ret != APP_ERROR_NONE)
  {
    DALI_LOG_ERROR("Framework::Run(), ui_app_main() is failed. err = %d", ret);
  }
  mRunning = false;
}

void Framework::Quit()
{
  mImpl->ProcessAppFramework(FUNC_EXIT);
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
