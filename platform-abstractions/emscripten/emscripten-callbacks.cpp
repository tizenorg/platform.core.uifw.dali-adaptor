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

#include "emscripten-callbacks.h"

// EXTERNAL INCLUDES
#include <vector>
#include <dali/public-api/dali-core.h>
#include "emscripten/emscripten.h"
#include "emscripten/bind.h"
#include "emscripten/val.h"

// INTERNAL INCLUDES

namespace Dali
{
namespace Internal
{
namespace Emscripten
{

Statistics stats;

// Javascript callbacks
emscripten::val JSGetGlyphImage(emscripten::val::null());
emscripten::val JSGetImage(emscripten::val::null());
emscripten::val JSGetImageMetaData(emscripten::val::null());
emscripten::val JSRenderFinished(emscripten::val::null());


Integration::BitmapPtr GetGlyphImage( const std::string& fontFamily, const std::string& fontStyle, float fontSize, uint32_t character )
{
  Integration::BitmapPtr ret;

  // causes exception in browser if callback isnt set to a function;
  emscripten::val val = JSGetGlyphImage(fontFamily,
                                        fontStyle,
                                        fontSize,
                                        character);

  std::vector<unsigned char> data = emscripten::vecFromJSArray<unsigned char>(val);

  int step = fontSize * 4;

  if( data.size() )
  {
    ImageAttributes imageAttributes;

    Integration::Bitmap* bitmap = Integration::Bitmap::New(Integration::Bitmap::BITMAP_2D_PACKED_PIXELS,
                                                           ResourcePolicy::DISCARD); // dali manages buffer

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

  Integration::Bitmap* bitmap = Integration::Bitmap::New(Integration::Bitmap::BITMAP_2D_PACKED_PIXELS,
                                                         Dali::ResourcePolicy::DISCARD); // dali manages buffer

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

void RenderFinished()
{
  if (JSRenderFinished.typeof().as<std::string>() == "function")
  {
    emscripten::val val = JSRenderFinished();
  }
}


}; // namespace Emscripten
}; // namespace Internal
}; // namespace Dali

