#ifndef __DALI_INTERNAL_BASE_ADAPTOR_INTERFACE_H__
#define __DALI_INTERNAL_BASE_ADAPTOR_INTERFACE_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// EXTERNAL INCLUDES

#include <boost/function.hpp>
#include <dali/public-api/common/dali-vector.h>

#include <dali/integration-api/render-controller.h>
#include <dali/public-api/adaptor-framework/common/adaptor.h>
#include <dali/public-api/adaptor-framework/common/sound-player.h>
#include <dali/public-api/adaptor-framework/common/tts-player.h>
#include <dali/public-api/adaptor-framework/common/clipboard-event-notifier.h>

// INTERNAL INCLUDES
#include <slp-platform-abstraction.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{
class EventHandler;
class EglImplementation;
class GestureManager;
class GlImplementation;
class RenderSurface;
class UpdateRenderController;
class TriggerEvent;
class CallbackManager;
class FeedbackPluginProxy;

class Adaptor;


/**
 * Base Adaptor interface.
 */
class AdaptorInterface : public Integration::RenderController
{
public:
  enum State
  {
    Ready,     ///< Adaptor is ready to be run
    Running,   ///< Adaptor is running
    Suspended, ///< Adaptor has been suspended (usually by the platform)
    Stopped,   ///< Adaptor has been stopped (by application or platform)
  };

  /**
   * Observes adaptor lifetime events
   */
  class Observer
  {
  public:
    /**
     * Called when the adaptor has started.
     */
    virtual void OnStart() = 0;

    /**
     * Called when the adaptor is about to pause.
     */
    virtual void OnPause() = 0;

    /**
     * Called when the adaptor is about to resume.
     */
    virtual void OnResume() = 0;

    /**
     * Called when the adaptor is about to stop.
     */
    virtual void OnStop() = 0;

    /**
     * Called when the adaptor is about to be destroyed.
     */
    virtual void OnDestroy() = 0;
  };

  typedef std::vector<Observer*> ObserverContainer;

public:

  /**
   * @copydoc Dali::Adaptor::Start()
   */
  virtual void Start() = 0;

  /**
   * @copydoc Dali::Adaptor::Pause()
   */
  virtual void Pause() = 0;

  /**
   * @copydoc Dali::Adaptor::Resume()
   */
  virtual void Resume() = 0;

  /**
   * @copydoc Dali::Adaptor::Stop()
   */
  virtual void Stop() = 0;

  /**
   * @copydoc Dali::Adaptor::FeedTouchEvent()
   */
  virtual void FeedTouchPoint( TouchPoint& point, int timeStamp ) = 0;

  /**
   * Move and resize the render surface.
   * This function only works if RenderSurface represents a Window.
   * If the render surface is a pixmap then ReplaceSurface() should be used.
   *
   * @param[in] positionSize position & size information of render target
   * @return true if moved or resized successfully, false otherwise
   */
  virtual bool MoveResize( const PositionSize& positionSize ) = 0;

  /**
   * Let the adaptor know the render surface has been resized externally.
   * Used in situations where the application creates a window, and resizes
   * that window itself (without calling MoveResize())
   *
   * @param[in] positionSize position & size information of the RenderSurface
   */
  virtual void SurfaceResized( const PositionSize& positionSize ) = 0;

  /**
   * Replaces the existing render surface with a new one.
   * Typically used for resizing pixmaps. A pixmap can't be resized,
   * so a new one should be created as a RenderSurface
   * then adaptor->ReplaceSurface(newSurface) can be called.
   * The surface replace is asynchronous, but this method is synchronous.
   * It is applications responsibility to delete the old surface after this method returns.
   * @param surface the new render surface
   */
  virtual void ReplaceSurface( Dali::RenderSurface& surface ) = 0;

  /**
   * Make rendering synchronization beween offscreen and onscreen.
   * When application uses pixmap render surface (i.e. offscreen), it will post
   * the pixmap to a part of window (i.e. onscreen). In this case dali can
   * render next frame after current frame was posted to window.
   * The application should let adaptor know the posting by this method.
   */
  virtual void RenderSync() = 0;

  /**
   * @copydoc Dali::Adaptor::GetSurface()
   */
  virtual Dali::RenderSurface& GetSurface() const = 0;

  /**
   * @copydoc Dali::Adaptor::GetTtsPlayer()
   */
  virtual Dali::TtsPlayer GetTtsPlayer(Dali::TtsPlayer::Mode mode) = 0;

  /**
   * @copydoc Dali::Adaptor::AddIdle()
   */
  virtual bool AddIdle( boost::function<void(void)> callBack ) = 0;

  /**
   * @copydoc Internal::Framework::CallFromMainLoop()
   */
  virtual bool CallFromMainLoop(boost::function<void(void)> callBack) = 0;

  /**
   * @copydoc Dali::Adaptor::RegisterSingleton()
   */
  virtual void RegisterSingleton(const std::type_info& info, BaseHandle singleton) = 0;

  /**
   * @copydoc Dali::Adaptor::GetSingleton()
   */
  virtual BaseHandle GetSingleton(const std::type_info& info) const = 0;

protected:
  /**
   * Protected virtual destructor
   */
  virtual ~AdaptorInterface() {}
};


} // namespace Internal

} // namespace Adaptor

} // namespace Dali

#endif // __DALI_INTERNAL_BASE_ADAPTOR_INTERFACE_H__
