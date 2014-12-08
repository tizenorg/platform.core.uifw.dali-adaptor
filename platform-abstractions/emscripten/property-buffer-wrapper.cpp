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
#include "property-value-wrapper.h"

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

void SetPropertyBufferDataRaw(Dali::PropertyBuffer& self, const std::string& data )
{
  self.SetData( reinterpret_cast<void*>( const_cast<char*>(data.c_str()) ) );
}


}; // namespace Emscripten
}; // namespace Internal
}; // namespace Dali
