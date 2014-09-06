#ifndef __DALI_INTERNAL_SURFACE_IMAGE_H__
#define __DALI_INTERNAL_SURFACE_IMAGE_H__

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
#include <dali/public-api/adaptor-framework/common/surface-image.h>
#include <dali/public-api/adaptor-framework/common/adaptor.h>
#include <public-api/adaptor-framework/application.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

namespace Dali
{

namespace Internal
{
  class EglImageExtensions;

namespace Adaptor
{


/**
 * Dali internal SurfaceImage.
 */
class SurfaceImage
{
public:

  /**
   * Create a new SurfaceImage internally.
   * Depending on hardware the width and height may have to be a power of two.
   * @param[in] width The width of the image.
   * @param[in] height The height of the image.
   * @param[in] depth color depth of the pixmap
   * @param[in] application reference to dali application
   * @param[in] pixmap contains either: pixmap of type X11 Pixmap , a Ecore_X_Pixmap or is empty
   * @return A smart-pointer to a newly allocated image.
   */
  static SurfaceImage* New(unsigned int width,
                          unsigned int height,
                          Dali::SurfaceImage::ColorDepth depth,
                          Dali::Application application,
                          ISurfaceImageRenderer* renderer);

  /**
   * Create a new SurfaceImage internally.
   * Depending on hardware the width and height may have to be a power of two.
   * @param[in] width The width of the image.
   * @param[in] height The height of the image.
   * @param[in] depth color depth of the pixmap
   * @param[in] adaptor reference to dali adaptor
   * @param[in] pixmap contains either: pixmap of type X11 Pixmap , a Ecore_X_Pixmap or is empty
   * @return A smart-pointer to a newly allocated image.
   */
  static SurfaceImage* New(unsigned int width,
                          unsigned int height,
                          Dali::SurfaceImage::ColorDepth depth,
                          Dali::Adaptor& adaptor,
                          ISurfaceImageRenderer* renderer);


  /**
   * destructor
   */
  ~SurfaceImage();

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

  /**
   * @copydoc Dali::NativeImage::DoCleanUpOnContextDestroyed()
   */
  void DoCleanUpOnContextDestroyed();

  /**
   * Clear the ISurfaceImageRenderer mRenderer from the surface image object
   * to prevent race condition on destruction.
   */
  void ClearRenderer();


  void RequestUpdate();

private:

  /**
   * Private constructor; @see SurfaceImage::New()
   * @param[in] width The width of the image.
   * @param[in] height The height of the image.
   * @param[in] colour depth of the pixmap
   * @param[in] adaptor a reference to Dali adaptor
   * @param[in] pixmap contains either: pixmap of type X11 Pixmap , a Ecore_X_Pixmap or is empty
   */
  SurfaceImage(unsigned int width,
               unsigned int height,
               Dali::SurfaceImage::ColorDepth depth,
               Dali::Adaptor &adaptor,
               ISurfaceImageRenderer* renderer);


  /**
   * 2nd phase construction.
   */
  void Initialize();

  /**
   * Uses X11 to get the default depth.
   * @param depth the PixelImage depth enum
   * @return default x11 pixel depth
   */
  int GetPixelDepth(Dali::SurfaceImage::ColorDepth depth) const;

  /**
   * Sets the pixel format based on the bit depth
   * @param depth depth in bytes
   */
  void SetPixelFormat(int depth);

  /**
   * Gets the pixmap from the boost::any parameter
   * @param pixmap contains either: pixmap of type X11 Pixmap , a Ecore_X_Pixmap or is empty
   * @return pixmap x11 pixmap
   */
  // Ecore_X_Pixmap GetPixmapFromBoostAny(boost::any pixmap) const;

private:

  unsigned int mWidth;                        ///< pixmap width
  unsigned int mHeight;                       ///< pixmap heights
  Pixel::Format mPixelFormat;                 ///< pixmap pixel format
  Dali::SurfaceImage::ColorDepth mColorDepth;  ///< color depth of pixmap
  ISurfaceImageRenderer* mRenderer;           /// callbacks for rendering to surface
  bool mSurfaceTextureCreated;
  Adaptor& mAdaptor;                          ///< adaptor
};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_PIXMAP_IMAGE_H__
