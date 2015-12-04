#ifndef __DALI_EGL_BUFFER_H__
#define __DALI_EGL_BUFFER_H__

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
#include <EGL/egl.h>
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/images/native-image-interface.h>

// INTERNAL INCLUDES
#include <dali/public-api/adaptor-framework/application.h>

namespace Dali DALI_IMPORT_API
{
class Adaptor;

namespace Internal DALI_INTERNAL
{
namespace Adaptor
{
class EglBuffer;
}
}

class EglBuffer;
typedef Dali::IntrusivePtr<EglBuffer> EglBufferPtr;

/**
 * Interface to allow custom drawing code to write into the EglBuffer prior to
 * rendering.
 */
class IEglBufferRenderer
{
public:
  /**
   * Called to create the buffer from which the egl image will be generated.
   * @param[in] width Width of the buffer to create
   * @param[in] height Height of the buffer to create
   * @param[out] buffer The EGLClientBuffer to generate the image from. Ownership
   * remains with the renderer.
   */
  virtual void CreateTexture( unsigned int width, unsigned int height, EGLClientBuffer& buffer ) = 0;

  /**
   * Called when the texture is about to be destroyed to enable the renderer
   * to clean up it's buffer.
   */
  virtual void DestroyTexture() = 0;

  /**
   * Called after the texture is bound but before it is rendered via a gl Draw call.
   * Enables a renderer to update the texture.
   * @param[in] eglImage The egl image.
   */
  virtual void UpdateTexture( void* eglImage ) = 0;

protected:
  virtual ~IEglBufferRenderer()
  {
  }
};


/**
 * Used for displaying/streaming Android EglTextures.
 *
 */
class EglBuffer : public Dali::NativeImageInterface
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
   * Create a new EglBuffer.
   * Depending on hardware the width and height may have to be a power of two.
   * @param[in] width The width of the buffer.
   * @param[in] height The height of the buffer.
   * @param[in] depth color depth of the pixmap
   * @param[in] renderer The renderer that will draw into this buffer
   * @return A smart-pointer to a newly allocated buffer.
   */
  static EglBufferPtr New( unsigned int width, unsigned int height, ColorDepth depth, IEglBufferRenderer* renderer );

  /**
   * Clear the IEglBufferRenderer mRenderer from the egl buffer object
   * to prevent race condition on destruction.
   */
  void ClearRenderer();

private:   // NativeImageInterface

  /**
   * @copydoc NativeImageInterface::GlExtensionCreate();
   */
  virtual bool GlExtensionCreate();

  /**
   * @copydoc NativeImageInterface::GlExtensionDestroy();
   */
  virtual void GlExtensionDestroy();

  /**
   * @copydoc NativeImageInterface::TargetTexture();
   */
  virtual unsigned int TargetTexture();

  /**
   * @copydoc NativeImageInterface::TextureCreated();
   */
  virtual void TextureCreated(unsigned textureId);

  /**
   * @copydoc NativeImageInterface::PrepareTexture();
   */
  virtual void PrepareTexture();

  /**
   * @copydoc NativeImageInterface::Resize()
   */
  virtual void Resize( const Vector2& newSize );

  /**
   * @copydoc NativeImageInterface::IsReady()
   */
  virtual bool IsReady();

  /**
   * @copydoc NativeImageInterface::GetWidth()
   */
  virtual unsigned int GetWidth() const;

  /**
   * @copydoc NativeImageInterface::GetHeight()
   */
  virtual unsigned int GetHeight() const;

  /**
   * @copydoc NativeImageInterface::RequiresBlending()
   */
  bool RequiresBlending() const;


private:

  /**
   * Private constructor
   * @param[in] width The width of the buffer.
   * @param[in] height The height of the buffer.
   * @param[in] depth color depth of the pixmap
   * @param[in] renderer An existing egl buffer renderer
   */
  EglBuffer(unsigned int width, unsigned int height, ColorDepth depth, IEglBufferRenderer* renderer);

  /**
   * A reference counted object may only be deleted by calling Unreference().
   * The implementation should destroy the NativeBuffer resources.
   */
public:
  virtual ~EglBuffer();

  /**
   * Undefined assignment operator.
   * This avoids accidental calls to a default assignment operator.
   * @param[in] rhs A reference to the object to copy.
   */
  EglBuffer& operator=(const EglBuffer& rhs);

  /**
   * @copydoc NativeBuffer::GlContextCreated
   */
  virtual void GlContextCreated();

  /**
   * @copydoc NativeBuffer::GlContextDestroyed
   */
  virtual void GlContextDestroyed();

private:

  Internal::Adaptor::EglBuffer* mImpl;

};

} // namespace Dali

#endif // __DALI_EGL_BUFFER_H__
