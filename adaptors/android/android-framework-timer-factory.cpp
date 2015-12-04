/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

#include "android-framework-timer-factory.h"

#include <algorithm>

namespace Dali
{
namespace Integration
{

struct Framework::Timer
{
  void* mContext;
  bool  (*mFunction)(void*);
  bool  mRemoved;
  bool  mActive;
  float mTime;
  float mPeriodTime;

  Timer( float periodTime,
         bool (*callback)(void*),
         void* context )
  : mContext(context),
    mFunction(callback),
    mRemoved(false),
    mActive(true),
    mTime(0.0f),
    mPeriodTime(periodTime)
  {
  }

  bool Call( float elapsedTime )
  {
    mTime += elapsedTime;

    if( mActive && mTime >= mPeriodTime )
    {
      mActive = (*mFunction)(mContext);
      if( mActive )
      {
        mTime = 0.0f;
      }
    }

    return mRemoved;
  }
};



TimerFactory::TimerFactory()
{
}

TimerFactory::~TimerFactory()
{
  Reset();
}

Framework::Timer*  TimerFactory::Create( float precision, bool (*func)(void*), void* context )
{
  Framework::Timer* newTimer = new Framework::Timer(precision, func, context);
  mTimers.push_back(newTimer);
  return newTimer;
}


void TimerFactory::Destroy(Framework::Timer* timer)
{
  std::list<Framework::Timer*>::iterator it = std::find(mTimers.begin(), mTimers.end(), timer);
  if (it != mTimers.end())
  {
    timer->mRemoved = true;
  }
}

void TimerFactory::RunTimers()
{
  struct timespec now;
  int err = clock_gettime(CLOCK_MONOTONIC, &now);
  assert(err == 0);
  unsigned long time = (now.tv_sec * 1000) + (now.tv_nsec /  1000000);

  // Dali timer expects milliseconds.
  float elapsedTime = (time - mCurrentTime) * 0.001f;
  mCurrentTime = time;

  std::list<Framework::Timer*>::iterator it = mTimers.begin();
  while(it != mTimers.end())
  {
    Framework::Timer* timer = *it;
    if ( timer->mRemoved )
    {
      ++it;
    }
    else if (timer->Call(elapsedTime))
    {
      it = mTimers.erase(it);
    }
    else
    {
      ++it;
    }
  }
}


void TimerFactory::Reset()
{
  if ( 0 != mTimers.size() )
  {
    std::list<Framework::Timer*>::iterator it = mTimers.begin();
    while (it != mTimers.end())
    {
      Framework::Timer* timer = *it;
      delete timer;
      ++it;
    }
  }
  mTimers.clear();
}

} // namespace Integration
} // namespace Dali
