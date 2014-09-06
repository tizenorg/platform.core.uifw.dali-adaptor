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
#include "orientation-impl.h"

// EXTERNAL INCLUDES
#include <dali/integration-api/core.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <internal/common/window-impl.h>
#include <internal/common/adaptor-impl.h>

namespace Dali
{
namespace Internal
{
namespace Adaptor
{

Orientation* Orientation::New(Window* window)
{
  Orientation* orientation = new Orientation(window);

  return orientation;
}

Orientation::Orientation(Window* window)
: mWindow(window),
  mOrientation(0),
  mWindowWidth(0),
  mWindowHeight(0)
{
}

Orientation::~Orientation()
{
  // Note, there is only one orientation object that's owned by window,
  // so it will live longer than adaptor. (hence, no need to remove rotation observer)
}

void Orientation::SetAdaptor(Dali::Adaptor& adaptor)
{
  Adaptor::GetImplementation(adaptor).SetRotationObserver(this);
}

int Orientation::GetDegrees() const
{
  return mOrientation;
}

float Orientation::GetRadians() const
{
  return Math::PI * (float)mOrientation / 180.0f;
}

Orientation::OrientationSignalV2& Orientation::ChangedSignal()
{
  return mChangedSignal;
}

void Orientation::OnRotationPrepare( const RotationEvent& rotation )
{
  mOrientation  = rotation.angle;
  mWindowWidth  = rotation.width;
  mWindowHeight = rotation.height;
}

void Orientation::OnRotationRequest()
{
  // Emit signal
  if( !mChangedSignal.Empty() )
  {
    Dali::Orientation handle( this );
    mChangedSignal.Emit( handle );
  }

  if( mWindow != NULL )
  {
    mWindow->RotationDone( mOrientation, mWindowWidth, mWindowHeight );
  }
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
