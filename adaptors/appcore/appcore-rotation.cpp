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

#include "appcore-rotation.h"

#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include <vconf.h>
#include <Ecore_X.h>
#include <Ecore.h>
#include <X11/Xlib.h>

#if defined(SENSOR_ENABLED)
namespace
{
const char* STR_ATOM_ROTATION_LOCK("_E_ROTATION_LOCK");
}
#endif

namespace Dali
{
namespace Internal
{
namespace Adaptor
{

AppcoreRotation::AppcoreRotation(Observer* observer)
: mRotation(),
  mLock(false),
  mSensorServiceHandle(0),
  mSensorServiceConnected(false),
  mSensorServiceEnabled(false),
  mObserver(observer)
{
  Connect();
}

AppcoreRotation::~AppcoreRotation()
{
  Disconnect();
}

enum appcore_rm AppcoreRotation::GetRotation()
{
  return mRotation;
}

bool AppcoreRotation::Enable()
{
#if defined(SENSOR_ENABLED)
  if( mSensorServiceConnected && ! mSensorServiceEnabled )
  {
    int registered = sf_register_event( mSensorServiceHandle, ACCELEROMETER_EVENT_ROTATION_CHECK, NULL,
                                        RotationChangedCallback, this );

    int r = sf_start( mSensorServiceHandle, 0 );
    if( r < 0 && registered )
    {
      sf_unregister_event(mSensorServiceHandle, ACCELEROMETER_EVENT_ROTATION_CHECK);
    }
    mSensorServiceEnabled = true;
  }

  AddLockCallback();

  enum appcore_rm rotation = FetchState();
  if( mRotation != rotation && mLock == false )
  {
    if( mObserver )
    {
      mObserver->RotationChanged(rotation);
    }

    mRotation = rotation;
  }
#endif

  return mSensorServiceEnabled;
}

void AppcoreRotation::Disable()
{
#if defined(SENSOR_ENABLED)
  if( mSensorServiceConnected && mSensorServiceEnabled )
  {
    RemoveLockCallback();

    sf_unregister_event( mSensorServiceHandle, ACCELEROMETER_EVENT_ROTATION_CHECK );
    sf_stop( mSensorServiceHandle );

    mSensorServiceEnabled = false;
  }
#endif
}

void AppcoreRotation::Connect()
{
  mSensorServiceConnected = false;

#if defined(SENSOR_ENABLED)
  mSensorServiceHandle = sf_connect(ACCELEROMETER_SENSOR);

  if( mSensorServiceHandle >= 0 )
  {
    int r = sf_register_event(mSensorServiceHandle, ACCELEROMETER_EVENT_ROTATION_CHECK, NULL, RotationChangedCallback, this);
    if (r < 0)
    {
      sf_disconnect(mSensorServiceHandle);
      mSensorServiceHandle = -1;
    }
    else
    {
      r = sf_start(mSensorServiceHandle, 0);
      if (r < 0)
      {
        // Error, disconnect.
        sf_unregister_event(mSensorServiceHandle, ACCELEROMETER_EVENT_ROTATION_CHECK);
        sf_disconnect(mSensorServiceHandle);
        mSensorServiceHandle = -1;
      }
      else
      {
        mSensorServiceConnected = true;
        mSensorServiceEnabled = true;

        AddLockCallback();

        mAtomRotationLock = ecore_x_atom_get(STR_ATOM_ROTATION_LOCK);
        mRootWindow = ecore_x_window_root_first_get();
        XSelectInput(static_cast<Display*>(ecore_x_display_get()), mRootWindow, PropertyChangeMask);
      }
    }
  }
#endif
}

void AppcoreRotation::Disconnect()
{
#if defined(SENSOR_ENABLED)
  RemoveLockCallback();

  if( mSensorServiceConnected )
  {
    sf_unregister_event(mSensorServiceHandle, ACCELEROMETER_EVENT_ROTATION_CHECK);
    sf_stop(mSensorServiceHandle);
    sf_disconnect(mSensorServiceHandle);

    mSensorServiceEnabled = false;
    mSensorServiceConnected = false;
    mSensorServiceHandle = -1;
  }
#endif
}

enum appcore_rm ConvertEvent(unsigned long event_data)
{
  enum appcore_rm m = APPCORE_RM_UNKNOWN;

#if defined(SENSOR_ENABLED)

