#ifndef __DALI_INTERNAL_ADAPTOR_VCONF_EVENTS_H__
#define __DALI_INTERNAL_ADAPTOR_VCONF_EVENTS_H__

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

#include <appcore-efl.h>
#include <vconf.h>

#include <vector>
#include <cstdio>

#define APPCORE_NUMBER_OF_EVENTS (APPCORE_EVENT_REGION_CHANGE+1)

namespace Dali
{
namespace Internal
{
namespace Adaptor
{

typedef int (EventCallback)  (void*);

/**
 * Struct to hold callback pointer and client data
 */
struct EventCallbackAndData
{
  EventCallbackAndData()
  : callback(NULL),
    data(NULL)
  {
  }
  EventCallback* callback;
  void*          data;
};


/**
 * Handles particular events from VConf
 */
class VconfEvents
{
public:
  class Observer
  {
  public:
    virtual void OnLowMemory() = 0;
  };

public:
  /**
   * Operations to perform following receipt of Vconf callback for key
   */
  struct EventOperation
  {
    /**
     * Constructor
     * @param[in] key VConf key of operation
     * vconf The parent VconfEvents object
     */
    EventOperation(const char *key, VconfEvents& vconf);

    /**
     * Interface requires virtual destructor
     */
    virtual ~EventOperation();

    /**
     * Pre-operation algorithm
     * @param[in] key VConf key of operation
     */
    virtual void PreOperation(keynode_t* key) = 0;

    /**
     * Perform operation (will trigger callbacks)
     * @param[in] key VConf key of operation
     */
    virtual void Operation(keynode_t* key) = 0;

    /**
     * Post-operation algorithm
     * @param[in] key VConf key of operation
     */
    virtual void PostOperation(keynode_t* key) = 0;

    /**
     * VConf Callback handler - invokes operation methods on derived class
     * @param[in] key  VConf key of operation
     * @param[in] data Client data
     */
    static void Callback(keynode_t *key, void *data);

    /**
     * Invoke callback on VconfEvents class
     * @param[in] event Event of invoking operation
     */
    void InvokeCallback(enum appcore_event event);

    /**
     * Invoke VconfEvents::Observer::OnLowMemory() method on observer
     */
    void InvokeLowMemoryObserver();

    VconfEvents& mVconfEventHandler; ///< Parent object
  };

public:
  /**
   * Constructor
   * @param[in] observer Observer of low memory event
   */
  VconfEvents(Observer& observer);

  /**
   * Destructor
   */
  ~VconfEvents();

  /**
   * Add an event callback.
   * Only one callback can be added per event
   * @param[in] event Event to watch
   * @param[in] cb Callback to invoke when event occurs
   * @param[in] data Client data
   */
  void AddEventCallback(enum appcore_event event, int (*cb) (void *), void *data);

private:
  /**
   * Invokes any event callback for the given event
   */
  void InvokeEventCallback(enum appcore_event event);

private:
  Observer&                    mObserver; ///< Low memory observer
  std::vector<EventOperation*> mEventOperations; ///< The operations to perform for various events
  EventCallbackAndData         mEventCallbacks[APPCORE_NUMBER_OF_EVENTS]; // Callbacks for each event
};

} // Adaptor
} // Internal
} // Dali

#endif // __DALI_INTERNAL_ADAPTOR_VCONF_EVENTS_H__
