#ifndef __DALI_INTERNAL_EGL_IMAGE_EXTENSIONS_H__
#define __DALI_INTERNAL_EGL_IMAGE_EXTENSIONS_H__

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
};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_EGL_IMAGE_EXTENSIONS_H__
