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
#include <dali/integration-api/glyph-set.h>
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
        printf("  Error empty surface when decoding image? (SDL RW Memory ptr=%d) %s. %d\n", (long long)(memory), SDL_GetError(), blobSize);
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
    mGetDefaultFontSizeResult(12.f),
    mSize(10,10),
    mReadGlobalMetricsResult(false),
    mReadMetricsResult(false),
    mReadGlobalMetrics(
      64.f,  // lineHeight;         ///< Distance between baselines
      9.f,  // ascender;           ///< Distance from baseline to top of cell
      2048.f/64.f,  // unitsPerEM;         ///< font units/EM
      0.f,  // underlinePosition;  ///< Underline distance from baseline
      0.f,  // underlineThickness; ///< Underline thickness
      0.f,  // padAdjustX;         ///< X adjustment value for padding around distance field
      0.f,  // padAdjustY;         ///< Y adjustment value for padding around distance field
      0.f,  // maxWidth;           ///< Width of widest glyph
      0.f   // maxHeight;          ///< Height of tallest glyph
      ),
    mFontListMode(LIST_ALL_FONTS)
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
    case Integration::ResourceModel:
    {
    }
    break;
    case Integration::ResourceMesh:
    {
    }
    break;
    case Integration::ResourceText:
    {
      EM_LOG("SdlPlatformAbstraction::LoadResourceSynchronously text");
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
      case Integration::ResourceModel:
      {
        printf("SdlPlatformAbstraction::LoadResource ResourceModel\n");
      }
      break;
      case Integration::ResourceMesh:
      {
        printf("SdlPlatformAbstraction::LoadResource ResourceMesh\n");
      }
      break;
      case Integration::ResourceText:
      {
        EM_LOG("SdlPlatformAbstraction::LoadResource ResourceText");
        printf("SdlPlatformAbstraction::LoadResource ResourceText\n");
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

float SdlPlatformAbstraction::GetDefaultFontSize() const
{
  EM_LOG("SdlPlatformAbstraction::GetDefaultFontSize");
  return mGetDefaultFontSizeResult;
}

PixelSize SdlPlatformAbstraction::GetFontLineHeightFromCapsHeight(const std::string& fontFamily, const std::string& fontStyle, const CapsHeight capsHeight) const
{
  EM_LOG("SdlPlatformAbstraction::GetFontLineHeightFromCapsHeight");
  DALI_ASSERT_ALWAYS("!Not Implemented");
  // LineHeight will be bigger than CapsHeight, so return capsHeight + 1
  return PixelSize(capsHeight + 1);
}

Integration::GlyphSet* SdlPlatformAbstraction::GetGlyphData ( const Integration::TextResourceType& textRequest,
                                                              const std::string& fontFamily,
                                                              bool getBitmapPtr) const
{
  EM_LOG("SdlPlatformAbstraction::GetGlyphData");

  Integration::GlyphSet* set = new Dali::Integration::GlyphSet();
  Integration::BitmapPtr bitmapData;

  std::set<uint32_t> characters;

  for( Integration::TextResourceType::CharacterList::const_iterator it = textRequest.mCharacterList.begin(), endIt = textRequest.mCharacterList.end(); it != endIt; ++it )
  {
    if( characters.find( it->character ) == characters.end() )
    {
      characters.insert( it->character );
      Integration::GlyphMetrics character = {it->character, Integration::GlyphMetrics::LOW_QUALITY,  64.0f,  64.0f, 9.0f, 1.0f, 64.0f, it->xPosition, it->yPosition };

      if( getBitmapPtr )
      {
        // callout to js for glyph data
        bitmapData = Dali::Internal::Emscripten::GetGlyphImage( fontFamily,
                                                                textRequest.mStyle,
                                                                64.f, // @todo what should this be
                                                                it->character );

      }
      set->AddCharacter(bitmapData, character);
    }
  }

  set->mLineHeight = 64.0f;
  set->mAscender = 9.0f;
  set->mUnitsPerEM = 2048.0f/64.0f;

  return set;
}

Integration::GlyphSet* SdlPlatformAbstraction::GetCachedGlyphData( const Integration::TextResourceType& textRequest,
                                                                   const std::string& fontFamily ) const
{
  EM_LOG("SdlPlatformAbstraction::GetCachedGlyphData");
  return GetGlyphData( textRequest, fontFamily, true ); // @todo; we just recreate the data rather than using a cache
}

void SdlPlatformAbstraction::GetGlobalMetrics( const std::string& fontFamily,
                                               const std::string& fontStyle,
                                               Integration::GlobalMetrics& globalMetrics ) const
{
  EM_LOG("SdlPlatformAbstraction::GetGlobalMetrics");

  globalMetrics = mReadGlobalMetrics;

  // globalMetrics.lineHeight = 10.0f;
  // globalMetrics.ascender = 9.0f;
  // globalMetrics.unitsPerEM = 2048.0f/64.0f;
}

std::string SdlPlatformAbstraction::GetFontPath(const std::string& family, bool bold, bool italic) const
{
  EM_LOG("SdlPlatformAbstraction::GetFontPath");
  DALI_ASSERT_ALWAYS("!Not Implemented");
  return mGetFontPathResult;
}

void SdlPlatformAbstraction::SetDpi(unsigned int dpiHorizontal, unsigned int dpiVertical)
{
  DALI_ASSERT_ALWAYS("!Unimplemented");
}

const std::string& SdlPlatformAbstraction::GetFontFamilyForChars(const Integration::TextArray& charsRequested) const
{
  EM_LOG("SdlPlatformAbstraction::GetFontFamilyForChars");
  DALI_ASSERT_ALWAYS("!Not Implemented");
  return mGetDefaultFontFamilyResult;
}

bool SdlPlatformAbstraction::AllGlyphsSupported(const std::string& name, const std::string& fontStyle, const Integration::TextArray& text) const
{
  EM_LOG("SdlPlatformAbstraction::AllGlyphsSupported");
  return true;
}

bool SdlPlatformAbstraction::ValidateFontFamilyName(const std::string& fontFamily, const std::string& fontStyle, bool& isDefaultSystemFont, std::string& closestMatch, std::string& closestStyleMatch) const
{
  EM_LOG("SdlPlatformAbstraction::ValidateFontFamilyName");
  DALI_ASSERT_ALWAYS("!Unimplemented");

  if(fontFamily.empty())
  {
    closestMatch = "Arial";
    isDefaultSystemFont = true;
  }
  else
  {
    closestMatch = fontFamily;     // browser can probably do it all... @todo
  }

  if(fontStyle.empty())
  {
    closestStyleMatch = "normal";
  }
  else
  {
    closestStyleMatch = fontStyle;  //
  }

  return true;
}

void SdlPlatformAbstraction::GetFontList( FontListMode mode, std::vector<std::string>& fontList ) const
{
  EM_LOG("SdlPlatformAbstraction::GetFontList");
  mFontListMode = mode;
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

void SdlPlatformAbstraction::UpdateDefaultsFromDevice()
{
  DALI_ASSERT_ALWAYS("!Unimplemented");
  mGetDefaultFontFamilyResult+=1.0f;
}

Integration::DynamicsFactory* SdlPlatformAbstraction::GetDynamicsFactory()
{
  return NULL;
}

bool SdlPlatformAbstraction::ReadGlobalMetricsFromCache( const std::string& fontFamily,
                                                         const std::string& fontStyle,
                                                         Integration::GlobalMetrics& globalMetrics )
{
  EM_LOG("SdlPlatformAbstraction::ReadGlobalMetricsFromCache");
  globalMetrics = mReadGlobalMetrics; // Want to copy contents...
  return mReadGlobalMetricsResult; // Default false (will be set to true on subsequent write)
}

void SdlPlatformAbstraction::WriteGlobalMetricsToCache( const std::string& fontFamily,
                                                        const std::string& fontStyle,
                                                        const Integration::GlobalMetrics& globalMetrics )
{
  EM_LOG("SdlPlatformAbstraction::WriteGlobalMetricsToCache");

  // Copy so next read uses written values. TODO: Could add method
  // to turn this behaviour off for more extensive testing.
  mReadGlobalMetrics = globalMetrics;
  mReadGlobalMetricsResult = true;
}

bool SdlPlatformAbstraction::ReadMetricsFromCache( const std::string& fontFamily,
                                                   const std::string& fontStyle,
                                                   std::vector<Integration::GlyphMetrics>& glyphMetricsContainer )
{
  EM_LOG("SdlPlatformAbstraction::ReadMetricsFromCache");
  glyphMetricsContainer = mReadMetrics;
  return mReadMetricsResult; // Default false (will be set to true on subsequent write)
}

void SdlPlatformAbstraction::WriteMetricsToCache( const std::string& fontFamily,
                                                  const std::string& fontStyle,
                                                  const Integration::GlyphSet& glyphSet )
{
  EM_LOG("SdlPlatformAbstraction::WriteMetricsToCache");

  // Copy so next read uses written values. TODO: Could add method
  // to turn this behaviour off for more extensive testing.
  const Integration::GlyphSet::CharacterList& charList =  glyphSet.GetCharacterList();
  mReadMetrics.clear();
  for(std::size_t i=0, end=charList.size(); i<end; ++i)
  {
    mReadMetrics.push_back(charList[i].second);
  }
  mReadMetricsResult = true;
}

void SdlPlatformAbstraction::GetFileNamesFromDirectory( const std::string& /*directoryName*/,
                                                        std::vector<std::string>& /*fileNames*/ )
{

}

Integration::BitmapPtr SdlPlatformAbstraction::GetGlyphImage( const std::string& fontFamily, const std::string& fontStyle, float fontSize, uint32_t character ) const
{
  EM_LOG("SdlPlatformAbstraction::GetGlyphImage");
  return Integration::BitmapPtr();
}


void SdlPlatformAbstraction::IncrementGetTimeResult(size_t milliseconds)
{
}

} // Dali
