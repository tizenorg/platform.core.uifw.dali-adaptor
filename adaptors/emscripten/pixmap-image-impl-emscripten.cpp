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
#include "pixmap-image-impl.h"

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <render-surface.h>

// INTERNAL INCLUDES
#include <gl/egl-image-extensions.h>
#include <gl/egl-factory.h>
#include <adaptor-impl.h>
#include <bitmap-saver.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{
using Dali::Integration::PixelBuffer;

// Pieces needed to save compressed images (temporary location while plumbing):
namespace
{
  // int GetPixelDepth(Dali::PixmapImage::ColorDepth depth)
  // {
  //   switch (depth)
  //   {
  //     case Dali::PixmapImage::COLOR_DEPTH_8:
  //     {
  //       return 8;
  //     }
  //     case Dali::PixmapImage::COLOR_DEPTH_16:
  //     {
  //       return 16;
  //     }
  //     case Dali::PixmapImage::COLOR_DEPTH_24:
  //     {
  //       return 24;
  //     }
  //     case Dali::PixmapImage::COLOR_DEPTH_32:
  //     {
  //       return 32;
  //     }
  //     default:
  //     {
  //       DALI_ASSERT_DEBUG(0 && "unknown color enum");
  //       return 0;
  //     }
  //   }
  // }

  // Pixel::Format SetPixelFormat(int depth)
  // {
  //   Pixel::Format ret = Pixel::RGB8888;

  //   // store the pixel format based on the depth
  //   switch (depth)
  //   {
  //     case 8:
  //     {
  //       ret = Pixel::A8;
  //       break;
  //     }
  //     case 16:
  //     {
  //       ret = Pixel::RGB565;
  //       break;
  //     }
  //     case 32:
  //     {
  //       ret = Pixel::RGBA8888;
  //       break;
  //     }
  //     case 24:
  //     default:
  //     {
  //       ret = Pixel::RGB888;
  //       break;
  //     }
  //   }

  //   return ret;
  // }

  void* GetPixmapFromAny(Any pixmap)
  {
    if (pixmap.Empty())
    {
      return 0;
    }

    return AnyCast<void*>(pixmap);
  }

}

PixmapImage* PixmapImage::New(unsigned int width, unsigned int height, Dali::PixmapImage::ColorDepth depth, Dali::Adaptor& adaptor,  Any pixmap )
{
  PixmapImage* image = new PixmapImage( width, height, depth, adaptor, pixmap );
  DALI_ASSERT_DEBUG( image && "PixmapImage allocation failed." );

  return image;
}

PixmapImage::PixmapImage(unsigned int width, unsigned int height, Dali::PixmapImage::ColorDepth depth, Dali::Adaptor& adaptor, Any pixmap)
: mWidth(width),
  mHeight(height),
  // mOwnPixmap(true),
  mPixelFormat(Pixel::RGB888),
  mAdaptor(Internal::Adaptor::Adaptor::GetImplementation(adaptor)),
  mPixmap(NULL)
  // mColorDepth(depth),
{
  // assign the pixmap
  mPixmap = GetPixmapFromAny(pixmap);
}

// void PixmapImage::Initialize()
// {
// }

PixmapImage::~PixmapImage()
{
  // Lost the opportunity to call GlExtensionDestroy() if Adaptor is destroyed first
  if( Adaptor::IsAvailable() )
  {
    // GlExtensionDestroy() called from GLCleanup on the render thread. Checking this is done here.
    // (mEglImageKHR is now read/written from different threads although ref counted destruction
    //  should mean this isnt concurrent)
    DALI_ASSERT_ALWAYS( NULL == mEglImageKHR && "NativeImage GL resources have not been properly cleaned up" );
  }
}

Any PixmapImage::GetPixmap(Dali::PixmapImage::PixmapAPI api) const
{
  return Any(mPixmap);
}

Any PixmapImage::GetDisplay() const
{
  // return ecore x11 type
  return Any();
}

bool PixmapImage::GetPixels(std::vector<unsigned char>& pixbuf, unsigned& width, unsigned& height, Pixel::Format& pixelFormat) const
{
  DALI_ASSERT_DEBUG(sizeof(unsigned) == 4);
  bool success = false;
  width  = mWidth;
  height = mHeight;

  if(!success)
  {
    DALI_LOG_ERROR("Failed to get pixels from PixmapImage.");
    pixbuf.resize(0);
    width = 0;
    height = 0;
  }
  return success;
}

bool PixmapImage::EncodeToFile(const std::string& filename) const
{
  // std::vector< unsigned char > pixbuf;
  // unsigned int width(0), height(0);
  // Pixel::Format pixelFormat;

  return false;
}

bool PixmapImage::GlExtensionCreate()
{
  // if the image existed previously delete it.
  if (mEglImageKHR != NULL)
  {
    GlExtensionDestroy();
  }

  EglImageExtensions* eglImageExtensions = GetEglImageExtensions();

  // casting from an unsigned int to a void *, which should then be cast back
  // to an unsigned int in the driver.
  EGLClientBuffer eglBuffer = reinterpret_cast< EGLClientBuffer > (mPixmap);

  mEglImageKHR = eglImageExtensions->CreateImageKHR( eglBuffer );

  return mEglImageKHR != NULL;
}

void PixmapImage::GlExtensionDestroy()
{
  EglImageExtensions* eglImageExtensions = GetEglImageExtensions();

  eglImageExtensions->DestroyImageKHR(mEglImageKHR);

  mEglImageKHR = NULL;
}

unsigned int PixmapImage::TargetTexture()
{
  EglImageExtensions* eglImageExtensions = GetEglImageExtensions();

  eglImageExtensions->TargetTextureKHR(mEglImageKHR);

  return 0;
}


// void PixmapImage::GetPixmapDetails()
// {
// }

EglImageExtensions* PixmapImage::GetEglImageExtensions() const
{
  EglFactory& factory = mAdaptor.GetEGLFactory();
  EglImageExtensions* egl = factory.GetImageExtensions();
  DALI_ASSERT_DEBUG( egl && "EGL Image Extensions not initialized" );
  return egl;
}

} // namespace Adaptor

} // namespace internal

} // namespace Dali
