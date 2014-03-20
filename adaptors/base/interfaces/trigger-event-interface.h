#ifndef __DALI_INTERNAL_ADAPTOR_BASE_TRIGGER_EVENT_INTERFACE_H__
#define __DALI_INTERNAL_ADAPTOR_BASE_TRIGGER_EVENT_INTERFACE_H__

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


namespace Dali
{

namespace Internal
{

namespace Adaptor
{

/**
 * Interface for a trigger event class.
 * The trigger event is a way of running a function call back on the main event thread of Dali.
 * To create a trigger event interface use the factory interface.
 */
class TriggerEventInterface
{

public:

  /**
   * Virtual destructor
   */
  virtual ~TriggerEventInterface()
  {
  }

  /**
   * Triggers the event.
   *
   * This can be called from one thread in order to wake up another thread.
   */
  virtual void Trigger() = 0;

protected:

  /**
   * Constructor
   */
  TriggerEventInterface( )
  {
  }

private:
  // Undefined copy constructor.
  TriggerEventInterface( const TriggerEventInterface& );

  // Undefined assignment operator.
  TriggerEventInterface& operator=( const TriggerEventInterface& );


};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_ADAPTOR_BASE_TRIGGER_EVENT_INTERFACE_H__
