#ifndef __DALI_SDL_APPLICATION_H__
#define __DALI_SDL_APPLICATION_H__

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

// INTERNAL INCLUDES
#include "platform-abstractions/emscripten/sdl-platform-abstraction.h"
#include "sdl-gesture-manager.h"
#include "adaptors/common/gl/gl-implementation.h"
#include "sdl-gl-sync-abstraction.h"
#include "sdl-render-controller.h"
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/signals/connection-tracker.h>
#include <dali/integration-api/core.h>
#include <gl/egl-implementation.h>

namespace Dali
{

class DALI_IMPORT_API SdlApplication : public ConnectionTracker
{
public:

  // Default values derived from H2 device.
  static const unsigned int DEFAULT_SURFACE_WIDTH;
  static const unsigned int DEFAULT_SURFACE_HEIGHT;

#ifdef _CPP11
  static constexpr float DEFAULT_HORIZONTAL_DPI = 220.0f;
  static constexpr float DEFAULT_VERTICAL_DPI   = 217.0f;
#else
  static const float DEFAULT_HORIZONTAL_DPI = 220.0f;
  static const float DEFAULT_VERTICAL_DPI   = 217.0f;
#endif

  static const unsigned int DEFAULT_RENDER_INTERVAL = 1;

  SdlApplication( size_t surfaceWidth  = DEFAULT_SURFACE_WIDTH,
                  size_t surfaceHeight = DEFAULT_SURFACE_HEIGHT,
                  float  horizontalDpi = DEFAULT_HORIZONTAL_DPI,
                  float  verticalDpi   = DEFAULT_VERTICAL_DPI );

  virtual ~SdlApplication();

  // update
  bool DoUpdate( void );

  // render
  void DoRender();

  // loop DoRender
  void MainLoop();

  void SendTouchEvent(double x, double y, int mouseIsDown);

  /* static void LogMessage(Dali::Integration::Log::DebugPriority level, std::string& message); */

  /* Dali::Integration::Core& GetCore(); */

  /* TestPlatformAbstraction& GetPlatform(); */

  /* TestRenderController& GetRenderController(); */

  /* TestGlAbstraction& GetGlAbstraction(); */

  /* TestGestureManager& GetGestureManager(); */

  /* void SendNotification(); */

  void SetSurfaceWidth( unsigned int width, unsigned height );

  /* bool Render( unsigned int intervalMilliseconds = DEFAULT_RENDER_INTERVAL ); */

  /* bool UpdateOnly( unsigned int intervalMilliseconds = DEFAULT_RENDER_INTERVAL ); */

  /* bool RenderOnly(); */

  /* void ResetContext(); */

protected:
  SdlPlatformAbstraction   mPlatformAbstraction;
  SdlGlSyncAbstraction     mGlSyncAbstraction;
  SdlRenderController      mRenderController;
  Internal::Adaptor::GlImplementation mGlAbstraction;
  SdlGestureManager        mGestureManager;

  Integration::UpdateStatus mStatus;
  Integration::RenderStatus mRenderStatus;

  Integration::Core* mCore;
  Internal::Adaptor::EglImplementation mEglImplementation;

  unsigned int mSurfaceWidth;
  unsigned int mSurfaceHeight;
  unsigned int mFrame;

  unsigned int mSeconds;
  unsigned int mMicroSeconds;

  void SdlCreateWindow(size_t surfaceWidth,
                       size_t surfaceHeight,
                       const std::string &title);

};

} // Dali

#endif // header
