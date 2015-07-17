#ifndef __DALI_INTERNAL_CONDITIONAL_WAIT_H__
#define __DALI_INTERNAL_CONDITIONAL_WAIT_H__

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
 *
 */

// EXTERNAL INCLUDES
#include <dali/devel-api/common/mutex.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

/**
 * Helper class to allow conditional waiting and notifications between multiple threads.
 */
class ConditionalWait
{
public:

  /**
   * @brief Allows client code to synchronize updates to its own state with the
   * internal state of a ConditionalWait object.
   */
  class AtomicScope
  {
  public:
    /**
     * Constructor
     * @brief Will acquire the internal mutex of the ConditionalWait object passed in.
     * @param[in] wait The ConditionalWait object to lock.
     */
    AtomicScope( ConditionalWait& wait );

    /**
     * Destructor
     * @brief Will release the internal mutex of the ConditionalWait object passed in.
     */
    ~AtomicScope();

    /**
     * Getter for the ConditionalWait locked for this instance's lifetime.
     * @return the ConditionalWait object currently locked.
     */
    ConditionalWait& GetLockedWait() const { return mWait; }

  private:

    // Not implemented as AtomicScope should not be copied:
    AtomicScope( const AtomicScope& );
    const AtomicScope& operator=( const AtomicScope& );

    ConditionalWait& mWait;
  };

  /**
   * @brief Constructor, creates the internal synchronization objects
   */
  ConditionalWait();

  /**
   * @brief Destructor, non-virtual as this is not a base class
   */
  ~ConditionalWait();

  /**
   * @brief Notifies another thread to continue if it is blocked on a wait.
   *
   * Can be called from any thread.
   * Does not block the current thread but may cause a rescheduling of threads.
   */
  void Notify();

  /**
   * @brief Wait for another thread to notify us when the condition is true and we can continue
   *
   * Will always block current thread until Notify is called
   */
  void Wait();

  /**
   * @brief Wait for another thread to notify us when the condition is true and we can continue
   *
   * Will always block current thread until Notify is called.
   * Assumes that the AtomicScope object passed in has already locked the internal state of
   * this object. Releases the lock while waiting and re-acquires it when returning
   * from the wait.
   * param[in] scope A preexisting lock on the internal state of this object.
   * @pre scope must have been passed this ConditionalWait during its construction.
   */
  void Wait( const AtomicScope& scope );

  /**
   * @brief Return the count of threads waiting for this conditional
   * @return count of waits
   */
  unsigned int GetWaitCount() const;

private:

  // Not implemented as ConditionalWait is not copyable
  ConditionalWait( const ConditionalWait& );
  const ConditionalWait& operator=( const ConditionalWait& );

  struct ConditionalWaitImpl;
  ConditionalWaitImpl* mImpl;

};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_CONDITIONAL_WAIT_H__
