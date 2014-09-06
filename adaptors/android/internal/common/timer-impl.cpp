/*
Copyright (c) 2000-2013 Samsung Electronics Co., Ltd All Rights Reserved

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
#include "timer-impl.h"

// EXTERNAL INCLUDES
#include <android-interface.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

// LOCAL STUFF
namespace
{
bool TimerSourceFunc (void *data)
{
  Timer* timer = static_cast<Timer*>(data);
  return timer->Tick();
}
} // unnamed namespace

/**
 * Struct to hide away Ecore implementation details
 */
struct Timer::Impl
{
  Impl( unsigned int milliSec )
  : mId(NULL),
    mInterval(milliSec)
  {
  }

  AndroidTimer* mId;
  unsigned int  mInterval;
};

TimerPtr Timer::New( unsigned int milliSec )
{
  TimerPtr timer( new Timer( milliSec ) );
  return timer;
}

Timer::Timer( unsigned int milliSec )
: mImpl(new Impl(milliSec))
{
}

Timer::~Timer()
{
  // stop timers
  Stop();

  delete mImpl;
}

void Timer::Start()
{
  if(mImpl->mId > 0)
  {
    Stop();
  }
  mImpl->mId = androidcore_add_timer( (float)mImpl->mInterval/1000.0f, TimerSourceFunc, this );
}

void Timer::Stop()
{
  if (mImpl->mId != NULL)
  {
    androidcore_remove_timer(mImpl->mId);
    mImpl->mId = NULL;
  }
}

void Timer::SetInterval( unsigned int interval )
{
  // stop existing timer
  Stop();
  mImpl->mInterval = interval;
  // start new tick
  Start();
}

unsigned int Timer::GetInterval() const
{
  return mImpl->mInterval;
}

bool Timer::Tick()
{
  // Guard against destruction during signal emission
  Dali::Timer handle( this );

  bool retVal( false );

  // Override with new signal if used
  if( !mTickSignal.Empty() )
  {
    retVal = mTickSignal.Emit();

    // Timer stops if return value is false
    if (retVal == false)
    {
      Stop();
    }
    else
    {
      retVal = true;   // continue emission
    }
  }
  else // no callbacks registered
  {
    // periodic timer is started but nobody listens, continue
    retVal = true;
  }

  return retVal;
}

Dali::Timer::TimerSignalV2& Timer::TickSignal()
{
  return mTickSignal;
}

bool Timer::IsRunning() const
{
  return mImpl->mId != NULL;
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
