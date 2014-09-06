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
#include <dali/public-api/adaptor-framework/common/timer.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

// INTERNAL INCLUDES
#include <internal/common/timer-impl.h>

namespace Dali
{

Timer::Timer()
{
}

Timer Timer::New( unsigned int milliSec )
{
  Internal::Adaptor::TimerPtr internal = Internal::Adaptor::Timer::New( milliSec );
  return Timer(internal.Get());
}

Timer::Timer( const Timer& timer )
: BaseHandle(timer)
{
}

Timer& Timer::operator=( const Timer& timer )
{
  // check self assignment
  if( *this != timer )
  {
    BaseHandle::operator=(timer);
  }
  return *this;
}

Timer::~Timer()
{
}

void Timer::Start()
{
  Internal::Adaptor::GetImplementation(*this).Start();
}

void Timer::Stop()
{
  Internal::Adaptor::GetImplementation(*this).Stop();
}

void Timer::SetInterval( unsigned int interval )
{
  Internal::Adaptor::GetImplementation(*this).SetInterval( interval );
}

unsigned int Timer::GetInterval() const
{
  return Internal::Adaptor::GetImplementation(*this).GetInterval();
}

bool Timer::IsRunning() const
{
  return Internal::Adaptor::GetImplementation(*this).IsRunning();
}

Timer::TimerSignalV2& Timer::TickSignal()
{
  return Internal::Adaptor::GetImplementation(*this).TickSignal();
}

//Timer::TimerSignalDeprecated Timer::SignalTick()
//{
//  return Internal::Adaptor::GetImplementation(*this).SignalTick();
//}

Timer::Timer(Internal::Adaptor::Timer* timer)
: BaseHandle(timer)
{
}


} // namespace Dali
