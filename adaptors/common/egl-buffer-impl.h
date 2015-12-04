#ifndef __DALI_INTERNAL_EGL_BUFFER_H__
#define __DALI_INTERNAL_EGL_BUFFER_H__

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
#include <egl-buffer.h>
#include <adaptor.h>
#include <public-api/adaptor-framework/application.h>
#include <dali/integration-api/gl-abstraction.h>
#include <common/gl/egl-factory.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

/**
 * Dali internal EglBuffer.
 */
class EglBuffer
{
public:

/**
   * Create a new EglBuffer internally.
   * Depending on hardware the width and height may have to be a power of two.
   * @param[in] width The width of the buffer.
   * @param[in] height The height of the buffer.
   * @param[in] depth color depth of the pixmap
   * @param[in] adaptor reference to dali adaptor
   * @param[in] renderer contains a pointer to a egl buffer renderer
   * @return A smart-pointer to a newly allocated buffer.
   */
  static EglBuffer* New( unsigned int width,
                         unsigned int height,
                         Dali::EglBuffer::ColorDepth depth,
                         Dali::Adaptor& adaptor,
                         IEglBufferRenderer* renderer );


  /**
   * destructor
   */
  ~EglBuffer();

  /**
   * @copydoc Dali::NativeBuffer::GlExtensionCreate()
   */
  bool GlExtensionCreate();

  /**
   * @copydoc Dali::NativeBuffer::GlExtensionDestroy()
   */
  void GlExtensionDestroy();

  /**
   * @copydoc Dali::NativeBuffer::TargetTexture()
   */
  unsigned int TargetTexture();

   /**
   * @copydoc Dali::NativeBuffer::TextureCreated()
   */
  void TextureCreated(unsigned textureId);

  /**
   * @copydoc Dali::NativeBuffer::PrepareTexture()
   */
  void PrepareTexture();

  /**
   * @copydoc Dali::NativeBuffer::Resize()
   */
  void Resize( const Vector2& newSize );

  /**
   * @copydoc Dali::NativeBuffer::GlContextCreated()
   */
  void GlContextCreated();

  bool IsReady();

  /**
   * @copydoc Dali::NativeBuffer::GetWidth()
   */
  unsigned int GetWidth() const
  {
    return mWidth;
  }

  /**
   * @copydoc Dali::NativeBuffer::GetHeight()
   */
  unsigned int GetHeight() const
  {
    return mHeight;
  }

  /**
   * @copydoc Dali::NativeBuffer::DoCleanUpOnContextDestroyed()
   */
  void GlContextDestroyed();

  /**
   * Clear the IEglBufferRenderer mRenderer from the egl buffer object
   * to prevent race condition on destruction.
   */
  void ClearRenderer();

  void RequestUpdate();

  bool RequiresBlending() const;

private:

  /**
   * Private constructor; @see EglBuffer::New()
   * @param[in] width The width of the buffer.
   * @param[in] height The height of the buffer.
   * @param[in] colour depth of the pixmap
   * @param[in] adaptor a reference to Dali adaptor
   * @param[in] renderer A pointer to a EglBuffer renderer
   */
  EglBuffer( unsigned int width,
            unsigned int height,
            Dali::EglBuffer::ColorDepth depth,
            Dali::Adaptor &adaptor,
            IEglBufferRenderer* renderer );


  /**
   * 2nd phase construction.
   */
  void Initialize();

  /**
   * @param depth the PixelBuffer depth enum
   * @return default x11 pixel depth
   */
  int GetPixelDepth(Dali::EglBuffer::ColorDepth depth) const;

private:
  Adaptor&                         mAdaptor; ///< adaptor
  Integration::GlAbstraction*      mGlInterface;
  EglImageExtensions*              mEglImageExtensions;
  void*                            mEglImageKHR; ///< From EGL extension
  IEglBufferRenderer*              mRenderer; /// callbacks for rendering to egl
  unsigned int                     mWidth; ///< pixmap width
  unsigned int                     mHeight; ///< pixmap heights
  Dali::EglBuffer::ColorDepth      mColorDepth; ///< color depth of pixmap
};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_EGL_BUFFER_H__
