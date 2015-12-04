#ifndef __DALI_INTERNAL_SURFACE_IMAGE_H__
#define __DALI_INTERNAL_SURFACE_IMAGE_H__

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

// INTERNAL INCLUDES
#include <surface-image.h>
#include <adaptor.h>
#include <public-api/adaptor-framework/application.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <common/gl/gl-implementation.h>

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
   * @param[in] renderer A surface image renderer
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
   * @param[in] renderer contains a pointer to a surface image renderer
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
   * @copydoc Dali::NativeImage::Resize()
   */
  void Resize( const Vector2& newSize );

  /**
   * @copydoc Dali::NativeImage::GlContextCreated()
   */
  void GlContextCreated();

  bool IsReady();

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
   * @copydoc Dali::NativeImage::DoCleanUpOnContextDestroyed()
   */
  void GlContextDestroyed();

  /**
   * Clear the ISurfaceImageRenderer mRenderer from the surface image object
   * to prevent race condition on destruction.
   */
  void ClearRenderer();


  void RequestUpdate();

  bool RequiresBlending() const;
private:

  /**
   * Private constructor; @see SurfaceImage::New()
   * @param[in] width The width of the image.
   * @param[in] height The height of the image.
   * @param[in] colour depth of the pixmap
   * @param[in] adaptor a reference to Dali adaptor
   * @param[in] renderer A pointer to a SurfaceImage renderer
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

private:

  unsigned int mWidth;                        ///< pixmap width
  unsigned int mHeight;                       ///< pixmap heights
  Dali::SurfaceImage::ColorDepth mColorDepth;  ///< color depth of pixmap
  ISurfaceImageRenderer* mRenderer;           /// callbacks for rendering to surface
  bool mSurfaceTextureCreated;
  Adaptor& mAdaptor;                          ///< adaptor
  Integration::GlAbstraction*  mGlAbstraction;
};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SURFACE_IMAGE_H__
