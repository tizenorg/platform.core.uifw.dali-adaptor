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

// CLASS HEADER
#include "android-callback-manager.h"

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <adaptors/integration-api/framework.h>

// EXTERNAL INCLUDES


namespace Dali
{

namespace Internal
{

namespace Adaptor
{

AndroidCallbackManager::AndroidCallbackManager()
:mRunning(false)
{
}

AndroidCallbackManager::~AndroidCallbackManager()
{
  mFramework->SetIdleHandler( NULL );
}

void AndroidCallbackManager::SetFramework( Integration::Framework* framework )
{
  mFramework = framework;
  mFramework->SetIdleHandler( this );
}

void AndroidCallbackManager::Start()
{
  DALI_ASSERT_DEBUG( mRunning == false );

  mRunning = true;
}

void AndroidCallbackManager::Stop()
{
  // make sure we're not called twice
  DALI_ASSERT_DEBUG( mRunning == true );

  RemoveAllCallbacks();

  mRunning = false;
}

void AndroidCallbackManager::Run()
{
  for( CallbackList::iterator  iter =  mCallbackContainer.begin(); iter != mCallbackContainer.end(); ++iter)
  {
    CallbackBase* callback = (*iter);

    iter = mCallbackContainer.erase( iter );

    // run the function
    CallbackBase::Execute( *callback );

    delete callback;
  }
  mCallbackContainer.clear();
}


bool AndroidCallbackManager::AddIdleCallback( CallbackBase* callback )
{
  bool added(false);

  if( mRunning )
  {
    // add the call back to the container
    mCallbackContainer.push_front( callback );
    added = true;
  }

  return added;
}


void AndroidCallbackManager::RemoveAllCallbacks()
{
  // always called from main thread

  for( CallbackList::iterator  iter =  mCallbackContainer.begin(); iter != mCallbackContainer.end(); ++iter)
  {
    CallbackBase* callback = (*iter);
    delete callback;
  }
  mCallbackContainer.clear();
}

// Creates a concrete interface for CallbackManager
CallbackManager* CallbackManager::New()
{
  return new AndroidCallbackManager;
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
