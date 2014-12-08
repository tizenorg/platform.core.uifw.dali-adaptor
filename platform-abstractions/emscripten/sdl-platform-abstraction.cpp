/*
Copyright (c) 2000-2013 Samsung Electronics Co., Ltd All Rights Reserved

This file is part of Dali

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

#include "sdl-platform-abstraction.h"

// EXTERNAL INCLUDES
#include <set>
#include <stdint.h>
#include <cstring>
#include <SDL_surface.h>
#include <SDL_image.h>
#include "emscripten/emscripten.h"
#include "emscripten-callbacks.h"

#define EM_LOG(x); // EM_ASM( console.log( x ) );


// INTERNAL INCLUDES
#include <dali/integration-api/platform-abstraction.h>
#include <dali/public-api/common/ref-counted-dali-vector.h>

namespace
{

Dali::Integration::BitmapPtr EncodedImage( Dali::RefCountedVector<uint8_t>* encodedBlob )
{
  Dali::Integration::BitmapPtr bitmapPtr = NULL;

  if( encodedBlob != 0 )
  {
    const size_t blobSize     = encodedBlob->GetVector().Size();
    uint8_t * const blobBytes = &(encodedBlob->GetVector()[0]);
    DALI_ASSERT_DEBUG( blobSize > 0U );
    DALI_ASSERT_DEBUG( blobBytes != 0U );

    if( blobBytes != 0 && blobSize > 0U )
    {

      SDL_RWops *memory = SDL_RWFromMem(blobBytes, blobSize);

      if(!memory)
      {
        printf("  Error Null pointer from SDL RW memory?\n");
      }

      SDL_Surface *surface = IMG_Load_RW(memory, 0);

      if(surface)
      {
        bitmapPtr = Dali::Integration::Bitmap::New( Dali::Integration::Bitmap::BITMAP_2D_PACKED_PIXELS,
                                                    Dali::ResourcePolicy::DISCARD ); // DISCARD; Dali manages

        Dali::Integration::Bitmap::PackedPixelsProfile* packedProfile = bitmapPtr->GetPackedPixelsProfile();
        DALI_ASSERT_ALWAYS(packedProfile);

        unsigned char bytesPerPixel = surface->format->BytesPerPixel;

        Dali::Integration::PixelBuffer* pixels = NULL;

        unsigned char targetBytesPerPixel = 3; // bytesPerPixel;

        // How did we get here?
        // sdl in emscripten returns us a 4byteperpixel image regardless of rgb/png etc
        // Theres no apparent way to differentiate an image with an alpha channel
        // In dali if an image has an alpha channel it gets sorted. This introduces odd artifacts on rotation
        // as the sorting algorithm gets it wrong (presumes front on view)
        // So here well just support pngs with an alpha channel.
        // We're poking around in the format as emscripten lacks a file memory api where we could use
        // Dali's machinery to read the format.
        unsigned char *pBytes = blobBytes;

        if( 0x89 == *(pBytes+0) && 0x50 == *(pBytes+1) ) // magic bytes for png_all_filters
        {
          pBytes+=8; // 8 bytes for header
          pBytes+=4; // 4 bytes for chunk length
          pBytes+=4; // 4 bytes for chunk type
          // ihdr data (must be first chunk)
          pBytes+=4; // 4 for width,height
          pBytes+=4;
          pBytes+=1; // 1 for bit depth
          unsigned char ihdr_colorType= *pBytes;    // 1 for bit colorType
          if( (4 == ihdr_colorType ||                    // 4 is 8,16 bit depth with alpha LA
               6 == ihdr_colorType) )                    // 6 is 8,16 bit depth with alpha RGBA
          {
            targetBytesPerPixel = 4;
          }
        }

        if(3 == targetBytesPerPixel)
        {
          pixels = packedProfile->ReserveBuffer(Dali::Pixel::RGB888,
                                                surface->w, surface->h,
                                                surface->w, surface->h);
        }
        else if(4 == targetBytesPerPixel)
        {
          pixels = packedProfile->ReserveBuffer(Dali::Pixel::RGBA8888,
                                                surface->w, surface->h,
                                                surface->w, surface->h);
        }
        else
        {
          DALI_ASSERT_ALWAYS(0 && "bad bytes per pixel");
        }

        unsigned char* fromPtr = static_cast<unsigned char*>(surface->pixels);

        int stride = surface->pitch;
        int index = 0;
        for(int h = 0; h < surface->h; ++h)
        {
          for(int w = 0; w < (surface->w*bytesPerPixel); w+=bytesPerPixel)
          {
            for(int j = 0; j < targetBytesPerPixel; ++j)
            {
              pixels[ index++ ] = *( (fromPtr + (h * stride) ) + w + j );
            }
          }
        }
      } // if surface
      else
      {
        printf("  Error empty surface when decoding image? (SDL RW Memory ptr=%llx) %s. %d\n", (long long)(memory), SDL_GetError(), blobSize);
      }

    } // if blobSize
    else
    {
      printf(" Error No bytes in image?\n");
    }

  } // if encodedBlob
  else
  {
    printf("  Error Null pointer given for decoding image?\n");
  }

  return bitmapPtr;
}

} // anon namespace

namespace Dali
{

SdlPlatformAbstraction::SdlPlatformAbstraction()
  :
    mSize(10,10)
{
  // Initialize();
}


SdlPlatformAbstraction::~SdlPlatformAbstraction()
{
}

void SdlPlatformAbstraction::GetTimeMicroseconds(unsigned int &seconds, unsigned int &microSeconds)
{
  double current = EM_ASM_DOUBLE_V({ return new Date().getTime(); }); // getTime() in ms

  seconds         = static_cast<unsigned int>(current/1000.0);
  microSeconds    = (static_cast<unsigned int>(current) - seconds*1000.0) * 1000;
}

void SdlPlatformAbstraction::Suspend()
{
  DALI_ASSERT_ALWAYS("!Not Implemented");
}

void SdlPlatformAbstraction::Resume()
{
  DALI_ASSERT_ALWAYS("!Not Implemented");
}

void SdlPlatformAbstraction::LoadImageMetadata(const std::string filename, Vector2 &size)
{
  Dali::Internal::Emscripten::LoadImageMetadata(filename, size);
}

void SdlPlatformAbstraction::GetClosestImageSize( const std::string& filename,
                                                  const ImageAttributes& /*attributes*/,
                                                  Vector2& closestSize )
{
  // @todo
  Dali::Internal::Emscripten::LoadImageMetadata(filename, closestSize);
}

