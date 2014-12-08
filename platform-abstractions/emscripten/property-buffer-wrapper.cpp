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

#include "property-buffer-wrapper.h"

// EXTERNAL INCLUDES

// INTERNAL INCLUDES

namespace
{

unsigned char* CopyFloats( unsigned length, const emscripten::val& value, unsigned char*data )
{
  if( length == value["length"].as<unsigned>() ) {
    for(unsigned int k = 0; k < length; k++)
    {
      *(reinterpret_cast<float*>(data)) = value[k].as<float>();
      data += sizeof(float);
    }
  }
  else
  {
    assert( !"Copy Failed"); // @todo
  }
  return data;
}

size_t sizeOfPropertyType(Dali::Property::Type& type)
{
  size_t ret = 0;
  switch(type)
  {
    case Dali::Property::VECTOR4:
    {
      ret = sizeof(float) * 4;
      break;
    }
    case Dali::Property::ROTATION:
    {
      ret = sizeof(float) * 4;
      break;
    }
    case Dali::Property::VECTOR3:
    {
      ret = sizeof(float) * 3;
      break;
    }
    case Dali::Property::VECTOR2:
    {
      ret = sizeof(float) * 2;
      break;
    }
    case Dali::Property::BOOLEAN:
    {
      ret = sizeof(bool);
      break;
    }
    case Dali::Property::FLOAT:
    {
      ret = sizeof(float);
      break;
    }
    case Dali::Property::INTEGER:
    {
      ret = sizeof(int);
      break;
    }
    case Dali::Property::RECTANGLE:
    {
      ret = sizeof(int) * 4;
      break;
    }
    case Dali::Property::MATRIX3:
    {
      ret = sizeof(float) * 9;
      break;
    }
    case Dali::Property::MATRIX:
    {
      ret = sizeof(float) * 16;
      break;
    }
    case Dali::Property::NONE:
    case Dali::Property::STRING:
    case Dali::Property::ARRAY:
    case Dali::Property::MAP:
    {
      ret = 0;
      break;
    }
  } // switch type

  return ret;

} // sizeOfPropertyType

}; // anon namespace

namespace Dali
{
namespace Internal
{
namespace Emscripten
{


namespace
{

// stores format type and size in bytes
struct FormatRecord
{
  std::string name;
  Dali::Property::Type type;
  size_t size;
  FormatRecord(const std::string& n, const Dali::Property::Type t, const size_t s) :
    name(n), type(t), size(s) {}
};

}

void SetPropertyBufferData(Dali::PropertyBuffer& self, const emscripten::val& val )
{
  const Dali::Property::Map& map = self.GetFormat();
  size_t mapSize = map.Count();

  typedef std::vector<FormatRecord> Format;
  Format format;

  format.reserve(mapSize);
  for(size_t i = 0; i < mapSize; i++)
  {
    Dali::StringValuePair& pair = map.GetPair(i);
    Dali::Property::Type type = static_cast<Dali::Property::Type>(pair.second.Get<int>());
    format.push_back( FormatRecord(pair.first, type, sizeOfPropertyType(type)) );
  }

  // size_t size = format.size();
  size_t allocSize = 0;

  for(Format::iterator iter = format.begin(); iter != format.end(); ++iter)
  {
    allocSize += (*iter).size;
  }

  unsigned valueLength = val["length"].as<unsigned>();

  unsigned char* data = NULL;
  unsigned char* allocData = NULL;
  data = allocData = new unsigned char[allocSize * valueLength];

  size_t typeIndex = 0;

  Dali::Property::Type type = format[typeIndex].type;

  for(unsigned int i = 0; i < valueLength; i++)
  {
    const emscripten::val& item = val[i];
    unsigned itemLength = item["length"].as<unsigned>();
    for(unsigned int j = 0; j < itemLength; ++j)
    {
      switch(type)
      {
        case Dali::Property::BOOLEAN:
        {
          *(reinterpret_cast<bool*>(data)) = val.as<bool>();
          data += sizeof(bool);
          break;
        }
        case Dali::Property::FLOAT:
        {
          *(reinterpret_cast<float*>(data)) = val.as<float>();
          data += sizeof(float);
          break;
        }
        case Dali::Property::INTEGER:
        {
          *(reinterpret_cast<int*>(data)) = val.as<int>();
          data += sizeof(int);
          break;
        }
        case Dali::Property::VECTOR2:
        {
          data = CopyFloats(2, item, data);
          break;
        }
        case Dali::Property::VECTOR3:
        {
          data = CopyFloats(3, item, data);
          break;
        }
        case Dali::Property::VECTOR4:
        {
          data = CopyFloats(4, item, data);
          break;
        }
        case Dali::Property::MATRIX3:
        {
          data = CopyFloats(9, item, data);
          break;
        }
        case Dali::Property::MATRIX:
        {
          data = CopyFloats(16, item, data);
          break;
        }
        case Dali::Property::ROTATION:
        {
          data = CopyFloats(4, item, data);
          break;
        }
        case Dali::Property::NONE:
        case Dali::Property::RECTANGLE:
        case Dali::Property::STRING:
        case Dali::Property::ARRAY:
        case Dali::Property::MAP:
        {
          break;
        }
      }
      if(++typeIndex >= format.size())
      {
        typeIndex = 0;
      }
      type = format[typeIndex].type;
    } // for item ; property type
  } // for val ; array of structs

  assert( (data - allocData) == (allocSize * valueLength) );

  self.SetData(allocData);

}

}; // namespace Emscripten
}; // namespace Internal
}; // namespace Dali
