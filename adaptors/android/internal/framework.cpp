/*
Copyright (c) 2000-2012 Samsung Electronics Co., Ltd All Rights Reserved

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
#include "framework.h"

// EXTERNAL INCLUDES
#include <boost/bind.hpp>
#include <dali/integration-api/debug.h>
#include <android-interface.h>

// INTERNAL INCLUDES
#include <internal/common/callback-manager.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace
{

/// Application Status Enum
namespace AppStatus
{
  enum Enum
  {
    APP_CREATE,
    APP_TERMINATE,
    APP_PAUSE,
    APP_RESUME,
    APP_SURFACE_CREATED,
    APP_SURFACE_DESTROYED,
    APP_RESET,
    APP_LANGUAGE_CHANGE,
    NOTIFICATION_NEEDED
  };
};

} // Unnamed namespace

/**
 * Impl to hide EFL data members
 */
struct Framework::Impl
{
  // Constructor

  Impl(void* data)
  {
    mOps.create = AppCreate;
    mOps.terminate = AppTerminate;
    mOps.pause = AppPause;
    mOps.resume = AppResume;
    mOps.surfaceCreated = SurfaceCreated;
    mOps.surfaceDestroyed = SurfaceDestroyed;
    mOps.reset = AppReset;
    mOps.notificationNeeded = NotificationNeeded;
    mOps.data = data;
    mCallbackManager = CallbackManager::New();

  }

  ~Impl()
  {
    // we're quiting the main loop so
    // mCallbackManager->RemoveAllCallBacks() does not need to be called
    // to delete our abort handler
    delete mCallbackManager;
  }

  // Data

  boost::function<void(void)> mAbortCallBack;
  struct android_core_ops mOps;
  CallbackManager *mCallbackManager;
  // Static methods

  /**
   * Called when the application is aborted.
   */
  static bool AbortCallback(void *data, int type, void *event)
  {
    Framework* framework = static_cast<Framework*>(data);

    // If our abort callback function is set then call it, otherwise just quit the event loop.
    if (framework->mImpl->mAbortCallBack)
    {
      framework->mImpl->mAbortCallBack();
    }
    else
    {
      framework->Quit();
    }

    return true;
  }

  /**
   * Called by AppCore on application creation.
   */
  static int AppCreate(void *data)
  {
    return static_cast<Framework*>(data)->SlpAppStatusHandler(AppStatus::APP_CREATE);
  }

  /**
   * Called by AppCore when the application should terminate.
   */
  static int AppTerminate(void *data)
  {
    return static_cast<Framework*>(data)->SlpAppStatusHandler(AppStatus::APP_TERMINATE);
  }

  /**
   * Called by AppCore when the application is paused.
   */
  static int AppPause(void *data)
  {
    return static_cast<Framework*>(data)->SlpAppStatusHandler(AppStatus::APP_PAUSE);
  }

  /**
   * Called by AppCore when the application is resumed.
   */
  static int AppResume(void *data)
  {
    return static_cast<Framework*>(data)->SlpAppStatusHandler(AppStatus::APP_RESUME);
  }

  /**
   * Called by AppCore whenever surface is created.
   */
  static int SurfaceCreated(void *data)
  {
    return static_cast<Framework*>(data)->SlpAppStatusHandler(AppStatus::APP_SURFACE_CREATED);
  }

  /**
   * Called by AppCore whenever surface is destroyed.
   */
  static int SurfaceDestroyed(void *data)
  {
    return static_cast<Framework*>(data)->SlpAppStatusHandler(AppStatus::APP_SURFACE_DESTROYED);
  }

  static int NotificationNeeded(void *data)
  {
    return static_cast<Framework*>(data)->SlpAppStatusHandler(AppStatus::NOTIFICATION_NEEDED);
  }

  /**
   * Called by AppCore when the application is launched from another module (e.g. homescreen).
   * @param[in] b the bundle data which the launcher module sent
   */
  static int AppReset(void *b, void *data)
  {
    int ret = 0;
    Framework* framework = static_cast<Framework*>(data);

    if(framework)
    {
      if(b)
      {
        //FIXME: refactor Framework
        std::string bundleName = "DaliActivity";
        framework->SetBundleName(bundleName);
      }
      ret = framework->SlpAppStatusHandler(AppStatus::APP_RESET);
    }

    return ret;
  }

  /**
   * Called by AppCore when the language changes on the device.
   */
  static int AppLanguageChange(void* data)
  {
    return static_cast<Framework*>(data)->SlpAppStatusHandler(AppStatus::APP_LANGUAGE_CHANGE);
  }
};

Framework::Framework(Framework::Observer& observer, int *argc, char ***argv, const std::string& name)
: mObserver(observer),
  mInitialised(false),
  mRunning(false),
  mArgc(argc),
  mArgv(argv),
  mName(name),
  mBundleName(""),
  mImpl(NULL)
{
  mImpl = new Impl(this);
}

Framework::~Framework()
{
  if (mRunning)
  {
    Quit();
  }

  delete mImpl;
}

void Framework::Run()
{
  mRunning = true;
  androidcore_main(mName.c_str(), mArgc, mArgv, &mImpl->mOps);
  mRunning = false;
}

void Framework::Quit()
{
  androidcore_quit();
}

bool Framework::IsMainLoopRunning()
{
  return mRunning;
}

void Framework::AddAbortCallback(boost::function<void(void)> callBack)
{
  mImpl->mAbortCallBack = callBack;
}


void Framework::AbortCallback()
{
  // if an abort call back has been installed run it.
  if (mImpl->mAbortCallBack)
  {
    mImpl->mAbortCallBack();
  }
  else
  {
    Quit();
  }
}

std::string Framework::GetBundleName() const
{
  return mBundleName;
}

void Framework::SetBundleName(const std::string& name)
{
  mBundleName = name;
}

int Framework::SlpAppStatusHandler(int type)
{
  switch (type)
  {
    case AppStatus::APP_CREATE:
    {
      mInitialised = true;

      // Connect the exit signal handler

//      mImpl->mCallbackManager->AddEventCallback(boost::bind(&Framework::AbortCallback, this),
//                                                    ECORE_EVENT_SIGNAL_EXIT,
//                                                    CallbackManager::CALLBACK_PASS_ON);

#if 0
      // Connect to the language changed signal
      int retVal = appcore_set_event_callback(APPCORE_EVENT_LANG_CHANGE, &Impl::AppLanguageChange, this);
      if (retVal != 0)
      {
        DALI_LOG_WARNING("Unable to set Language changed callback\n");
      }
#endif

      mObserver.OnInit();
      break;
    }

    case AppStatus::APP_RESET:
      mObserver.OnReset();
      break;

    case AppStatus::APP_RESUME:
      mObserver.OnResume();
      break;

    case AppStatus::APP_SURFACE_CREATED:
      mObserver.OnSurfaceCreated();
      break;

    case AppStatus::APP_SURFACE_DESTROYED:
      mObserver.OnSurfaceDestroyed();
      break;

    case AppStatus::APP_TERMINATE:
      mObserver.OnTerminate();
      break;

    case AppStatus::APP_PAUSE:
      mObserver.OnPause();
      break;

    case AppStatus::APP_LANGUAGE_CHANGE:
      mObserver.OnLanguageChanged();
      break;

    case AppStatus::NOTIFICATION_NEEDED:
      mObserver.OnNotificationRequested();
      break;
      
    default:
      break;
  }

  return 0;
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
