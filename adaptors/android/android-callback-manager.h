#ifndef __DALI_ANDROID_CALLBACK_MANAGER_H__
#define __DALI_ANDROID_CALLBACK_MANAGER_H__

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
#include <list>

// INTERNAL INCLUDES
#include <callback-manager.h>
#include <adaptors/integration-api/framework.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

struct CallbackData;

/**
 * Interface to install call backs in the application's main loop. Registers a single callback onto the main loop, and multiplexes to interested parties.
 */
class AndroidCallbackManager : public CallbackManager, public Integration::Framework::Idler
{

public:
  /**
   * constructor
   */
  AndroidCallbackManager();

  /**
   * destructor
   */
  ~AndroidCallbackManager();

  /**
   * @copydoc CallbackManager::SetFramework()
   */
  virtual void SetFramework( Integration::Framework* framework );

  /**
   * @copydoc CallbackManager::AddCallback()
   */
  virtual bool AddIdleCallback( CallbackBase* callback );

  /**
   * @copydoc CallbackManager::Start()
   */
  virtual void Start();

  /**
   * @copydoc CallbackManager::Stop()
   */
  virtual void Stop();

public: // From Framework::Idler
  /**
   * @copydoc Framework::Idler::Run()
   */
  virtual void Run();

private:

  /**
   * @brief Remove all idle call backs that are pending
   * Called by Stop()
   * Always called from the main thread
   */
  void RemoveAllCallbacks();

  /**
   * Remove a standard call back from main loop
   * Always called from main thread
   * @param callbackData callback data
   */
  void RemoveStandardCallback(CallbackData *callbackData);

private:
  typedef std::list<CallbackBase *>  CallbackList;

  Integration::Framework* mFramework; ///< The platform framework

  CallbackList mCallbackContainer; ///< container of live idle callbacks
  bool         mRunning;           ///< flag is set to true if when running
};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_ANDROID_CALLBACK_MANAGER_H__
