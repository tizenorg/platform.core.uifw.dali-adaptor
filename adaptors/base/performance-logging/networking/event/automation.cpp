
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

//INTERNAL INCLUDES
#include <dali/public-api/dali-core.h>
#include <dali/integration-api/debug.h>

// EXTERNAL INCLUDES
#include <sstream>
#include <iomanip>

using namespace Dali;

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

void SetProperty( Actor actor, int propertyId, JsonPropertyValue& propertyValue )
{
  Property::Type type = actor.GetPropertyType( propertyId );
  switch( type )
  {
  case Property::FLOAT:
  {
    float val = propertyValue.GetFloat();
    actor.SetProperty( propertyId, Property::Value( val ) );
    break;
  }
  case Property::INTEGER:
  {
    int val = propertyValue.GetInt();
    actor.SetProperty( propertyId, Property::Value( val ) );
    break;
  }
  case Property::BOOLEAN:
  {
    bool val = propertyValue.GetBoolean();
    actor.SetProperty( propertyId, Property::Value( val ) );
    break;
  }
  case Property::STRING:
  {
    std::string str = propertyValue.GetString();
    actor.SetProperty( propertyId, Property::Value( str ) );
    break;
  }
  case Property::VECTOR2:
  {
    Dali::Vector2 val = propertyValue.GetVector2();
    actor.SetProperty( propertyId, Property::Value( val ) );
    break;
  }
  case Property::VECTOR3:
  {
    Dali::Vector3 val = propertyValue.GetVector3();
    actor.SetProperty( propertyId, Property::Value( val ) );
    break;
  }
  case Property::VECTOR4:
  {
    Dali::Vector4 val = propertyValue.GetVector4();
    actor.SetProperty( propertyId, Property::Value( val ) );
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

      Actor root = Stage::GetCurrent().GetRootLayer();
      int id = atoi( actorId.c_str() );
      Actor a = root.FindChildById( id );
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

std::string DumpVector( const Vector2& vec2 )
{
  std::ostringstream msg;
  msg << std::setprecision( 4 ) << "[" << vec2.x << "," << vec2.y << "]";
  return msg.str();
}

std::string DumpVector( const Vector3& vec3 )
{
  std::ostringstream msg;
  msg << std::setprecision( 4 ) << "[" << vec3.x << "," << vec3.y << "," << vec3.z << "]";
  return msg.str();
}

std::string DumpVector( const Vector4& vec4 )
{
  std::ostringstream msg;
  msg << std::setprecision( 4 ) << "[" << vec4.r << "," << vec4.g << "," << vec4.b << "," << vec4.a << "]";
  return msg.str();
}

std::string DumpRect( const Rect<int>& r )
{
  std::ostringstream msg;
  msg << std::setprecision( 4 ) << "[" << r.x << "," << r.y << "," << r.width << "," << r.height << "]";
  return msg.str();
}

std::string DumpMatrix( const Matrix& matrix )
{
  std::ostringstream msg;
  msg << std::setprecision( 4 );
  msg << matrix;
  return msg.str();
}

}
;
//   un-named namespace

inline std::string Quote( const std::string& in )
{
  return (std::string( "\"" ) + in + std::string( "\"" ));
}


template<class T>
std::string t_to_string( T i )
{
  std::stringstream ss;
  std::string s;
  ss << i;
  s = ss.str();

  return s;
}

std::string GetPropertyValue( Actor actor, int propertyIndex )
{
  Property::Type propertyType = actor.GetPropertyType( propertyIndex );
  Property::Value value = actor.GetProperty( propertyIndex );
  Vector2 v2;
  Vector3 v3;
  Vector4 v4;
  Quaternion q;
  Matrix m;
  Rect<int> rect;
  Property::Map map;
  std::string retvalue = "";

  switch( propertyType )
  {
  case Property::STRING:
    retvalue = (value.Get<std::string>()).c_str();
    break;
  case Property::VECTOR2:
    value.Get( v2 );
    retvalue = DumpVector( v2 );
    break;
  case Property::VECTOR3:
    value.Get( v3 );
    retvalue = DumpVector( v3 );
    break;
  case Property::VECTOR4:
    value.Get( v4 );
    retvalue = DumpVector( v4 );
    break;
  case Property::MATRIX:
    value.Get( m );
    retvalue = DumpMatrix( m );
    break;
  case Property::BOOLEAN:
    retvalue = t_to_string( value.Get<bool>() );
    break;
  case Property::FLOAT:
    retvalue = t_to_string( value.Get<float>() );
    break;
  case Property::INTEGER:
    retvalue = t_to_string( value.Get<int>() );
    break;
  case Property::RECTANGLE:
    value.Get( rect );
    retvalue = DumpRect( rect );
    break;
  case Property::UNSIGNED_INTEGER:
    retvalue = t_to_string( value.Get<unsigned int>() );
    break;
  case Property::MATRIX3:
    retvalue = "unsupported MATRIX3";
    break;
  case Property::ROTATION:
    value.Get( q );
    v4 = q.EulerAngles();
    retvalue = DumpVector( v4 );
    break;
  case Property::ARRAY:
    retvalue = "unsupported ARRAY";
    break;
  case Property::MAP:
  {
    value.Get( map );
    std::ostringstream m2;
    m2 << "Map type containing " << map.Count() << " elements";
    retvalue = m2.str();
  }
    break;
  case Property::TYPE_COUNT:
    retvalue = "unsupported TYPE_COUNT";
    break;
  default:
  {
    std::ostringstream m2;
    m2 << "unsupported type = " << propertyType;
    retvalue = m2.str();
  }
    break;
  }
  return retvalue;
}

bool ExcludeProperty( int propIndex )
{
  return (propIndex == Dali::Actor::NAME    ||

      // all of these are repeat properties of values in vectors....
      // We don't really need these in the UI
      propIndex == Dali::Actor::ANCHOR_POINT_X || propIndex == Dali::Actor::ANCHOR_POINT_Y || propIndex == Dali::Actor::ANCHOR_POINT_Z || propIndex == Dali::Actor::PARENT_ORIGIN_X
      || propIndex == Dali::Actor::PARENT_ORIGIN_Y || propIndex == Dali::Actor::PARENT_ORIGIN_Z || propIndex == Dali::Actor::COLOR_RED || propIndex == Dali::Actor::COLOR_GREEN
      || propIndex == Dali::Actor::COLOR_BLUE || propIndex == Dali::Actor::COLOR_ALPHA || propIndex == Dali::Actor::POSITION_X || propIndex == Dali::Actor::POSITION_Y
      || propIndex == Dali::Actor::POSITION_Z || propIndex == Dali::Actor::SIZE_WIDTH || propIndex == Dali::Actor::SIZE_HEIGHT || propIndex == Dali::Actor::SCALE_X || propIndex == Dali::Actor::SCALE_Y
      || propIndex == Dali::Actor::SCALE_Z || propIndex == Dali::Actor::SIZE_DEPTH);
}

std::string DumpJson( Actor actor, int level )
{
  // All the information about this actor
  std::ostringstream msg;
  msg << "{ " << Quote( "Name" ) << " : " << Quote( actor.GetName() ) << ", " << Quote( "level" ) << " : " << level << ", " << Quote( "id" ) << " : " << actor.GetId() << ", " << Quote( "IsVisible" )
      << " : " << actor.IsVisible() << ", " << Quote( "IsSensitive" ) << " : " << actor.IsSensitive();

  msg << ", " << Quote( "properties" ) << ": [ ";

  Property::IndexContainer indices;
  actor.GetPropertyIndices( indices );

  Property::IndexContainer::iterator iter = indices.begin();
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
  Actor actor = Dali::Stage::GetCurrent().GetRootLayer();
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
