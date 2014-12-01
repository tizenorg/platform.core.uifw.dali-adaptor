#ifndef __DALI_INTERNAL_ADAPTOR_APPCORE_ROTATION_H__
#define __DALI_INTERNAL_ADAPTOR_APPCORE_ROTATION_H__

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

#include <Ecore_X.h>
#include <vconf.h>
#include <appcore-efl.h>

// @todo Enable through feature flags (switch off for TV)
#undef SENSOR_ENABLED
//#define SENSOR_ENABLED

#if defined(SENSOR_ENABLED)

#if defined(TIZEN_SDK_2_2)
#include <sensor.h>
#elif defined(TIZEN_SDK_2_3)
#include <sensor_internal.h>
#endif

typedef sensor_event_data_t* SensorEventData;

#else  // SENSOR_ENABLED

typedef void* SensorEventData;

#endif // SENSOR_ENABLED

namespace Dali
{
namespace Internal
{
namespace Adaptor
{

/**
 * This class tracks the current orientation of the device, informing the observer
 * when the orientation changes. It also tracks the orientation lock that allows the
 * user to lock the application to portrait orientation.
 */
class AppcoreRotation
{
public:
  class Observer
  {
  public:
    /**
     * Inform observer of device rotation change
     * @param[in] the new rotation
     */
    virtual void RotationChanged(enum appcore_rm rotation) = 0;
  };

public:
  /**
   * Constructor.
   * Connect to the sensor service
   * @param[in] observer Observer of rotation events
   */
  AppcoreRotation(Observer* observer);

  /**
   * Destructor
   * Disconnect from the sensor service
   */
  ~AppcoreRotation();

  /**
   * Get the current rotation
   * @return The current rotation
   */
  enum appcore_rm GetRotation();

  /**
   * Enable callback. (Enabled by default after a connect)
   * @return true if enabled
   */
  bool Enable();

  /**
   * Disable callback. Useful when appcore is in a paused state
   */
  void Disable();

private:
  /**
   * Connect to the sensor service
   */
  void Connect();

  /**
   * Disconnect from the sensor service
   */
  void Disconnect();

  /**
   * Fetch the current rotation from the sensor
   */
  enum appcore_rm FetchState();

  /**
   * Callback for rotation changed
   * @param[in] eventType Accelerometer event type
   * @param[in] event     event (incl. rotation data)
   * @param[in] data      Client data
   */
  static void RotationChangedCallback(unsigned int eventType, SensorEventData event, void *data);

  /**
   * Handler for rotation changed.
   * @param[in] eventType Accelerometer event type
   * @param[in] event     event (incl. rotation data)
   */
  void OnRotationChanged(unsigned int event_type, SensorEventData event);

  /**
   * Fetch user lock state and add callback for lock state change
   */
  void AddLockCallback();

  /**
   * Remove lock callback
   */
  void RemoveLockCallback();

  /**
   * Callback for user rotation lock state change
   * @param[in] node The Vconf node for lock state change
   * @param[in] data Client data (this class pointer)
   */
  static void LockCallback(keynode_t *node, void *data);

  /**
   * Lock state change handler.
   * On locked, change rotation back to Portrait
   * On Unlocked, change rotation to current orientation
   * @param[in] node The Vconf node for lock state change
   */
  void OnLock(keynode_t *node);

private:
  Ecore_X_Atom   mAtomRotationLock; ///< Window atom for rotation lock
  Ecore_X_Window mRootWindow;       ///< Root window of application

  enum appcore_rm mRotation;        ///< Current orientation
  bool mLock;                       ///< Current user lock status
  int mSensorServiceHandle;         ///< Handle to sensor service
  int mSensorServiceConnected;      ///< Whether there is a connection to the sensor service
  int mSensorServiceEnabled;        ///< If the sensor connection is enabled

  Observer* mObserver;              ///< Observer of rotation events
};

} //Adaptor
} //Internal
} //Dali


#endif //__DALI_INTERNAL_ADAPTOR_APPCORE_ROTATION_H__
