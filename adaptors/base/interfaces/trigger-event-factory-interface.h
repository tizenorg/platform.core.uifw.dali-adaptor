#ifndef __DALI_INTERNAL_BASE_TRIGGER_EVENT_FACTORY_INTERFACE_H__
#define __DALI_INTERNAL_BASE_TRIGGER_EVENT_FACTORY_INTERFACE_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// INTERNAL INCLUDES
#include <base/interfaces/trigger-event-interface.h>

// EXTERNAL INCLUDES
#include <boost/function.hpp>


namespace Dali
{

namespace Internal
{

namespace Adaptor
{

/**
 * Trigger interface factory class for creating a TriggerEvent implementation
 *
 */
class TriggerEventFactoryInterface
{
public:

  /**
   * Create a new concrete implementation of the event trigger interface.
   * @param functor the function to call when interface->Trigger() is called
   * @return pointer to a new trigger event
   */
  virtual TriggerEventInterface* CreateTriggerEvent(  boost::function<void()> functor ) = 0;


protected:

  /**
   * Constructor
   */
  TriggerEventFactoryInterface()
  {
  }

  /**
   * Virtual Destructor
   */
  virtual ~TriggerEventFactoryInterface()
  {
  }

private:


  // Undefined copy constructor.
  TriggerEventFactoryInterface( const TriggerEventFactoryInterface& );

  // Undefined assignment operator.
  TriggerEventFactoryInterface& operator=( const TriggerEventFactoryInterface& );

};


} // namespace Internal

} // namespace Adaptor

} // namespace Dali

#endif // __DALI_INTERNAL_BASE_TRIGGER_EVENT_FACTORY_INTERFACE_H__
