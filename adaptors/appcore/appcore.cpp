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
 */

// CLASS HEADER
#include "appcore.h"

// EXTERNAL INCLUDES
#include <cstdio>
#include <cstdlib>
#include <locale.h>
#include <unistd.h>
#include <libintl.h>
#include <glib-object.h>
#include <glib.h>
#include <dlfcn.h>
#include <aul.h>
#include <vconf.h>
#include <Ecore.h>
#include <Ecore_X.h>
#include <Ecore_IMF.h>
#include <malloc.h>

// INTERNAL INCLUDES

#include "appcore-i18n.h"
#include "appcore-win.h"
#include "vconf-events.h"

#ifdef __cplusplus
extern "C" {
#endif
  void aul_finalize(void);
#ifdef __cplusplus
}
#endif


#define PKGNAME_MAX 256
#define PATH_APP_ROOT "/opt/usr/apps"
#define PATH_RO_APP_ROOT "/usr/apps"
#define PATH_RES "/res"
#define PATH_LOCALE "/locale"

#define SQLITE_FLUSH_MAX    (1024*1024)

namespace Dali
{
namespace Internal
{
namespace Adaptor
{

Appcore::State::State(Appcore& appcore)
: mState(INIT),
  mAppcore(appcore)
{
}

bool Appcore::State::operator==(Appcore::State::States state)
{
  return mState == state;
}

void Appcore::State::operator=(Appcore::State::States state)
{
  if( mAppcore.mResourceReclamation )
  {
    if( state != PAUSED)
    {
      ecore_timer_del(mAppcore.mReclaimTimer);
    }
    else
    {
      mAppcore.mReclaimTimer = ecore_timer_add(5, &Appcore::MemoryFlushCallback, &mAppcore);
    }
  }

  switch(mState)
  {
    case INIT:
      switch(state)
      {
        case RUNNING:
          mState = state;
          break;
        default:
          break;
      }
      break;

    case RUNNING:
      switch(state)
      {
        case PAUSED:
          mAppcore.mOperations.Pause();
          mState = state;
          break;
        case INIT:
          mAppcore.mOperations.Terminate();
          mState = state;
          break;
        default:
          break;
      }

    case PAUSED:
      switch(state)
      {
        case RUNNING:
          mAppcore.mOperations.Resume();
          mState = state;
          break;

        case INIT:
          mAppcore.mOperations.Terminate();
          mState = state;
          break;

        default:
          break;
      }
  }
}


Appcore::Appcore(appcore_ops* ops)
: mOperations(ops),
  mInitialised(false),
  mRotateCallback(NULL),
  mRotateData(NULL),
  mRotationSensor(NULL),
  mState(*this),
  mVconfEventHandler(NULL),
  mAtomParent(0),
  mShowHandler(NULL),
  mHideHandler(NULL),
  mVisibilityHandler(NULL),
  mExitHandler(NULL),
  mResourceReclamation(false),
  mReclaimTimer(NULL)
{
}

Appcore::Appcore(app_event_callback_s* eventCallbacks, void* userData)
: mOperations(eventCallbacks, userData),
  mInitialised(false),
  mRotateCallback(NULL),
  mRotateData(NULL),
  mRotationSensor(NULL),
  mState(*this),
  mVconfEventHandler(NULL),
  mAtomParent(0),
  mShowHandler(NULL),
  mHideHandler(NULL),
  mVisibilityHandler(NULL),
  mExitHandler(NULL),
  mResourceReclamation(false),
  mReclaimTimer(NULL)
{
}

Appcore::~Appcore()
{
}

void Appcore::Run(const char* name, int argc, char** argv)
{
  Initialize(name, argc, argv);
  ecore_main_loop_begin();
  Shutdown();
}

void Appcore::Exit()
{
  ecore_main_loop_quit();
}

void Appcore::Initialize(const char* name, int argc, char** argv)
{
  // From appcore-efl.c:__before_loop()
  {
    g_type_init();
    // From elm_init()
    {
      // From elm_quicklaunch_init()
      {
        eina_init();
        // eina_log_domain_register()
        ecore_init();
        ecore_app_args_set(argc, (const char**)argv);
        //ecore_file_init();
        mExitHandler = ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, ExitCallback, NULL);
      }
      // From elm_quicklaunch_sub_init()
      {
        // From _elm_config_sub_init()
        {
          ecore_x_init(NULL);
        }
        ecore_imf_init();
      }
    }
    // From appcore_init()
    {
      char dirname[PATH_MAX];
      GetDirName(dirname, PATH_MAX);
      SetupEnvironment(name, dirname);
      mVconfEventHandler = new VconfEvents(*this);

      aul_launch_init(AulHandlerCallback, this);
      aul_launch_argv_handler(argc, const_cast<char**>(argv));
    }
  }
  AddWindowCallbacks();

