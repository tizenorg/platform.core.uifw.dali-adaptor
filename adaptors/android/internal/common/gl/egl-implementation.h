#ifndef __DALI_INTERNAL_EGL_IMPLEMENTATION_H__
#define __DALI_INTERNAL_EGL_IMPLEMENTATION_H__

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
#include <EGL/egl.h>
#include <boost/any.hpp>

// INTERNAL INCLUDES
#include <base/interfaces/egl-interface.h>

namespace Dali
{
namespace Internal
{
namespace Adaptor
{

enum ColorDepth
{
  COLOR_DEPTH_24 = 24,
  COLOR_DEPTH_32 = 32
};

/**
 * EglImplementation class provides an EGL implementation.
 */
class EglImplementation : public EglInterface
{
public:
  /**
   * Constructor
   */
  EglImplementation();

  /**
   * Destructor
   */
  virtual ~EglImplementation();

public:

 /**
   * (Called from  Android::RenderSurface, not RenderThread, so not in i/f, hence, not virtual)
   * Initialize GL
   * @param display The display
   * @param isOwnSurface whether the surface is own or not
   * @return true on success, false on failure
   */
  bool InitializeGles( EGLNativeDisplayType display, bool isOwnSurface = true );

  /**
    * Create the OpenGL context.
    * @return true if successful
    */
  virtual bool CreateContext();

  /**
    * Destroy the OpenGL context.
    */
  void DestroyContext();

  /**
    * Destroy the OpenGL surface.
    */
  void DestroySurface();

  /**
   * Make the OpenGL context current
   */
  virtual void MakeContextCurrent();

  /**
   * clear the OpenGL context
   */
  void MakeContextNull();

  /**
   * Terminate GL
   */
  virtual void TerminateGles();

  /**
   * Checks if GL is initialised
   * @return true if it is
   */
  bool IsGlesInitialized() const;

  /**
   * Sets the refresh sync mode.
   * @see SyncMode
   */
  virtual bool SetRefreshSync( SyncMode mode );

  /**
   * Performs an OpenGL swap buffers command
   */
  virtual void SwapBuffers();

  /**
   * Performs an OpenGL copy buffers command
   */
  virtual void CopyBuffers();

  /**
   * Performs an EGL wait GL command
   */
  virtual void WaitGL();

  /**
   * Choose config of egl
   * @param isWindowType whether the config for window or pixmap
   * @param colorDepth Bit per pixel value (ex. 32 or 24)
  */
  void ChooseConfig( bool isWindowType, ColorDepth depth );

  /**
    * Create an OpenGL surface using a window
    * @param window The window to create the surface on
    * @param colorDepth Bit per pixel value (ex. 32 or 24)
    * @return true on success, false on failure
    */
  void CreateSurfaceWindow( EGLNativeWindowType window, ColorDepth depth );

  /**
   * Create the OpenGL surface using a pixmap
   * @param pixmap The pixmap to create the surface on
   * @param colorDepth Bit per pixel value (ex. 32 or 24)
   * @return true on success, false on failure
   */
  void CreateSurfacePixmap( EGLNativePixmapType pixmap, ColorDepth depth );

  /**
   * Replaces the render surface
   * @param[in] window, the window to create the new surface on
   * @param[in] display, the display
   * @return true if the context was lost due to a change in display
   *         between old surface and new surface
   */
  bool ReplaceSurfaceWindow( EGLNativeWindowType window, EGLNativeDisplayType display );

  /**
   * Replaces the render surface
   * @param[in] pixmap, the pixmap to create the new surface on
   * @param[in] display, the display
   * @return true if the context was lost due to a change in x-display
   *         between old surface and new surface
   */
  bool ReplaceSurfacePixmap( EGLNativePixmapType pixmap, EGLNativeDisplayType display );

  /**
   * returns the display with which this object was initialized
   * @return the EGL Display.
   */
  EGLDisplay GetDisplay() const;

  /**
   * Returns the EGL context
   * @return the EGL context.
   */
  EGLContext GetContext() const;

private:

  EGLint               mContextAttribs[3];

  EGLNativeDisplayType mEglNativeDisplay;
  EGLNativeWindowType  mEglNativeWindow;
  EGLNativePixmapType  mEglNativePixmap;

  EGLDisplay           mEglDisplay;
  EGLConfig            mEglConfig;
  EGLContext           mEglContext;
  EGLSurface           mEglSurface;

  bool                 mGlesInitialized;
  bool                 mIsOwnSurface;
  SyncMode             mSyncMode;
  bool                 mContextCurrent;
  bool                 mIsWindow;
  ColorDepth           mColorDepth;
};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_EGL_IMPLEMENTATION_H__
