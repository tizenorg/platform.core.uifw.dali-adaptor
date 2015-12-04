
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

// CLASS HEADER
#include <dali/public-api/adaptor-framework/surface-image.h>

// INTERNAL INCLUDES
#include <surface-image-impl.h>

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
  int width = renderer->getWidth();
  int height = renderer->getHeight();
  SurfaceImagePtr image = new SurfaceImage(width, height, COLOR_DEPTH_DEFAULT, adaptor, renderer);
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

void SurfaceImage::Resize( const Vector2& newSize )
{
  mImpl->Resize(newSize);
}

bool SurfaceImage::IsReady()
{
  return mImpl->IsReady();
}

unsigned int SurfaceImage::GetWidth() const
{
  return mImpl->GetWidth();
}

unsigned int SurfaceImage::GetHeight() const
{
  return mImpl->GetHeight();
}

bool SurfaceImage::RequiresBlending() const
{
  return mImpl->RequiresBlending();
}

void SurfaceImage::GlContextCreated()
{
  mImpl->GlContextCreated();
}

void SurfaceImage::GlContextDestroyed()
{
  mImpl->GlContextDestroyed();
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