void SdlPlatformAbstraction::GetClosestImageSize( Integration::ResourcePointer resourceBuffer,
                                                  const ImageAttributes& attributes,
                                                  Vector2& closestSize )
{
  // @todo
  // LoadImageMetadata(filename, closestSize);
}

Integration::ResourcePointer SdlPlatformAbstraction::LoadResourceSynchronously( const Integration::ResourceType& resourceType, const std::string& resourcePath )
{
  Integration::ResourcePointer ret;

  switch(resourceType.id)
  {
    case Integration::ResourceBitmap:
    {
      const Integration::BitmapResourceType& bitmapResource( static_cast<const Integration::BitmapResourceType&>(resourceType) );

      Integration::BitmapPtr bitmapPtr = Dali::Internal::Emscripten::GetImage(
        bitmapResource.imageAttributes,
        resourcePath );

      ret = bitmapPtr;
    }
    break;
    case Integration::ResourceNativeImage:
    {
    }
    break;
    case Integration::ResourceTargetImage:
    {
    }
    break;
    case Integration::ResourceShader:
    {
    }
    break;
    case Integration::ResourceMesh:
    {
    }
    break;
  } // switch(resourceType->id)

  return ret;
}

void SdlPlatformAbstraction::LoadResource(const Integration::ResourceRequest& request)
{
  std::string path = request.GetPath();

  Integration::ResourceType *type            = request.GetType();
  Integration::ResourceId   resourceId       = request.GetId();
  Integration::ResourcePointer resourcePtr   = request.GetResource();

  if( type )
  {
    switch(type->id)
    {
      case Integration::ResourceBitmap:
      {
        Integration::BitmapPtr bitmapPtr = NULL;

        if( NULL == request.GetResource().Get()  )
        {
          const Integration::BitmapResourceType& bitmapResource( static_cast<const Integration::BitmapResourceType&>(*type) );

          Integration::BitmapPtr bitmapPtr = Dali::Internal::Emscripten::GetImage(
            bitmapResource.imageAttributes, path );
        }
        else
        {
          // 2) load it (usually on worker thread)
          // DALI_LOG_TRACE_METHOD( mLogFilter );
          // DALI_LOG_INFO(mLogFilter, Debug::Verbose, "%s(%s)\n", __FUNCTION__, request.GetPath().c_str());

          bitmapPtr = EncodedImage( reinterpret_cast<Dali::RefCountedVector<uint8_t>*>( request.GetResource().Get() ) );
        }

        if( bitmapPtr )
        {
          mResourceQueue.push( ResourceIdBitmapPair( resourceId, bitmapPtr ) );
        }

      }
      break;
      case Integration::ResourceNativeImage:
      {
        printf("SdlPlatformAbstraction::LoadResource ResourceNativeImage\n");
      }
      break;
      case Integration::ResourceTargetImage:
      {
        printf("SdlPlatformAbstraction::LoadResource ResourceTargetImage\n");
      }
      break;
      case Integration::ResourceShader:
      {
        printf("SdlPlatformAbstraction::LoadResource ResourceShader\n");
      }
      break;
      case Integration::ResourceMesh:
      {
        printf("SdlPlatformAbstraction::LoadResource ResourceMesh\n");
      }
      break;
    } // switch(id)

  } // if(type)

}

