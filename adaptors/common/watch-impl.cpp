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
#include "watch-impl.h"

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

WatchPtr Watch::New(
  int* argc,
  char **argv[],
  const std::string& stylesheet,
  Dali::Watch::WINDOW_MODE windowMode)
{
  WatchPtr watch ( new Watch (argc, argv, stylesheet, windowMode ) );
  return watch;
}

Watch::Watch( int* argc, char** argv[], const std::string& stylesheet, Dali::Watch::WINDOW_MODE windowMode )
: Application(argc, argv, stylesheet, windowMode, Framework::WATCH)
{
}

Watch::~Watch()
{
}

void Watch::OnTimeTick(watch_time_h *time)
{
  Dali::Watch watch(this);
  mTickSignal.Emit( watch, time );
}

void Watch::OnAmbientTick(watch_time_h *time)
{
  Dali::Watch watch(this);
  mAmbientTickSignal.Emit( watch, time );
}

void Watch::OnAmbientChanged(bool ambient)
{
  Dali::Watch watch(this);
  mAmbientChangeSignal.Emit( watch, ambient );
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
