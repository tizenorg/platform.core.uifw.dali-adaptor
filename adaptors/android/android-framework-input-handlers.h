#ifndef DALI_INTEGRATION_FRAMEWORK_INPUT_HANDLERS_H
#define DALI_INTEGRATION_FRAMEWORK_INPUT_HANDLERS_H

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

#include <list>
#include <adaptors/integration-api/framework.h>

class AndroidTouchEvent;

namespace Dali
{
namespace Integration
{

class InputHandlerFactory
{
public:
  /**
   * Constructor
   */
  InputHandlerFactory();

  /**
   * Destructor
   */
  ~InputHandlerFactory();

  /**
   * register an input callback handler
   * @param[in] context
   * @param[in] callback
   * @return The associated handler
   */
  Framework::InputCallback* Register( void* context, bool (*callback)(void*, const Framework::TouchEvent&));

  /**
   * Unregister an input call back handler
   * @param[in] callback The callback handler to unregister
   */
  void Unregister( Framework::InputCallback* callback );

  /**
   * Invokes all the registered callback handlers with the given event
   * @param[in] event The event to send
   */
  void Execute( const Framework::TouchEvent& event );

private:
  typedef std::list<Framework::InputCallback*> InputCallbacks;
  InputCallbacks mInputCallbacks;
};

} // namespace Integration
} // namespace Dali


#endif // DALI_INTEGRATION_FRAMEWORK_INPUT_HANDLERS_H
