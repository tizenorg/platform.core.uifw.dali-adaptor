
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

#include "automation.h"

#include <dali/public-api/dali-core.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/integration-api/resource-dump.h>
#include <sstream>
#include <iomanip>
#include <map>
#include <boost/thread/condition_variable.hpp>

using namespace Dali;

boost::condition_variable dumpWaitUpdate;
boost::mutex dumpMutex;

namespace  // un-named namespace
{
std::map<unsigned int, unsigned long> propertyMap;
std::map<unsigned int, std::pair<std::string, std::string> > diffMap;

unsigned long hashFunction(unsigned char * buffer, int len)
{
  unsigned long hash = 5381;
  int c;

  while (len--) {
    c = *buffer++;
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  }
  return hash;
}

class PropValue
{
public:
  PropValue( std::string str )
      : mString( str )
  {
    std::replace( mString.begin(), mString.end(), '[', ' ' );
    std::replace( mString.begin(), mString.end(), ']', ' ' );
  }
  std::string getString()
  {
    return mString;
  }
  float getFloat()
  {
    return atof( mString.c_str() );
  }
  int getInt()
  {
    return atoi( mString.c_str() );
  }
  bool getBoolean()
  {
    return (getInt() != 0);
  }

  Dali::Vector2 getVector2()
  {
    std::istringstream iss( mString );
    std::string xstr, ystr;
    getline( iss, xstr, ',' );
    getline( iss, ystr, ',' );
    float x = (float) atof( xstr.c_str() );
    float y = (float) atof( ystr.c_str() );
    return Dali::Vector2( x, y );
  }

  Dali::Vector3 getVector3()
  {
    std::istringstream iss( mString );
    std::string xstr, ystr, zstr;
    getline( iss, xstr, ',' );
    getline( iss, ystr, ',' );
    getline( iss, zstr, ',' );
    float x = (float) atof( xstr.c_str() );
    float y = (float) atof( ystr.c_str() );
    float z = (float) atof( zstr.c_str() );
    return Dali::Vector3( x, y, z );
  }

  Dali::Vector4 getVector4()
  {
    std::istringstream iss( mString );
    std::string xstr, ystr, zstr, wstr;
    getline( iss, xstr, ',' );
    getline( iss, ystr, ',' );
    getline( iss, zstr, ',' );
    getline( iss, wstr, ',' );
    float x = (float) atof( xstr.c_str() );
    float y = (float) atof( ystr.c_str() );
    float z = (float) atof( zstr.c_str() );
    float w = (float) atof( wstr.c_str() );
    return Dali::Vector4( x, y, z, w );
  }

private:
  std::string mString;

};

void setProperty( Actor a, int propId, PropValue& pv )
{
  Property::Type type = a.GetPropertyType( propId );
  switch( type )
  {
  case Property::FLOAT:
  {
    float val = pv.getFloat();
    a.SetProperty( propId, Property::Value( val ) );
    break;
  }
  case Property::INTEGER:
  {
    int val = pv.getInt();
    a.SetProperty( propId, Property::Value( val ) );
    break;
  }
  case Property::BOOLEAN:
  {
    bool val = pv.getBoolean();
    a.SetProperty( propId, Property::Value( val ) );
    break;
  }
  case Property::STRING:
  {
    std::string str = pv.getString();
    a.SetProperty( propId, Property::Value( str ) );
    break;
  }
  case Property::VECTOR2:
  {
    Dali::Vector2 val = pv.getVector2();
    a.SetProperty( propId, Property::Value( val ) );
    break;
  }
  case Property::VECTOR3:
  {
    Dali::Vector3 val = pv.getVector3();
    a.SetProperty( propId, Property::Value( val ) );
    break;
  }
  case Property::VECTOR4:
  {
    Dali::Vector4 val = pv.getVector4();
    a.SetProperty( propId, Property::Value( val ) );
    break;
  }
  default:
  {
    break;
  }
  }
}

int setProperties( std::string setPropsMsg )
{
  std::istringstream iss( setPropsMsg );
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
          PropValue pv( propValue );
          setProperty( a, propId, pv );
        }

      }
    }
  }

  return 0;
}

