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

#include "appcore.h"
#include "app.h"

#include <appcore-efl.h>
#include <cstdio>
#include <Ecore.h>
#include <boost/thread.hpp>

#include <dali/public-api/common/dali-common.h>


boost::thread_specific_ptr<Dali::Internal::Adaptor::Appcore> gThreadLocalAppcore;

int app_efl_main(int *argc, char ***argv, app_event_callback_s *callbacks, void *userData)
{
  DALI_ASSERT_ALWAYS(gThreadLocalAppcore.get() == NULL && "Cannot create more than one Appcore per thread");

  gThreadLocalAppcore.reset(new Dali::Internal::Adaptor::Appcore(callbacks, userData));
  const char* appName = Dali::Internal::Adaptor::Appcore::GetAppName();
  gThreadLocalAppcore->Run(appName, *argc, *argv);
  gThreadLocalAppcore.release();

  return 0;
}

void app_efl_exit(void)
{
  DALI_ASSERT_ALWAYS(gThreadLocalAppcore.get() != NULL && "Appcore exit called before creation");

  gThreadLocalAppcore->Exit();
}

int appcore_efl_main(const char* name, int* argc, char*** argv, appcore_ops* operations)
{
  DALI_ASSERT_ALWAYS(gThreadLocalAppcore.get() == NULL && "Cannot create more than one Appcore per thread");

  gThreadLocalAppcore.reset(new Dali::Internal::Adaptor::Appcore(operations));

  gThreadLocalAppcore->Run(name, *argc, *argv);

  gThreadLocalAppcore.release();

  return 0;
}

int appcore_set_event_callback(enum appcore_event event, int (*cb) (void *), void *data)
{
  if(gThreadLocalAppcore.get() != NULL)
  {
    gThreadLocalAppcore->AddEventCallback(event, cb, data);
  }

  return 0;
}

int appcore_set_rotation_cb(int (*cb) (enum appcore_rm, void *), void *data)
{
  if(gThreadLocalAppcore.get() != NULL)
  {
    gThreadLocalAppcore->AddRotationCallback(cb, data);
  }

  return 0;
}

int appcore_unset_rotation_cb(void)
{
  if(gThreadLocalAppcore.get() != NULL)
  {
    gThreadLocalAppcore->RemoveRotationCallback();
  }
  return 0;
}

int appcore_set_system_resource_reclaiming(bool enable)
{
  if(gThreadLocalAppcore.get() != NULL)
  {
    gThreadLocalAppcore->SetSystemResourceReclamation(enable);
  }
  return 0;
}
