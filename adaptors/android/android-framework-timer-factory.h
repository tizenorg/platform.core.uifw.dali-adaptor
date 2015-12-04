#ifndef ANDROID_FRAMEWORK_TIMER_FACTORY_H
#define ANDROID_FRAMEWORK_TIMER_FACTORY_H

#include <list>
#include <time.h>
#include <assert.h>

#include <adaptors/integration-api/framework.h>

namespace Dali
{
namespace Integration
{

/**
 *
 */
class TimerFactory
{
public:

  TimerFactory();
  ~TimerFactory();

  /**
   * Create a timer
   * @param[in] periodTime The timer period in milliseconds
   * @param[in] func The callback function to use when the timer expires
   * @param[in] context Private data
   * @return The newly created timer
   */
  Framework::Timer* Create(float periodTime, bool (*func)(void*), void* context );

  /**
   * Destroy a timer
   * @param[in] timer The timer to destroy. It will get removed on the next
   * mainloop execution.
   */
  void Destroy(Framework::Timer* timer);

  /**
   * Run the timer loop
   */
  void RunTimers();

  /**
   * Reset the factory - clear down all the timers
   */
  void Reset();

private:
  typedef std::list<Framework::Timer*> FrameworkTimers;
  FrameworkTimers mTimers;
  unsigned long mCurrentTime;
};

} // namespace Integration

} // namespace Dali


#endif // ANDROID_FRAMEWORK_TIMER_FACTORY_H
