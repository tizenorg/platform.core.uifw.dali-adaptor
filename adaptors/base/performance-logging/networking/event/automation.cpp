
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

// CLASS HEADER
#include "automation.h"

// EXTERNAL INCLUDES
#include <sstream>
#include <iomanip>
#include <stdio.h>
#include <dali/public-api/dali-core.h>
#include <dali/integration-api/debug.h>


namespace  // un-named namespace
{

const unsigned int MAX_SET_PROPERTY_STRING_LENGTH = 256; ///< maximum length of a set property command

class JsonPropertyValue
{
public:
  JsonPropertyValue( const std::string& str )
  {
    std::size_t strLength = str.length();

    mString.reserve( strLength );
    for( std::size_t i = 0; i < strLength; ++i )
    {
      const char c = str[i];
      if( (c != '[') && c != ']')
      {
        mString.push_back( c );
      }
    }

  }
  std::string GetString() const
  {
    return mString;
  }
  float GetFloat() const
  {
    return atof( mString.c_str() );
  }
  int GetInt()
  {
    return atoi( mString.c_str() );
  }
  bool GetBoolean()
  {
    return (GetInt() != 0);
  }

  Dali::Vector2 GetVector2()
  {
    Dali::Vector2 vec2;

    int count = sscanf( mString.c_str(),"%f,%f",&vec2.x,&vec2.y );
    if( count != 2 )
    {
      DALI_LOG_ERROR("Bad format");
    }
    return vec2;
  }

  Dali::Vector3 GetVector3()
  {
    Dali::Vector3 vec3;

    int count = sscanf( mString.c_str(),"%f,%f,%f",&vec3.x,&vec3.y,&vec3.z );
    if( count != 3 )
    {
      DALI_LOG_ERROR("Bad format");
    }
    return vec3;
  }

  Dali::Vector4 GetVector4()
  {
    Dali::Vector4 vec4;

    int count = sscanf( mString.c_str(),"%f,%f,%f,%f", &vec4.x, &vec4.y, &vec4.z, &vec4.w );
    if( count != 4 )
    {
      DALI_LOG_ERROR("Bad format");
    }
    return vec4;
  }

private:
  std::string mString;

};

void SetProperty( Dali::Handle handle, int propertyId, JsonPropertyValue& propertyValue )
{
  Dali::Property::Type type = handle.GetPropertyType( propertyId );
  switch( type )
  {
  case Dali::Property::FLOAT:
  {
    float val = propertyValue.GetFloat();
    handle.SetProperty( propertyId, Dali::Property::Value( val ) );
    break;
  }
  case Dali::Property::INTEGER:
  {
    int val = propertyValue.GetInt();
    handle.SetProperty( propertyId, Dali::Property::Value( val ) );
    break;
  }
  case Dali::Property::BOOLEAN:
  {
    bool val = propertyValue.GetBoolean();
    handle.SetProperty( propertyId, Dali::Property::Value( val ) );
    break;
  }
  case Dali::Property::STRING:
  {
    std::string str = propertyValue.GetString();
    handle.SetProperty( propertyId, Dali::Property::Value( str ) );
    break;
  }
  case Dali::Property::VECTOR2:
  {
    Dali::Vector2 val = propertyValue.GetVector2();
    handle.SetProperty( propertyId, Dali::Property::Value( val ) );
    break;
  }
  case Dali::Property::VECTOR3:
  {
    Dali::Vector3 val = propertyValue.GetVector3();
    handle.SetProperty( propertyId, Dali::Property::Value( val ) );
    break;
  }
  case Dali::Property::VECTOR4:
  {
    Dali::Vector4 val = propertyValue.GetVector4();
    handle.SetProperty( propertyId, Dali::Property::Value( val ) );
    break;
  }
  default:
  {
    break;
  }
  }
}

int SetProperties( const std::string& setPropertyMessage )
{
  std::istringstream iss( setPropertyMessage );
  std::string token;
  getline( iss, token, '|' ); // swallow command name
  while( getline( iss, token, '|' ) )
  {
    std::string actorId, propName, propValue;
    if( token.compare( "---" ) != 0 )
    {
      std::istringstream propss( token );
      getline( propss, actorId, ';' );
      getline( propss, propName, ';' );
      getline( propss, propValue );

      Dali::Actor root = Dali::Stage::GetCurrent().GetRootLayer();
      int id = atoi( actorId.c_str() );
      Dali::Actor a = root.FindChildById( id );
      if( a )
      {
        // lookup by name for custom properties
        int propId = a.GetPropertyIndex( propName );
        if( propId > 0 )
        {
          JsonPropertyValue pv( propValue );
          SetProperty( a, propId, pv );
        }

      }
    }
  }

  return 0;
}

std::string DumpVector( const Dali::Vector2& vec2 )
{
  std::ostringstream msg;
  msg << vec2 ;
  return msg.str();
}

std::string DumpVector( const Dali::Vector3& vec3 )
{
  std::ostringstream msg;
  msg << vec3;
  return msg.str();
}

std::string DumpVector( const Dali::Vector4& vec4 )
{
  std::ostringstream msg;
  msg << vec4 ;
  return msg.str();
}

std::string DumpRect( const Dali::Rect<int>& r )
{
  std::ostringstream msg;
  msg <<  "[" << r.x << "," << r.y << "," << r.width << "," << r.height << "]";
  return msg.str();
}

std::string DumpMatrix( const Dali::Matrix& matrix )
{
  std::ostringstream msg;
  msg << matrix;
  return msg.str();
}

}; //   un-named namespace

