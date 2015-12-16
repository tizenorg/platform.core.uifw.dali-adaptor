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
#include <platform-abstractions/tizen/tizen-platform-abstraction.h>

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

const char* OES_FRAGMENT_SHADER(
"#extension GL_OES_EGL_image_external:require\n"
"  \n"
"#ifdef GL_FRAGMENT_PRECISION_HIGH\n"
"  precision highp float;\n"
"#else\n"
" precision mediump float;\n"
"#endif\n"
"  uniform samplerExternalOES seTexture;\n"
"  uniform vec4 uColor;\n"
"  varying vec2 vTexCoord;\n"
"  \n"
"  void main()\n"
"  {\n"
"    gl_FragColor = texture2D( seTexture, vTexCoord.xy ) * uColor;\n"
"  }\n"
);

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
  mOwnTbmsurface( true ),
  mTbmsurface( NULL ),
  mBlendingRequired( false ),
  mColorDepth( depth ),
  mEglImageKHR( NULL ),
  mEglImageExtensions( NULL )
{
  DALI_ASSERT_ALWAYS( Adaptor::IsAvailable() );
  EglFactory& eglFactory = Adaptor::GetImplementation( Adaptor::Get() ).GetEGLFactory();
  mEglImageExtensions = eglFactory.GetImageExtensions();
  DALI_ASSERT_DEBUG( mEglImageExtensions );

  Dali::Integration::PlatformAbstraction& platform = Adaptor::GetImplementation( Adaptor::Get() ).GetPlatformAbstractionInterface();
  Dali::TizenPlatform::TizenPlatformAbstraction* tizenPlatform = dynamic_cast< Dali::TizenPlatform::TizenPlatformAbstraction* >( &platform );

  if( tizenPlatform != NULL )
  {
    tizenPlatform->SetNativeSourceFragmentShaderCode( OES_FRAGMENT_SHADER );
  }

  mTbmsurface = GetSurfaceFromAny( nativeImageSource );

  if( mTbmsurface != NULL )
  {
    mWidth = tbm_surface_get_width( mTbmsurface );
    mHeight = tbm_surface_get_height( mTbmsurface );
  }
}

void NativeImageSource::Initialize()
{
  if( mTbmsurface != NULL )
  {
    mOwnTbmsurface = false;
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

    switch( format )
    {
      case TBM_FORMAT_RGB888:
      {
        pixelFormat = Pixel::RGB888;
        pixbuf.resize( width*height*3 );
        unsigned char* bufptr = &pixbuf[0];

        for( unsigned int r = 0; r < height; ++r )
        {
          memcpy( &bufptr[r*width*3], &ptr[r*stride], width*3 );
        }
        break;
      }
      case TBM_FORMAT_BGRA8888:
      {
        pixelFormat = Pixel::BGRA8888;
        pixbuf.resize( width*height*4 );
        unsigned char* bufptr = &pixbuf[0];

        for( unsigned int r = 0; r < height; ++r )
        {
          memcpy( &bufptr[r*width*4], &ptr[r*stride], width*4 );
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

void NativeImageSource::SetNativeImageSource( Any nativeImageSource )
{
  if( mOwnTbmsurface && mTbmsurface != NULL )
  {
    if( tbm_surface_destroy( mTbmsurface ) != TBM_SURFACE_ERROR_NONE )
    {
      DALI_LOG_ERROR( "Failed to destroy tbm_surface" );
    }

    mTbmsurface = NULL;
  }

  mTbmsurface = GetSurfaceFromAny( nativeImageSource );
  mOwnTbmsurface = false;

  if( mTbmsurface != NULL )
  {
    mWidth = tbm_surface_get_width( mTbmsurface );
    mHeight = tbm_surface_get_height( mTbmsurface );
  }
}

bool NativeImageSource::GlExtensionCreate()
{
  if( mEglImageKHR != NULL )
  {
    GlExtensionDestroy();
  }

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

int NativeImageSource::GetPixelDepth(Dali::NativeImageSource::ColorDepth depth) const
{
  switch (depth)
  {
    case Dali::NativeImageSource::COLOR_DEPTH_DEFAULT:
    {
      // ToDo Get the default screen depth
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

} // namespace Adaptor

} // namespace internal

} // namespace Dali
