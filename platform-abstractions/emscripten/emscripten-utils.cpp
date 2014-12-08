/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

#include "emscripten-utils.h"

// EXTERNAL INCLUDES


// INTERNAL INCLUDES

namespace Dali
{
namespace Internal
{
namespace Emscripten
{

std::string JavaScriptNameToPropertyName(const std::string& camelCase)
{
  std::string ret;

  int countUpper = 0;
  for(unsigned int i = 0; i < camelCase.size(); ++i)
  {
    if(std::isupper(camelCase[i]))
    {
      countUpper++;
    }
  }

  if(countUpper)
  {
    ret.reserve(camelCase.size() + countUpper);

    for(unsigned int i = 0; i < camelCase.size(); ++i)
    {
      char c = camelCase[i];
      if(std::isupper(c))
      {
        ret.push_back('-');
      }

      ret.push_back(std::tolower(c));
    }
  }
  else
  {
    return camelCase;
  }

  return ret;
}

std::string PropertyNameToJavaScriptName(const std::string& hyphenatedName)
{
  std::string ret;

  ret.reserve(hyphenatedName.size());

  bool capitlizeNext = false;
  for(unsigned int i = 0; i < hyphenatedName.size(); ++i)
  {
    char c = hyphenatedName[i];
    if(c == '-')
    {
      capitlizeNext = true;
    }
    else
    {
      if(capitlizeNext)
      {
        ret.push_back(std::toupper(c));
        capitlizeNext = false;
      }
      else
      {
        ret.push_back(c);
      }
    }
  }

  return ret;
}

Dali::Image CreateImageRGBA(unsigned int width, unsigned int height, const std::string& data)
{
  Dali::BufferImage b = Dali::BufferImage::New( width, height, Dali::Pixel::RGBA8888 );

  const Dali::PixelBuffer* from = reinterpret_cast<const Dali::PixelBuffer*>( data.c_str() );
  Dali::PixelBuffer* to = b.GetBuffer();

  unsigned int len = std::max( width * height * 4, data.size() );
  for(int i = 0; i < len; i++)
  {
    *to++ = *from++;
  }
  return b;
}

Dali::Image CreateImageRGB(unsigned int width, unsigned int height, const std::string& data)
{
  Dali::BufferImage b = Dali::BufferImage::New( width, height, Dali::Pixel::RGB888 );

  const Dali::PixelBuffer* from = reinterpret_cast<const Dali::PixelBuffer*>( data.c_str() );
  Dali::PixelBuffer* to = b.GetBuffer();

  unsigned int len = std::max( width * height * 3, data.size() );
  for(int i = 0; i < len; i++)
  {
    *to++ = *from++;
  }
  return b;
}

Dali::Image GetImage(const std::string& data)
{
  const uint8_t* const ptr = reinterpret_cast<const uint8_t * const>(data.c_str());
  return Dali::EncodedBufferImage::New(ptr, data.size());
}


}; // namespace Emscripten
}; // namespace Internal
}; // namespace Dali
