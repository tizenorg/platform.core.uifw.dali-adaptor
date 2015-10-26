#ifndef __DALI_INTERNAL_THREAD_CONTROLLER_H__
#define __DALI_INTERNAL_THREAD_CONTROLLER_H__

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

namespace Dali
{

class RenderSurface;

namespace Internal
{

namespace Adaptor
{

class AdaptorInternalServices;
class EnvironmentOptions;
class ThreadControllerInterface;

/**
 * Class to control all the threads.
 */
class ThreadController
{
public:

  /**
   * Constructor
   */
  ThreadController( AdaptorInternalServices& adaptorInterfaces, const EnvironmentOptions& environmentOptions );

  /**
   * Non virtual destructor. Not intended as base class.
   */
  ~ThreadController();

  /**
   * Initializes the thread controller
   */
  void Initialize();

  /**
   * @copydoc Dali::Adaptor::Start()
   */
  void Start();

  /**
   * @copydoc Dali::Adaptor::Pause()
   */
  void Pause();

  /**
   * @copydoc Dali::Adaptor::Resume()
   */
  void Resume();

  /**
   * @copydoc Dali::Adaptor::Stop()
   */
  void Stop();

  /**
   * Called by the adaptor when core requires another update
   */
  void RequestUpdate();

  /**
   * Called by the adaptor when core requires one update
   * If Adaptor is paused, we do one update and return to pause
   */
  void RequestUpdateOnce();

  /**
   * Replaces the surface.
   * @param surface new surface
   */
  void ReplaceSurface( RenderSurface* surface );

  /**
   * @copydoc Dali::Adaptor::SetRenderRefreshRate()
   */
  void SetRenderRefreshRate( unsigned int numberOfVSyncsPerRender );

private:

  // Undefined copy constructor.
  ThreadController( const ThreadController& );

  // Undefined assignment operator.
  ThreadController& operator=( const ThreadController& );

private:

  ThreadControllerInterface* mThreadControllerInterface;
};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_THREAD_CONTROLLER_H__
