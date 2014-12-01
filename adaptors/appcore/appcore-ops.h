#ifndef __DALI_INTERNAL_ADAPTOR_APPCORE_OPS_H__
#define __DALI_INTERNAL_ADAPTOR_APPCORE_OPS_H__

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

#define LOG_TAG "DALI_APPCORE"

#include <dlog.h>
#include <appcore-efl.h>
#include <app.h>

namespace Dali
{
namespace Internal
{
namespace Adaptor
{

/**
 * Wrapper class to simplify invoking operations callbacks
 */
class Operations
{
public:
  Operations(appcore_ops* ops)
  : mOps(ops),
    mEventCallbacks(NULL),
    mUserData(NULL)
  {
  }

  Operations(app_event_callback_s* eventCallbacks, void* userData)
  : mOps(NULL),
    mEventCallbacks(eventCallbacks),
    mUserData(userData)
  {
  }

  ~Operations()
  {
  }

  inline void Create()
  {
    if( mOps && mOps->create )
    {
      LOGD("Appcore operation: Create\n");
      mOps->create(mOps->data);
    }
    if( mEventCallbacks && mEventCallbacks->create )
    {
      LOGD("App event callback: Create\n");
      mEventCallbacks->create(mUserData);
    }
  }

  inline void Reset(bundle* bundle)
  {
    if( mOps && mOps->reset )
    {
      LOGD("Appcore operation: Reset\n");
      mOps->reset(bundle, mOps->data);
    }
  }

  inline void Terminate()
  {
    if( mOps && mOps->terminate )
    {
      LOGD("Appcore operation: Terminate\n");
      mOps->terminate(mOps->data);
    }
    if( mEventCallbacks && mEventCallbacks->terminate )
    {
      LOGD("App event callback: Terminate\n");
      mEventCallbacks->terminate(mUserData);
    }
  }

  inline void Resume()
  {
    if( mOps && mOps->resume )
    {
      LOGD("Appcore operation: Resume\n");
      mOps->resume(mOps->data);
    }
    if( mEventCallbacks && mEventCallbacks->resume )
    {
      LOGD("App event callback: Resume\n");
      mEventCallbacks->resume(mUserData);
    }
  }

  inline void Pause()
  {
    if( mOps && mOps->pause )
    {
      LOGD("Appcore operation: Pause\n");
      mOps->pause(mOps->data);
    }
    if( mEventCallbacks && mEventCallbacks->pause )
    {
      LOGD("App event callback: Pause\n");
      mEventCallbacks->pause(mUserData);
    }
  }

private:
  appcore_ops* mOps;
  app_event_callback_s* mEventCallbacks;
  void *mUserData;
};

} // Adaptor
} // Internal
} // Dali

#endif // __DALI_INTERNAL_ADAPTOR_APPCORE_OPS_H__
