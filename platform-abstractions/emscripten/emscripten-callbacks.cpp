#include <vector>

#include <dali/public-api/dali-core.h>

#include "emscripten-callbacks.h"
#include "emscripten/emscripten.h"
#include "emscripten/bind.h"
#include "emscripten/val.h"

using namespace Dali;

//
namespace EmscriptenCallbacks
{

// Javascript callbacks
emscripten::val JSGetGlyphImage(emscripten::val::null());
emscripten::val JSGetImage(emscripten::val::null());
emscripten::val JSGetImageMetaData(emscripten::val::null());

// void DebugPrintData(const std::vector<unsigned char>& data, int width)
// {
//   printf("data size:%d\n", data.size());
//   int step = width * 4;
//   for(int y = 0; y < width; ++y)
//   {
//     for(int x = 0; x < step; x+=4)
//     {
//       printf( "[%2x,%2x,%2x,%2x]",
//               data[ x + (y*step) +0],
//               data[ x + (y*step) +1],
//               data[ x + (y*step) +2],
//               data[ x + (y*step) +3] );

//       data[ x + (y*step) +0] = data[ x + (y*step) +1] = data[ x + (y*step) +2] = data[ x + (y*step) +3] = 0xFF;

//     }
//     printf("\n");

//   }
//   printf("\n");
// }


Integration::BitmapPtr GetGlyphImage( const std::string& fontFamily, const std::string& fontStyle, float fontSize, uint32_t character )
{
  Integration::BitmapPtr ret;

  // causes exception in browser if callback isnt set to a function;
  emscripten::val val = JSGetGlyphImage(fontFamily,
                                        fontStyle,
                                        fontSize,
                                        character);

  std::vector<unsigned char> data = emscripten::vecFromJSArray<unsigned char>(val);

  // int step = fontSize * 4;
  // for(int y = 0; y < fontSize; ++y)
  // {
  //   for(int x = 0; x < step; x+=4)
  //   {
  //     if( data[ x + (y*step) +3] )
  //     {
  //       data[ x + (y*step) +0] = data[ x + (y*step) +1] = data[ x + (y*step) +2] = data[ x + (y*step) +3] = 0xFF;
  //     }
  //   }
  // }
  // printf("data size:%d\n", data.size());
  int step = fontSize * 4;

  // data[ 0] = /*data[ 1] = data[ 2] = */ data[ 3] = 0xFF;

  // for(int j =0; j < step; j+=8)
  // {
  //   data[ j + (0*step) +0] = /* data[ j + (0*step) +1] = data[ j + (0*step) +2] = */ data[ j + (0*step) +3] = 0xFF;
  // }

  // for(int j =0; j < 32; ++j)
  // {
  //   data[ 0 + (j*step) +0] = data[ 0 + (j*step) +1] = data[ 0 + (j*step) +2] = data[ 0 + (j*step) +3] = 0xFF;
  // }

  // for(int y = 0; y < fontSize; ++y)
  // {
  //   for(int x = 0; x < step; x+=4)
  //   {
  //     if( data[ x + (y*step) +3] )
  //     {
  //       data[ x + (y*step) +0] = data[ x + (y*step) +1] = data[ x + (y*step) +2] = data[ x + (y*step) +3] = 0xFF;
  //     }

  //     printf( "[%2x,%2x,%2x,%2x]",
  //             data[ x + (y*step) +0],
  //             data[ x + (y*step) +1],
  //             data[ x + (y*step) +2],
  //             data[ x + (y*step) +3] );
  //   }
  //   printf("\n");

  // }
  // printf("\n");

  if( data.size() )
  {
    ImageAttributes imageAttributes;

    // ret = Integration::BitmapPtr(new Integration::Bitmap( true, reinterpret_cast<const uint8_t*>( data.c_str() ) )); // bool discardable = false, Dali::Integration::PixelBuffer* pixBuf = 0 );

    Integration::Bitmap* bitmap = Integration::Bitmap::New(Integration::Bitmap::BITMAP_2D_PACKED_PIXELS,
                                                           true); // dali manages buffer

    if(bitmap)
    {
      Integration::Bitmap::PackedPixelsProfile* profile = bitmap->GetPackedPixelsProfile();

      if(profile)
      {
        std::vector<unsigned char> *buffer = new std::vector<unsigned char>;

        buffer->reserve( fontSize * fontSize );

        // take only alpha
        for(int y = 0; y < fontSize; ++y)
        {
          for(int x = 0; x < step; x+=4)
          {
            buffer->push_back(data[ x + (y*step) +3]);
          }
        }

        if( buffer )
        {
          profile->AssignBuffer(Pixel::A8,
                                &(*buffer)[0],
                                (*buffer).size(),
                                fontSize,
                                fontSize);
        }

        ret = Integration::BitmapPtr( bitmap );
      }
      else
      {
        printf("bitmap has no packedpixelsprofile\n");
      }
    }
    else
    {
      printf("bitmap not created\n");
    }

    // DALI_ASSERT_ALWAYS(data.size() == fontSize*fontSize);

    // ret->Initialize(Pixel::RGBA8888,
    //                 fontSize,  // @todo should width/height be fontSize?
    //                 fontSize);
  }
  else
  {
    printf("Image data from javascript is empty\n");
  }

  return ret;
}



Integration::BitmapPtr GetImage( const ImageAttributes& attributes, const std::string& filename )
{
  Integration::BitmapPtr ret;

  // causes exception in browser if callback isnt set to a function;
  emscripten::val val = JSGetImage(filename);

  emscripten::val array = val["array"];
  int w     = val["x"].as<int>();
  int h     = val["y"].as<int>();

  std::vector<unsigned char> data = emscripten::vecFromJSArray<unsigned char>(array);

  ImageAttributes imageAttributes;

  // ret = Integration::BitmapPtr(new Integration::Bitmap( true, reinterpret_cast<const uint8_t*>( data.c_str() ) )); // bool discardable = false, Dali::Integration::PixelBuffer* pixBuf = 0 );

  Integration::Bitmap* bitmap = Integration::Bitmap::New(Integration::Bitmap::BITMAP_2D_PACKED_PIXELS,
                                                         true); // dali manages buffer

  if(bitmap)
  {
    Integration::Bitmap::PackedPixelsProfile* profile = bitmap->GetPackedPixelsProfile();

    if(profile)
    {
      std::vector<unsigned char> *buffer = new std::vector<unsigned char>(data.begin(), data.end());

      if( buffer )
      {
        profile->AssignBuffer(Pixel::RGBA8888,
                              &(*buffer)[0],
                              (*buffer).size(),
                              w,
                              h);
      }

      ret = Integration::BitmapPtr( bitmap );
    }
    else
    {
      printf("bitmap has no packedpixelsprofile\n");
    }
  }
  else
  {
    printf("bitmap not created\n");
  }

  // DALI_ASSERT_ALWAYS(data.size() == fontSize*fontSize);

  return ret;
}

void LoadImageMetadata(const std::string filename, Dali::Vector2& size)
{
  emscripten::val val = JSGetImageMetaData(filename);

  // @todo
  // size.x = val["w"]
  // size.y = val["h"]
  //

}


} // namespace EmscriptenCallbacks

