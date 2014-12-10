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

#include "resource-thread-image.h"
#include <dali/public-api/common/ref-counted-dali-vector.h>
#include <dali/integration-api/bitmap.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/resource-cache.h>

#include <stdint.h>
#include <android-interface.h>
#include <android/asset_manager_jni.h>
#include <dali/public-api/common/dali-vector.h>
#include "nativeLogging.h"
#include "platform-capabilities.h"
#include <dali/integration-api/resource-types.h>

// #include "loader-bmp.h"
// #include "loader-gif.h"
// #include "loader-jpeg.h"
#include "loader-png.h"
// #include "loader-ico.h"
// #include "loader-ktx.h"
// #include "loader-wbmp.h"

using namespace std;
using namespace Dali::Integration;
using boost::mutex;
using boost::unique_lock;

namespace Dali
{

namespace SlpPlatform
{

namespace
{

typedef bool (*LoadBitmapFunction)(const Dali::Vector<uint8_t>&, Bitmap&, ImageAttributes&);
typedef bool (*LoadBitmapHeaderFunction)(const Dali::Vector<uint8_t>&, const ImageAttributes& attrs, unsigned int& width, unsigned int& height );

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
bool GetBitmapLoaderFunctions( const Dali::Vector<uint8_t>& data,
                               FileFormats format,
                               LoadBitmapFunction& loader,
                               LoadBitmapHeaderFunction& header,
                               Bitmap::Profile& profile )
{
  if (data.Size() < MAGIC_LENGTH)
  {
    return false;
  }

  unsigned char magic[MAGIC_LENGTH];
  memcpy(magic, data.Begin(), MAGIC_LENGTH);

  bool loaderFound = false;
  const BitmapLoader *lookupPtr = BITMAP_LOADER_LOOKUP_TABLE;
  ImageAttributes attrs;

  // try hinted format first
  if ( format != FORMAT_UNKNOWN )
  {
    lookupPtr = BITMAP_LOADER_LOOKUP_TABLE + format;
    if ( format >= FORMAT_MAGIC_BYTE_COUNT ||
         ( lookupPtr->magicByte1 == magic[0] && lookupPtr->magicByte2 == magic[1] ) )
    {
      unsigned int width = 0;
      unsigned int height = 0;
      loaderFound = lookupPtr->header(data, attrs, width, height );
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
        loaderFound = lookupPtr->header(data, attrs, width, height);
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
      loaderFound = lookupPtr->header(data, attrs, width, height);
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

}

ResourceThreadImage::ResourceThreadImage(ResourceLoader& resourceLoader)
: ResourceThreadBase(resourceLoader) 
{
}

ResourceThreadImage::~ResourceThreadImage()
{
}

ResourcePointer ResourceThreadImage::LoadResourceSynchronously( const Integration::ResourceType& resourceType, const std::string& resourcePath )
{
  BitmapPtr bitmap;
  Dali::Vector<uint8_t> data;
  androidcore_LoadAsset(resourcePath, data);
  if(data.Size())
  {
    bool result = ConvertStreamToBitmap( resourceType, resourcePath, data, bitmap);
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

void ResourceThreadImage::GetClosestImageSize( const std::string& filename,
                                               const ImageAttributes& attributes,
                                               Vector2 &closestSize )
{
  Dali::Vector<uint8_t> data;
  if (androidcore_LoadAsset(filename, data))
  {
    LoadBitmapFunction loaderFunction;
    LoadBitmapHeaderFunction headerFunction;
    Bitmap::Profile profile;

    if ( GetBitmapLoaderFunctions( data,
                                   GetFormatHint(filename),
                                   loaderFunction,
                                   headerFunction,
                                   profile ) )
    {
      unsigned int width;
      unsigned int height;

      const bool read_res = headerFunction(data, attributes, width, height);
      if(!read_res)
      {
        DALI_LOG_WARNING("Image Decoder failed to read header for %s\n", filename.c_str());
      }

      closestSize.width = (float)width;
      closestSize.height = (float)height;
    }
    else
    {
      DALI_LOG_WARNING("Image Decoder for %s unavailable\n", filename.c_str());
    }
  }
}


void ResourceThreadImage::GetClosestImageSize( ResourcePointer resourceBuffer,
                                               const ImageAttributes& attributes,
                                               Vector2 &closestSize )
{
  BitmapPtr bitmap = 0;

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
      // Open a file handle on the memory buffer:
        LoadBitmapFunction loaderFunction;
        LoadBitmapHeaderFunction headerFunction;
        Bitmap::Profile profile;

        if ( GetBitmapLoaderFunctions( encodedBlob->GetVector(),
                                       FORMAT_UNKNOWN,
                                       loaderFunction,
                                       headerFunction,
                                       profile ) )
        {
          unsigned int width;
          unsigned int height;
          const bool read_res = headerFunction(encodedBlob->GetVector(), attributes, width, height);
          if(!read_res)
          {
            DALI_LOG_WARNING("Image Decoder failed to read header for resourceBuffer\n");
          }

          closestSize.width = (float) width;
          closestSize.height = (float) height;
        }
    }
  }
}

//--------------------------------------------------------------------------

ResourceThreadImage::ImageType ResourceThreadImage::GetImageType(const std::string& path)
{
  if(path.empty())
  {
    return eInvalidImage;
  }
  else
  {
    // Classify by loader
    // find_first_of(): Searches the string for the first character that matches any of the characters specified in its arguments.
    // find(): Searches the string for the first occurrence of the sequence specified by its arguments.
    if(path.find("content:") == 0 || path.find("res:") == 0 || path.find("shape:") == 0)
    {
      return eResourceFromJava;
    }
  }

  return eResourceFromAsset;
}

//----------------- Called from separate thread (mThread) -----------------

void ResourceThreadImage::Load(const ResourceRequest& request)
{
  DALI_LOG_TRACE_METHOD( mLogFilter );
  DALI_LOG_INFO( mLogFilter, Debug::Verbose, "%s(%s)\n", __FUNCTION__, request.GetPath().c_str() );

  bool file_not_found = false;
  BitmapPtr bitmap = 0;

  ImageType imageType = GetImageType(request.GetPath());
  switch(imageType)
  {
    case eResourceFromJava:
    {
      LOGI("Image Request from java: %s\n", request.GetPath().c_str());
      Android::ImageDataHandle imageData = androidcore_LoadBitmap(request.GetPath());
      if (imageData)
      {
        unsigned int bufferWidth  = GetTextureDimension(imageData->GetWidth());
        unsigned int bufferHeight = GetTextureDimension(imageData->GetHeight());
        Pixel::Format pixelFormat = Pixel::RGBA8888;
        unsigned int bpp = Pixel::GetBytesPerPixel(pixelFormat);
        Bitmap::Profile profile = Bitmap::BITMAP_2D_PACKED_PIXELS;
        bitmap = Bitmap::New(profile, ResourcePolicy::DISCARD);
        unsigned char *bitmapBuffer = bitmap->GetPackedPixelsProfile()->ReserveBuffer(pixelFormat, imageData->GetWidth(), imageData->GetHeight(), bufferWidth, bufferHeight);

        // copy memory area from y and x to bitmap buffer line-by-line
        unsigned char *bufptr = bitmapBuffer;
        unsigned char *lptr = imageData->GetBuffer();
        for (unsigned int i = 0; i < imageData->GetHeight(); ++i)
        {
          memcpy (bufptr, lptr, imageData->GetStride());
          bufptr += bufferWidth * bpp;
          lptr += imageData->GetStride();
        }

        // Construct LoadedResource and ResourcePointer for image data
        LoadedResource resource( request.GetId(), request.GetType()->id, ResourcePointer( bitmap.Get() ) );
        // Queue the loaded resource
        mResourceLoader.AddLoadedResource( resource );
      }
      break;
    }

    default:
    {
      Dali::Vector<uint8_t> data;
      androidcore_LoadAsset(request.GetPath(), data);
      if(data.Size())
      {
        bool result = ConvertStreamToBitmap( *request.GetType(), request.GetPath(), data, bitmap);
        if (result && bitmap)
        {
          // Construct LoadedResource and ResourcePointer for image data
          LoadedResource resource( request.GetId(), request.GetType()->id, ResourcePointer( bitmap.Get() ) );
          // Queue the loaded resource
          mResourceLoader.AddLoadedResource( resource );          
        }
        else 
        {
          DALI_LOG_WARNING( "Unable to decode %s\n", request.GetPath().c_str() );
        }
      }
      else
      {
        DALI_LOG_WARNING( "Failed to open file to load \"%s\"\n", request.GetPath().c_str() );
        file_not_found = true;
      }
      break;
    }
  }

  // Log the load if it worked:
  if ( bitmap )
  {
    const unsigned int width  = bitmap->GetImageWidth();
    const unsigned int height = bitmap->GetImageHeight();
    const unsigned int bufSize   = bitmap->GetBufferSize();
    Pixel::Format pixelFormat = bitmap->GetPixelFormat();
    const int formatVal = pixelFormat;
  }
  else
  {
    if( file_not_found )
    {
      FailedResource resource(request.GetId(), FailureFileNotFound  );
      mResourceLoader.AddFailedLoad(resource);
    }
    else
    {
      FailedResource resource(request.GetId(), FailureUnknown);
      mResourceLoader.AddFailedLoad(resource);
    }
  }
}

void ResourceThreadImage::Decode(const ResourceRequest& request)
{
  DALI_LOG_TRACE_METHOD( mLogFilter );
  DALI_LOG_INFO(mLogFilter, Debug::Verbose, "%s(%s)\n", __FUNCTION__, request.GetPath().c_str());

  BitmapPtr bitmap = 0;

  // Get the blob of binary data that we need to decode:
  DALI_ASSERT_DEBUG( request.GetResource() );

  DALI_ASSERT_DEBUG( 0 != dynamic_cast<Dali::RefCountedVector<uint8_t>*>( request.GetResource().Get() ) && "Only blobs of binary data can be decoded." );
  Dali::RefCountedVector<uint8_t>* const encodedBlob = reinterpret_cast<Dali::RefCountedVector<uint8_t>*>( request.GetResource().Get() );

  if( encodedBlob != 0 )
  {
    const size_t blobSize     = encodedBlob->GetVector().Size();
    uint8_t * const blobBytes = encodedBlob->GetVector().Begin();
    DALI_ASSERT_DEBUG( blobSize > 0U );
    DALI_ASSERT_DEBUG( blobBytes != 0U );

    if( blobBytes != 0 && blobSize > 0U )
    {
      bool result = ConvertStreamToBitmap(*request.GetType(), request.GetPath(), encodedBlob->GetVector(), bitmap);
      if ( !result )
      {
        DALI_LOG_WARNING( "Unable to decode bitmap supplied as in-memory blob.\n" );
      }
    }
  }

  // Log the decode if it worked:
  if (bitmap)
  {
    const unsigned int width  = bitmap->GetImageWidth();
    const unsigned int height = bitmap->GetImageHeight();
    const unsigned int bufSize   = bitmap->GetBufferSize();
    Pixel::Format pixelFormat = bitmap->GetPixelFormat();
    const int formatVal = pixelFormat;

  }
  else
  {
    FailedResource resource(request.GetId(), FailureUnknown);
    mResourceLoader.AddFailedLoad(resource);
  }
}

void ResourceThreadImage::Save(const Integration::ResourceRequest& request)
{
  DALI_LOG_TRACE_METHOD( mLogFilter );
  DALI_ASSERT_DEBUG( request.GetType()->id == ResourceBitmap );
  DALI_LOG_WARNING( "Image saving not supported on background resource threads." );
}

bool ResourceThreadImage::ConvertStreamToBitmap(const ResourceType& resourceType, const std::string& path, const Dali::Vector<uint8_t>& data, BitmapPtr& ptr)
{
  DALI_LOG_TRACE_METHOD(mLogFilter);
  DALI_ASSERT_DEBUG( ResourceBitmap == resourceType.id );

  bool result = false;
  BitmapPtr bitmap = 0;

  if (data.Size())
  {
    // Only png, jpg, bmp, gif, and compressed-data-containing ktx files are supported.
    LoadBitmapFunction function;
    LoadBitmapHeaderFunction header;
    Bitmap::Profile profile;

    if ( GetBitmapLoaderFunctions( data,
                                   GetFormatHint( path ),
                                   function,
                                   header,
                                   profile ) )
    {
      bitmap = Bitmap::New(profile, ResourcePolicy::DISCARD);

      DALI_LOG_SET_OBJECT_STRING(bitmap, path);
      const BitmapResourceType& resType = static_cast<const BitmapResourceType&>(resourceType);
      ImageAttributes attributes  = resType.imageAttributes;

      result = function(data, *bitmap, attributes);

      if (!result)
      {
        DALI_LOG_WARNING("Unable to decode %s\n", path.c_str());
        bitmap.Reset();
      }
    }
    else
    {
      DALI_LOG_WARNING("Image Decoder for %s unavailable\n", path.c_str());
    }
  }

  ptr.Reset(bitmap.Get());
  return result;
}


} // namespace SlpPlatform

} // namespace Dali
