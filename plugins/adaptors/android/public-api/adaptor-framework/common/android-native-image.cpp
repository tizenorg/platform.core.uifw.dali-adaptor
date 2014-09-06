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

// CLASS HEADER
#include <dali/public-api/adaptor-framework/common/android-native-image.h>

// INTERNAL INCLUDES
#include <internal/common/android-native-image-impl.h>

namespace Dali
{

AndroidNativeImagePtr AndroidNativeImage::New(unsigned int width, unsigned int height, ColorDepth depth, Application application)
{
  AndroidNativeImagePtr image = new AndroidNativeImage(width, height, depth, application);
  return image;
}

AndroidNativeImagePtr AndroidNativeImage::New(unsigned int width, unsigned int height, ColorDepth depth, Adaptor& adaptor)
{
  AndroidNativeImagePtr image = new AndroidNativeImage(width, height, depth, adaptor);
  return image;
}

bool AndroidNativeImage::GlExtensionCreate()
{
  return mImpl->GlExtensionCreate();
}

void AndroidNativeImage::GlExtensionDestroy()
{
  mImpl->GlExtensionDestroy();
}

unsigned int AndroidNativeImage::TargetTexture()
{
  return mImpl->TargetTexture();
}

void AndroidNativeImage::TextureCreated(unsigned textureId)
{
  mImpl->TextureCreated(textureId);
}

void AndroidNativeImage::PrepareTexture()
{ 
  mImpl->PrepareTexture();
}

unsigned int AndroidNativeImage::GetWidth() const
{
  return mImpl->GetWidth();
}

unsigned int AndroidNativeImage::GetHeight() const
{
  return mImpl->GetHeight();
}

Pixel::Format AndroidNativeImage::GetPixelFormat() const
{
  return mImpl->GetPixelFormat();
}

void AndroidNativeImage::UploadImage(Android::ImageDataHandle imageData)
{
  mImpl->UploadImage(imageData);
}

void AndroidNativeImage::DoCleanUpOnContextDestroyed()
{
  mImpl->DoCleanUpOnContextDestroyed();
}

AndroidNativeImage::AndroidNativeImage(unsigned int width, unsigned int height, ColorDepth depth, Application application)
{
   mImpl = Internal::Adaptor::AndroidNativeImage::New(width, height, depth, application);
}

AndroidNativeImage::AndroidNativeImage(unsigned int width, unsigned int height, ColorDepth depth, Adaptor& adaptor)
{
   mImpl = Internal::Adaptor::AndroidNativeImage::New( width, height, depth, adaptor);
}

AndroidNativeImage::~AndroidNativeImage()
{
  delete mImpl;
}

void AndroidNativeImage::RequestUpdate()
{
  mImpl->RequestUpdate();
}

} // namespace Dali