void SdlPlatformAbstraction::SaveResource(const Integration::ResourceRequest& request)
{
  DALI_ASSERT_ALWAYS("!Not Implemented");
}

void SdlPlatformAbstraction::CancelLoad(Integration::ResourceId id, Integration::ResourceTypeId typeId)
{
  DALI_ASSERT_ALWAYS("!Not Implemented");
}

void SdlPlatformAbstraction::GetResources(Integration::ResourceCache& cache)
{
  while( !mResourceQueue.empty() )
  {
    Integration::ResourceId    resourceId  = mResourceQueue.front().first;
    Integration::BitmapPtr     bitmapPtr   = mResourceQueue.front().second;

    cache.LoadResponse( resourceId,
                        Integration::ResourceBitmap,
                        bitmapPtr,
                        Integration::RESOURCE_COMPLETELY_LOADED );
    mResourceQueue.pop();
  }
}

bool SdlPlatformAbstraction::IsLoading()
{
  EM_LOG("SdlPlatformAbstraction::IsLoading");
  return false;
}

const std::string& SdlPlatformAbstraction::GetDefaultFontFamily() const
{
  EM_LOG("SdlPlatformAbstraction::GetDefaultFontFamily");
  DALI_ASSERT_ALWAYS("!Not Implemented");
  return mGetDefaultFontFamilyResult;
}

int SdlPlatformAbstraction::GetDefaultFontSize() const
{
  EM_LOG("SdlPlatformAbstraction::GetDefaultFontSize");
  return 12;
}

bool SdlPlatformAbstraction::LoadFile( const std::string& filename, std::vector< unsigned char >& buffer ) const
{
  EM_LOG("SdlPlatformAbstraction::LoadFile");
  return false;
}

bool SdlPlatformAbstraction::SaveFile(const std::string& filename, std::vector< unsigned char >& buffer) const
{
  EM_LOG("SdlPlatformAbstraction::SaveFile");

  DALI_ASSERT_ALWAYS("!Unimplemented");
  return false;
}

void SdlPlatformAbstraction::JoinLoaderThreads()
{
  DALI_ASSERT_ALWAYS("!Unimplemented");
}

void SdlPlatformAbstraction::GetDefaultFontDescription( std::string& // family
                                                        , std::string& // style
  ) const
{
}

bool SdlPlatformAbstraction::LoadShaderBinFile( const std::string& filename, std::vector< unsigned char >& buffer ) const
{
  return false;
}


void SdlPlatformAbstraction::UpdateDefaultsFromDevice()
{
  DALI_ASSERT_ALWAYS("!Unimplemented");
  mGetDefaultFontFamilyResult+=1.0f;
}

Integration::DynamicsFactory* SdlPlatformAbstraction::GetDynamicsFactory()
{
  return NULL;
}

void SdlPlatformAbstraction::IncrementGetTimeResult(size_t milliseconds)
{
}

} // Dali
