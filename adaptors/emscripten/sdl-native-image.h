#ifndef __SDL_NATIVE_IMAGE_H__
#define __SDL_NATIVE_IMAGE_H__

/*
Copyright (c) 2000-2012 Samsung Electronics Co., Ltd All Rights Reserved

This file is part of Dali

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

// INTERNAL INCLUDES
#include <dali/public-api/images/native-image.h>

namespace Dali
{
class SdlNativeImage;
typedef IntrusivePtr<SdlNativeImage> SdlNativeImagePointer;

class DALI_IMPORT_API SdlNativeImage : public Dali::NativeImage
{
public:
  static SdlNativeImagePointer New(int width, int height) {return new SdlNativeImage(width, height);};
  virtual bool GlExtensionCreate() {return true;};
  virtual void GlExtensionDestroy() {};
  virtual GLenum TargetTexture() {return 1;};
  virtual void PrepareTexture() {};
  virtual unsigned int GetWidth() const {return mWidth;};
  virtual unsigned int GetHeight() const {return mHeight;};
  virtual Pixel::Format GetPixelFormat() const {return Pixel::RGBA8888;};

private:
  SdlNativeImage(int width, int height) : mWidth(width), mHeight(height) {};
  virtual ~SdlNativeImage() {};

  int mWidth;
  int mHeight;
};

} // Dali

#endif
