/*
Copyright (c) 2000-2014 Samsung Electronics Co., Ltd All Rights Reserved

This file is part of Dali

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

// HEADER
#include <public-api/adaptor-framework/common/event-feeder.h>

// INTERNAL INCLUDES
#include <internal/common/adaptor-impl.h>

namespace Dali
{

namespace EventFeeder
{

void FeedTouchPoint( TouchPoint& point, int timeStamp )
{
  if ( Adaptor::IsAvailable() )
  {
    Internal::Adaptor::Adaptor::GetImplementation( Adaptor::Get() ).FeedTouchPoint( point, timeStamp );
  }
}

void FeedWheelEvent( MouseWheelEvent& wheelEvent )
{
  if ( Adaptor::IsAvailable() )
  {
    Internal::Adaptor::Adaptor::GetImplementation( Adaptor::Get() ).FeedWheelEvent( wheelEvent );
  }
}

void FeedKeyEvent( KeyEvent& keyEvent )
{
  if ( Adaptor::IsAvailable() )
  {
    Internal::Adaptor::Adaptor::GetImplementation( Adaptor::Get() ).FeedKeyEvent( keyEvent );
  }
}

} // namespace EventFeeder

} // namespace Dali
