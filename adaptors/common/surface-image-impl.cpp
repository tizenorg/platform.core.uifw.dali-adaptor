
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
#include "surface-image-impl.h"

// EXTERNAL INCLUDES
#include <dali/public-api/math/math-utils.h> // NextPowerOfTwo
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <render-surface.h>
#include <application-impl.h>
#include <adaptor-impl.h>
#if (DALI_GLES_VERSION == 20)
#include <GLES2/gl2.h>
#else
#include <GLES3/gl3.h>
#endif
#include <EGL/eglext.h>
#include <android/log.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{


SurfaceImage* SurfaceImage::New(unsigned int width, unsigned int height, Dali::SurfaceImage::ColorDepth depth, Dali::Application application,  ISurfaceImageRenderer* renderer )
{
  Application& internalApp = Dali::Internal::Adaptor::GetImplementation(application);
  SurfaceImage* image = new SurfaceImage(width, height, depth, internalApp.GetAdaptor(), renderer);

  // 2nd phase construction
  image->Initialize();

  return image;
}

SurfaceImage* SurfaceImage::New(unsigned int width, unsigned int height, Dali::SurfaceImage::ColorDepth depth, Dali::Adaptor& adaptor,  ISurfaceImageRenderer* renderer )
{
  SurfaceImage* image = new SurfaceImage(width, height, depth, adaptor, renderer);

  // 2nd phase construction
  image->Initialize();

  return image;
}

SurfaceImage::SurfaceImage(unsigned int width, unsigned int height, Dali::SurfaceImage::ColorDepth depth, Dali::Adaptor& adaptor, ISurfaceImageRenderer* renderer)
: mWidth(width),
  mHeight(height),
  mColorDepth(depth),
  mRenderer(renderer),
  mSurfaceTextureCreated(false),
  mAdaptor(Internal::Adaptor::Adaptor::GetImplementation(adaptor))
{
  mGlAbstraction = &(mAdaptor.GetGlAbstraction());
}

void SurfaceImage::Initialize()
{
}

SurfaceImage::~SurfaceImage()
{
  if ( NULL != mRenderer )
  {
    mRenderer->destroySurfaceTexture();
  }
  delete mRenderer;
  mRenderer = NULL;

  GlExtensionDestroy();
}

bool SurfaceImage::GlExtensionCreate()
{
  return true;
}

void SurfaceImage::GlExtensionDestroy()
{
}

unsigned int SurfaceImage::TargetTexture()
{
  if ( NULL != mRenderer )
  {
    mRenderer->updateTexture();
  }

  return 0;
}

void SurfaceImage::PrepareTexture()
{
  if ( NULL != mRenderer )
  {
    mRenderer->updateTexture();
  }
}

void SurfaceImage::Resize( const Vector2& newSize )
{
  if ( NULL != mRenderer )
  {
    mRenderer->Resize(newSize);
  }
}

bool SurfaceImage::IsReady()
{
  if ( NULL != mRenderer )
  {
    return mRenderer->isReady();
  }
  return true;
}

void SurfaceImage::GlContextCreated()
{
  if (mRenderer)
  {
    mRenderer->GlContextCreated();
  }
}

void SurfaceImage::GlContextDestroyed()
{
  // Avoid destruction race condition
  if ( NULL != mRenderer )
  {
    mRenderer->destroySurfaceTexture();
  }
  mSurfaceTextureCreated = false;
}

void SurfaceImage::TextureCreated(unsigned textureId)
{
  if (!mRenderer) // ClearRenderer can be called before this
    return;

  if( mSurfaceTextureCreated )
  {
    mRenderer->destroySurfaceTexture();
  }

  mRenderer->createSurfaceTexture(mWidth, mHeight, textureId);
  mSurfaceTextureCreated = true;
}

int SurfaceImage::GetPixelDepth(Dali::SurfaceImage::ColorDepth depth) const
{
  switch (depth)
  {
    case Dali::SurfaceImage::COLOR_DEPTH_8: return 8;
    case Dali::SurfaceImage::COLOR_DEPTH_16: return 16;
    case Dali::SurfaceImage::COLOR_DEPTH_24: return 24;
    case Dali::SurfaceImage::COLOR_DEPTH_DEFAULT:
    case Dali::SurfaceImage::COLOR_DEPTH_32: return 32;
    default: DALI_ASSERT_DEBUG(0 && "unknown color enum"); return 0;
  }
}

void SurfaceImage::ClearRenderer()
{
  mRenderer = NULL;
}

void SurfaceImage::RequestUpdate()
{
  mAdaptor.RequestUpdateOnce();
}

bool SurfaceImage::RequiresBlending() const
{
  return GetPixelDepth(mColorDepth) == 32;
}

} // namespace Adaptor

} // namespace internal

} // namespace Dali
