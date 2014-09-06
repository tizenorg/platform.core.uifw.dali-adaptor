#ifndef __DALI_INTERNAL_UPDATE_RENDER_CONTROLLER_H__
#define __DALI_INTERNAL_UPDATE_RENDER_CONTROLLER_H__

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
#include <dali/public-api/math/rect.h>
#include <dali/integration-api/core.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/gl-abstraction.h>
#include <dali/integration-api/platform-abstraction.h>

// INTERNAL INCLUDES
#include "internal/common/damage-observer.h"
#include <base/interfaces/adaptor-internal-services.h>

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
class UpdateRenderSynchronization;
class RenderSurface;
class AdaptorInternalServices;
class LogOptions;

/**
 * Class to control the update and render threads.
 */
class UpdateRenderController
{
public:

  /**
   * Constructor
   */
  UpdateRenderController( AdaptorInternalServices& adaptorInterfaces, const LogOptions& logOptions );

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
   * Terminates the render thread synchronously
   */
  void TerminateRenderThread();

  /**
   * Start the render thread
   */
  void StartRenderThread();

  /**
   * @copydoc Dali::Adaptor::RenderSync()
   */
  void RenderSync();

  /**
   * @copydoc Dali::Internal::Adaptor::RenderSync()
   */
  void DisableVSync();

private:

  // Undefined copy constructor.
  UpdateRenderController( const UpdateRenderController& );

  // Undefined assignment operator.
  UpdateRenderController& operator=( const UpdateRenderController& );

  UpdateThread*                mUpdateThread;     ///< The update-thread owned by UpdateRenderController
  RenderThread*                mRenderThread;     ///< The render-thread owned by UpdateRenderController
  VSyncNotifier*               mVSyncNotifier;    ///< The vsync-thread owned by UpdateRenderController
  UpdateRenderSynchronization* mUpdateRenderSync; ///< Used to synchronize the update & render threads; also owned by UpdateRenderController
};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_UPDATE_RENDER_CONTROLLER_H__
