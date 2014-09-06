#ifndef __DALI_SURFACE_IMAGE_H__
#define __DALI_SURFACE_IMAGE_H__

/*
Copyright (c) 2000-2012 Samsung Electronics Co., Ltd All Rights Reserved

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
#include <boost/any.hpp>
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/images/native-image.h>

// INTERNAL INCLUDES
#include <public-api/adaptor-framework/application.h>

namespace Dali DALI_IMPORT_API
{
class Adaptor;

namespace Internal DALI_INTERNAL
{
namespace Adaptor
{
class SurfaceImage;
}
}

class SurfaceImage;
typedef Dali::IntrusivePtr<SurfaceImage> SurfaceImagePtr;

class ISurfaceImageRenderer  {
public:
  virtual int getWidth()=0;
  virtual int getHeight()=0;
  // create Surface Texture Object
  virtual void createSurfaceTexture(int width, int height, int texID)=0;
  virtual void destroySurfaceTexture()=0;

  // call SurfaceTexture::updateTexImage - must be on render thread
  virtual void updateTexture()=0;

  // could be on different thread
  virtual void render()=0;
};


/**
 * Used for displaying/streaming Android SurfaceTextures.
 *
 */
class SurfaceImage : public NativeImage
{
public:

   /**
    * When creating a pixmap the color depth has to be specified
    */
   enum ColorDepth
   {
     COLOR_DEPTH_DEFAULT,     ///< Uses the current X screen default depth (recommended)
     COLOR_DEPTH_8,           ///< 8 bits per pixel
     COLOR_DEPTH_16,          ///< 16 bits per pixel
     COLOR_DEPTH_24,          ///< 24 bits per pixel
     COLOR_DEPTH_32           ///< 32 bits per pixel
   };

  /**
   * Create a new SurfaceImage.
   * Depending on hardware the width and height may have to be a power of two.
   * @param[in] width The width of the image.
   * @param[in] height The height of the image.
   * @param[in] depth color depth of the pixmap
   * @param[in] application reference to dali application
   * @return A smart-pointer to a newly allocated image.
   */
  static SurfaceImagePtr New( unsigned int width, unsigned int height, ColorDepth depth, Application application );

  /**
   * Create a new SurfaceImage.
   * Depending on hardware the width and height may have to be a power of two.
   * @param[in] width The width of the image.
   * @param[in] height The height of the image.
   * @param[in] depth color depth of the pixmap
   * @param[in] adaptor reference to dali adaptor
   * @return A smart-pointer to a newly allocated image.
   */
  static SurfaceImagePtr New( unsigned int width, unsigned int height, ColorDepth depth,  Adaptor& adaptor );

  /**
   * Create a new SurfaceImage from an existing pixmap.
   * @param[in] pixmap must be a X11 pixmap or a Ecore_X_Pixmap
   * @param[in] application a reference to Dali adaptor
   * @return A smart-pointer to a newly allocated image.
   */
  static SurfaceImagePtr New( ISurfaceImageRenderer* renderer, Application application);

  /**
   * Create a new SurfaceImage from an existing pixmap.
   * @param[in] pixmap must be a X11 pixmap or a Ecore_X_Pixmap
   * @param[in] adaptor reference to dali adaptor
   * @return A smart-pointer to a newly allocated image.
   */
  static SurfaceImagePtr New( ISurfaceImageRenderer* renderer, Adaptor& adaptor );

  /**
   * Clear the ISurfaceImageRenderer mRenderer from the surface image object
   * to prevent race condition on destruction.
   */
  void ClearRenderer();

private:   // native image

  /**
   * From NativeImage
   */
  virtual bool GlExtensionCreate();

  /**
   * From NativeImage
   */
  virtual void GlExtensionDestroy();

  /**
   * From NativeImage
   * @param[in] textureId the texture id used from the native texture.
   */
  virtual unsigned int TargetTexture();

  /**
   * From NativeImage
   * @param[in] textureId the texture id used from the native texture.
   */
  virtual void TextureCreated(unsigned textureId);

    /**
   * Called in each NativeTexture::Bind() call to allow implementation specific operations.
   * The correct texture sampler has already been bound before the function gets called.
   * @pre glAbstraction is being used by context in current thread
   */
  virtual void PrepareTexture();

  /**
   * From NativeImage
   * @return pixmap width
   */
  virtual unsigned int GetWidth() const;

  /**
   * From NativeImage
   * @return pixmap height
   */
  virtual unsigned int GetHeight() const;

  /**
   * From NativeImage
   * @return Pixel format
   */
  virtual Pixel::Format GetPixelFormat() const;

private:

  /**
   * Private constructor
   * @param[in] width The width of the image.
   * @param[in] height The height of the image.
   * @param[in] depth color depth of the pixmap
   * @param[in] application a reference to Dali application
   * @param[in] pixmap contains either: pixmap of type X11 Pixmap , a Ecore_X_Pixmap or is empty
   */
  SurfaceImage(unsigned int width, unsigned int height, ColorDepth depth, Application application, ISurfaceImageRenderer* renderer);

  /**
   * Private constructor
   * @param[in] width The width of the image.
   * @param[in] height The height of the image.
   * @param[in] depth color depth of the pixmap
   * @param[in] adaptor a reference to Dali adaptor
   * @param[in] pixmap contains either: pixmap of type X11 Pixmap , a Ecore_X_Pixmap or is empty
   */
  SurfaceImage(unsigned int width, unsigned int height, ColorDepth depth, Adaptor& adaptor, ISurfaceImageRenderer* renderer);

  /**
   * A reference counted object may only be deleted by calling Unreference().
   * The implementation should destroy the NativeImage resources.
   */
public:
  virtual ~SurfaceImage();

  /**
   * Undefined assignment operator.
   * This avoids accidental calls to a default assignment operator.
   * @param[in] rhs A reference to the object to copy.
   */
  SurfaceImage& operator=(const SurfaceImage& rhs);

  /**
   * @copydoc NativeImage::DoCleanUpOnContextDestroyed
   */
  void DoCleanUpOnContextDestroyed();


  void RequestUpdate();

private:

  Internal::Adaptor::SurfaceImage* mImpl;

};

} // namespace Dali

#endif // __DALI_PIXMAP_IMAGE_H__