  mInitialised = true;
  mOperations.Create();
}

void Appcore::Shutdown()
{
  RemoveRotationCallback();
  delete mVconfEventHandler;
  aul_finalize();
  mState = State::INIT;

  RemoveWindowCallbacks();

  // from elm_shutdown();
  {
    // from _elm_win_shutdown();
    {
      //while (_elm_win_list) evas_object_del(_elm_win_list->data);
      //if ( _elm_win_state_eval_job )
      //{
      //  ecore_job_del(_elm_win_state_eval_job);
      //  _elm_win_state_eval_job = NULL;
      //}
    }
    // while (_elm_win_deferred_free) ecore_main_loop_iterate();

    // from elm_quicklaunch_sub_shutdown();
    {
      ecore_imf_shutdown();

      // ecore_evas_shutdown();

      // from _elm_config_sub_shutdown()
      {
        ecore_x_shutdown();
      }
      // evas_shutdown();
    }
    // from elm_quicklaunch_shutdown();
    {
      ecore_event_handler_del( mExitHandler );
      // _elm_config_shutdown
      // ecore_file_shutdown();
      ecore_shutdown();
      eina_shutdown();
    }
  }
}

Eina_Bool Appcore::ExitCallback(void* data, int eventType, void* event)
{
  if( data != NULL )
  {
    static_cast<Appcore*>(data)->Exit();
  }

  return ECORE_CALLBACK_PASS_ON;
}

int Appcore::AulHandlerCallback(aul_type type, bundle* bundleData, void* data)
{
  if( data != NULL )
  {
    return static_cast<Appcore*>(data)->AulHandler(type, bundleData);
  }
  return 0;
}

int Appcore::AulHandler(aul_type type, bundle* bundle)
{
  switch (type)
  {
    case AUL_START:
    {
      if (mInitialised)
      {
        mOperations.Reset(bundle);
        mState = State::RUNNING;
      }
    }
    break;

    case AUL_RESUME:
      AppcoreRaiseWin(getpid());
      break;

    case AUL_TERMINATE:
      mState = State::INIT; // side effect: calls ops->Terminate().
      break;

    default:
      break;
  }

  return 0;
}

int Appcore::GetDirName(char *dirname, int bufferLength)
{
  char pkg_name[PKGNAME_MAX];
  int r = -1;
  int pid = getpid();

  if( pid > 0 )
  {
    if( aul_app_get_pkgname_bypid( pid, pkg_name, PKGNAME_MAX ) == AUL_R_OK )
    {
      r = snprintf( dirname, bufferLength, PATH_APP_ROOT "/%s" PATH_RES PATH_LOCALE, pkg_name );
      if(r > 0)
      {
        if( access( dirname, R_OK ) == 0 )
        {
          r = 0;
        }
        else
        {
          r = snprintf( dirname, bufferLength, PATH_RO_APP_ROOT "/%s" PATH_RES PATH_LOCALE,pkg_name );
          if (r >= 0)
          {
            r = 0;
          }
        }
      }
    }
  }
  return r;
}

void Appcore::AddEventCallback(enum appcore_event event, int (*cb) (void *), void *data)
{
  if( mVconfEventHandler != NULL )
  {
    mVconfEventHandler->AddEventCallback(event, cb, data);
  }
}

void Appcore::AddRotationCallback(RotateCallback* rotateCallback, void* data)
{
  if( mRotationSensor == NULL )
  {
    mRotationSensor = new AppcoreRotation(this);
  }

  mRotateCallback = rotateCallback;
  mRotateData = data;
}

void Appcore::RemoveRotationCallback()
{
  mRotateCallback = NULL;
  mRotateData = NULL;
  delete mRotationSensor;
  mRotationSensor = NULL;
}

void Appcore::SetSystemResourceReclamation(bool enable)
{
  mResourceReclamation = enable;
}

const char* Appcore::GetAppName()
{
  static char buffer[1024]= {0, };
  const char* appName=NULL;

  int retval = aul_app_get_appid_bypid(getpid(), buffer, sizeof(buffer));
  if( retval >= 0 && buffer[0] != '\0' )
  {
    // App Id is valid
    // Id is of the form com.vendor.name. We only want the name token.
    char* nameToken = NULL;
    nameToken = strrchr( buffer, '.' );
    if( nameToken != NULL )
    {
      ++nameToken;
      appName = strdup(nameToken);
    }
  }
  return appName;
}


void Appcore::RotationChanged(enum appcore_rm rotation)
{
  if( mRotateCallback != NULL )
  {
    mRotateCallback(rotation, mRotateData);
  }
}

void Appcore::OnLowMemory()
{
  if( mState == State::PAUSED )
  {
    int (*flush_fn) (int);
    flush_fn = (int(*)(int)) dlsym(RTLD_DEFAULT, "sqlite3_release_memory");
    if (flush_fn)
    {
      flush_fn(SQLITE_FLUSH_MAX);
    }
  }
  malloc_trim(0);
  // stack_trim();
}

Eina_Bool Appcore::MemoryFlushCallback(void* data)
{
  Appcore* appcore = static_cast<Appcore*>(data);
  if( appcore != NULL )
  {
    appcore->OnLowMemory();
    appcore->mReclaimTimer = NULL;
  }
  return ECORE_CALLBACK_CANCEL;
}


// From appcore-efl.c:__add_climsg_cb()
void Appcore::AddWindowCallbacks()
{
  mAtomParent = ecore_x_atom_get("_E_PARENT_BORDER_WINDOW");

  mShowHandler = ecore_event_handler_add(ECORE_X_EVENT_WINDOW_SHOW, ShowCallback, this);
  mHideHandler = ecore_event_handler_add(ECORE_X_EVENT_WINDOW_HIDE, HideCallback, this);
  mVisibilityHandler = ecore_event_handler_add(ECORE_X_EVENT_WINDOW_VISIBILITY_CHANGE, VisibilityCallback, this);
}

void Appcore::RemoveWindowCallbacks()
{
  if(mShowHandler)
  {
    ecore_event_handler_del(mShowHandler);
  }
  if(mHideHandler)
  {
    ecore_event_handler_del(mHideHandler);
  }
  if(mVisibilityHandler)
  {
    ecore_event_handler_del(mVisibilityHandler);
  }
}

Eina_Bool Appcore::ShowCallback(void *data, int type, void *event)
{
  Appcore* appcore = static_cast<Appcore*>(data);
  if( appcore != NULL )
  {
    return appcore->OnShow(type, event);
  }
  return ECORE_CALLBACK_PASS_ON;
}

Eina_Bool Appcore::HideCallback(void *data, int type, void *event)
{
  Appcore* appcore = static_cast<Appcore*>(data);
  if( appcore != NULL )
  {
    return appcore->OnHide(type, event);
  }
  return ECORE_CALLBACK_PASS_ON;
}

Eina_Bool Appcore::VisibilityCallback(void *data, int type, void *event)
{
  Appcore* appcore = static_cast<Appcore*>(data);
  if( appcore != NULL )
  {
    return appcore->OnVisibility(type, event);
  }
  return ECORE_CALLBACK_PASS_ON;
}

Eina_Bool Appcore::OnShow(int type, void *event)
{
  Eina_Bool result = ECORE_CALLBACK_PASS_ON;
  if( event != NULL )
  {
    Ecore_X_Event_Window_Show *ev = static_cast<Ecore_X_Event_Window_Show*>(event);
    Ecore_X_Window parent;

    int ret = ecore_x_window_prop_window_get(ev->win, mAtomParent, &parent, 1);
    if( ret == 1 )
    {
      // Check WM rotation support
      result = ECORE_CALLBACK_RENEW;
    }
  }
  return result;
}

Eina_Bool Appcore::OnHide(int type, void *event)
{
  mState = State::PAUSED;

  return ECORE_CALLBACK_RENEW;
}

Eina_Bool Appcore::OnVisibility(int type, void *event)
{
  if( event != NULL )
  {
    Ecore_X_Event_Window_Visibility_Change *ev =
      static_cast<Ecore_X_Event_Window_Visibility_Change*>(event);

    if( ev->fully_obscured )
    {
      mState = State::PAUSED;
    }
    else // visible
    {
      mState = State::RUNNING;
    }
  }

  return ECORE_CALLBACK_RENEW;
}


} // Adaptor
} // Internal
} // Dali
