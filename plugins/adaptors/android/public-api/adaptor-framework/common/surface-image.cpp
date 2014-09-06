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
#include <dali/public-api/adaptor-framework/common/surface-image.h>

// INTERNAL INCLUDES
#include <internal/common/surface-image-impl.h>

namespace Dali
{

SurfaceImagePtr SurfaceImage::New(unsigned int width, unsigned int height, ColorDepth depth, Application application)
{
  ISurfaceImageRenderer* renderer = NULL;
  SurfaceImagePtr image = new SurfaceImage(width, height, depth, application, renderer);
  return image;
}

SurfaceImagePtr SurfaceImage::New(unsigned int width, unsigned int height, ColorDepth depth, Adaptor& adaptor)
{
  ISurfaceImageRenderer* renderer = NULL;
  SurfaceImagePtr image = new SurfaceImage(width, height, depth, adaptor, renderer);
  return image;
}

SurfaceImagePtr SurfaceImage::New(ISurfaceImageRenderer* renderer, Application application)
{
  int width = renderer->getWidth();
  int height = renderer->getHeight();
  SurfaceImagePtr image = new SurfaceImage(width, height, COLOR_DEPTH_DEFAULT, application, renderer);
  return image;
}

SurfaceImagePtr SurfaceImage::New(ISurfaceImageRenderer* renderer, Adaptor& adaptor)
{
  SurfaceImagePtr image = new SurfaceImage(0, 0, COLOR_DEPTH_DEFAULT, adaptor, renderer);
  return image;
}

bool SurfaceImage::GlExtensionCreate()
{
  return mImpl->GlExtensionCreate();
}

void SurfaceImage::GlExtensionDestroy()
{
  mImpl->GlExtensionDestroy();
}

unsigned int SurfaceImage::TargetTexture()
{
  return mImpl->TargetTexture();
}

void SurfaceImage::TextureCreated(unsigned textureId)
{
  mImpl->TextureCreated(textureId);
}


void SurfaceImage::PrepareTexture()
{ 
  mImpl->PrepareTexture();
}

unsigned int SurfaceImage::GetWidth() const
{
  return mImpl->GetWidth();
}

unsigned int SurfaceImage::GetHeight() const
{
  return mImpl->GetHeight();
}

Pixel::Format SurfaceImage::GetPixelFormat() const
{
  return mImpl->GetPixelFormat();
}

void SurfaceImage::DoCleanUpOnContextDestroyed()
{
  mImpl->DoCleanUpOnContextDestroyed();
}

void SurfaceImage::RequestUpdate()
{
  mImpl->RequestUpdate();
}

SurfaceImage::SurfaceImage(unsigned int width, unsigned int height, ColorDepth depth, Application application, ISurfaceImageRenderer* renderer)
{
   mImpl = Internal::Adaptor::SurfaceImage::New( width, height, depth, application, renderer);
}

SurfaceImage::SurfaceImage(unsigned int width, unsigned int height, ColorDepth depth, Adaptor& adaptor, ISurfaceImageRenderer* renderer)
{
   mImpl = Internal::Adaptor::SurfaceImage::New( width, height, depth, adaptor, renderer);
}

SurfaceImage::~SurfaceImage()
{
  delete mImpl;
}

void SurfaceImage::ClearRenderer()
{
  mImpl->ClearRenderer();
}

} // namespace Dali
