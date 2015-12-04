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
 */

#include "image-loader.h"

#include <dali/devel-api/common/ref-counted-dali-vector.h>
#include <dali/integration-api/bitmap.h>
#include <dali/integration-api/debug.h>
#include <adaptors/integration-api/framework.h>

#include "loader-png.h"

#include <cstring>
#include <android/asset_manager_jni.h>

using namespace Dali::Integration;

namespace Dali
{
namespace SlpPlatform
{

namespace
{
typedef bool (*LoadBitmapFunction)( const ResourceLoadingClient& client, const ImageLoader::Input& input, Integration::Bitmap& bitmap );
typedef bool (*LoadBitmapHeaderFunction)( const ImageLoader::Input& input, unsigned int& width, unsigned int& height );

#if defined(DEBUG_ENABLED)
Integration::Log::Filter* gLogFilter = Debug::Filter::New( Debug::Concise, false, "LOG_IMAGE_LOADING" );
#endif

/**
 * Stores the magic bytes, and the loader and header functions used for each image loader.
 */
struct BitmapLoader
{
  unsigned char magicByte1;        ///< The first byte in the file should be this
  unsigned char magicByte2;        ///< The second byte in the file should be this
  LoadBitmapFunction loader;       ///< The function which decodes the file
  LoadBitmapHeaderFunction header; ///< The function which decodes the header of the file
  Bitmap::Profile profile;         ///< The kind of bitmap to be created
                                   ///  (addressable packed pixels or an opaque compressed blob).
};

/**
 * Enum for file formats, has to be in sync with BITMAP_LOADER_LOOKUP_TABLE
 */
enum FileFormats
{
  // Unknown file format
  FORMAT_UNKNOWN = -1,

  // formats that use magic bytes
  FORMAT_PNG = 0,
  FORMAT_JPEG,
  FORMAT_BMP,
  FORMAT_GIF,
  FORMAT_KTX,
  FORMAT_ICO,
  FORMAT_MAGIC_BYTE_COUNT,