inline std::string Quote( const std::string& in )
{
  return (std::string( "\"" ) + in + std::string( "\"" ));
}

template<class T>
std::string ToString( T i )
{
  std::stringstream ss;
  std::string s;
  ss << i;
  s = ss.str();

  return s;
}

// similar to debug function PropertyValueToString in build-impl.cpp
// however we can't use them because it's in toolkit and only available
// in debug.
// Also currently rotations are output in Euler format ( may change)
std::string GetPropertyValue( Dali::Handle handle, int propertyIndex )
{
  Dali::Property::Type propertyType = handle.GetPropertyType( propertyIndex );
  Dali::Property::Value value = handle.GetProperty( propertyIndex );

  std::string retvalue = "";

  switch( propertyType )
  {
    case Dali::Property::STRING:
    {
      retvalue = (value.Get<std::string>()).c_str();
      break;
    }
    case Dali::Property::VECTOR2:
    {
      Dali::Vector2 v2;
      value.Get( v2 );
      retvalue = DumpVector( v2 );
      break;
    }
    case Dali::Property::VECTOR3:
    {
      Dali::Vector3 v3;
      value.Get( v3 );
      retvalue = DumpVector( v3 );
      break;
    }
    case Dali::Property::VECTOR4:
    {
      Dali::Vector4 v4;
      value.Get( v4 );
      retvalue = DumpVector( v4 );
      break;
    }
    case Dali::Property::MATRIX:
    {
      Dali::Matrix m;
      value.Get( m );
      retvalue = DumpMatrix( m );
      break;
    }
    case Dali::Property::BOOLEAN:
    {
      retvalue = ToString( value.Get<bool>() );
      break;
    }
    case Dali::Property::FLOAT:
    {
      retvalue = ToString( value.Get<float>() );
      break;
    }
    case Dali::Property::INTEGER:
    {
      retvalue = ToString( value.Get<int>() );
      break;
    }
    case Dali::Property::RECTANGLE:
    {
      Dali::Rect<int> rect;
      value.Get( rect );
      retvalue = DumpRect( rect );
      break;
    }
    case Dali::Property::UNSIGNED_INTEGER:
    {
      retvalue = ToString( value.Get<unsigned int>() );
      break;
    }
    case Dali::Property::MATRIX3:
    {
      retvalue =  ToString( value.Get<Dali::Matrix3>() );
      break;
    }
    case Dali::Property::ROTATION:
    {
      Dali::Quaternion q;
      value.Get( q );
      Dali::Vector4 v4 = q.EulerAngles();
      retvalue = DumpVector( v4 );
      break;
    }
    case Dali::Property::ARRAY:
    {
      retvalue = "unsupported ARRAY";
      break;
    }
    case Dali::Property::MAP:
    {
      Dali::Property::Map map;
      value.Get( map );
      std::ostringstream m2;
      m2 << "Map type containing " << map.Count() << " elements";
      retvalue = m2.str();
      break;
    }
    case Dali::Property::TYPE_COUNT:
    {
      retvalue = "unsupported TYPE_COUNT";
      break;
    }
    default:
    {
      std::ostringstream m2;
      m2 << "unsupported type = " << propertyType;
      retvalue = m2.str();
      break;
    }
  }
  return retvalue;
}