  switch (event_data)
  {
    case ROTATION_EVENT_0:
      m = APPCORE_RM_PORTRAIT_NORMAL;
      break;
    case ROTATION_EVENT_90:
      m = APPCORE_RM_LANDSCAPE_NORMAL;
      break;
    case ROTATION_EVENT_180:
      m = APPCORE_RM_PORTRAIT_REVERSE;
      break;
    case ROTATION_EVENT_270:
      m = APPCORE_RM_LANDSCAPE_REVERSE;
      break;

    default:
      break;
  }
#endif

  return m;
}

enum appcore_rm AppcoreRotation::FetchState()
{
  enum appcore_rm currentRotation = APPCORE_RM_UNKNOWN;

#if defined(SENSOR_ENABLED)
  unsigned long event;
  int r = sf_check_rotation(&event);
  if( r >= 0 )
  {
    currentRotation = ConvertEvent(event);
  }
#endif

  return currentRotation;
}


void AppcoreRotation::RotationChangedCallback(unsigned int event_type, SensorEventData event, void *data)
{
#if defined(SENSOR_ENABLED)
  AppcoreRotation* appcoreRotation = static_cast<AppcoreRotation*>( data );
  if( appcoreRotation != NULL )
  {
    appcoreRotation->OnRotationChanged( event_type, event );
  }
#endif
}

void AppcoreRotation::OnRotationChanged(unsigned int event_type, SensorEventData event)
{
#if defined(SENSOR_ENABLED)
  enum appcore_rm rotation;

  if( ! mLock && event_type == ACCELEROMETER_EVENT_ROTATION_CHECK )
  {
    rotation = ConvertEvent(*(long unsigned *)(event->event_data));

    if( mObserver != NULL && mRotation != rotation)
    {
      mObserver->RotationChanged(rotation);
      mRotation = rotation;
    }
  }
#endif
}

void AppcoreRotation::AddLockCallback()
{
#if defined(SENSOR_ENABLED)
  int lock;

  lock = 0;
  vconf_get_bool(VCONFKEY_SETAPPL_AUTO_ROTATE_SCREEN_BOOL, &lock);

  mLock = (lock == 0);

  vconf_notify_key_changed(VCONFKEY_SETAPPL_AUTO_ROTATE_SCREEN_BOOL, LockCallback, this);
#endif
}

void AppcoreRotation::RemoveLockCallback()
{
#if defined(SENSOR_ENABLED)
  vconf_ignore_key_changed(VCONFKEY_SETAPPL_AUTO_ROTATE_SCREEN_BOOL, LockCallback);
  mLock = false;
#endif
}

void AppcoreRotation::LockCallback(keynode_t *node, void *data)
{
#if defined(SENSOR_ENABLED)
  AppcoreRotation* appcoreRotation = static_cast<AppcoreRotation*>(data);
  if( appcoreRotation != NULL )
  {
    appcoreRotation->OnLock(node);
  }
#endif
}

void AppcoreRotation::OnLock(keynode_t *node)
{
#if defined(SENSOR_ENABLED)

  mLock = ! vconf_keynode_get_bool(node);

  if (mLock)
  {
    enum appcore_rm rotation = APPCORE_RM_PORTRAIT_NORMAL;
    if (mRotation != rotation)
    {
      if( mObserver != NULL)
      {
        mObserver->RotationChanged(rotation);
      }
      mRotation = rotation;
    }
  }
  else
  {
    if( mObserver != NULL )
    {
      enum appcore_rm rotation = FetchState();
      if( mRotation != rotation )
      {
        mObserver->RotationChanged(rotation);
        mRotation = rotation;
      }
    }
  }
#endif
}

} // Adaptor
} // Internal
} // Dali
