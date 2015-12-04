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

#include "android-framework-input-handlers.h"

#include <algorithm>
#include <dali/public-api/events/touch-point.h>

namespace Dali
{
namespace Integration
{


struct Framework::InputCallback
{
  void* mContext;
  bool  (*mFunction)(void*, const TouchEvent&);

  InputCallback( bool (*callback)(void*, const Framework::TouchEvent&), void* context)
  : mContext(context),
    mFunction(callback)
  {
  }

  void Call( const Framework::TouchEvent& event)
  {
    (*mFunction)( mContext, event );
  };
};

InputHandlerFactory::InputHandlerFactory()
{
}

InputHandlerFactory::~InputHandlerFactory()
{
}


Framework::InputCallback* InputHandlerFactory::Register(
  void* context,
  bool (*callback)(void*, const Framework::TouchEvent&))
{
  Framework::InputCallback* newCallback = new Framework::InputCallback(callback, context);
  mInputCallbacks.push_back(newCallback);

  return newCallback;
}

void InputHandlerFactory::Unregister( Framework::InputCallback* callback )
{
  std::list<Framework::InputCallback*>::iterator it = std::find(mInputCallbacks.begin(), mInputCallbacks.end(), callback);
  if (it != mInputCallbacks.end())
  {
    Framework::InputCallback* cb = (*it);
    it = mInputCallbacks.erase(it);
    delete cb;
  }
}

void InputHandlerFactory::Execute( const Framework::TouchEvent& event )
{
  for(std::list<Framework::InputCallback*>::iterator it = mInputCallbacks.begin(); it != mInputCallbacks.end(); ++it)
  {
    (*it)->Call( event );
  }
}


} // Integration
} // Dali
