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
 *
 */

// CLASS HEADER
#include "timer-impl.h"

// EXTERNAL INCLUDES
#include "android-interface.h"

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
