#ifndef __DALI_INTERNAL_MULTI_THREAD_CONTROLLER_H__
#define __DALI_INTERNAL_MULTI_THREAD_CONTROLLER_H__

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

// INTERNAL INCLUDES
#include <base/thread-controller-interface.h>

namespace Dali
{

class RenderSurface;

namespace Internal
{

namespace Adaptor
{

class UpdateThread;
class RenderThread;
class VSyncNotifier;
class ThreadSynchronization;
class AdaptorInternalServices;
class EnvironmentOptions;

/**
 * Class to control multiple threads:
 *  - Main Event Thread
 *  - VSync Thread
 *  - Update Thread
 *  - Render Thread
 */
class MultiThreadController : public ThreadControllerInterface
{
public:

  /**
   * Constructor
   */
  MultiThreadController( AdaptorInternalServices& adaptorInterfaces, const EnvironmentOptions& environmentOptions );

  /**
   * Non virtual destructor. Not intended as base class.
   */
  ~MultiThreadController();

  /**
   * @copydoc ThreadControllerInterface::Initialize()
   */
  void Initialize();

  /**
   * @copydoc ThreadControllerInterface::Start()
   */
  void Start();

  /**
   * @copydoc ThreadControllerInterface::Pause()
   */
  void Pause();

  /**
   * @copydoc ThreadControllerInterface::Resume()
   */
  void Resume();

  /**
   * @copydoc ThreadControllerInterface::Stop()
   */
  void Stop();

  /**
   * @copydoc ThreadControllerInterface::RequestUpdate()
   */
  void RequestUpdate();

  /**
   * @copydoc ThreadControllerInterface::RequestUpdateOnce()
   */
  void RequestUpdateOnce();

  /**
   * @copydoc ThreadControllerInterface::ReplaceSurface()
   */
  void ReplaceSurface( RenderSurface* surface );

  /**
   * @copydoc ThreadControllerInterface::SetRenderRefreshRate()
   */
  void SetRenderRefreshRate( unsigned int numberOfVSyncsPerRender );

private:

  // Undefined copy constructor.
  MultiThreadController( const MultiThreadController& );

  // Undefined assignment operator.
  MultiThreadController& operator=( const MultiThreadController& );

  AdaptorInternalServices&     mAdaptorInterfaces;

  UpdateThread*                mUpdateThread;     ///< The update-thread owned by MultiThreadController
  RenderThread*                mRenderThread;     ///< The render-thread owned by MultiThreadController
  VSyncNotifier*               mVSyncNotifier;    ///< The vsync-thread owned by MultiThreadController
  ThreadSynchronization*       mThreadSync;       ///< Used to synchronize all the threads; owned by MultiThreadController
  unsigned int                 mNumberOfVSyncsPerRender; ///< Frame skipping count
};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_MULTI_THREAD_CONTROLLER_H__
