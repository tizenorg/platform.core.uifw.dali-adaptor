
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

// For glEGLImageTargetTexture2DOES
#define GL_GLEXT_PROTOTYPES

// For eglCreateImageKHR etc.
#define EGL_EGLEXT_PROTOTYPES

// CLASS HEADER
#include "egl-buffer-impl.h"

// EXTERNAL INCLUDES

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <EGL/eglplatform.h>

#include <dali/public-api/math/math-utils.h> // NextPowerOfTwo
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <common/adaptor-impl.h>
#include <common/gl/gl-implementation.h>
#include <android/log.h>


namespace Dali
{

namespace Internal
{

namespace Adaptor
{

EglBuffer* EglBuffer::New(unsigned int width, unsigned int height, Dali::EglBuffer::ColorDepth depth, Dali::Adaptor& adaptor,  IEglBufferRenderer* renderer )
{
  EglBuffer* buffer = new EglBuffer(width, height, depth, adaptor, renderer);

  // 2nd phase construction
  buffer->Initialize();

  return buffer;
}

EglBuffer::EglBuffer(unsigned int width, unsigned int height, Dali::EglBuffer::ColorDepth depth, Dali::Adaptor& adaptor, IEglBufferRenderer* renderer)
: mAdaptor( Internal::Adaptor::Adaptor::GetImplementation(adaptor) ),
  mGlInterface( NULL ),
  mEglImageExtensions( NULL ),
  mEglImageKHR( NULL ),
  mRenderer(renderer),
  mWidth(width),
  mHeight(height),
  mColorDepth(depth)
{
  mGlInterface = &(mAdaptor.GetGlAbstraction());
  EglFactory& eglFactory = mAdaptor.GetEGLFactory();
  mEglImageExtensions = eglFactory.GetImageExtensions();
}

void EglBuffer::Initialize()
{
}

EglBuffer::~EglBuffer()
{
  mRenderer = NULL;

  GlExtensionDestroy();
}

bool EglBuffer::GlExtensionCreate()
{
  bool createdImage = false;

  if( NULL != mRenderer && mWidth != 0 && mHeight != 0 )
  {
    EGLClientBuffer buffer;
    mRenderer->CreateTexture( mWidth, mHeight, buffer );
    createdImage = true;

    EGLint eglImgAttrs[] = { EGL_IMAGE_PRESERVED_KHR, EGL_TRUE, EGL_NONE, EGL_NONE };

    mEglImageKHR = eglCreateImageKHR( eglGetDisplay(EGL_DEFAULT_DISPLAY), EGL_NO_CONTEXT,
                                      EGL_NATIVE_BUFFER_ANDROID,
                                      buffer,
                                      eglImgAttrs);
  }
  return createdImage;
}

void EglBuffer::GlExtensionDestroy()
{
  if( mEglImageKHR != NULL )
  {
    eglDestroyImageKHR(eglGetDisplay(EGL_DEFAULT_DISPLAY), mEglImageKHR);
    mEglImageKHR = NULL;

    if( NULL != mRenderer )
    {
      mRenderer->DestroyTexture();
    }
  }
}

unsigned int EglBuffer::TargetTexture()
{
  if( mEglImageKHR != NULL )
  {
    glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, mEglImageKHR);
  }

  return 0;
}

void EglBuffer::PrepareTexture()
{
  if( mEglImageKHR )
  {
    if( NULL != mRenderer )
    {
      mRenderer->UpdateTexture( mEglImageKHR );
    }

    glEGLImageTargetTexture2DOES( GL_TEXTURE_2D, mEglImageKHR );
  }
}

void EglBuffer::Resize( const Vector2& newSize )
{
  bool sizeChanged = (newSize.width != mWidth || newSize.height != mHeight);
  mWidth = newSize.width;
  mHeight = newSize.height;

  if( sizeChanged )
  {
    GlExtensionDestroy();
    GlExtensionCreate();
  }
}

bool EglBuffer::IsReady()
{
  return true;
}

void EglBuffer::GlContextCreated()
{
}

void EglBuffer::GlContextDestroyed()
{
  GlExtensionDestroy();
}

void EglBuffer::TextureCreated(unsigned textureId)
{
  // Do nothing - we don't need the GL texture id
}

int EglBuffer::GetPixelDepth(Dali::EglBuffer::ColorDepth depth) const
{
  switch (depth)
  {
    case Dali::EglBuffer::COLOR_DEPTH_8: return 8;
    case Dali::EglBuffer::COLOR_DEPTH_16: return 16;
    case Dali::EglBuffer::COLOR_DEPTH_24: return 24;
    case Dali::EglBuffer::COLOR_DEPTH_DEFAULT:
    case Dali::EglBuffer::COLOR_DEPTH_32: return 32;
    default: DALI_ASSERT_DEBUG(0 && "unknown color enum"); return 0;
  }
}

void EglBuffer::ClearRenderer()
{
  mRenderer = NULL;
}

bool EglBuffer::RequiresBlending() const
{
  return GetPixelDepth(mColorDepth) == 32;
}



} // namespace Adaptor

} // namespace internal

} // namespace Dali
