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
#include <dali/public-api/adaptor-framework/common/orientation.h>

// INTERNAL INCLUDES
#include <internal/common/orientation-impl.h>

namespace Dali
{

Orientation::Orientation()
{
}

Orientation::~Orientation()
{
}

int Orientation::GetDegrees() const
{
  return Internal::Adaptor::GetImplementation(*this).GetDegrees();
}

float Orientation::GetRadians() const
{
  return Internal::Adaptor::GetImplementation(*this).GetRadians();
}

Orientation::OrientationSignalV2& Orientation::ChangedSignal()
{
  return Internal::Adaptor::GetImplementation(*this).ChangedSignal();
}

//Orientation::OrientationSignal Orientation::SignalChange()
//{
//  return Internal::Adaptor::GetImplementation(*this).SignalChange();
//}

Orientation::Orientation( Internal::Adaptor::Orientation* orientation )
: BaseHandle(orientation)
{
}

} // namespace Dali
