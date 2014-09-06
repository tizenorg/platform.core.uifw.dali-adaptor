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

// CLASS HEADER
#include "tilt-sensor.h"

// INTERNAL INCLUDES
#include <internal/common/tilt-sensor-impl.h>
#include <internal/common/adaptor-impl.h>

namespace Dali
{

const char* const TiltSensor::SIGNAL_TILTED = "tilted";

const float TiltSensor::DEFAULT_UPDATE_FREQUENCY = 60.0f;

TiltSensor::TiltSensor()
{
}

TiltSensor TiltSensor::Get()
{
  return Internal::Adaptor::TiltSensor::New();
}

TiltSensor::~TiltSensor()
{
}

bool TiltSensor::Enable()
{
  return GetImplementation(*this).Enable();
}

void TiltSensor::Disable()
{
  GetImplementation(*this).Disable();
}

bool TiltSensor::IsEnabled() const
{
  return GetImplementation(*this).IsEnabled();
}

float TiltSensor::GetRoll() const
{
  return GetImplementation(*this).GetRoll();
}

float TiltSensor::GetPitch() const
{
  return GetImplementation(*this).GetPitch();
}

Quaternion TiltSensor::GetRotation() const
{
  return GetImplementation(*this).GetRotation();
}

TiltSensor::TiltedSignalV2& TiltSensor::TiltedSignal()
{
  return GetImplementation(*this).TiltedSignal();
}

void TiltSensor::SetUpdateFrequency( float frequencyHertz )
{
  GetImplementation(*this).SetUpdateFrequency( frequencyHertz );
}

float TiltSensor::GetUpdateFrequency() const
{
  return GetImplementation(*this).GetUpdateFrequency();
}

void TiltSensor::SetRotationThreshold(Radian rotationThreshold)
{
  GetImplementation(*this).SetRotationThreshold( rotationThreshold );
}

Radian TiltSensor::GetRotationThreshold() const
{
  return GetImplementation(*this).GetRotationThreshold();
}

TiltSensor::TiltSensor( Internal::Adaptor::TiltSensor* sensor )
: BaseHandle( sensor )
{
}

} // namespace Dali