  // formats after this one do not use magic bytes
  FORMAT_WBMP = FORMAT_MAGIC_BYTE_COUNT,
  FORMAT_TOTAL_COUNT
};

/**
 * A lookup table containing all the bitmap loaders with the appropriate information.
 * Has to be in sync with enum FileFormats
 */
const BitmapLoader BITMAP_LOADER_LOOKUP_TABLE[FORMAT_TOTAL_COUNT] =
{
  { Png::MAGIC_BYTE_1,  Png::MAGIC_BYTE_2,  LoadBitmapFromPng,  LoadPngHeader,  Bitmap::BITMAP_2D_PACKED_PIXELS },
#if 0
  { Jpeg::MAGIC_BYTE_1, Jpeg::MAGIC_BYTE_2, LoadBitmapFromJpeg, LoadJpegHeader, Bitmap::BITMAP_2D_PACKED_PIXELS },
  { Bmp::MAGIC_BYTE_1,  Bmp::MAGIC_BYTE_2,  LoadBitmapFromBmp,  LoadBmpHeader,  Bitmap::BITMAP_2D_PACKED_PIXELS },
  { Gif::MAGIC_BYTE_1,  Gif::MAGIC_BYTE_2,  LoadBitmapFromGif,  LoadGifHeader,  Bitmap::BITMAP_2D_PACKED_PIXELS },
  { Ktx::MAGIC_BYTE_1,  Ktx::MAGIC_BYTE_2,  LoadBitmapFromKtx,  LoadKtxHeader,  Bitmap::BITMAP_COMPRESSED       },
  { Ico::MAGIC_BYTE_1,  Ico::MAGIC_BYTE_2,  LoadBitmapFromIco,  LoadIcoHeader,  Bitmap::BITMAP_2D_PACKED_PIXELS },
  { 0x0,                0x0,                LoadBitmapFromWbmp, LoadWbmpHeader, Bitmap::BITMAP_2D_PACKED_PIXELS },
#endif
};

const unsigned int MAGIC_LENGTH = 2;

/**
 * This code tries to predict the file format from the filename to help with format picking.
 */
struct FormatExtension
{
  const std::string extension;
  FileFormats format;
};

const FormatExtension FORMAT_EXTENSIONS[] =
{
 { ".png",  FORMAT_PNG  },
 { ".jpg",  FORMAT_JPEG },
 { ".bmp",  FORMAT_BMP  },
 { ".gif",  FORMAT_GIF  },
 { ".ktx",  FORMAT_KTX  },
 { ".ico",  FORMAT_ICO  },
 { ".wbmp", FORMAT_WBMP }
};

const unsigned int FORMAT_EXTENSIONS_COUNT = sizeof(FORMAT_EXTENSIONS) / sizeof(FormatExtension);

FileFormats GetFormatHint( const std::string& filename )
{
  FileFormats format = FORMAT_UNKNOWN;

  for ( unsigned int i = 0; i < FORMAT_EXTENSIONS_COUNT; ++i )
  {
    unsigned int length = FORMAT_EXTENSIONS[i].extension.size();
    if ( ( filename.size() > length ) &&
         ( 0 == filename.compare( filename.size() - length, length, FORMAT_EXTENSIONS[i].extension ) ) )
    {
      format = FORMAT_EXTENSIONS[i].format;
      break;
    }
  }

  return format;
}

/**
 * Checks the magic bytes of the file first to determine which Image decoder to use to decode the
 * bitmap.
 * @param[in]   fp      The file to decode
 * @param[in]   format  Hint about what format to try first
 * @param[out]  loader  Set with the function to use to decode the image
 * @param[out]  header  Set with the function to use to decode the header
 * @param[out]  profile The kind of bitmap to hold the bits loaded for the bitmap.
 * @return true, if we can decode the image, false otherwise
 */
bool GetBitmapLoaderFunctions( uint8_t* buffer,
                               size_t size,
                               FileFormats format,
                               LoadBitmapFunction& loader,
                               LoadBitmapHeaderFunction& header,
                               Bitmap::Profile& profile )
{
  if (size < MAGIC_LENGTH)
  {
    return false;
  }

  unsigned char magic[MAGIC_LENGTH];
  memcpy(magic, buffer, MAGIC_LENGTH);

  bool loaderFound = false;
  const BitmapLoader *lookupPtr = BITMAP_LOADER_LOOKUP_TABLE;
  ImageLoader::Input defaultInput( buffer, size );

  // try hinted format first
  if ( format != FORMAT_UNKNOWN )
  {
    lookupPtr = BITMAP_LOADER_LOOKUP_TABLE + format;
    if ( format >= FORMAT_MAGIC_BYTE_COUNT ||
         ( lookupPtr->magicByte1 == magic[0] && lookupPtr->magicByte2 == magic[1] ) )
    {
      unsigned int width = 0;
      unsigned int height = 0;
      loaderFound = lookupPtr->header( defaultInput, width, height );
    }
  }

  // then try to get a match with formats that have magic bytes
  if ( false == loaderFound )
  {
    for ( lookupPtr = BITMAP_LOADER_LOOKUP_TABLE;
          lookupPtr < BITMAP_LOADER_LOOKUP_TABLE + FORMAT_MAGIC_BYTE_COUNT;
          ++lookupPtr )
    {
      if ( lookupPtr->magicByte1 == magic[0] && lookupPtr->magicByte2 == magic[1] )
      {
        // to seperate ico file format and wbmp file format
        unsigned int width = 0;
        unsigned int height = 0;
        loaderFound = lookupPtr->header( defaultInput, width, height );
      }
      if (loaderFound)
      {
        break;
      }
    }
  }

  // finally try formats that do not use magic bytes
  if ( false == loaderFound )
  {
    for ( lookupPtr = BITMAP_LOADER_LOOKUP_TABLE + FORMAT_MAGIC_BYTE_COUNT;
          lookupPtr < BITMAP_LOADER_LOOKUP_TABLE + FORMAT_TOTAL_COUNT;
          ++lookupPtr )
    {
      // to seperate ico file format and wbmp file format
      unsigned int width = 0;
      unsigned int height = 0;
      loaderFound = lookupPtr->header( defaultInput, width, height);
      if (loaderFound)
      {
        break;
      }
    }
  }

  // if a loader was found set the outputs
  if ( loaderFound )
  {
    loader  = lookupPtr->loader;
    header  = lookupPtr->header;
    profile = lookupPtr->profile;
  }

  return loaderFound;
}

} // anonymous namespace


namespace ImageLoader
{

bool ConvertStreamToBitmap(const ResourceType& resourceType, const std::string& path, const Dali::Vector<uint8_t>& data, const ResourceLoadingClient& client, BitmapPtr& ptr)
{
  return ConvertStreamToBitmap( resourceType, path, data.Begin(), data.Size(), client, ptr );
}

bool ConvertStreamToBitmap(const ResourceType& resourceType, const std::string& path, uint8_t* buffer, size_t size, const ResourceLoadingClient& client, BitmapPtr& ptr)
{
  DALI_LOG_TRACE_METHOD( gLogFilter );
  DALI_ASSERT_DEBUG( ResourceBitmap == resourceType.id );

  bool result = false;
  BitmapPtr bitmap = 0;

  if( size > 0 )
  {
    // Only png, jpg, bmp, gif, and compressed-data-containing ktx files are supported.
    LoadBitmapFunction function;
    LoadBitmapHeaderFunction header;
    Bitmap::Profile profile;

    if ( GetBitmapLoaderFunctions( buffer,
                                   size,
                                   GetFormatHint( path ),
                                   function,
                                   header,
                                   profile ) )
    {
      bitmap = Bitmap::New( profile, ResourcePolicy::OWNED_DISCARD );

      DALI_LOG_SET_OBJECT_STRING( bitmap, path );
      const BitmapResourceType& resType = static_cast<const BitmapResourceType&>( resourceType );
      const ScalingParameters scalingParameters( resType.size, resType.scalingMode, resType.samplingMode );
      const ImageLoader::Input input( buffer, size, scalingParameters, resType.orientationCorrection );

      result = function( client, input, *bitmap );

      if (!result)
      {
        DALI_LOG_WARNING("Unable to decode %s\n", path.c_str());
        bitmap.Reset();
      }
      // Apply the requested image attributes in best-effort fashion:
//      client.InterruptionPoint(); // Note: By design, this can throw an exception
//      bitmap = Internal::Platform::ApplyAttributesToBitmap( bitmap, resType.size, resType.scalingMode, resType.samplingMode );
    }
    else
    {
      DALI_LOG_WARNING( "Image Decoder for %s unavailable\n", path.c_str() );
    }
  }

  ptr.Reset(bitmap.Get());
  return result;
}

ResourcePointer LoadResourceSynchronously( const Integration::ResourceType& resourceType, const std::string& resourcePath )
{
  BitmapPtr bitmap;
  Dali::Vector<uint8_t> data;
  LoadAsset(resourcePath, data);
  if(data.Size())
  {
    bool result = ConvertStreamToBitmap( resourceType, resourcePath, data, StubbedResourceLoadingClient(), bitmap );
    if( !result )
    {
      DALI_LOG_WARNING( "Unable to decode %s\n", resourcePath.c_str() );
    }
  }
  else
  {
    DALI_LOG_WARNING( "Failed to open file to load \"%s\"\n", resourcePath.c_str() );
  }

  ResourcePointer resource;
  if(bitmap)
  {
    resource.Reset(bitmap.Get());
  }

  return resource;
}

///@ToDo: Rename GetClosestImageSize() functions. Make them use the orientation correction and scaling information. Requires jpeg loader to tell us about reorientation. [Is there still a requirement for this functionality at all?]
ImageDimensions GetClosestImageSize( const std::string& filename,
                                     ImageDimensions size,
                                     FittingMode::Type fittingMode,
                                     SamplingMode::Type samplingMode,
                                     bool orientationCorrection )
{
  unsigned int width = 0;
  unsigned int height = 0;

  Dali::Vector<uint8_t> data;
  if (LoadAsset(filename, data))
  {
    LoadBitmapFunction loaderFunction;
    LoadBitmapHeaderFunction headerFunction;
    Bitmap::Profile profile;

    if ( GetBitmapLoaderFunctions( data.Begin(),
                                   data.Size(),
                                   GetFormatHint(filename),
                                   loaderFunction,
                                   headerFunction,
                                   profile ) )
    {
      const ImageLoader::Input input( data, ScalingParameters( size, fittingMode, samplingMode ), orientationCorrection );

      const bool read_res = headerFunction( data, width, height );
      if(!read_res)
      {
        DALI_LOG_WARNING("Image Decoder failed to read header for %s\n", filename.c_str());
      }
    }
    else
    {
      DALI_LOG_WARNING("Image Decoder for %s unavailable\n", filename.c_str());
    }
  }
  return ImageDimensions( width, height );
}



ImageDimensions GetClosestImageSize( Integration::ResourcePointer resourceBuffer,
                                     ImageDimensions size,
                                     FittingMode::Type fittingMode,
                                     SamplingMode::Type samplingMode,
                                     bool orientationCorrection )
{
  unsigned int width = 0;
  unsigned int height = 0;

  // Get the blob of binary data that we need to decode:
  DALI_ASSERT_DEBUG( resourceBuffer );
  Dali::RefCountedVector<uint8_t>* const encodedBlob = reinterpret_cast<Dali::RefCountedVector<uint8_t>*>( resourceBuffer.Get() );

  if( encodedBlob != 0 )
  {

    const size_t blobSize     = encodedBlob->GetVector().Size();
    uint8_t * const blobBytes = encodedBlob->GetVector().Begin();
    DALI_ASSERT_DEBUG( blobSize > 0U );
    DALI_ASSERT_DEBUG( blobBytes != 0U );

    if( blobBytes != 0 && blobSize > 0U )
    {
        LoadBitmapFunction loaderFunction;
        LoadBitmapHeaderFunction headerFunction;
        Bitmap::Profile profile;

        const Dali::Vector<uint8_t>& data = encodedBlob->GetVector();

        if ( GetBitmapLoaderFunctions( data.Begin(),
                                       data.Size(),
                                       FORMAT_UNKNOWN,
                                       loaderFunction,
                                       headerFunction,
                                       profile ) )
        {
          const ImageLoader::Input input( encodedBlob->GetVector(), ScalingParameters( size, fittingMode, samplingMode ), orientationCorrection );
          const bool read_res = headerFunction( input, width, height );
          if( !read_res )
          {
            DALI_LOG_WARNING( "Image Decoder failed to read header for resourceBuffer\n" );
          }
        }

    }
  }
  return ImageDimensions( width, height );
}


bool LoadAsset(const std::string& name, Dali::Vector<uint8_t>& buffer)
{
  // The asset manager is valid on any thread.
  AAssetManager* assetManager = Framework::GetAssetManager();

  DALI_LOG_INFO( gLogFilter, Debug::Concise, "%s loading %s\n", __PRETTY_FUNCTION__, name.c_str());
  AAsset* asset = AAssetManager_open(assetManager, name.c_str(), AASSET_MODE_BUFFER);
  if( asset )
  {
    off_t totalSize = AAsset_getLength(asset);
    if (totalSize > 0)
    {
      off_t bytesReadTotal = 0;
      buffer.Resize(totalSize);
      while(bytesReadTotal < totalSize)
      {
        int bytesRead = AAsset_read(asset, &buffer[bytesReadTotal], totalSize);
        if (bytesRead > 0)
        {
          bytesReadTotal += bytesRead;
          totalSize -= bytesRead;
        }
      }
    }

    AAsset_close(asset);

    return true;
  }
  return false;
}


} // ImageLoader
} // SlpPlatform
} // Dali
