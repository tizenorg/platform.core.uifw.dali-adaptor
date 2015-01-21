/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
 *
 */

// CLASS HEADER
#include <tizen-buffer-image-impl.h>

// INTERNAL INCLUDES
#include <adaptor-impl.h>
#include <gl/egl-factory.h>
#include <gl/egl-image-extensions.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

TizenBufferImage* TizenBufferImage::New( tbm_surface_h* surface )
{
  TizenBufferImage* image = new TizenBufferImage( surface );
  return image;
}

TizenBufferImage::TizenBufferImage( tbm_surface_h* surface )
: mEglImageKHR( NULL )
, mEglImageExtensions( NULL )
, mTbmsurface( surface )
, mWidth( 0 )
, mHeight( 0 )
{
  EglFactory& eglFactory = Adaptor::GetImplementation( Adaptor::Get() ).GetEGLFactory();
  mEglImageExtensions = eglFactory.GetImageExtensions();
}

TizenBufferImage::~TizenBufferImage()
{
}

void TizenBufferImage::SetSurface( tbm_surface_h* surface )
{
  mTbmsurface = surface;
}

bool TizenBufferImage::GlExtensionCreate()
{
  if( mEglImageKHR != NULL )
  {
    GlExtensionDestroy();
  }

  if( mTbmsurface == NULL )
  {
    return false;
  }

  if( mEglImageExtensions == NULL )
  {
    return false;
  }

  mEglImageKHR = mEglImageExtensions->CreateImageKHR( *mTbmsurface );

  return mEglImageKHR != NULL;
}

void TizenBufferImage::GlExtensionDestroy()
{
  if( mEglImageExtensions == NULL )
  {
    return;
  }

  mEglImageExtensions->DestroyImageKHR( mEglImageKHR );

  mEglImageKHR = NULL;
}

unsigned int TizenBufferImage::TargetTexture()
{
  if( mEglImageExtensions == NULL )
  {
    return 0;
  }

  mEglImageExtensions->TargetTextureKHR( mEglImageKHR );

  return 0;
}

void TizenBufferImage::PrepareTexture()
{
}

unsigned int TizenBufferImage::GetWidth() const
{
  if( mTbmsurface == NULL )
  {
    return 0;
  }
  return tbm_surface_get_width( *mTbmsurface );
}

unsigned int TizenBufferImage::GetHeight() const
{
  if( mTbmsurface == NULL )
  {
    return 0;
  }
  return tbm_surface_get_height( *mTbmsurface );
}

Pixel::Format TizenBufferImage::GetPixelFormat() const
{
  DALI_ASSERT_ALWAYS( mTbmsurface != NULL && "TBM surface pointer is NULL" );

  tbm_format format = tbm_surface_get_format( *mTbmsurface );

  switch( format )
  {
    case TBM_FORMAT_RGB565:
      return Pixel::RGB565;
    case TBM_FORMAT_BGR565:
      return Pixel::BGR565;
    case TBM_FORMAT_RGBA4444:
      return Pixel::RGBA4444;
    case TBM_FORMAT_BGRA4444:
      return Pixel::BGRA4444;
    case TBM_FORMAT_RGBA5551:
      return Pixel::RGBA5551;
    case TBM_FORMAT_BGRA5551:
      return Pixel::BGRA5551;
    case TBM_FORMAT_RGB888:
      return Pixel::RGB888;
    case TBM_FORMAT_RGBA8888:
      return Pixel::RGBA8888;
    case TBM_FORMAT_BGRA8888:
      return Pixel::BGRA8888;
    case TBM_FORMAT_C8:
      return Pixel::C8;
    case TBM_FORMAT_RGBX8888:
      return Pixel::RGBX8888;
    case TBM_FORMAT_NV12:
      return Pixel::NV12;
    case TBM_FORMAT_NV21:
      return Pixel::NV21;
    case TBM_FORMAT_YUV420:
      return Pixel::YUV420;
    case TBM_FORMAT_YVU420:
      return Pixel::YVU420;
  }
  DALI_ASSERT_ALWAYS( false && "TBM surface has a format which dali does not support." );
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
