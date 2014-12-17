#ifndef __DALI_INTERNAL_EGL_IMAGE_EXTENSIONS_H__
#define __DALI_INTERNAL_EGL_IMAGE_EXTENSIONS_H__

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

// EXTERNAL INCLUDES
#include <EGL/egl.h>

#if _TIZEN_BUFFER_MANAGER_SUPPORT_

#include <tbm_surface.h>

// TBM surface support
#ifndef EGL_NATIVE_SURFACE_TIZEN
# define EGL_NATIVE_SURFACE_TIZEN 0x32A1
#endif

#else

typedef int* tbm_surface_h;

#endif // if _TIZEN_SDK_2_3_

#include <dali/public-api/images/pixel.h>

namespace Dali
{
namespace Internal
{
namespace Adaptor
{
class EglImplementation;

/**
 * EglImageExtensions class provides EGL image extension support
 */
class EglImageExtensions
{
public:
  /**
   * Constructor
   */
  EglImageExtensions(EglImplementation* impl);

  /**
   * Destructor
   */
  ~EglImageExtensions();


public:   // EGLImageKHR extension support

  /**
   * If the EGL Image extension is available this function returns a
   * EGLImageKHR
   * @param pixmap The pixmap
   * @return an object that holds a EGLImageKHR
   */
  void* CreateImageKHR(EGLClientBuffer pixmap);

  /**
   * If the EGL Image extension is available this function returns a
   * EGLImageKHR
   * @param tbmSurface The TBM (Tizen Buffer Manager) surface
   * @return an object that holds a EGLImageKHR
   */
  void* CreateImageKHR(tbm_surface_h surface);

  /**
   * If the EGL Image extension is available this function
   * destroys the a EGLImageKHR
   * @param eglImageKHR Object that holds a EGLImageKHR
   */
  void DestroyImageKHR(void* eglImageKHR);

  /**
   * defines a 2D texture
   * @param eglImageKHR Object that holds a EGLImageKHR
   */
  void TargetTextureKHR(void* eglImageKHR);

  /**
   * Get the functions for using ImageKHR
   */
  void InitializeEglImageKHR();

private:
  EglImplementation* mEglImplementation;

  bool mImageKHRInitialized;             ///< Flag for whether extended KHR functions loaded
  bool mImageKHRInitializeFailed;        ///< Flag to avoid trying to reload extended KHR functions, if
                                         /// it fails the first time
  bool mInputTbmSurface;                 /// < Flag for whether TBM surface input
};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_EGL_IMAGE_EXTENSIONS_H__
