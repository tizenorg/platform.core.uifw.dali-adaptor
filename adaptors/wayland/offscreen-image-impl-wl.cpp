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
#include "offscreen-image-impl.h"

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <gl/egl-image-extensions.h>
#include <gl/egl-factory.h>
#include <adaptor-impl.h>
#include <render-surface.h>

// Allow this to be encoded and saved:
#include <platform-abstractions/tizen/resource-loader/resource-loader.h>
#include <bitmap-saver.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{
using Dali::Integration::PixelBuffer;

OffscreenImage* OffscreenImage::New(unsigned int width, unsigned int height, Dali::OffscreenImage::ColorDepth depth, Any pixmap )
{
  OffscreenImage* image = new OffscreenImage( width, height, depth, pixmap );
  DALI_ASSERT_DEBUG( image && "OffscreenImage allocation failed." );

  // 2nd phase construction
  if(image) //< Defensive in case we ever compile without exceptions.
  {
    image->Initialize();
  }

  return image;
}

OffscreenImage::OffscreenImage( unsigned int width, unsigned int height, Dali::OffscreenImage::ColorDepth depth, Any pixmap )
: mWidth( width ),
  mHeight( height ),
  mOwnPixmap( true ),
  mColorDepth( depth ),
  mEglImageKHR( NULL ),
  mEglImageExtensions( NULL )
{
  DALI_ASSERT_ALWAYS( Adaptor::IsAvailable() );
  EglFactory& eglFactory = Adaptor::GetImplementation( Adaptor::Get() ).GetEGLFactory();
  mEglImageExtensions = eglFactory.GetImageExtensions();
  SetBlending( mColorDepth );

  DALI_ASSERT_DEBUG( mEglImageExtensions );
}

void OffscreenImage::Initialize()
{
}

OffscreenImage::~OffscreenImage()
{
}

Any OffscreenImage::GetPixmap() const
{
  DALI_ASSERT_ALWAYS( false && "OffscreenImage::GetPixmap() is not supported for Wayland." );
  return Any();
}

bool OffscreenImage::GetPixels(std::vector<unsigned char>& pixbuf, unsigned& width, unsigned& height, Pixel::Format& pixelFormat) const
{
    return false;
}

bool OffscreenImage::EncodeToFile(const std::string& filename) const
{
  std::vector< unsigned char > pixbuf;
  unsigned int width(0), height(0);
  Pixel::Format pixelFormat;

  if(GetPixels(pixbuf, width, height, pixelFormat))
  {
    return Dali::EncodeToFile(&pixbuf[0], filename, pixelFormat, width, height);
  }
  return false;
}

bool OffscreenImage::GlExtensionCreate()
{
    return false;
}

void OffscreenImage::GlExtensionDestroy()
{
  mEglImageExtensions->DestroyImageKHR(mEglImageKHR);

  mEglImageKHR = NULL;
}

unsigned int OffscreenImage::TargetTexture()
{
  mEglImageExtensions->TargetTextureKHR(mEglImageKHR);

  return 0;
}

void OffscreenImage::SetBlending(Dali::OffscreenImage::ColorDepth depth)
{
  switch (depth)
  {
    case Dali::OffscreenImage::COLOR_DEPTH_16: //Pixel::RGB565
    case Dali::OffscreenImage::COLOR_DEPTH_24: // Pixel::RGB888
    {
      mBlendingRequired = false;
      break;
    }
    case Dali::OffscreenImage::COLOR_DEPTH_8: //Pixel::A8
    case Dali::OffscreenImage::COLOR_DEPTH_32: // Pixel::RGBA8888
    {
      mBlendingRequired = true;
      break;
    }
    default:
    {
      DALI_ASSERT_DEBUG(0 && "unknown color enum");
    }
  }
}

void OffscreenImage::GetPixmapDetails()
{
}

} // namespace Adaptor

} // namespace internal

} // namespace Dali
