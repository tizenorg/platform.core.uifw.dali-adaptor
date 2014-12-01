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

#include "vconf-events.h"

#include <vconf.h>

#include "appcore-i18n.h"

namespace Dali
{
namespace Internal
{
namespace Adaptor
{


VconfEvents::EventOperation::EventOperation(const char *key, VconfEvents& vconf)
: mVconfEventHandler(vconf)
{
  vconf_notify_key_changed(key, Callback, this);
}

VconfEvents::EventOperation::~EventOperation()
{
}

void VconfEvents::EventOperation::Callback(keynode_t *key, void *data)
{
  if( data != NULL )
  {
    VconfEvents::EventOperation* eventOperation = static_cast<VconfEvents::EventOperation*>(data);
    eventOperation->PreOperation(key);
    eventOperation->Operation(key);
    eventOperation->PostOperation(key);
  }
}

void VconfEvents::EventOperation::InvokeCallback(enum appcore_event event)
{
  mVconfEventHandler.InvokeEventCallback(event);
}

void VconfEvents::EventOperation::InvokeLowMemoryObserver()
{
  mVconfEventHandler.mObserver.OnLowMemory();
}

/**
 * Low memory handler
 */
struct LowMemoryOperation : public VconfEvents::EventOperation
{
  LowMemoryOperation(const char* key, VconfEvents& vconf)
  : VconfEvents::EventOperation(key, vconf)
  {
  }

  virtual ~LowMemoryOperation(){}

  virtual void PreOperation(keynode_t *key)
  {
  }

  virtual void Operation(keynode_t *key)
  {
    int val = vconf_keynode_get_int(key);

    if (val >= VCONFKEY_SYSMAN_LOW_MEMORY_SOFT_WARNING)
    {
      InvokeCallback(APPCORE_EVENT_LOW_MEMORY);
    }
  }

  virtual void PostOperation(keynode_t *key)
  {
    int val = vconf_keynode_get_int(key);

    if (val >= VCONFKEY_SYSMAN_LOW_MEMORY_SOFT_WARNING)
    {
      InvokeLowMemoryObserver();
    }
  }
};

struct LowBatteryOperation : public VconfEvents::EventOperation
{
  LowBatteryOperation(const char* key, VconfEvents& vconf)
  : VconfEvents::EventOperation(key, vconf)
  {
  }

  virtual ~LowBatteryOperation(){}

  virtual void PreOperation(keynode_t *key)
  {
  }

  virtual void Operation(keynode_t *key)
  {
    int val = vconf_keynode_get_int( key );
    if( val <= VCONFKEY_SYSMAN_BAT_CRITICAL_LOW )
    {
      InvokeCallback( APPCORE_EVENT_LOW_BATTERY );
    }
  }

  virtual void PostOperation(keynode_t *key)
  {
  }
};

struct LanguageSetOperation : public VconfEvents::EventOperation
{
  LanguageSetOperation(const char* key, VconfEvents& vconf)
  : VconfEvents::EventOperation(key, vconf)
  {
  }

  virtual ~LanguageSetOperation(){}

  virtual void PreOperation(keynode_t *key)
  {
    UpdateLanguage();
  }

  virtual void Operation(keynode_t *key)
  {
    InvokeCallback( APPCORE_EVENT_LANG_CHANGE );
  }

  virtual void PostOperation(keynode_t *key)
  {
  }
};

struct RegionFormatOperation : public VconfEvents::EventOperation
{
  RegionFormatOperation(const char* key, VconfEvents& vconf)
  : VconfEvents::EventOperation(key, vconf)
  {
  }

  virtual ~RegionFormatOperation(){}

  virtual void PreOperation(keynode_t *key)
  {
    UpdateRegion();
  }

  virtual void Operation(keynode_t *key)
  {
    InvokeCallback( APPCORE_EVENT_REGION_CHANGE );
  }

  virtual void PostOperation(keynode_t *key)
  {
  }
};

struct RegionChangeOperation : public VconfEvents::EventOperation
{
  RegionChangeOperation(const char* key, VconfEvents& vconf)
  : VconfEvents::EventOperation(key, vconf)
  {
  }

  virtual ~RegionChangeOperation(){}

  virtual void PreOperation(keynode_t *key)
  {
  }

  virtual void Operation(keynode_t *key)
  {
    InvokeCallback( APPCORE_EVENT_REGION_CHANGE );
  }

  virtual void PostOperation(keynode_t *key)
  {
  }
};


VconfEvents::VconfEvents(Observer& observer)
: mObserver(observer)
{
  mEventOperations.push_back(new LowMemoryOperation(VCONFKEY_SYSMAN_LOW_MEMORY, *this));
  mEventOperations.push_back(new LowBatteryOperation(VCONFKEY_SYSMAN_BATTERY_STATUS_LOW, *this));
  mEventOperations.push_back(new LanguageSetOperation(VCONFKEY_LANGSET, *this));
  mEventOperations.push_back(new RegionFormatOperation(VCONFKEY_REGIONFORMAT, *this));
  mEventOperations.push_back(new RegionChangeOperation(VCONFKEY_REGIONFORMAT_TIME1224, *this));

  for(size_t i=0; i < APPCORE_NUMBER_OF_EVENTS; i++)
  {
    mEventCallbacks[i].callback = NULL;
    mEventCallbacks[i].data = NULL;
  }
}

VconfEvents::~VconfEvents()
{
  for( std::size_t i = 0; i < mEventOperations.size(); i++ )
  {
    delete mEventOperations[i];
  }
  mEventOperations.clear();
}

void VconfEvents::AddEventCallback(enum appcore_event event, int (*cb) (void *), void *data)
{
  if(event > APPCORE_EVENT_UNKNOWN && event < APPCORE_NUMBER_OF_EVENTS)
  {
    mEventCallbacks[event].callback = cb;
    mEventCallbacks[event].data = data;
  }
}

void VconfEvents::InvokeEventCallback(enum appcore_event event)
{
  if(event > APPCORE_EVENT_UNKNOWN && event < APPCORE_NUMBER_OF_EVENTS)
  {
    if( mEventCallbacks[event].callback != NULL )
    {
      mEventCallbacks[event].callback(mEventCallbacks[event].data);
    }
  }
}


} // Adaptor
} // Internal
} // Dali