std::string dumpVector( Vector2& v )
{
  std::ostringstream msg;
  msg << std::setprecision( 4 ) << "[" << v.x << "," << v.y << "]";
  return msg.str();
}

std::string dumpVector( Vector3& v )
{
  std::ostringstream msg;
  msg << std::setprecision( 4 ) << "[" << v.x << "," << v.y << "," << v.z << "]";
  return msg.str();
}

std::string dumpVector( Vector4& v )
{
  std::ostringstream msg;
  msg << std::setprecision( 4 ) << "[" << v.r << "," << v.g << "," << v.b << "," << v.a << "]";
  return msg.str();
}

std::string dumpRect( Rect<int>& r )
{
  std::ostringstream msg;
  msg << std::setprecision( 4 ) << "[" << r.x << "," << r.y << "," << r.width << "," << r.height << "]";
  return msg.str();
}

std::string dumpRect( Rect<float>& r )
{
  std::ostringstream msg;
  msg << std::setprecision( 4 ) << "[" << r.x << "," << r.y << "," << r.width << "," << r.height << "]";
  return msg.str();
}

std::string dumpMatrix( Matrix& m )
{
  std::ostringstream msg;
  msg << std::setprecision( 4 );
  msg << m;
  return msg.str();
}

}
;
//   un-named namespace

inline std::string quote( std::string in )
{
  return (std::string( "\"" ) + in + std::string( "\"" ));
}

inline std::string tostr( int val )
{
  return static_cast<std::ostringstream*>( &(std::ostringstream() << (val)) )->str();
}

