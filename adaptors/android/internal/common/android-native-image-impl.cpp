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
#include "android-native-image-impl.h"

// EXTERNAL INCLUDES
#include <dali/public-api/math/math-utils.h> // NextPowerOfTwo
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali/public-api/adaptor-framework/common/render-surface.h>
#include <internal/application-impl.h>
#include <internal/common/adaptor-impl.h>
#include <graphicbuffer/graphicbufferwrapper.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include "nativeLogging.h"

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

AndroidNativeImage* AndroidNativeImage::New(unsigned int width, unsigned int height, Dali::AndroidNativeImage::ColorDepth depth, Dali::Application application)
{
  Application& internalApp = Dali::Internal::Adaptor::GetImplementation(application);
  AndroidNativeImage* image = new AndroidNativeImage(width, height, depth, internalApp.GetAdaptor());

  // 2nd phase construction
  image->Initialize();

  return image;
}

AndroidNativeImage* AndroidNativeImage::New(unsigned int width, unsigned int height, Dali::AndroidNativeImage::ColorDepth depth, Dali::Adaptor& adaptor)
{
  AndroidNativeImage* image = new AndroidNativeImage(width, height, depth, adaptor);

  // 2nd phase construction
  image->Initialize();

  return image;
}

AndroidNativeImage::AndroidNativeImage(unsigned int width, unsigned int height, Dali::AndroidNativeImage::ColorDepth depth, Dali::Adaptor& adaptor)
: mWidth(width),
  mHeight(height),
  mPixelFormat(Pixel::RGBA8888),
  mGraphicBuffer(NULL),
  mEglImage(0),
  mRequestQueue(16),
  mColorDepth(Dali::AndroidNativeImage::COLOR_DEPTH_32),
  mAdaptor(Internal::Adaptor::Adaptor::GetImplementation(adaptor))
{
}

void AndroidNativeImage::Initialize()
{
  mGraphicBuffer = GraphicBufferWrapper::New(mWidth, mHeight);
}

AndroidNativeImage::~AndroidNativeImage()
{
  GlExtensionDestroy();

  if (mGraphicBuffer)
  {
    delete mGraphicBuffer;
    mGraphicBuffer = NULL;
  }

  Request* request = NULL;
  while(mRequestQueue.pop(request))
  {
    delete request;
  }
}

bool AndroidNativeImage::GlExtensionCreate()
{
  return true;
}

void AndroidNativeImage::GlExtensionDestroy()
{
  // TODO: what to do with the eglImage ?
  // eglDestroyImageKHR(eglGetCurrentDisplay(), mEglImage);
  // mEglImage = 0;
}

unsigned int AndroidNativeImage::TargetTexture()
{
  // perform any pending uploads.
  DrainQueue();

  if (mEglImage == 0)
  {
    mEglImage = mGraphicBuffer->CreateEGLImage();
  }

  if (mEglImage)
  {
    glEGLImageTargetTexture2DOES(GL_TEXTURE_EXTERNAL_OES, mEglImage);
  }

  return 0;
}

void AndroidNativeImage::PrepareTexture()
{
  if (mEglImage)
  {
    glEGLImageTargetTexture2DOES(GL_TEXTURE_EXTERNAL_OES, mEglImage);
  }
}

void AndroidNativeImage::DoCleanUpOnContextDestroyed()
{
}

void AndroidNativeImage::TextureCreated(unsigned textureId)
{
  if (mEglImage)
  {
    glEGLImageTargetTexture2DOES(GL_TEXTURE_EXTERNAL_OES, mEglImage);
  }
}

int AndroidNativeImage::GetPixelDepth(Dali::AndroidNativeImage::ColorDepth depth) const
{
  switch (depth)
  {
    case Dali::AndroidNativeImage::COLOR_DEPTH_8: return 8;
    case Dali::AndroidNativeImage::COLOR_DEPTH_16: return 16;
    case Dali::AndroidNativeImage::COLOR_DEPTH_24: return 24;
    case Dali::AndroidNativeImage::COLOR_DEPTH_DEFAULT:
    case Dali::AndroidNativeImage::COLOR_DEPTH_32: return 32;
    default: DALI_ASSERT_DEBUG(0 && "unknown color enum"); return 0;
  }
}

void AndroidNativeImage::DrainQueue()
{
  if (mGraphicBuffer)
  {
    Request* request = NULL;
    while(mRequestQueue.pop(request))
    {
      GraphicBufferMemMap mm(mGraphicBuffer);
      if (mm.isValid())
      {
        uint32_t* target = (uint32_t*)mm.GetAddress();
        Android::ImageDataHandle data = request->mImageData;

        if (data->empty())
        {
          LOGI("Empty data in upload request !\n");
        }

        // This is a line-by-line copy as the two buffers have different strides
        for(int row = 0; row < data->GetHeight(); ++row)
        {
          uint8_t* sourceAddr = data->GetBuffer() + (row * data->GetStride());
          // GraphicBuffer stride is not in bytes!
          uint32_t* destAddr = (uint32_t*)target + (row  * mm.GetStride());
          memcpy(destAddr, sourceAddr, data->GetStride());
        }

        delete request;
      }
      else
      {
        // Keep request if the lock has failed.
        mRequestQueue.push(request);
      }
    }
  }
  else
  {
    LOGI("GraphicBuffer does not exist, storing request for later.\n");
  }
}

void AndroidNativeImage::UploadImage(Android::ImageDataHandle imageData)
{
  Request* request = new Request(imageData);
  mRequestQueue.push(request);
}

void AndroidNativeImage::RequestUpdate()
{
  mAdaptor.RequestUpdateOnce();
}

void AndroidNativeImage::SetPixelFormat(int depth)
{
  return;

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

} // namespace Adaptor

} // namespace internal

} // namespace Dali
