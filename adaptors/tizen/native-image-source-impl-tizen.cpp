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
#include "native-image-source-impl.h"

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <cstring>

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

namespace
{
const char* FRAGMENT_PREFIX = "#extension GL_OES_EGL_image_external:require\n";
const char* SAMPLER_TYPE = "samplerExternalOES";
}

using Dali::Integration::PixelBuffer;

NativeImageSource* NativeImageSource::New(unsigned int width, unsigned int height, Dali::NativeImageSource::ColorDepth depth, Any nativeImageSource )
{
  NativeImageSource* image = new NativeImageSource( width, height, depth, nativeImageSource );
  DALI_ASSERT_DEBUG( image && "NativeImageSource allocation failed." );

  if( image )
  {
    image->Initialize();
  }

  return image;
}

NativeImageSource::NativeImageSource( unsigned int width, unsigned int height, Dali::NativeImageSource::ColorDepth depth, Any nativeImageSource )
: mWidth( width ),
  mHeight( height ),
  mOwnTbmsurface( false ),
  mTbmsurface( NULL ),
  mBlendingRequired( false ),
  mColorDepth( depth ),
  mEglImageKHR( NULL ),
  mEglImageExtensions( NULL ),
  mSetSource( false )
{
  DALI_ASSERT_ALWAYS( Adaptor::IsAvailable() );
  EglFactory& eglFactory = Adaptor::GetImplementation( Adaptor::Get() ).GetEGLFactory();
  mEglImageExtensions = eglFactory.GetImageExtensions();
  DALI_ASSERT_DEBUG( mEglImageExtensions );

  mTbmsurface = GetSurfaceFromAny( nativeImageSource );

  if( mTbmsurface != NULL )
  {
    mWidth = tbm_surface_get_width( mTbmsurface );
    mHeight = tbm_surface_get_height( mTbmsurface );
  }
}

void NativeImageSource::Initialize()
{
  if( mTbmsurface != NULL || mWidth == 0 || mHeight == 0 )
  {
    return;
  }

  tbm_format format = TBM_FORMAT_RGB888;
  int depth = 0;

  switch( mColorDepth )
  {
    case Dali::NativeImageSource::COLOR_DEPTH_DEFAULT:
    {
      format = TBM_FORMAT_RGBA8888;
      depth = 32;
      break;
    }
    case Dali::NativeImageSource::COLOR_DEPTH_8:
    {
      format = TBM_FORMAT_C8;
      depth = 8;
      break;
    }
    case Dali::NativeImageSource::COLOR_DEPTH_16:
    {
      format = TBM_FORMAT_RGB565;
      depth = 16;
      break;
    }
    case Dali::NativeImageSource::COLOR_DEPTH_24:
    {
      format = TBM_FORMAT_RGB888;
      depth = 24;
      break;
    }
    case Dali::NativeImageSource::COLOR_DEPTH_32:
    {
      format = TBM_FORMAT_RGBA8888;
      depth = 32;
      break;
    }
    default:
    {
      DALI_LOG_WARNING( "Wrong color depth." );
      return;
    }
  }

  // set whether blending is required according to pixel format based on the depth
  /* default pixel format is RGB888
     If depth = 8, Pixel::A8;
     If depth = 16, Pixel::RGB565;
     If depth = 32, Pixel::RGBA8888 */
  mBlendingRequired = ( depth == 32 || depth == 8 );

  mTbmsurface = tbm_surface_create( mWidth, mHeight, format );
  mOwnTbmsurface = true;
}

tbm_surface_h NativeImageSource::GetSurfaceFromAny( Any source ) const
{
  if( source.Empty() )
  {
    return NULL;
  }

  if( source.GetType() == typeid( tbm_surface_h ) )
  {
    return AnyCast< tbm_surface_h >( source );
  }
  else
  {
    return NULL;
  }
}

NativeImageSource::~NativeImageSource()
{
  if( mOwnTbmsurface && mTbmsurface != NULL )
  {
    if( tbm_surface_destroy( mTbmsurface ) != TBM_SURFACE_ERROR_NONE )
    {
      DALI_LOG_ERROR( "Failed to destroy tbm_surface" );
    }
  }
}

Any NativeImageSource::GetNativeImageSource() const
{
  return Any( mTbmsurface );
}

bool NativeImageSource::GetPixels(std::vector<unsigned char>& pixbuf, unsigned& width, unsigned& height, Pixel::Format& pixelFormat) const
{
  if( mTbmsurface != NULL )
  {
    tbm_surface_info_s surface_info;

    if( tbm_surface_map( mTbmsurface, TBM_SURF_OPTION_READ, &surface_info) != TBM_SURFACE_ERROR_NONE )
    {
      DALI_LOG_ERROR( "Fail to map tbm_surface" );

      width = 0;
      height = 0;

      return false;
    }

    tbm_format format = surface_info.format;
    uint32_t stride = surface_info.planes[0].stride;
    unsigned char* ptr = surface_info.planes[0].ptr;

    width = mWidth;
    height = mHeight;
    size_t lineSize;
    size_t offset;
    size_t cOffset;

    switch( format )
    {
      case TBM_FORMAT_RGB888:
      {
        lineSize = width*3;
        pixelFormat = Pixel::RGB888;
        pixbuf.resize( lineSize*height );
        unsigned char* bufptr = &pixbuf[0];

        for( unsigned int r = 0; r < height; ++r, bufptr += lineSize )
        {
          for( unsigned int c = 0; c < width; ++c )
          {
            cOffset = c*3;
            offset = cOffset + r*stride;
            *(bufptr) = ptr[offset+2];
            *(bufptr+cOffset+1) = ptr[offset+1];
            *(bufptr+cOffset+2) = ptr[offset];
          }
        }
        break;
      }
      case TBM_FORMAT_RGBA8888:
      {
        lineSize = width*4;
        pixelFormat = Pixel::RGBA8888;
        pixbuf.resize( lineSize*height );
        unsigned char* bufptr = &pixbuf[0];

        for( unsigned int r = 0; r < height; ++r, bufptr += lineSize )
        {
          for( unsigned int c = 0; c < width; ++c )
          {
            cOffset = c*4;
            offset = cOffset + r*stride;
            *(bufptr) = ptr[offset+3];
            *(bufptr+cOffset+1) = ptr[offset+2];
            *(bufptr+cOffset+2) = ptr[offset+1];
            *(bufptr+cOffset+3) = ptr[offset];
          }
        }
        break;
      }
      default:
      {
        DALI_LOG_WARNING( "Tbm surface has unsupported pixel format." );

        pixbuf.resize( 0 );
        width = 0;
        height = 0;

        return false;
      }
    }

    if( tbm_surface_unmap( mTbmsurface ) != TBM_SURFACE_ERROR_NONE )
    {
      DALI_LOG_ERROR( "Fail to unmap tbm_surface" );
    }

    return true;
  }

  DALI_LOG_WARNING( "TBM surface does not exist." );

  width = 0;
  height = 0;

  return false;
}

