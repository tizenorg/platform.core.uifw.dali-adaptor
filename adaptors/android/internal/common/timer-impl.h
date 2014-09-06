#ifndef __DALI_INTERNAL_TIMER_H__
#define __DALI_INTERNAL_TIMER_H__

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

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/adaptor-framework/common/timer.h>

// INTERNAL INCLUDES
#include <base/interfaces/timer-interface.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{
class Timer;

typedef IntrusivePtr<Timer> TimerPtr;

/**
 * Implementation of the timer
 */
class Timer : public BaseObject, public TimerInterface
{
public:
  static TimerPtr New( unsigned int milliSec );

  /**
   * Constructor
   * @param[in]  milliSec  Interval in milliseconds.
   */
  Timer( unsigned int milliSec );

  /**
   * Destructor.
   */
  virtual ~Timer();

public:

  /**
   * @copydoc Dali::Timer::Start()
   */
  virtual void Start();

  /**
   * @copydoc Dali::Timer::Stop()
   */
  virtual void Stop();

  /**
   * @copydoc Dali::Timer::SetInterval()
   */
  virtual void SetInterval( unsigned int interval );

  /**
   * @copydoc Dali::Timer::GetInterval()
   */
  virtual unsigned int GetInterval() const;

  /**
   * @copydoc Dali::Timer::IsRunning()
   */
  virtual bool IsRunning() const;

  /**
   * Tick
   */
  bool Tick();

public: // Signals

  Dali::Timer::TimerSignalV2& TickSignal();

private: // Implementation

  // not implemented
  Timer( const Timer& );
  Timer& operator=( const Timer& );

private: // Data

  Dali::Timer::TimerSignalV2 mTickSignal;

  // To hide away implementation details
  struct Impl;
  Impl* mImpl;
};

inline Timer& GetImplementation(Dali::Timer& timer)
{
  DALI_ASSERT_ALWAYS(timer && "Timer handle is empty");

  BaseObject& handle = timer.GetBaseObject();

  return static_cast<Internal::Adaptor::Timer&>(handle);
}

inline const Timer& GetImplementation(const Dali::Timer& timer)
{
  DALI_ASSERT_ALWAYS(timer && "Timer handle is empty");

  const BaseObject& handle = timer.GetBaseObject();

  return static_cast<const Internal::Adaptor::Timer&>(handle);
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_TIMER_H__
