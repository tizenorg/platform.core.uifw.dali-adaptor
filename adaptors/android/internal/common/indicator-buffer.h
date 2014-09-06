#ifndef __DALI_INTERNAL_INDICATOR_BUFFER_H__
#define __DALI_INTERNAL_INDICATOR_BUFFER_H__

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
#include <internal/common/native-bitmap-buffer-impl.h>
#include <dali/public-api/object/ref-object.h>
#include <internal/common/adaptor-impl.h>

namespace Dali
{

namespace Internal
{
namespace Adaptor
{

class NativeBitmapBuffer;
class IndicatorBuffer;

typedef IntrusivePtr<IndicatorBuffer> IndicatorBufferPtr;

/**
 * The IndicatorBuffer class uses the best available implementation for rendering indicator data.
 * On platforms where EglImage is available it uses either SharedGlBuffer or PixmapImage, on older
 * platforms it falls back to using a bitmap buffer based solution.
 */
class IndicatorBuffer : public RefObject
{
public:

  /**
   * Constructor
   */
  IndicatorBuffer( Adaptor* adaptor, unsigned int width, unsigned int height, Pixel::Format pixelFormat );

  /**
   * Copy bitmap data to pixel buffer.
   * @param src  bitmap data source
   * @param size size of bitmap data
   * @return true if successful, false otherwise
   */
  bool UpdatePixels( const unsigned char *src, size_t size );

  /**
   * Returns the NativeImage used internally
   * @return the NativeImage used internally
   */
  NativeImage& GetNativeImage() const;

  /**
   * Set currently used Adaptor
   * @param adaptor
   */
  void SetAdaptor( Adaptor* adaptor );

private:
  NativeImagePtr mNativeImage; ///< Image buffer created for shared file copy

  NativeBitmapBufferPtr mBitmapBuffer;    ///< Image buffer created for shared file copy if extension not available

  Adaptor*      mAdaptor;

  int           mImageWidth;
  int           mImageHeight;
  Pixel::Format mPixelFormat;

  // Only used with fallback bitmap buffer implementation
  bool  mUpdatingBitmap:1;      ///< Whether BitmapImage is being uploaded to graphics memory
  bool  mUpdateBitmapAgain:1;   ///< Whether to update BitmapImage again after upload complete
};

} // Adaptor
} // Internal
} // Dali

#endif // __DALI_INTERNAL_INDICATOR_H__
