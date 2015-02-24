#ifndef __DALI_INTERNAL_UPDATE_RENDER_CONTROLLER_H__
#define __DALI_INTERNAL_UPDATE_RENDER_CONTROLLER_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

class DisplayConnection;

namespace Internal
{

namespace Adaptor
{

class UpdateThread;
class RenderThread;
class VSyncNotifier;
class UpdateRenderSynchronization;
class RenderSurface;
class AdaptorInternalServices;
class EnvironmentOptions;

/**
 * Class to control the update and render threads.
 */
class UpdateRenderController
{
public:

  /**
   * Constructor
   */
  UpdateRenderController( AdaptorInternalServices& adaptorInterfaces, Dali::DisplayConnection displayConnection, const EnvironmentOptions& environmentOptions );

  /**
   * Non virtual destructor. Not intended as base class.
   */
  ~UpdateRenderController();

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
   * Ensure the frame time values are reset before the next call to Core::Update()
   * following a Resume application state change.
   */
  void ResumeFrameTime();

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
   * Provides a new surface. Should be used if the old surface has been lost
   * for any reason.
   * @param surface new surface
   */
  void NewSurface( RenderSurface* surface );

  /**
   * @copydoc Dali::Adaptor::SetRenderRefreshRate()
   */
  void SetRenderRefreshRate( unsigned int numberOfVSyncsPerRender );

private:

  // Undefined copy constructor.
  UpdateRenderController( const UpdateRenderController& );

  // Undefined assignment operator.
  UpdateRenderController& operator=( const UpdateRenderController& );

  AdaptorInternalServices&     mAdaptorInterfaces;

  UpdateThread*                mUpdateThread;     ///< The update-thread owned by UpdateRenderController
  RenderThread*                mRenderThread;     ///< The render-thread owned by UpdateRenderController
  VSyncNotifier*               mVSyncNotifier;    ///< The vsync-thread owned by UpdateRenderController
  UpdateRenderSynchronization* mUpdateRenderSync; ///< Used to synchronize the update & render threads; also owned by UpdateRenderController
  unsigned int                 mNumberOfVSyncsPerRender; ///< Frame skipping count
};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_UPDATE_RENDER_CONTROLLER_H__