inline void dumpTextActor( std::ostringstream& msg, TextActor ta )
{
  msg << ", " << quote( "Text" ) << " : " << quote( ta.GetText() );
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

std::string getPropertyValue( Actor a, int propIndex )
{
  Property::Type t = a.GetPropertyType( propIndex );
  Property::Value val = a.GetProperty( propIndex );
  Vector2 v2;
  Vector3 v3;
  Vector4 v4;
  Quaternion q;
  Matrix m;
  Rect<int> rect;
  Property::Map map;
  std::string retvalue = "";

  switch( t )
  {
  case Property::STRING:
    retvalue = (val.Get<std::string>()).c_str();
    break;
  case Property::VECTOR2:
    val.Get( v2 );
    retvalue = dumpVector( v2 );
    break;
  case Property::VECTOR3:
    val.Get( v3 );
    retvalue = dumpVector( v3 );
    break;
  case Property::VECTOR4:
    val.Get( v4 );
    retvalue = dumpVector( v4 );
    break;
  case Property::MATRIX:
    val.Get( m );
    retvalue = dumpMatrix( m );
    break;
  case Property::BOOLEAN:
    retvalue = t_to_string( val.Get<bool>() );
    break;
  case Property::FLOAT:
    retvalue = t_to_string( val.Get<float>() );
    break;
  case Property::INTEGER:
    retvalue = t_to_string( val.Get<int>() );
    break;
  case Property::RECTANGLE:
    val.Get( rect );
    retvalue = dumpRect( rect );
    break;
  case Property::UNSIGNED_INTEGER:
    retvalue = t_to_string( val.Get<unsigned int>() );
    break;
  case Property::MATRIX3:
    retvalue = "unsupported MATRIX3";
    break;
  case Property::ROTATION:
    val.Get( q );
    v4 = q.EulerAngles();
    retvalue = dumpVector( v4 );
    break;
  case Property::ARRAY:
    retvalue = "unsupported ARRAY";
    break;
  case Property::MAP:
  {
    val.Get( map );
    std::ostringstream m2;
    m2 << "Map type containing " << map.size() << " elements";
    retvalue = m2.str();
  }
    break;
  case Property::TYPE_COUNT:
    retvalue = "unsupported TYPE_COUNT";
    break;
  default:
  {
    std::ostringstream m2;
    m2 << "unsupported type = " << t;
    retvalue = m2.str();
  }
    break;
  }
  return retvalue;
}

unsigned long propHashValue(Actor a, int propIndex)
{
  Property::Type t = a.GetPropertyType( propIndex );
  Property::Value val = a.GetProperty( propIndex );
  Vector2 v2;
  Vector3 v3;
  Vector4 v4;
  Quaternion q;
  Matrix m;
  Rect<int> rect;

  unsigned long newHashValue = 0;

  switch( t )
  {
  case Property::STRING:
  {
    std::string str = val.Get<std::string>();
    newHashValue = hashFunction((unsigned char*)str.c_str(), str.length());
    break;
  }
  case Property::VECTOR2:
    val.Get( v2 );
    newHashValue = hashFunction((unsigned char*)&v2[0], sizeof(v2));
    break;
  case Property::VECTOR3:
    val.Get( v3 );
    newHashValue = hashFunction((unsigned char*)&v3[0], sizeof(v3));
    break;
  case Property::VECTOR4:
    val.Get( v4 );
    newHashValue = hashFunction((unsigned char*)&v4[0], sizeof(v4));
    break;
  case Property::MATRIX:
    val.Get( m );
    newHashValue = hashFunction((unsigned char*)m.getMatrix(), sizeof(float)*16);
    break;
  case Property::BOOLEAN:
    newHashValue = (unsigned int)val.Get<bool>();
    break;
  case Property::FLOAT:
    newHashValue = (unsigned int)val.Get<float>();
    break;
  case Property::INTEGER:
    newHashValue = (unsigned int)val.Get<int>();
    break;
  case Property::RECTANGLE:
    val.Get( rect );
    newHashValue = hashFunction((unsigned char*)&rect, sizeof(rect));
    break;
  case Property::UNSIGNED_INTEGER:
    newHashValue = val.Get<unsigned int>();
    break;
  case Property::MATRIX3:
    newHashValue = 0;
    break;
  case Property::ROTATION:
    val.Get( q );
    v4 = q.EulerAngles();
    newHashValue = hashFunction((unsigned char*)&v4[0], sizeof(v4));
    break;
  case Property::ARRAY:
    newHashValue = 0;
    break;
  case Property::MAP:
    newHashValue = 0;
    break;
  case Property::TYPE_COUNT:
    newHashValue = 0;;
    break;
  default:
    newHashValue = 0;
    break;
  }
  return newHashValue;
}

void updatePropertyHash( Actor a, int propIndex )
{
  unsigned int id = a.GetId() << 16;
  id += propIndex;
  std::map<unsigned int, unsigned long>::iterator iter = propertyMap.find(id);
  unsigned long currentHashValue = 0;
  if (iter != propertyMap.end()) {
    currentHashValue = (*iter).second;
  }
  unsigned long newHashValue = propHashValue(a, propIndex);

  if (currentHashValue != newHashValue)
  {
    propertyMap[id] = newHashValue;
  }
}


void updatePropertyDiffMap( Actor a, int propIndex )
{
  unsigned int id = a.GetId() << 16;
  id += propIndex;
  std::map<unsigned int, unsigned long>::iterator iter = propertyMap.find(id);
  unsigned long currentHashValue = 0;
  if (iter != propertyMap.end()) {
    currentHashValue = (*iter).second;
  }
  unsigned long newHashValue = propHashValue(a, propIndex);

  if (currentHashValue != newHashValue)
  {
    propertyMap[id] = newHashValue;
    diffMap[id] = std::pair<std::string, std::string>(a.GetPropertyName(propIndex), getPropertyValue(a, propIndex));
  }
}

bool excludeProperty( int propIndex )
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

std::string dumpJson( Actor a, int level)
{
  // All the information about this actor
  std::ostringstream msg;
  msg << "{ " << quote( "Name" ) << " : " << quote( a.GetName().c_str() ) << ", "  << quote( "id" ) << " : " << a.GetId();

  msg << ", " << quote( "properties" ) << ": [ ";

  Property::IndexContainer indices;
  a.GetPropertyIndices( indices );

  Property::IndexContainer::iterator iter = indices.begin();
  int numProperties = 0;
  for( ; iter != indices.end() ; iter++ )
  {
    int i = *iter;
    if( !excludeProperty( i ) )
    {
      if( numProperties++ != 0 )
      {
        msg << ", ";
      }

      updatePropertyHash(a, i);

      msg << "[";

      std::string str = a.GetPropertyName( i );
      msg << quote( str ) << ",";
      msg << quote( getPropertyValue( a, i ) );

      msg << "]";
    }
  }
  msg << "]";
  msg << ", " << quote( "resources" ) << " : [ ";
  ImageActor ia = ImageActor::DownCast(a);
  if (ia) {
      Dali::Image i = ia.GetImage();
      Dali::Integration::ResourceId rid = i.GetResourceId();
      msg << "[" << quote("resourceId") << "," << rid << "]";
  }
  msg << "]";

  msg << ", " << quote( "children" ) << " : [ ";

  // Recursively dump all the children as well
  for( unsigned int i = 0 ; i < a.GetChildCount() ; ++i )
  {
    if( i )
    {
      msg << " , ";
    }
    msg << dumpJson( a.GetChildAt( i ), level + 1);
  }
  msg << "] }";

  return msg.str();
}

void calcDiff( Actor a )
{
  Property::IndexContainer indices;
  a.GetPropertyIndices( indices );

  Property::IndexContainer::iterator iter = indices.begin();
  for( ; iter != indices.end() ; iter++ )
  {
    int i = *iter;
    if( !excludeProperty( i ) )
    {
      updatePropertyDiffMap(a, i);
    }
  }
  for( unsigned int i = 0 ; i < a.GetChildCount() ; ++i )
  {
    calcDiff( a.GetChildAt( i ));
  }
}


std::string getActorTree(bool delta)
{
  Actor a = Dali::Stage::GetCurrent().GetRootLayer();
  std::string str = dumpJson( a, 0);
  str += "\n";
  return str;
}

std::string getDiffTree()
{
  Actor a = Dali::Stage::GetCurrent().GetRootLayer();
  diffMap.clear();
  calcDiff( a);
  std::ostringstream msg;
  std::map<unsigned int, std::pair<std::string, std::string> >::iterator iter;
  for (iter=diffMap.begin();iter!=diffMap.end();iter++)
  {
    std::pair<unsigned int, std::pair<std::string, std::string> > val = *iter;
    unsigned int id = val.first;
    unsigned int propId = id & 0xffff;
    id = id >> 16;
    msg << id << " | " << propId << " | " << val.second.first << " | " << val.second.second << std::endl;
  }
  return msg.str();
}

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace Automation
{

void SetProperty( std::string message )
{
  setProperties( message );
}

void DumpScene( unsigned int clientId, ClientSendDataInterface* sendData )
{
  char buf[32];
  std::string json = getActorTree(true);
  int length = json.length();
  sprintf( buf, "%d\n", length );
  std::string header( buf );
  json = buf + json;
  sendData->SendData( json.c_str(), json.length(), clientId );
}

void DumpFrame( unsigned int clientId, ClientSendDataInterface* sendData )
{
  char buf[128];
  std::string diffs = getDiffTree();
  int length = diffs.length();
  sprintf( buf, "%d\n", length );
  std::string header( buf );
  diffs = buf + diffs;
  sendData->SendData( diffs.c_str(), diffs.length(), clientId );
}

void GetResource(unsigned int clientId, ClientSendDataInterface* sendData, unsigned int id )
{
  Integration::Bitmap* bmp = Dali::Integration::GetBitmapResource(id);
  std::string str("doesn't exist");
  if (bmp != NULL) {
    char buf[256];
    unsigned int sz = bmp->GetBufferSize();
    Pixel::Format fmt = bmp->GetPixelFormat();
    sprintf(buf, "%d %d %d\n%d\n", bmp->GetImageWidth(), bmp->GetImageHeight(), fmt, sz);
    str = std::string(buf);
    PixelBuffer* buffer = bmp->GetBuffer();
    std::string output((const char*)buffer, sz);
    str += output;
  }
  sendData->SendData( str.c_str(), str.length(), clientId );
}

void ClearDifferences()
{
  propertyMap.clear();
}

} // namespace Automation

} // namespace Internal

} // namespace Adaptor

} // namespace Dali
