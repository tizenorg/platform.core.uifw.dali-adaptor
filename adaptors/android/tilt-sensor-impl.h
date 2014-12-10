#ifndef __DALI_INTERNAL_ADAPTOR_TILT_SENSOR_H__
#define __DALI_INTERNAL_ADAPTOR_TILT_SENSOR_H__

/*
Copyright (c) 2000-2012 Samsung Electronics Co., Ltd All Rights Reserved

This file is part of Dali Adaptor

PROPRIETARY/CONFIDENTIAL

This software is the confidential and proprietary information of
SAMSUNG ELECTRONICS ("Confidential Information"). You shall not
disclose such Confidential Information and shall use it only in
accordance with the terms of the license agreement you entered
into with SAMSUNG ELECTRONICS.

SAMSUNG make no representations or warranties about the suitability
of the software, either express or implied, including but not limited
to the implied warranties of merchantability, fitness for a particular
purpose, or non-infringement. SAMSUNG shall not be liable for any
damages suffered by licensee as a result of using, modifying or
distributing this software or its derivatives.
*/

// EXTERNAL INCLUDES
#include <deque>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/adaptor-framework/timer.h>

// INTERNAL INCLUDES
#include <public-api/adaptor-framework/tilt-sensor.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

/**
 * TiltSensor provides pitch & roll values when the device is tilted.
 */
class TiltSensor : public Dali::BaseObject
{
public:

  typedef Dali::TiltSensor::TiltedSignalV2 TiltedSignalV2;

  /**
   * Create a TiltSensor.
   * This should only be called once by the Adaptor class.
   * @return A newly allocated tilt-sensor.
   */
  static Dali::TiltSensor New();

  /**
   * @copydoc Dali::TiltSensor::Get()
   */
  static Dali::TiltSensor Get();

  /**
   * @copydoc Dali::TiltSensor::Enable()
   */
  bool Enable();

  /**
   * @copydoc Dali::TiltSensor::Disable()
   */
  void Disable();

  /**
   * @copydoc Dali::TiltSensor::IsEnabled()
   */
  bool IsEnabled() const;

  /**
   * @copydoc Dali::TiltSensor::GetRoll()
   */
  float GetRoll() const;

  /**
   * @copydoc Dali::TiltSensor::GetPitch()
   */
  float GetPitch() const;

  /**
   * @copydoc Dali::TiltSensor::GetRotation()
   */
  Quaternion GetRotation() const;

  /**
   * @copydoc Dali::TiltSensor::TiltedSignal()
   */
  TiltedSignalV2& TiltedSignal();

  /**
   * @copydoc Dali::TiltSensor::SetUpdateFrequency()
   */
  void SetUpdateFrequency( float frequencyHertz );

  /**
   * @copydoc Dali::TiltSensor::GetUpdateFrequency()
   */
  float GetUpdateFrequency() const;

  /**
   * @copydoc Dali::TiltSensor::SetRotationThreshold()
   */
  void SetRotationThreshold(Radian rotationThreshold);

  /**
   * @copydoc Dali::TiltSensor::GetRotationThreshold()
   */
  Radian GetRotationThreshold() const;

  /**
   * Connects a callback function with the object's signals.
   * @param[in] object The object providing the signal.
   * @param[in] tracker Used to disconnect the signal.
   * @param[in] signalName The signal to connect to.
   * @param[in] functor A newly allocated FunctorDelegate.
   * @return True if the signal was connected.
   * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the caller is responsible for deleting the unused functor.
   */
  static bool DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor );

private:

  /**
   * Private constructor; see also TiltSensor::New()
   */
  TiltSensor();

  /**
   * Destructor
   */
  virtual ~TiltSensor();

  /**
   * Timer callback to update the tilt values
   */
  bool Update();

  // Undefined
  TiltSensor(const TiltSensor&);

  // Undefined
  TiltSensor& operator=(TiltSensor&);

private:

  float mFrequencyHertz;
  Dali::Timer mTimer;
  SlotDelegate< TiltSensor > mTimerSlot;

  int mSensorFrameworkHandle;

  float mRoll;
  float mPitch;
  Quaternion mRotation;

  Radian mRotationThreshold;

  std::deque<float> mRollValues;
  std::deque<float> mPitchValues;

  TiltedSignalV2 mTiltedSignalV2;
};

} // namespace Adaptor

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::Adaptor::TiltSensor& GetImplementation(Dali::TiltSensor& sensor)
{
  DALI_ASSERT_ALWAYS( sensor && "TiltSensor handle is empty" );

  BaseObject& handle = sensor.GetBaseObject();

  return static_cast<Internal::Adaptor::TiltSensor&>(handle);
}

inline const Internal::Adaptor::TiltSensor& GetImplementation(const Dali::TiltSensor& sensor)
{
  DALI_ASSERT_ALWAYS( sensor && "TiltSensor handle is empty" );

  const BaseObject& handle = sensor.GetBaseObject();

  return static_cast<const Internal::Adaptor::TiltSensor&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_ADAPTOR_TILT_SENSOR_H__
