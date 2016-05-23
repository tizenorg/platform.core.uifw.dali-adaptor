/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <watch-time.h>

// EXTERNAL INCLUDES
#ifdef APPCORE_WATCH_AVAILABLE
#include <appcore-watch/watch_app.h>
#endif

namespace Dali
{

WatchTime::WatchTime()
  :mTimeHandle(NULL)
{
}

WatchTime::WatchTime(void *time_handle)
{
  mTimeHandle = time_handle;
}

#ifdef APPCORE_WATCH_AVAILABLE

int WatchTime::GetHour() const
{
  int hour;

  watch_time_get_hour(reinterpret_cast<watch_time_h>(mTimeHandle), &hour);
  return hour;
}

int WatchTime::GetHour24() const
{
  int hour24;

  watch_time_get_hour24(reinterpret_cast<watch_time_h>(mTimeHandle), &hour24);
  return hour24;
}

int WatchTime::GetMinute() const
{
  int minute;

  watch_time_get_minute(reinterpret_cast<watch_time_h>(mTimeHandle), &minute);
  return minute;
}

int WatchTime::GetSecond() const
{
  int second;

  watch_time_get_second(reinterpret_cast<watch_time_h>(mTimeHandle), &second);
  return second;
}

#else

int WatchTime::GetHour() const
{
  return 0;
}

int WatchTime::GetHour24() const
{
  return 0;
}

int WatchTime::GetMinute() const
{
  return 0;
}

int WatchTime::GetSecond() const
{
  return 0;
}

#endif

} // namespace Dali
