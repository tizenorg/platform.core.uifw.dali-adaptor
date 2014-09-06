#ifndef __DALI_INTERNAL_WINDOW_H__
#define __DALI_INTERNAL_WINDOW_H__

/*
Copyright (c) 2000-2012 Samsung Electronics Co., Ltd All Rights Reserved

This file is part of Dali

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
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/adaptor-framework/common/window.h>
#include <dali/public-api/adaptor-framework/common/render-surface.h>

// INTERNAL INCLUDES
#include <dali/public-api/adaptor-framework/common/window.h>
#include <dali/public-api/adaptor-framework/common/orientation.h>
#include <dali/public-api/adaptor-framework/common/render-surface.h>
#include <dali/public-api/adaptor-framework/common/drag-and-drop-detector.h>
#include <internal/common/adaptor-impl.h>
#include <base/lifecycle-observer.h>

namespace Dali
{
class Orientation;
class Adaptor;

namespace Integration
{
class SystemOverlay;
}

namespace Internal
{
namespace Adaptor
{
class RenderSurface;
class Orientation;

class Window;
typedef Dali::IntrusivePtr<Window> WindowPtr;
typedef Dali::IntrusivePtr<Orientation> OrientationPtr;

/**
 * Window provides a surface to render onto with orientation & indicator properties.
 */
class Window : public Dali::BaseObject, public LifeCycleObserver
{
public:
  /**
   * Create a new Window. This should only be called once by the Application class
   * @param[in] windowPosition The position and size of the window
   * @param[in] name The window title
   * @param[in] isTransparent Whether window is transparent
   * @return A newly allocated Window
   */
  static Window* New(const PositionSize& posSize, const std::string& name, bool isTransparent = false);

  /**
   * Pass the adaptor back to the overlay. This allows the window to access Core's overlay.
   * @param[in] adaptor An initialized adaptor
   */
  void SetAdaptor(Dali::Adaptor& adaptor);

  /**
   * Get the window surface
   * @return The render surface
   */
  RenderSurface* GetSurface();

  /**
   * @copydoc Dali::Window::ShowIndicator()
   */
  void ShowIndicator( bool show );

  /**
   * @copydoc Dali::Window::SetIndicatorBgOpacity()
   */
  void SetIndicatorBgOpacity( Dali::Window::IndicatorBgOpacity opacity );

  /**
   * @copydoc Dali::Window::RotateIndicator()
   */
  void RotateIndicator( Dali::Window::WindowOrientation orientation );

  /**
   * @copydoc Dali::Window::SetClass()
   */
  void SetClass( std::string name, std::string klass );

  /**
   * @copydoc Dali::Window::Raise()
   */
  void Raise();

  /**
   * @copydoc Dali::Window::Lower()
   */
  void Lower();

  /**
   * @copydoc Dali::Window::GetOrientation()
   */
  OrientationPtr GetOrientation();

  /**
   * @copydoc Dali::Window::AddAvailableOrientation()
   */
  void AddAvailableOrientation(Dali::Window::WindowOrientation orientation);

  /**
   * @copydoc Dali::Window::RemoveAvailableOrientation()
   */
  void RemoveAvailableOrientation(Dali::Window::WindowOrientation orientation);

  /**
   * @copydoc Dali::Window::SetAvailableOrientations()
   */
  void SetAvailableOrientations(const std::vector<Dali::Window::WindowOrientation>& orientations);

  /**
   * @copydoc Dali::Window::GetAvailableOrientations()
   */
  const std::vector<Dali::Window::WindowOrientation>& GetAvailableOrientations();

  /**
   * @copydoc Dali::Window::SetPreferredOrientation()
   */
  void SetPreferredOrientation(Dali::Window::WindowOrientation orientation);

  /**
   * @copydoc Dali::Window::GetPreferredOrientation()
   */
  Dali::Window::WindowOrientation GetPreferredOrientation();

  /**
   * @copydoc Dali::Window::GetDragAndDropDetector() const
   */
  Dali::DragAndDropDetector GetDragAndDropDetector() const;

  /**
   * Called from Orientation after the Change signal has been sent
   */
  void RotationDone( int orientation, int width, int height );


private:
  /**
   * Private constructor.
   * @sa Window::New()
   */
  Window();

  /**
   * Destructor
   */
  virtual ~Window();

  /**
   * Second stage initialization
   */
  void Initialize(const PositionSize& posSize, const std::string& name);

  /**
   * Shows / hides the indicator bar.
   * Handles close/open if rotation changes whilst hidden
   */
  void DoShowIndicator( bool show, Dali::Window::WindowOrientation lastOrientation );

  /**
   * Close current indicator and open a connection onto the new indicator service.
   * Effect may not be synchronous if waiting for an indicator update on existing connection.
   */
  void DoRotateIndicator( Dali::Window::WindowOrientation orientation );

  /**
   * Change the indicator actor's rotation to match the current orientation
   */
  void SetIndicatorActorRotation();

  /**
   * Set the indicator properties on the window
   */
  void SetIndicatorProperties( bool isShown, Dali::Window::WindowOrientation lastOrientation );

private: // Adaptor::Observer interface

  /**
   * @copydoc Dali::Internal::Adaptor::Adaptor::Observer::OnStart()
   */
  virtual void OnStart();

  /**
   * @copydoc Dali::Internal::Adaptor::Adaptor::Observer::OnPause()
   */
  virtual void OnPause();

  /**
   * @copydoc Dali::Internal::Adaptor::Adaptor::Observer::OnResume()
   */
  virtual void OnResume();

  /**
   * @copydoc Dali::Internal::Adaptor::Adaptor::Observer::OnStop()
   */
  virtual void OnStop();

  /**
   * @copydoc Dali::Internal::Adaptor::Adaptor::Observer::OnDestroy()
   */
  virtual void OnDestroy();

private:

  RenderSurface*                   mSurface;
  bool                             mShowIndicator:1;    ///< public state
  bool                             mIndicatorIsShown:1; ///< private state
  bool                             mShowRotatedIndicatorOnClose:1;
  bool                             mStarted:1;
  bool                             mIsTransparent:1;
  bool                             mWMRotationAppSet:1;
  Dali::Window::WindowOrientation  mIndicatorOrientation;
  Dali::Window::WindowOrientation  mNextIndicatorOrientation;
  Dali::Window::IndicatorBgOpacity mIndicatorOpacityMode;
  Integration::SystemOverlay*      mOverlay;
  Adaptor*                         mAdaptor;
  Dali::DragAndDropDetector        mDragAndDropDetector;

  // struct EventHandler;
  // EventHandler*                    mEventHandler;

  OrientationPtr                               mOrientation;
  std::vector<Dali::Window::WindowOrientation> mAvailableOrientations;
  Dali::Window::WindowOrientation              mPreferredOrientation;
};

} // namespace Adaptor
} // namepsace Internal

// Helpers for public-api forwarding methods

inline Internal::Adaptor::Window& GetImplementation(Dali::Window& window)
{
  DALI_ASSERT_ALWAYS( window && "Window handle is empty" );
  BaseObject& object = window.GetBaseObject();
  return static_cast<Internal::Adaptor::Window&>(object);
}

inline const Internal::Adaptor::Window& GetImplementation(const Dali::Window& window)
{
  DALI_ASSERT_ALWAYS( window && "Window handle is empty" );
  const BaseObject& object = window.GetBaseObject();
  return static_cast<const Internal::Adaptor::Window&>(object);
}

} // namespace Dali


#endif // __DALI_INTERNAL_WINDOW_H__
