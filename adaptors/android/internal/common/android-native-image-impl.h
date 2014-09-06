#ifndef __DALI_INTERNAL_ANDROID_NATIVE_IMAGE_H__
#define __DALI_INTERNAL_ANDROID_NATIVE_IMAGE_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/adaptor-framework/common/android-native-image.h>
#include <dali/public-api/adaptor-framework/common/adaptor.h>
#include <public-api/adaptor-framework/application.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

class GraphicBufferWrapper;

namespace Dali
{

namespace Internal
{
  class EglImageExtensions;

namespace Adaptor
{


/**
 * Dali internal AndroidNativeImage.
 */
class AndroidNativeImage
{
public:

  /**
   * Create a new AndroidNativeImage internally.
   * Depending on hardware the width and height may have to be a power of two.
   * @param[in] width The width of the image.
   * @param[in] height The height of the image.
   * @param[in] depth color depth of the pixmap
   * @param[in] application reference to dali application
   * @param[in] pixmap contains either: pixmap of type X11 Pixmap , a Ecore_X_Pixmap or is empty
   * @return A smart-pointer to a newly allocated image.
   */
  static AndroidNativeImage* New( unsigned int width,
                                  unsigned int height,
                                  Dali::AndroidNativeImage::ColorDepth depth,
                                  Dali::Application applications);

  /**
   * Create a new AndroidNativeImage internally.
   * Depending on hardware the width and height may have to be a power of two.
   * @param[in] width The width of the image.
   * @param[in] height The height of the image.
   * @param[in] depth color depth of the pixmap
   * @param[in] adaptor reference to dali adaptor
   * @param[in] pixmap contains either: pixmap of type X11 Pixmap , a Ecore_X_Pixmap or is empty
   * @return A smart-pointer to a newly allocated image.
   */
  static AndroidNativeImage* New(unsigned int width,
                                 unsigned int height,
                                 Dali::AndroidNativeImage::ColorDepth depth,
                                 Dali::Adaptor& adaptor);

  /**
   * destructor
   */
  ~AndroidNativeImage();

  /**
   * @copydoc Dali::NativeImage::GlExtensionCreate()
   */
  bool GlExtensionCreate();

  /**
   * @copydoc Dali::NativeImage::GlExtensionDestroy()
   */
  void GlExtensionDestroy();

  /**
   * @copydoc Dali::NativeImage::TargetTexture()
   */
  unsigned int TargetTexture();

   /**
   * @copydoc Dali::NativeImage::TextureCreated()
   */
  void TextureCreated(unsigned textureId);

  /**
   * @copydoc Dali::NativeImage::PrepareTexture()
   */
  void PrepareTexture();

  /**
   * @copydoc Dali::NativeImage::GetWidth()
   */
  unsigned int GetWidth() const
  {
    return mWidth;
  }

  /**
   * @copydoc Dali::NativeImage::GetHeight()
   */
  unsigned int GetHeight() const
  {
    return mHeight;
  }

  /**
   * @return Pixel format
   */
  Pixel::Format GetPixelFormat() const
  {
    return mPixelFormat;
  }

  void UploadImage(Android::ImageDataHandle imageData);

  /**
   * @copydoc Dali::NativeImage::DoCleanUpOnContextDestroyed()
   */
  void DoCleanUpOnContextDestroyed();


  void RequestUpdate();

private:

  /**
   * Private constructor; @see AndroidNativeImage::New()
   * @param[in] width The width of the image.
   * @param[in] height The height of the image.
   * @param[in] colour depth of the pixmap
   * @param[in] adaptor a reference to Dali adaptor
   * @param[in] pixmap contains either: pixmap of type X11 Pixmap , a Ecore_X_Pixmap or is empty
   */
  AndroidNativeImage( unsigned int width,
                      unsigned int height,
                      Dali::AndroidNativeImage::ColorDepth depth,
                      Dali::Adaptor &adaptor);


  /**
   * 2nd phase construction.
   */
  void Initialize();

  /**
   * Uses X11 to get the default depth.
   * @param depth the PixelImage depth enum
   * @return default x11 pixel depth
   */
  int GetPixelDepth(Dali::AndroidNativeImage::ColorDepth depth) const;

  /**
   * Sets the pixel format based on the bit depth
   * @param depth depth in bytes
   */
  void SetPixelFormat(int depth);

  /**
   * Drains the stored request queue of bitmap uploads.
   */
  void DrainQueue();

private:

  struct Request
  {
    Android::ImageDataHandle mImageData;

    Request(Android::ImageDataHandle imageData) : mImageData(imageData)
    {     
    }
  };

  unsigned int mWidth;                                ///< pixmap width
  unsigned int mHeight;                               ///< pixmap heights
  Pixel::Format mPixelFormat;                         ///< pixmap pixel format
  Dali::AndroidNativeImage::ColorDepth mColorDepth;   ///< color depth of pixmap
  Adaptor& mAdaptor;                                  ///< adaptor
  GraphicBufferWrapper* mGraphicBuffer;               ///< GraphicBuffer's wrapper
  EGLImageKHR mEglImage;                              ///< eglImage

  boost::lockfree::queue<Request*> mRequestQueue;   

};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_PIXMAP_IMAGE_H__
