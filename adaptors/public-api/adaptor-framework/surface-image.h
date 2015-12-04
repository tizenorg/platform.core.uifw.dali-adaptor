#ifndef __DALI_SURFACE_IMAGE_H__
#define __DALI_SURFACE_IMAGE_H__
/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
 */

// EXTERNAL INCLUDES
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/images/native-image.h>

// INTERNAL INCLUDES
#include <dali/public-api/adaptor-framework/application.h>

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

  // call SurfaceTexture::updateTexImage - must be on render thread
  virtual void updateTexture()=0;
  // called from render thread when gl context is created
  virtual void GlContextCreated() = 0;
  // called from render thread
  virtual void destroySurfaceTexture()=0;

  virtual bool isReady()=0;

  // called from render thread
  virtual void Resize( const Vector2& newSize )=0;

  virtual ~ISurfaceImageRenderer()
  {
  }
};


/**
 * Used for displaying/streaming Android SurfaceTextures.
 *
 */
class SurfaceImage : public NativeImageInterface
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
   * Create a new SurfaceImage from an existing surface image renderer.
   * @param[in] renderer The existing renderer
   * @param[in] application a reference to Dali Application
   * @return A smart-pointer to a newly allocated image.
   */
  static SurfaceImagePtr New( ISurfaceImageRenderer* renderer, Application application);

  /**
   * Create a new SurfaceImage from an existing pixmap.
   * @param[in] renderer The existing renderer
   * @param[in] adaptor reference to Dali Adaptor
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
   * @brief Called after the corresponding call to Image::ResizeNativeImage( const Vector2& )
   *
   * @param[in] newSize The new size.
   */
  virtual void Resize( const Vector2& newSize );

  virtual bool IsReady();
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
   * @return whether the image requires blending
   */
  bool RequiresBlending() const;


private:

  /**
   * Private constructor
   * @param[in] width The width of the image.
   * @param[in] height The height of the image.
   * @param[in] depth color depth of the pixmap
   * @param[in] application a reference to Dali application
   * @param[in] renderer An existing surface image renderer
   */
  SurfaceImage(unsigned int width, unsigned int height, ColorDepth depth, Application application, ISurfaceImageRenderer* renderer);

  /**
   * Private constructor
   * @param[in] width The width of the image.
   * @param[in] height The height of the image.
   * @param[in] depth color depth of the pixmap
   * @param[in] adaptor a reference to Dali adaptor
   * @param[in] renderer An existing surface image renderer
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
   * @copydoc NativeImage::GlContextCreated
   */
  virtual void GlContextCreated();

  /**
   * @copydoc NativeImage::GlContextDestroyed
   */
  virtual void GlContextDestroyed();


  void RequestUpdate();

private:

  Internal::Adaptor::SurfaceImage* mImpl;

};

} // namespace Dali

#endif // __DALI_SURFACE_IMAGE_H__
