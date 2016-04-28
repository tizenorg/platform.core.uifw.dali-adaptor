#ifndef __DALI_INTERNAL_WATCH_H__
#define __DALI_INTERNAL_WATCH_H__

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

// INTERNAL INCLUDES
#include <watch.h>
#include <application-impl.h>

namespace Dali
{
class Adaptor;
//class Window;

namespace Internal
{

namespace Adaptor
{

class Watch;
typedef IntrusivePtr<Watch> WatchPtr;

/**
 * Implementation of the Watch class.
 */
class Watch : public Application
//, public Framework::Observer
{
public:

  typedef Dali::Watch::WatchTimeSignal WatchTimeSignal;
  typedef Dali::Watch::WatchBoolSignal WatchBoolSignal;
  /**
   * Create a new watch
   * @param[in]  argc        A pointer to the number of arguments
   * @param[in]  argv        A pointer to the argument list
   * @param[in]  stylesheet  The path to user defined theme file
   * @param[in]  windowMode  A member of Dali::Watch::WINDOW_MODE
   */
  static WatchPtr New( int* argc, char **argv[], const std::string& stylesheet, WINDOW_MODE windowMode );

public:
  /**
   * Called every second
   */
  virtual void OnTimeTick(watch_time_h *time);

  /**
   * Called every second in ambient mode
   */
  virtual void OnAmbientTick(watch_time_h *time);

  /**
   * Called when the device enters or exits ambient mode
   */
  virtual void OnAmbientChanged(bool ambient);

public:

  // Signals
  WatchTimeSignal                        mTickSignal;
  WatchTimeSignal                        mAmbientTickSignal;
  WatchBoolSignal                      mAmbientChangeSignal;

  /**
   * Private Constructor
   * @param[in]  argc        A pointer to the number of arguments
   * @param[in]  argv        A pointer to the argument list
   * @param[in]  stylesheet  The path to user defined theme file
   * @param[in]  windowMode  A member of Dali::Watch::WINDOW_MODE
   */
  Watch( int* argc, char **argv[], const std::string& stylesheet, WINDOW_MODE windowMode );

  /**
   * Destructor
   */
  virtual ~Watch();

  // Undefined
  Watch(const Watch&);
  Watch& operator=(Watch&);

};

inline Watch& GetImplementation(Dali::Watch& watch)
{
  DALI_ASSERT_ALWAYS(watch && "watch handle is empty");

  BaseObject& handle = watch.GetBaseObject();

  return static_cast<Internal::Adaptor::Watch&>(handle);
}

inline const Watch& GetImplementation(const Dali::Watch& watch)
{
  DALI_ASSERT_ALWAYS(watch && "Timre handle is empty");

  const BaseObject& handle = watch.GetBaseObject();

  return static_cast<const Internal::Adaptor::Watch&>(handle);
}


} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_WATCH_H__
