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

// INTERNAL INCLUDES
#include <dali/integration-api/platform-abstraction.h>


#include <dali/integration-api/glyph-set.h>
// #include <test-trace-call-stack.h>
#include <SDL_surface.h>
#include <SDL_image.h>

#include <dali/public-api/common/ref-counted-dali-vector.h>
// #include "resource-thread-image.h" // ConvertStreamToBitmap
// #include "platform-abstractions/slp/resource-loader/resource-thread-image.h" // ConvertStreamToBitmap
//#include "platform-abstractions/slp/resource-loader/resource-thread-base.h"

#if defined(EMSCRIPTEN)
# include "emscripten/emscripten.h"
# include "emscripten-callbacks.h"
#endif

#if defined(EMSCRIPTEN)
# define EM_LOG(x) EM_ASM( console.log( x ) );
#else
# define EM_LOG(x);
#endif

namespace
{

Dali::Integration::BitmapPtr EncodedImage( Dali::RefCountedVector<uint8_t>* encodedBlob )
{
  Dali::Integration::BitmapPtr bitmapPtr = NULL;

  //  Dali::RefCountedVector<uint8_t>* const encodedBlob = reinterpret_cast<Dali::RefCountedVector<uint8_t>*>( request.GetResource().Get() );

  if( encodedBlob != 0 )
  {
    const size_t blobSize     = encodedBlob->GetVector().Size();
    uint8_t * const blobBytes = &(encodedBlob->GetVector()[0]);
    DALI_ASSERT_DEBUG( blobSize > 0U );
    DALI_ASSERT_DEBUG( blobBytes != 0U );

    if( blobBytes != 0 && blobSize > 0U )
    {

      SDL_RWops *memory = SDL_RWFromMem(blobBytes, blobSize);

      SDL_Surface *surface = IMG_Load_RW(memory, 0);

      DALI_ASSERT_DEBUG(0 != surface->format);

      if( 0 && (SDL_PIXELFORMAT_RGBA8888 != surface->format->format) )
      {
        // printf(" converting surface:%x\n", surface->format->format );

        SDL_Surface *convertedSurface;
        // Uint32 rmask, gmask, bmask, amask;

        convertedSurface = SDL_CreateRGBSurface( SDL_SWSURFACE,
                                                 surface->w, surface->h,
                                                 32,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN /* OpenGL RGBA masks */
                                                 0x000000FF,
                                                 0x0000FF00,
                                                 0x00FF0000,
                                                 0xFF000000
#else
                                                 0xFF000000,
                                                 0x00FF0000,
                                                 0x0000FF00,
                                                 0x000000FF
#endif
          );

        SDL_BlitSurface(surface, NULL, convertedSurface, NULL);


        SDL_FreeSurface(surface);
        surface = convertedSurface;
        convertedSurface = NULL;
      }

      if(surface)
      {
        bool managePixelBuffer = true;
        bitmapPtr = Dali::Integration::Bitmap::New( Dali::Integration::Bitmap::BITMAP_2D_PACKED_PIXELS,
                                                    managePixelBuffer );
        Dali::Integration::Bitmap::PackedPixelsProfile* packedProfile = bitmapPtr->GetPackedPixelsProfile();
        DALI_ASSERT_ALWAYS(packedProfile);
        Dali::Integration::PixelBuffer* pixels = packedProfile->ReserveBuffer(Dali::Pixel::RGBA8888,
                                                                              surface->w, surface->h,
                                                                              surface->w, surface->h);

        unsigned char* fromPtr = static_cast<unsigned char*>(surface->pixels);

        // Dali::RefCountedVector<uint8_t>* const encodedBlob = reinterpret_cast<Dali::RefCountedVector<uint8_t>*>( request.GetResource().Get() );
        int stride = surface->pitch;
        int index = 0;
        for(int h = 0; h < surface->h; ++h)
        {
          for(int w = 0; w < (surface->w*4); w+=4)
          {
            pixels[ index++ ] = *( (fromPtr + (h * stride) ) + w + 0 );
            pixels[ index++ ] = *( (fromPtr + (h * stride) ) + w + 1 );
            pixels[ index++ ] = *( (fromPtr + (h * stride) ) + w + 2 );
            pixels[ index++ ] = *( (fromPtr + (h * stride) ) + w + 3 );
          }
        }
        printf("    index: %d\n", index);
      } // if surface

    } // if blobSize

  } // if encodedBlob

  return bitmapPtr;
}

} // anon namespace

namespace Dali
{

SdlPlatformAbstraction::SdlPlatformAbstraction()
  : mSeconds(0),
    mMicroSeconds(0),
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
  seconds = mSeconds;
  microSeconds = mMicroSeconds;
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
  EmscriptenCallbacks::LoadImageMetadata(filename, size);
}

