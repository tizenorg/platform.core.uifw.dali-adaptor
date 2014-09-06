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
#include "surface-image-impl.h"

// EXTERNAL INCLUDES
#include <dali/public-api/math/math-utils.h> // NextPowerOfTwo
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali/public-api/adaptor-framework/common/render-surface.h>
#include <internal/application-impl.h>
#include <internal/common/adaptor-impl.h>

#include <GLES2/gl2.h>
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
  mPixelFormat(Pixel::RGB888),
  mColorDepth(depth),
  mRenderer(renderer),
  mSurfaceTextureCreated(false),
  mAdaptor(Internal::Adaptor::Adaptor::GetImplementation(adaptor))
{
}

void SurfaceImage::Initialize()
{
  // Get render-surface being used by Dali
  Dali::RenderSurface& surface = mAdaptor.GetSurface();

  // get the X11 display pointer and store it
  // it is used by eglCreateImageKHR, and XFreePixmap
  // Any other display (x-connection) will fail in eglCreateImageKHR
  //boost::any display = surface.GetDisplay();

  // get the pixel depth
  int depth = GetPixelDepth(mColorDepth);

  // set the pixel format
  SetPixelFormat(depth);

  // Get the X-Renderable for which the pixmap is created on
  //boost::any renderableSurface =  surface.GetSurface();

  // if Dali is using a Pixmap or Window to render to it doesn't matter because they have the same
  // underlying type of unsigned long
  //ANativeWindow* daliWindow = boost::any_cast< ANativeWindow* >(renderableSurface);
  // mPixmap = ecore_x_pixmap_new(daliWindow, mWidth, mHeight, depth);
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
    mRenderer->render();
    mRenderer->updateTexture();
  }

  return 0;
}

void SurfaceImage::PrepareTexture()
{
  if ( NULL != mRenderer )
  {
    mRenderer->render();
    mRenderer->updateTexture();
  }
}

void SurfaceImage::DoCleanUpOnContextDestroyed()
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

  if (mSurfaceTextureCreated)
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

void SurfaceImage::SetPixelFormat(int depth)
{
  // store the pixel format based on the depth
  switch (depth)
  {
    case 8:
      mPixelFormat = Pixel::A8;
      break;

    case 16:
      mPixelFormat = Pixel::RGB565;
      break;

    case 32:
      mPixelFormat = Pixel::RGBA8888;
      break;

    case 24:
    default:
      mPixelFormat = Pixel::RGB888;
      break;
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

} // namespace Adaptor

} // namespace internal

} // namespace Dali