bool NativeImageSource::EncodeToFile(const std::string& filename) const
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

void NativeImageSource::SetSource( Any source )
{
  if( mOwnTbmsurface && mTbmsurface != NULL )
  {
    if( tbm_surface_destroy( mTbmsurface ) != TBM_SURFACE_ERROR_NONE )
    {
      DALI_LOG_ERROR( "Failed to destroy tbm_surface" );
    }

    mTbmsurface = NULL;
    mOwnTbmsurface = false;
  }

  mTbmsurface = GetSurfaceFromAny( source );
  mSetSource = true;

  if( mTbmsurface != NULL )
  {
    mWidth = tbm_surface_get_width( mTbmsurface );
    mHeight = tbm_surface_get_height( mTbmsurface );
  }

  if( mEglImageKHRContainer.Size() > 2 )
  {
    mEglImageExtensions->DestroyImageKHR(mEglImageKHRContainer[0]);
    mEglImageKHRContainer.Erase( mEglImageKHRContainer.Begin() );
  }

  mEglImageKHRContainer.PushBack( mEglImageKHR );
}

bool NativeImageSource::IsColorDepthSupported( Dali::NativeImageSource::ColorDepth colorDepth )
{
  uint32_t* formats;
  uint32_t formatNum;
  tbm_format format = TBM_FORMAT_RGB888;

  switch( colorDepth )
  {
    case Dali::NativeImageSource::COLOR_DEPTH_DEFAULT:
    {
      format = TBM_FORMAT_RGBA8888;
      break;
    }
    case Dali::NativeImageSource::COLOR_DEPTH_8:
    {
      format = TBM_FORMAT_C8;
      break;
    }
    case Dali::NativeImageSource::COLOR_DEPTH_16:
    {
      format = TBM_FORMAT_RGB565;
      break;
    }
    case Dali::NativeImageSource::COLOR_DEPTH_24:
    {
      format = TBM_FORMAT_RGB888;
      break;
    }
    case Dali::NativeImageSource::COLOR_DEPTH_32:
    {
      format = TBM_FORMAT_RGBA8888;
      break;
    }
  }

  if( tbm_surface_query_formats( &formats, &formatNum ) )
  {
    for( unsigned int i = 0; i < formatNum; i++ )
    {
      if( formats[i] == format )
      {
        free( formats );
        return true;
      }
    }
  }

  free( formats );
  return false;
}

bool NativeImageSource::GlExtensionCreate()
{
  // casting from an unsigned int to a void *, which should then be cast back
  // to an unsigned int in the driver.
  EGLClientBuffer eglBuffer = reinterpret_cast< EGLClientBuffer > (mTbmsurface);

  mEglImageKHR = mEglImageExtensions->CreateImageKHR( eglBuffer );

  return mEglImageKHR != NULL;
}

void NativeImageSource::GlExtensionDestroy()
{
  mEglImageExtensions->DestroyImageKHR(mEglImageKHR);

  mEglImageKHR = NULL;
}

unsigned int NativeImageSource::TargetTexture()
{
  mEglImageExtensions->TargetTextureKHR(mEglImageKHR);

  return 0;
}

void NativeImageSource::PrepareTexture()
{
  if( mSetSource && GlExtensionCreate() )
  {
    TargetTexture();
    mSetSource = false;
  }
}

int NativeImageSource::GetPixelDepth(Dali::NativeImageSource::ColorDepth depth) const
{
  switch (depth)
  {
    case Dali::NativeImageSource::COLOR_DEPTH_DEFAULT:
    {
      // ToDo: Get the default screen depth
      return 32;
    }
    case Dali::NativeImageSource::COLOR_DEPTH_8:
    {
      return 8;
    }
    case Dali::NativeImageSource::COLOR_DEPTH_16:
    {
      return 16;
    }
    case Dali::NativeImageSource::COLOR_DEPTH_24:
    {
      return 24;
    }
    case Dali::NativeImageSource::COLOR_DEPTH_32:
    {
      return 32;
    }
    default:
    {
      DALI_ASSERT_DEBUG(0 && "unknown color enum");
      return 0;
    }
  }
}

const char* NativeImageSource::GetCustomFragmentPreFix()
{
  return FRAGMENT_PREFIX;
}

const char* NativeImageSource::GetCustomSamplerTypename()
{
  return SAMPLER_TYPE;
}

} // namespace Adaptor

} // namespace internal

} // namespace Dali