void SdlPlatformAbstraction::GetClosestImageSize( const std::string& filename,
                                                  const ImageAttributes& /*attributes*/,
                                                  Vector2& closestSize )
{
  // @todo
  EmscriptenCallbacks::LoadImageMetadata(filename, closestSize);
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
  printf( "SdlPlatformAbstraction::LoadResourceSynchronously: \n" );

  Integration::ResourcePointer ret;

  switch(resourceType.id)
  {
    case Integration::ResourceBitmap:
    {
      const Integration::BitmapResourceType& bitmapResource( static_cast<const Integration::BitmapResourceType&>(resourceType) );

      Integration::BitmapPtr bitmapPtr = EmscriptenCallbacks::GetImage(
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
  printf( "SdlPlatformAbstraction::LoadResource\n" );
  // request.
  // ResourceId request.GetId()
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
          printf( "  as path load\n" );

          const Integration::BitmapResourceType& bitmapResource( static_cast<const Integration::BitmapResourceType&>(*type) );

          Integration::BitmapPtr bitmapPtr = EmscriptenCallbacks::GetImage(
            bitmapResource.imageAttributes, path );
        }
        else
        {
          printf( "  as memory decode\n" );
          // 2) load it (usually on worker thread)
          // DALI_LOG_TRACE_METHOD( mLogFilter );
          // DALI_LOG_INFO(mLogFilter, Debug::Verbose, "%s(%s)\n", __FUNCTION__, request.GetPath().c_str());

          bitmapPtr = EncodedImage( reinterpret_cast<Dali::RefCountedVector<uint8_t>*>( request.GetResource().Get() ) );
        }

        if( bitmapPtr )
        {
          printf( "  bitmap load OK\n" );
          mResourceQueue.push( ResourceIdBitmapPair( resourceId, bitmapPtr ) );
        }
        else
        {
          printf( "  bitmap load FAIL\n" );
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

    // if(mResources.loadFailed)
    // {
    //   cache.LoadFailed( mResources.loadFailedId, mResources.loadFailure );
    // }
    // if(mResources.saved)
    // {
    //   cache.SaveComplete( mResources.savedId, mResources.savedType );
    // }
    // if(mResources.saveFailed)
    // {
    //   cache.SaveFailed( mResources.saveFailedId, mResources.saveFailure );
    // }
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
        bitmapData = EmscriptenCallbacks::GetGlyphImage( fontFamily,
                                                         textRequest.mStyle,
                                                         64.f, // @todo what should this be
                                                         it->character );

      }

      printf("GetGlyphData %d, %lx\n", getBitmapPtr, (long)(bitmapData.Get()));

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
  // Do nothing with arguments
}

void SdlPlatformAbstraction::SetDpi(unsigned int dpiHorizontal, unsigned int dpiVertical)
{
  DALI_ASSERT_ALWAYS("!Unimplemented");
}

const std::string& SdlPlatformAbstraction::GetFontFamilyForChars(const TextArray& charsRequested) const
{
  EM_LOG("SdlPlatformAbstraction::GetFontFamilyForChars");
  DALI_ASSERT_ALWAYS("!Not Implemented");
  return mGetDefaultFontFamilyResult;
}

bool SdlPlatformAbstraction::AllGlyphsSupported(const std::string& name, const std::string& fontStyle, const TextArray& text) const
{
  EM_LOG("SdlPlatformAbstraction::AllGlyphsSupported");
  return true;
}

bool SdlPlatformAbstraction::ValidateFontFamilyName(const std::string& fontFamily, const std::string& fontStyle, bool& isDefaultSystemFont, std::string& closestMatch, std::string& closestStyleMatch) const
{
  EM_LOG("SdlPlatformAbstraction::ValidateFontFamilyName");
  printf("ValidateFontFamilyName:'%s', '%s', %d, '%s','%s'\n", fontFamily.c_str(), fontStyle.c_str(), isDefaultSystemFont, closestMatch.c_str(), closestStyleMatch.c_str());
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
  printf("Load file:%s\n", filename.c_str());

  // DALI_ASSERT_ALWAYS("!Unimplemented");
  // mTrace.PushCall("LoadFile", "");
  // if( mLoadFileResult.loadResult )
  // {
  //   buffer = mLoadFileResult.buffer;
  // }
  return false; // mLoadFileResult.loadResult;
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
  printf("ReadGlobalMetricsFromCache: '%s', '%s'\n", fontFamily.c_str(), fontStyle.c_str());
  globalMetrics = mReadGlobalMetrics; // Want to copy contents...
  return mReadGlobalMetricsResult; // Default false (will be set to true on subsequent write)
}

// GlobalMetrics
//   float lineHeight;         ///< Distance between baselines
//   float ascender;           ///< Distance from baseline to top of cell
//   float unitsPerEM;         ///< font units/EM
//   float underlinePosition;  ///< Underline distance from baseline
//   float underlineThickness; ///< Underline thickness
//   float padAdjustX;         ///< X adjustment value for padding around distance field
//   float padAdjustY;         ///< Y adjustment value for padding around distance field
//   float maxWidth;           ///< Width of widest glyph
//   float maxHeight;          ///< Height of tallest glyph


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

// void SdlPlatformAbstraction::SetGetTimeMicrosecondsResult(size_t sec, size_t usec)
// {
//   mSeconds = sec;
//   mMicroSeconds = usec;
// }

void SdlPlatformAbstraction::GetFileNamesFromDirectory( const std::string& /*directoryName*/,
                                                        std::vector<std::string>& /*fileNames*/ )
{

}

Integration::BitmapPtr SdlPlatformAbstraction::GetGlyphImage( const std::string& fontFamily, const std::string& fontStyle, float fontSize, uint32_t character ) const
{
  EM_LOG("SdlPlatformAbstraction::GetGlyphImage");
  printf("GetGlyphImage: '%s', '%s'\n", fontFamily.c_str(), fontStyle.c_str());

  return Integration::BitmapPtr();
}


void SdlPlatformAbstraction::IncrementGetTimeResult(size_t milliseconds)
{
  mMicroSeconds += milliseconds * 1000u;
  unsigned int additionalSeconds = mMicroSeconds / 1000000u;

  mSeconds += additionalSeconds;
  mMicroSeconds -= additionalSeconds * 1000000u;
}

// void SdlPlatformAbstraction::SetIsLoadingResult(bool result)
// {
//   mIsLoadingResult = result;
// }

// void SdlPlatformAbstraction::SetGetDefaultFontFamilyResult(std::string result)
// {
//   mGetDefaultFontFamilyResult = result;
// }

// void SdlPlatformAbstraction::SetGetDefaultFontSizeResult(float result)
// {
//   mGetDefaultFontSizeResult = result;
// }

// void SdlPlatformAbstraction::SetGetFontPathResult(std::string& result)
// {
//   mGetFontPathResult = result;
// }

// void SdlPlatformAbstraction::ClearReadyResources()
// {
//   memset(&mResources, 0, sizeof(Resources));
// }

// void SdlPlatformAbstraction::SetResourceLoaded(Integration::ResourceId  loadedId,
//                                                Integration::ResourceTypeId  loadedType,
//                                                Integration::ResourcePointer loadedResource)
// {
//   mResources.loaded = true;
//   mResources.loadedId = loadedId;
//   mResources.loadedType = loadedType;
//   mResources.loadedResource = loadedResource;
// }

// void SdlPlatformAbstraction::SetResourceLoadFailed(Integration::ResourceId  id,
//                                                    Integration::ResourceFailure failure)
// {
//   mResources.loadFailed = true;
//   mResources.loadFailedId = id;
//   mResources.loadFailure = failure;
// }

// void SdlPlatformAbstraction::SetResourceSaved(Integration::ResourceId savedId,
//                                                 Integration::ResourceTypeId  savedType)
// {
//   mResources.saved = true;
//   mResources.savedId = savedId;
//   mResources.savedType = savedType;
// }

// void SdlPlatformAbstraction::SetResourceSaveFailed(Integration::ResourceId  id,
//                                                    Integration::ResourceFailure failure)
// {
//   mResources.saveFailed = true;
//   mResources.saveFailedId = id;
//   mResources.saveFailure = failure;
// }

// Integration::ResourceRequest* SdlPlatformAbstraction::GetRequest()
// {
//   return mRequest;
// }

// void SdlPlatformAbstraction::DiscardRequest()
// {
//   delete mRequest;
//   mRequest = NULL;
// }

// void SdlPlatformAbstraction::SetImageMetaDataSize(const Vector2& size)
// {
//   mSize = size;
// }

// void SdlPlatformAbstraction::SetLoadFileResult( bool result, std::vector< unsigned char >& buffer )
// {
//   mLoadFileResult.loadResult = result;
//   if( result )
//   {
//     mLoadFileResult.buffer = buffer;
//   }
// }

// void SdlPlatformAbstraction::SetSaveFileResult( bool result )
// {
//   mSaveFileResult = result;
// }

// Integration::FontListMode SdlPlatformAbstraction::GetLastFontListMode( )
// {
//   return mFontListMode;
// }

// void SdlPlatformAbstraction::SetReadGlobalMetricsResult( bool success, Integration::GlobalMetrics& globalMetrics )
// {
//   mReadGlobalMetricsResult = success;
//   mReadGlobalMetrics = globalMetrics;
// }

// void SdlPlatformAbstraction::SetReadMetricsResult( bool success, std::vector<Integration::GlyphMetrics>& glyphMetricsContainer )
// {
//   mReadMetricsResult = success;
//   mReadMetrics = glyphMetricsContainer; // copy
// }

} // Dali