bool ExcludeProperty( int propIndex )
{
  return (propIndex == Dali::Actor::Property::Name    ||

      // all of these are repeat properties of values in vectors....
      // We don't really need these in the UI
      propIndex == Dali::Actor::Property::AnchorPointX || propIndex == Dali::Actor::Property::AnchorPointY || propIndex == Dali::Actor::Property::AnchorPointZ || propIndex == Dali::Actor::Property::ParentOriginX
      || propIndex == Dali::Actor::Property::ParentOriginY || propIndex == Dali::Actor::Property::ParentOriginZ || propIndex == Dali::Actor::Property::ColorRed || propIndex == Dali::Actor::Property::ColorGreen
      || propIndex == Dali::Actor::Property::ColorBlue || propIndex == Dali::Actor::Property::ColorAlpha || propIndex == Dali::Actor::Property::PositionX || propIndex == Dali::Actor::Property::PositionY
      || propIndex == Dali::Actor::Property::PositionZ || propIndex == Dali::Actor::Property::SizeWidth || propIndex == Dali::Actor::Property::SizeHeight || propIndex == Dali::Actor::Property::ScaleX || propIndex == Dali::Actor::Property::ScaleY
      || propIndex == Dali::Actor::Property::ScaleZ || propIndex == Dali::Actor::Property::SizeDepth);
}

std::string DumpJson( Dali::Actor actor, int level )
{
  // All the information about this actor
  std::ostringstream msg;
  msg << "{ " << Quote( "Name" ) << " : " << Quote( actor.GetName() ) << ", " << Quote( "level" ) << " : " << level << ", " << Quote( "id" ) << " : " << actor.GetId() << ", " << Quote( "IsVisible" )
      << " : " << actor.IsVisible() << ", " << Quote( "IsSensitive" ) << " : " << actor.IsSensitive();

  msg << ", " << Quote( "properties" ) << ": [ ";

  Dali::Property::IndexContainer indices;
  actor.GetPropertyIndices( indices );

  Dali::Property::IndexContainer::iterator iter = indices.begin();
  int numCustom = 0;
  for( ; iter != indices.end() ; iter++ )
  {
    int i = *iter;
    if( !ExcludeProperty( i ) )
    {
      if( numCustom++ != 0 )
      {
        msg << ", ";
      }
      msg << "[";

      std::string str = actor.GetPropertyName( i );
      msg << Quote( str ) << ",";
      msg << Quote( GetPropertyValue( actor, i ) );

      msg << "]";
    }
  }
  msg << "]";
  msg << ", " << Quote( "children" ) << " : [ ";

  // Recursively dump all the children as well
  for( unsigned int i = 0 ; i < actor.GetChildCount() ; ++i )
  {
    if( i )
    {
      msg << " , ";
    }
    msg << DumpJson( actor.GetChildAt( i ), level + 1 );
  }
  msg << "] }";

  return msg.str();
}

std::string GetActorTree()
{
  Dali::Actor actor = Dali::Stage::GetCurrent().GetRootLayer();
  std::string str = DumpJson( actor, 0 );
  return str;
}
namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace Automation
{

void SetProperty( const std::string& message )
{
  // check the set property length is within range
  if( message.length() > MAX_SET_PROPERTY_STRING_LENGTH )
  {
    DALI_LOG_ERROR("SetProperty message length too long, size = %ul\n", message.length());
    return;
  }

  SetProperties( message );
}

void DumpScene( unsigned int clientId, ClientSendDataInterface* sendData )
{
  char buf[32];
  std::string json = GetActorTree();
  int length = json.length();
  sprintf( buf, "%d\n", length );
  std::string header( buf );
  json = buf + json;
  sendData->SendData( json.c_str(), json.length(), clientId );
}

} // namespace Automation

} // namespace Internal

} // namespace Adaptor

} // namespace Dali
