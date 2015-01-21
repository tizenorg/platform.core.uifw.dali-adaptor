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
  DALI_LOG_ERROR( "TBM surface formats are not supported." );
  return static_cast< Pixel::Format >( -1 );
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
