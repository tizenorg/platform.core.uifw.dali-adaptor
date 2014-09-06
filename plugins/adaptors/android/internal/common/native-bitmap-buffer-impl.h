#ifndef __DALI_NATIVE_BITMAP_BUFFER_H__
#define __DALI_NATIVE_BITMAP_BUFFER_H__

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

#include <dali/public-api/images/native-image.h>
#include <dali/public-api/images/pixel.h>
#include <dali/integration-api/gl-abstraction.h>
#include <dali/integration-api/common/lockless-buffer.h>
#include <internal/common/adaptor-impl.h>
#include <vector>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

class NativeBitmapBuffer;
typedef Dali::IntrusivePtr<NativeBitmapBuffer> NativeBitmapBufferPtr;

/**
 * A Bitmap-based implementation of the NativeImage interface.
 */
class NativeBitmapBuffer : public NativeImage
{

public:
  /**
   * Constructor.
   * @param adaptor Adaptor used
   * @param width width of image
   * @param height height of image
   * @param pixelFormat pixel format for image
   */
  NativeBitmapBuffer( Adaptor* adaptor, unsigned int width, unsigned int height, Pixel::Format pixelFormat );

  /**
   * virtual destructor
   */
  virtual ~NativeBitmapBuffer();

  /**
   * Write to buffer. Does not block.
   * @param[in] src  data source
   * @param[in] size size of data in bytes
   * @return true if successful, false if currently reading from buffer in render thread
   */
  void Write( const unsigned char* src, size_t size );

public:
  /**
   * @copydoc Dali::NativeImage::GlExtensionCreate()
   */
  virtual bool GlExtensionCreate();

  /**
   * @copydoc Dali::NativeImage::GlExtensionDestroy()
   */
  virtual void GlExtensionDestroy();

  /**
   * @copydoc Dali::NativeImage::TargetTexture()
   */
  virtual unsigned int TargetTexture();

  /**
   * @copydoc Dali::NativeImage::PrepareTexture()
   */
  virtual void PrepareTexture();

  /**
   * @copydoc Dali::NativeImage::GetWidth()
   */
  virtual unsigned int GetWidth() const;

  /**
   * @copydoc Dali::NativeImage::GetHeight()
   */
  virtual unsigned int GetHeight() const;

  /**
   * @copydoc Dali::NativeImage::GetPixelFormat()
   */
  virtual Pixel::Format GetPixelFormat() const;

private:
  NativeBitmapBuffer( const NativeBitmapBuffer& );             ///< not defined
  NativeBitmapBuffer& operator =( const NativeBitmapBuffer& ); ///< not defined
  NativeBitmapBuffer(); ///< not defined

private:
  Integration::GlAbstraction*  mGlAbstraction; ///< GlAbstraction used

  Integration::LocklessBuffer* mBuffer;        ///< bitmap data double buffered
  unsigned int                 mWidth;         ///< Image width
  unsigned int                 mHeight;        ///< Image height
  Pixel::Format                mPixelFormat;   ///< Image pixelformat
};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_NATIVE_BITMAP_BUFFER_H__
