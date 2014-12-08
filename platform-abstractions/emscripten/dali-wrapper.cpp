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


// EXTERNAL INCLUDES
#include <sstream>
#include <dali/public-api/scripting/scripting.h>
#include "emscripten/emscripten.h"
#include "emscripten/bind.h"
#include "emscripten/val.h"

// INTERNAL INCLUDES
#include "sdl-application.h"
#include "actor-wrapper.h"
#include "animation-wrapper.h"
#include "emscripten-callbacks.h"
#include "handle-wrapper.h"
#include "layer-wrapper.h"
#include "material-wrapper.h"
#include "mesh-data-wrapper.h"
#include "property-map-wrapper.h"
#include "render-task-wrapper.h"
#include "sdl-platform-abstraction.h"
#include "shader-effect-wrapper.h"
#include "stage-wrapper.h"
#include "type-info-wrapper.h"
#include "type-registry-wrapper.h"

////////////////////////////////////////////////////////////////////////////////
//
// The browser javascript wrapper consists of this cpp file and a counterpart
// javascript file.
//
// They work in tandem to help make the interface natural and convenient
// for a Javascript programmer.
//
// Unfortunately there is no finalize/destruction in javascript so any cpp
// objects created must be explicitly deleted. Sometimes this wrapper can
// hide the details and simple types can be immediately marshalled to and
// from javascript values. More often objects created must have
// object.delete() called at the correct time.
//
////////////////////////////////////////////////////////////////////////////////

extern void EmscriptenMouseEvent(double x, double y, int mouseIsDown);
extern void EmscriptenUpdateOnce();
extern void EmscriptenRenderOnce();

namespace Dali
{
namespace Internal
{
namespace Emscripten
{

// Javascript callbacks
extern emscripten::val JSGetGlyphImage;
extern emscripten::val JSGetImage;
extern emscripten::val JSGetImageMetaData;
extern emscripten::val JSRenderFinished;

using namespace emscripten;


namespace
{

template <typename T>
std::string ToString(const T& value)
{
  std::stringstream ss;
  ss << value;
  return ss.str();
}

template <>
std::string ToString(const Dali::Rect<int>& value)
{
  std::stringstream ss;
  ss << value.x << "," << value.y << "," << value.width << "," << value.height;
  return ss.str();
}


std::string PropertyValueToString( const Dali::Property::Value& value )
{
  std::string ret;

  switch( value.GetType() )
  {
    case Dali::Property::NONE:
    {
      ret = "NONE";
      break;
    }            ///< No type
    case Dali::Property::BOOLEAN:
    {
      ret = value.Get<bool>() ? "True" : "False";
      break;
    }
    case Dali::Property::FLOAT:
    {
      ret = ToString( value.Get<float>() );
      break;
    }
    case Dali::Property::INTEGER:
    {
      ret = ToString( value.Get<int>() );
      break;
    }
    case Dali::Property::UNSIGNED_INTEGER:
    {
      ret = ToString( value.Get<unsigned int>() );
      break;
    }
    case Dali::Property::VECTOR2:
    {
      ret = ToString( value.Get<Dali::Vector2>() );
      break;
    }
    case Dali::Property::VECTOR3:
    {
      ret = ToString( value.Get<Dali::Vector3>() );
      break;
    }
    case Dali::Property::VECTOR4:
    {
      ret = ToString( value.Get<Dali::Vector4>() );
      break;
    }
    case Dali::Property::MATRIX3:
    {
      ret = ToString( value.Get<Dali::Matrix3>() );
      break;
    }
    case Dali::Property::MATRIX:
    {
      ret = ToString( value.Get<Dali::Matrix>() );
      break;
    }
    case Dali::Property::RECTANGLE:
    {
      ret = ToString( value.Get< Dali::Rect<int> >() );
      break;
    }
    case Dali::Property::ROTATION:
    {
      break;
    }
    case Dali::Property::STRING:
    {
      ret = value.Get<std::string>();
      break;
    }
    case Dali::Property::ARRAY:
    {
      ret = std::string("Array Size=") + ToString( value.GetSize() );
      break;
    }
    case Dali::Property::MAP:
    {
      ret = std::string("Map Size=") + ToString( value.GetSize() );
      break;
    }
    case Dali::Property::TYPE_COUNT:
    {
      ret = "";
      break;
    }
  }

  return ret;
}



ActorWrapper CreateActor(const std::string& name)
{
  ActorWrapper ret;

  Dali::TypeRegistry registry = Dali::TypeRegistry::Get();

  Dali::TypeInfo typeInfo = registry.GetTypeInfo( name );

  if(!typeInfo)
  {
    EM_ASM( throw "Invalid type name" );
  }
  else
  {
    Dali::BaseHandle handle = typeInfo.CreateInstance();

    if( Dali::Actor actor = Dali::Actor::DownCast(handle) )
    {
      ret = ActorWrapper( actor );
    }
  }

  return ret;
}

HandleWrapper CreateAny(const std::string& name)
{
  HandleWrapper ret;

  Dali::TypeRegistry registry = Dali::TypeRegistry::Get();

  Dali::TypeInfo typeInfo = registry.GetTypeInfo( name );

  if(!typeInfo)
  {
    EM_ASM( throw "Invalid type name" );
  }
  else
  {
    Dali::BaseHandle base = typeInfo.CreateInstance();

    Dali::Handle handle = Dali::Handle::DownCast(base);

    ret = HandleWrapper( handle );
  }

  return ret;
}

/**
 * The functor to be used in the hit-test algorithm to check whether the actor is hittable.
 */
bool IsActorHittableFunction(Dali::Actor actor, Dali::HitTestAlgorithm::TraverseType type)
{
  const std::string& name = actor.GetName();
  // by convention if not visible, or with a * starting the name or if the root layer
  return actor.IsVisible() && (name.size() ? name[0] != '*' : true);
};


ActorWrapper HitTest(float x, float y)
{
  Dali::HitTestAlgorithm::Results results;
  Dali::HitTestAlgorithm::HitTest( Dali::Stage::GetCurrent(), Dali::Vector2(x,y), results, IsActorHittableFunction );
  return ActorWrapper( results.actor );
}

ActorWrapper CreateMeshActor(MeshDataWrapper& wrapper)
{
  Dali::Mesh mesh = Dali::Mesh::New( wrapper.meshData );

  return ActorWrapper( Dali::MeshActor::New(mesh) );
}

const unsigned int MAX_BORDER_SIZE( 9 );

ActorWrapper CreateSolidColorActor( const Dali::Vector4& color, bool border, const Dali::Vector4& borderColor, const unsigned int borderSize )
{
  Dali::ImageActor image;
  if( borderSize > MAX_BORDER_SIZE )
  {
    return ActorWrapper(image);
  }

  const unsigned int bitmapWidth = borderSize * 2 + 2;

  // Using a (2 + border) x (2 + border) image gives a better blend with the GL implementation
  // than a (1 + border) x (1 + border) image
  const unsigned int bitmapSize = bitmapWidth * bitmapWidth;
  const unsigned int topLeft = bitmapWidth * borderSize + borderSize;
  const unsigned int topRight = topLeft + 1;
  const unsigned int bottomLeft = bitmapWidth * (borderSize + 1) + borderSize;
  const unsigned int bottomRight = bottomLeft + 1;

  Dali::BufferImage imageData;
  if(color.a != 1.0 || borderColor.a != 1.0)
  {
    imageData = Dali::BufferImage::New( bitmapWidth, bitmapWidth, Dali::Pixel::RGBA8888 );

    // Create the image
    Dali::PixelBuffer* pixbuf = imageData.GetBuffer();
    Dali::Vector4 outerColor = color;
    if ( border )
    {
      outerColor = borderColor;
    }

    for( size_t i = 0; i < bitmapSize; ++i )
    {
      if( i == topLeft ||
          i == topRight ||
          i == bottomLeft ||
          i == bottomRight )
      {
        pixbuf[i*4+0] = 0xFF * color.r;
        pixbuf[i*4+1] = 0xFF * color.g;
        pixbuf[i*4+2] = 0xFF * color.b;
        pixbuf[i*4+3] = 0xFF * color.a;
      }
      else
      {
        pixbuf[i*4+0] = 0xFF * outerColor.r;
        pixbuf[i*4+1] = 0xFF * outerColor.g;
        pixbuf[i*4+2] = 0xFF * outerColor.b;
        pixbuf[i*4+3] = 0xFF * outerColor.a;
      }
    }
  }
  else
  {
    imageData = Dali::BufferImage::New( bitmapWidth, bitmapWidth, Dali::Pixel::RGB888 );

    // Create the image
    Dali::PixelBuffer* pixbuf = imageData.GetBuffer();
    Dali::Vector4 outerColor = color;
    if ( border )
    {
      outerColor = borderColor;
    }

    for( size_t i = 0; i < bitmapSize; ++i )
    {
      if( i == topLeft ||
          i == topRight ||
          i == bottomLeft ||
          i == bottomRight )
      {
        pixbuf[i*3+0] = 0xFF * color.r;
        pixbuf[i*3+1] = 0xFF * color.g;
        pixbuf[i*3+2] = 0xFF * color.b;
      }
      else
      {
        pixbuf[i*3+0] = 0xFF * outerColor.r;
        pixbuf[i*3+1] = 0xFF * outerColor.g;
        pixbuf[i*3+2] = 0xFF * outerColor.b;
      }
    }
  }

  imageData.Update();
  image = Dali::ImageActor::New( imageData );
  image.SetAnchorPoint( Dali::AnchorPoint::CENTER );
  image.SetParentOrigin( Dali::ParentOrigin::CENTER );

  if( border )
  {
    image.SetStyle( Dali::ImageActor::STYLE_NINE_PATCH );
    image.SetNinePatchBorder( Dali::Vector4::ONE * (float)borderSize * 2.0f );
  }

  return ActorWrapper(image);
}

} // anon namespace

StageWrapper GetStage()
{
  return StageWrapper();
}

TypeRegistryWrapper GetTypeRegistry()
{
  return TypeRegistryWrapper();
}

bool HasKey(const emscripten::val& value, const std::string& key)
{
  val keys = val::global("Object").call<val>("keys", value);
  int keyLength = keys["length"].as<int>();
  for( int j = 0; j < keyLength; ++j )
  {
    if( keys[j].as<std::string>() == key )
    {
      printf("has key '%s'zn", keys[j].as<std::string>().c_str());
      return true;
    }
  }
  return false;
}

void RecursiveSetProperty(Dali::Property::Value& propertyValue, const emscripten::val& fromVal)
{
  static const std::string number("number"); // we could maybe just check the first three chars? (avoiding null ...if that's returned)
  const std::string fromType( fromVal.typeof().as<std::string>() );

  if( fromType == "object" )
  {
    // hasOwnProperty is the only way I can find to tell if the object is an array
    // (keys in HasKey returns the JS keys "0","1","2","3",...)
    if( fromVal.hasOwnProperty("length") )
    {
      int length = fromVal["length"].as<int>();
      // we can't tell if what the user of the property value wants with a JS Array
      // by default 'standard' length arrays are always interpreted as Vector2/3/4 etc
      // If the user specifically wants an array they must recast.
      bool isArray = false; // nested ie [ [1,2,3], [4,5,6] ]
      if( 4 == length )
      {
        if( number == fromVal["0"].typeof().as<std::string>() &&
            number == fromVal["1"].typeof().as<std::string>() &&
            number == fromVal["2"].typeof().as<std::string>() &&
            number == fromVal["3"].typeof().as<std::string>() )
        {
          propertyValue = Dali::Vector4( fromVal["0"].as<float>(),
                                         fromVal["1"].as<float>(),
                                         fromVal["2"].as<float>(),
                                         fromVal["3"].as<float>() );
        }
        else
        {
          isArray = true;
        }
      }
      else if( 3 == length )
      {
        if( number == fromVal["0"].typeof().as<std::string>() &&
            number == fromVal["1"].typeof().as<std::string>() &&
            number == fromVal["2"].typeof().as<std::string>() )
        {
          propertyValue = Dali::Vector3( fromVal["0"].as<float>(),
                                         fromVal["1"].as<float>(),
                                         fromVal["2"].as<float>() );
        }
        else
        {
          isArray = true;
        }
      }
      else if( 2 == length )
      {
        if( number == fromVal["0"].typeof().as<std::string>() &&
            number == fromVal["1"].typeof().as<std::string>() )
        {
          propertyValue = Dali::Vector2( fromVal["0"].as<float>(),
                                         fromVal["1"].as<float>() );
        }
        else
        {
          isArray = true;
        }
      }
      else
      {
        isArray = true;
      }

      if( isArray )
      {
        propertyValue = Dali::Property::Value(Dali::Property::ARRAY);
        for( int j = 0; j < length; ++j )
        {
          Dali::Property::Value add;
          propertyValue.AppendItem( add );

          std::stringstream ss;
          ss << j;

          emscripten::val val = fromVal[ ss.str() ];

          RecursiveSetProperty( propertyValue.GetItem(j), val );
        }
      }
    }
    else
    {
      propertyValue = Dali::Property::Value(Dali::Property::MAP);
      val keys = val::global("Object").call<val>("keys", fromVal);
      int keyLength = keys["length"].as<int>();
      for( int j = 0; j < keyLength; ++j )
      {
        Dali::Property::Value add;
        std::string key = keys[j].as<std::string>();
        propertyValue.SetValue( key , add );
        val keyVal = fromVal[ key ];
        RecursiveSetProperty( propertyValue.GetValue( key ), keyVal );
      }
    }
  }
  else if( fromType == "number" )
  {
    propertyValue = Dali::Property::Value( fromVal.as<float>() );
  }
  else if( fromType == "string" )
  {
    propertyValue = Dali::Property::Value( fromVal.as<std::string>() );
  }
  else
  {
    assert(0);
  }

}

emscripten::val JavascriptValue( const Dali::Property::Value& v )
{
  switch( v.GetType() )
  {
    case Dali::Property::BOOLEAN:
    {
      return emscripten::val(v.Get<bool>());
      break;
    }
    case Dali::Property::FLOAT:
    {
      return emscripten::val(v.Get<float>());
      break;
    }
    case Dali::Property::INTEGER:
    {
      return emscripten::val(v.Get<int>());
      break;
    }
    case Dali::Property::UNSIGNED_INTEGER:
    {
      return emscripten::val(v.Get<unsigned int>());
      break;
    }
    case Dali::Property::VECTOR2:
    {
      Dali::Vector2 in = v.Get<Dali::Vector2>();
      emscripten::val out = emscripten::val::array();
      out.set("0", emscripten::val(in.x) );
      out.set("1", emscripten::val(in.y) );
      return out;
      break;
    }
    case Dali::Property::VECTOR3:
    {
      Dali::Vector3 in = v.Get<Dali::Vector3>();
      emscripten::val out = emscripten::val::array();
      out.set("0", emscripten::val(in.x) );
      out.set("1", emscripten::val(in.y) );
      out.set("2", emscripten::val(in.z) );
      return out;
      break;
    }
    case Dali::Property::VECTOR4:
    {
      Dali::Vector4 in = v.Get<Dali::Vector4>();
      emscripten::val out = emscripten::val::array();
      out.set("0", emscripten::val(in.x) );
      out.set("1", emscripten::val(in.y) );
      out.set("2", emscripten::val(in.z) );
      out.set("3", emscripten::val(in.w) );
      return out;
      break;
    }
    case Dali::Property::MATRIX3:
    {
      emscripten::val val = emscripten::val::array();
      Dali::Matrix3 mat3 = v.Get<Dali::Matrix3>();

      for( int i = 0; i < 9; ++i )
      {
        std::stringstream key;
        key << i;
        val.set( key.str(), emscripten::val(mat3.AsFloat()[i]) );
      }
      return val;
      break;
    }
    case Dali::Property::MATRIX:
    {
      emscripten::val val = emscripten::val::array();
      Dali::Matrix mat = v.Get<Dali::Matrix>();

      for( int i = 0; i < 16; ++i )
      {
        std::stringstream key;
        key << i;
        val.set( key.str(), emscripten::val(mat.AsFloat()[i]) );
      }
      return val;
      break;
    }
    case Dali::Property::RECTANGLE:
    {
      Dali::Rect<int> in = v.Get<Dali::Rect<int> >();
      emscripten::val out = emscripten::val::array();
      out.set("0", emscripten::val(in.x) );
      out.set("1", emscripten::val(in.y) );
      out.set("2", emscripten::val(in.width) );
      out.set("3", emscripten::val(in.height) );
      return out;
      break;
    }
    case Dali::Property::ROTATION:
    {
      Dali::Quaternion in = v.Get<Dali::Quaternion>();
      emscripten::val out = emscripten::val::array();
      Dali::Vector3 axis;
      float angle;
      in.ToAxisAngle(axis, angle);

      out.set("0", emscripten::val( axis.x ) );
      out.set("1", emscripten::val( axis.y ) );
      out.set("2", emscripten::val( axis.z ) );
      out.set("3", emscripten::val( static_cast<float>(Dali::Degree(Dali::Radian(angle)) ) ) );

      return out;
      break;
    }
    case Dali::Property::STRING:
    {
      return emscripten::val( v.Get<std::string>() );
      break;
    }
    case Dali::Property::ARRAY:
    {
      emscripten::val val = emscripten::val::array();

      for( int i = 0; i < v.GetSize(); ++i )
      {
        Dali::Property::Value& property = v.GetItem( i );
        std::stringstream key;
        key << i;
        val.set( key.str(), JavascriptValue( property ) );
      }

      return val;
      break;
    }
    case Dali::Property::MAP:
    {
      emscripten::val val = emscripten::val::object();

      for( int i = 0; i < v.GetSize(); ++i )
      {
        std::string key;
        Dali::Property::Value& property = v.GetItem( i, key );
        val.set( key, JavascriptValue( property ) );
      }

      return val;
      break;
    }
    case Dali::Property::TYPE_COUNT:
    case Dali::Property::NONE:
    {
      break;
    }
  } // switch type

  return emscripten::val::undefined();

}

void SetCallbackGetGlyphImage(const emscripten::val& callback)
{
  JSGetGlyphImage = callback;
}

void SetCallbackGetImage(const emscripten::val& callback)
{
  JSGetImage = callback;
}

void SetCallbackGetImageMetadata(const emscripten::val& callback)
{
  JSGetImageMetaData = callback;
}

void SetCallbackRenderFinished(const emscripten::val& callback)
{
  JSRenderFinished = callback;
}

//
// Creating property Value Objects
//   javascript can't select on type so we have renamed constructors
//
// Emscripten can convert some basic types and ones we've declared as value_array(s)
// (These are given member access offsets/functions and are for simple structs etc)
//
// The composite types need converting.
//
Dali::Property::Value PropertyBoolean(bool v)
{
  return Dali::Property::Value(v);
}

Dali::Property::Value PropertyFloat(float v)
{
  return Dali::Property::Value(v);
}

Dali::Property::Value PropertyVector2( const Dali::Vector2& v )
{
  return Dali::Property::Value(v);
}

Dali::Property::Value PropertyVector3( const Dali::Vector3& v )
{
  return Dali::Property::Value(v);
}

Dali::Property::Value PropertyVector4( const Dali::Vector4& v )
{
  return Dali::Property::Value(v);
}

Dali::Property::Value PropertyMatrix( const Dali::Matrix& v )
{
  return Dali::Property::Value(v);
}

Dali::Property::Value PropertyMatrix3( const Dali::Matrix3& v )
{
  return Dali::Property::Value(v);
}

Dali::Property::Value PropertyEuler( const Dali::Vector3& v )
{
  return Dali::Property::Value( Dali::Quaternion(
                                  Dali::Radian(Dali::Degree(v.x)),
                                  Dali::Radian(Dali::Degree(v.y)),
                                  Dali::Radian(Dali::Degree(v.z)) ) );
}

Dali::Property::Value PropertyAxisAngle( const Dali::Vector4& v )
{
  Dali::Quaternion q( Dali::Quaternion::FromAxisAngle(v, Dali::Radian(Dali::Degree(v[3])) ) );
  return Dali::Property::Value( q );
}

Dali::Property::Value PropertyString( const std::string& v )
{
  return Dali::Property::Value(v);
}

Dali::Property::Value PropertyContainer( const emscripten::val& v )
{
  Dali::Property::Value ret;
  RecursiveSetProperty( ret, v );
  return ret;
}

//
// Property value accessors
//
//
bool PropertyGetBoolean(const Dali::Property::Value& v)
{
  return v.Get<bool>();
}

float PropertyGetFloat(const Dali::Property::Value& v)
{
  return v.Get<float>();
}

int PropertyGetInteger(const Dali::Property::Value& v)
{
  return v.Get<int>();
}

Dali::Vector2 PropertyGetVector2(const Dali::Property::Value& v)
{
  return v.Get<Dali::Vector2>();
}

Dali::Vector3 PropertyGetVector3(const Dali::Property::Value& v)
{
  return v.Get<Dali::Vector3>();
}

Dali::Vector4 PropertyGetVector4(const Dali::Property::Value& v)
{
  return v.Get<Dali::Vector4>();
}

std::string PropertyGetString(const Dali::Property::Value& v)
{
  return v.Get<std::string>();
}

emscripten::val PropertyGetMap(const Dali::Property::Value& v)
{
  return JavascriptValue(v);
}

emscripten::val PropertyGetArray(const Dali::Property::Value& v)
{
  return JavascriptValue(v);
}

emscripten::val PropertyGetMatrix(const Dali::Property::Value& v)
{
  return JavascriptValue(v);
}

emscripten::val PropertyGetMatrix3(const Dali::Property::Value& v)
{
  return JavascriptValue(v);
}

emscripten::val PropertyGetEuler(const Dali::Property::Value& v)
{
  return JavascriptValue(v);
}

emscripten::val PropertyGetRotation(const Dali::Property::Value& v)
{
  return JavascriptValue(v);
}

int PropertyGetType(Dali::Property::Value& v)
{
  return (int)v.GetType();
}

template <typename T>
float MatrixGetter(T &v, int n)
{
  return *(v.AsFloat() + n);
}

template <typename T>
void MatrixSetter(T &v, float f, int n)
{
  *(v.AsFloat() + n) = f;
}

float MatrixGetter0(const Dali::Matrix &v)        { return MatrixGetter(v, 0); }
float MatrixGetter1(const Dali::Matrix &v)        { return MatrixGetter(v, 1); }
float MatrixGetter2(const Dali::Matrix &v)        { return MatrixGetter(v, 2); }
float MatrixGetter3(const Dali::Matrix &v)        { return MatrixGetter(v, 3); }
float MatrixGetter4(const Dali::Matrix &v)        { return MatrixGetter(v, 4); }
float MatrixGetter5(const Dali::Matrix &v)        { return MatrixGetter(v, 5); }
float MatrixGetter6(const Dali::Matrix &v)        { return MatrixGetter(v, 6); }
float MatrixGetter7(const Dali::Matrix &v)        { return MatrixGetter(v, 7); }
float MatrixGetter8(const Dali::Matrix &v)        { return MatrixGetter(v, 8); }
float MatrixGetter9(const Dali::Matrix &v)        { return MatrixGetter(v, 9); }
float MatrixGetter10(const Dali::Matrix &v)        { return MatrixGetter(v,10); }
float MatrixGetter11(const Dali::Matrix &v)        { return MatrixGetter(v,11); }
float MatrixGetter12(const Dali::Matrix &v)        { return MatrixGetter(v,12); }
float MatrixGetter13(const Dali::Matrix &v)        { return MatrixGetter(v,13); }
float MatrixGetter14(const Dali::Matrix &v)        { return MatrixGetter(v,14); }
float MatrixGetter15(const Dali::Matrix &v)        { return MatrixGetter(v,15); }
float MatrixGetter16(const Dali::Matrix &v)        { return MatrixGetter(v,16); }

void MatrixSetter0(Dali::Matrix &v, float f)      { MatrixSetter(v, f, 0); }
void MatrixSetter1(Dali::Matrix &v, float f)      { MatrixSetter(v, f, 1); }
void MatrixSetter2(Dali::Matrix &v, float f)      { MatrixSetter(v, f, 2); }
void MatrixSetter3(Dali::Matrix &v, float f)      { MatrixSetter(v, f, 3); }
void MatrixSetter4(Dali::Matrix &v, float f)      { MatrixSetter(v, f, 4); }
void MatrixSetter5(Dali::Matrix &v, float f)      { MatrixSetter(v, f, 5); }
void MatrixSetter6(Dali::Matrix &v, float f)      { MatrixSetter(v, f, 6); }
void MatrixSetter7(Dali::Matrix &v, float f)      { MatrixSetter(v, f, 7); }
void MatrixSetter8(Dali::Matrix &v, float f)      { MatrixSetter(v, f, 8); }
void MatrixSetter9(Dali::Matrix &v, float f)      { MatrixSetter(v, f, 9); }
void MatrixSetter10(Dali::Matrix &v, float f)      { MatrixSetter(v, f,10); }
void MatrixSetter11(Dali::Matrix &v, float f)      { MatrixSetter(v, f,11); }
void MatrixSetter12(Dali::Matrix &v, float f)      { MatrixSetter(v, f,12); }
void MatrixSetter13(Dali::Matrix &v, float f)      { MatrixSetter(v, f,13); }
void MatrixSetter14(Dali::Matrix &v, float f)      { MatrixSetter(v, f,14); }
void MatrixSetter15(Dali::Matrix &v, float f)      { MatrixSetter(v, f,15); }
void MatrixSetter16(Dali::Matrix &v, float f)      { MatrixSetter(v, f,16); }

float Matrix3Getter0(const Dali::Matrix3 &v)        { return MatrixGetter(v, 0); }
float Matrix3Getter1(const Dali::Matrix3 &v)        { return MatrixGetter(v, 1); }
float Matrix3Getter2(const Dali::Matrix3 &v)        { return MatrixGetter(v, 2); }
float Matrix3Getter3(const Dali::Matrix3 &v)        { return MatrixGetter(v, 3); }
float Matrix3Getter4(const Dali::Matrix3 &v)        { return MatrixGetter(v, 4); }
float Matrix3Getter5(const Dali::Matrix3 &v)        { return MatrixGetter(v, 5); }
float Matrix3Getter6(const Dali::Matrix3 &v)        { return MatrixGetter(v, 6); }
float Matrix3Getter7(const Dali::Matrix3 &v)        { return MatrixGetter(v, 7); }
float Matrix3Getter8(const Dali::Matrix3 &v)        { return MatrixGetter(v, 8); }

void Matrix3Setter0(Dali::Matrix3 &v, float f)      { MatrixSetter(v, f, 0); }
void Matrix3Setter1(Dali::Matrix3 &v, float f)      { MatrixSetter(v, f, 1); }
void Matrix3Setter2(Dali::Matrix3 &v, float f)      { MatrixSetter(v, f, 2); }
void Matrix3Setter3(Dali::Matrix3 &v, float f)      { MatrixSetter(v, f, 3); }
void Matrix3Setter4(Dali::Matrix3 &v, float f)      { MatrixSetter(v, f, 4); }
void Matrix3Setter5(Dali::Matrix3 &v, float f)      { MatrixSetter(v, f, 5); }
void Matrix3Setter6(Dali::Matrix3 &v, float f)      { MatrixSetter(v, f, 6); }
void Matrix3Setter7(Dali::Matrix3 &v, float f)      { MatrixSetter(v, f, 7); }
void Matrix3Setter8(Dali::Matrix3 &v, float f)      { MatrixSetter(v, f, 8); }

val JavascriptUpdateCallback( val::undefined() );
bool JavascriptUpdateCallbackSet = false;

void JavascriptUpdate(int dt)
{
  if( JavascriptUpdateCallbackSet )
  {
    JavascriptUpdateCallback( val(dt) );
  }
}

void SetUpdateFunction( const emscripten::val& function )
{
  JavascriptUpdateCallback = function;
  JavascriptUpdateCallbackSet = true;
}

const emscripten::val& GetUpdateFunction()
{
  return JavascriptUpdateCallback;
}

EMSCRIPTEN_BINDINGS(dali_wrapper)
{
  //
  // Embind can do much of the Emscripten wrapping for us.
  //
  // One convention below is that where there is a function or method prepended
  // with '__' there is a corresponding Javascript helper function to help with
  // marshalling parameters and return values.
  //

  //
  // Embind knows about vectors so we tell it which ones to instantiate
  //
  register_vector<std::string>("VectorString");
  register_vector<int>("VectorInt");
  register_vector<float>("VectorFloat");
  register_vector<ActorWrapper>("VectorActor");
  register_vector<Dali::MeshData::Vertex>("VectorVertex");
  register_vector<Dali::MeshData::FaceIndex>("VectorFaceIndex");

  //
  // Creation.
  //
  emscripten::function("createAny", &Dali::Internal::Emscripten::CreateAny);
  emscripten::function("createActor", &CreateActor);
  emscripten::function("__createSolidColorActor", &CreateSolidColorActor);
  emscripten::function("__createMeshActor", &CreateMeshActor);


  emscripten::function("getTypeRegistry", &GetTypeRegistry);
  emscripten::function("__getStage", &GetStage);

  emscripten::function("javascriptValue", &JavascriptValue);

  emscripten::function("__hitTest", &HitTest);

  emscripten::function("sendMouseEvent", &EmscriptenMouseEvent);
  emscripten::function("__updateOnce", &EmscriptenUpdateOnce);
  emscripten::function("__renderOnce", &EmscriptenRenderOnce);

  //
  // Global callback functions
  //
  emscripten::function("setCallbackGetGlyphImage", &SetCallbackGetGlyphImage);
  emscripten::function("setCallbackGetImage", &SetCallbackGetImage);
  emscripten::function("setCallbackGetImageMetadata", &SetCallbackGetImageMetadata);
  emscripten::function("setCallbackRenderFinished", &SetCallbackRenderFinished);
  emscripten::function("setUpdateFunction", &SetUpdateFunction);
  emscripten::function("getUpdateFunction", &GetUpdateFunction);

  //
  // Property value creation (Javascript can't select on type)
  //
  emscripten::function("PropertyBoolean", &PropertyBoolean);
  emscripten::function("PropertyFloat", &PropertyFloat);
  emscripten::function("PropertyString", &PropertyString);
  emscripten::function("PropertyVector2", &PropertyVector2);
  emscripten::function("PropertyVector3", &PropertyVector3);
  emscripten::function("PropertyVector4", &PropertyVector4);
  emscripten::function("PropertyMatrix", &PropertyMatrix);
  emscripten::function("PropertyMatrix3", &PropertyMatrix3);
  emscripten::function("PropertyMap", &PropertyContainer);
  emscripten::function("PropertyArray", &PropertyContainer);
  emscripten::function("PropertyEuler", &PropertyEuler);
  emscripten::function("PropertyAxisAngle", &PropertyAxisAngle);
  emscripten::function("PropertyToString", &PropertyValueToString);

  //
  // One unfortunate aspect of wrapping for the browser is that we get no notification
  // of object deletion so all JS wrapper objects must be wrapper.delete() correctly.
  //
  // Embind has a mechanism around this for simple c style structs decared as value_arrays.
  // So these objects don't need explicit destruction.
  //
  value_array<Dali::Internal::Emscripten::Statistics>("Statistics")
    .element(&Dali::Internal::Emscripten::Statistics::on)
    .element(&Dali::Internal::Emscripten::Statistics::frameCount)
    .element(&Dali::Internal::Emscripten::Statistics::lastFrameDeltaSeconds)
    .element(&Dali::Internal::Emscripten::Statistics::lastSyncTimeMilliseconds)
    .element(&Dali::Internal::Emscripten::Statistics::nextSyncTimeMilliseconds)
    .element(&Dali::Internal::Emscripten::Statistics::keepUpdating)
    .element(&Dali::Internal::Emscripten::Statistics::needsNotification)
    .element(&Dali::Internal::Emscripten::Statistics::secondsFromLastFrame)
;

  value_array<Dali::Vector2>("Vector2")
    .element(&Dali::Vector2::x)
    .element(&Dali::Vector2::y)
;

  value_array<Dali::Vector3>("Vector3")
    .element(&Dali::Vector3::x)
    .element(&Dali::Vector3::y)
    .element(&Dali::Vector3::z)
;

  value_array<Dali::Vector4>("Vector4")
    .element(&Dali::Vector4::x)
    .element(&Dali::Vector4::y)
    .element(&Dali::Vector4::z)
    .element(&Dali::Vector4::w)
;

  value_array<Dali::MeshData::Vertex>("Vertex")
    .element(&Dali::MeshData::Vertex::x)
    .element(&Dali::MeshData::Vertex::y)
    .element(&Dali::MeshData::Vertex::z)
    .element(&Dali::MeshData::Vertex::u)
    .element(&Dali::MeshData::Vertex::v)
    .element(&Dali::MeshData::Vertex::nX) // normal or rgb
    .element(&Dali::MeshData::Vertex::nY)
    .element(&Dali::MeshData::Vertex::nZ)
;

  value_array<Dali::Matrix>("Matrix")
    .element(&MatrixGetter0, &MatrixSetter0)
    .element(&MatrixGetter1, &MatrixSetter1)
    .element(&MatrixGetter2, &MatrixSetter2)
    .element(&MatrixGetter3, &MatrixSetter3)
    .element(&MatrixGetter4, &MatrixSetter4)
    .element(&MatrixGetter5, &MatrixSetter5)
    .element(&MatrixGetter6, &MatrixSetter6)
    .element(&MatrixGetter7, &MatrixSetter7)
    .element(&MatrixGetter8, &MatrixSetter8)
    .element(&MatrixGetter9, &MatrixSetter9)
    .element(&MatrixGetter10, &MatrixSetter10)
    .element(&MatrixGetter11, &MatrixSetter11)
    .element(&MatrixGetter12, &MatrixSetter12)
    .element(&MatrixGetter13, &MatrixSetter13)
    .element(&MatrixGetter14, &MatrixSetter14)
    .element(&MatrixGetter15, &MatrixSetter15)
;

  value_array<Dali::Matrix3>("Matrix3")
    .element(&Matrix3Getter0, &Matrix3Setter0)
    .element(&Matrix3Getter1, &Matrix3Setter1)
    .element(&Matrix3Getter2, &Matrix3Setter2)
    .element(&Matrix3Getter3, &Matrix3Setter3)
    .element(&Matrix3Getter4, &Matrix3Setter4)
    .element(&Matrix3Getter5, &Matrix3Setter5)
    .element(&Matrix3Getter6, &Matrix3Setter6)
    .element(&Matrix3Getter7, &Matrix3Setter7)
    .element(&Matrix3Getter8, &Matrix3Setter8)
;

  //
  // enums
  //
  enum_<Dali::Property::Type>("PropertyType")
    .value("NONE", Dali::Property::NONE)
    .value("BOOLEAN", Dali::Property::BOOLEAN)
    .value("FLOAT", Dali::Property::FLOAT)
    .value("INTEGER", Dali::Property::INTEGER)
    .value("UNSIGNED_INTEGER", Dali::Property::UNSIGNED_INTEGER)
    .value("VECTOR2", Dali::Property::VECTOR2)
    .value("VECTOR3", Dali::Property::VECTOR3)
    .value("VECTOR4", Dali::Property::VECTOR4)
    .value("MATRIX3", Dali::Property::MATRIX3)
    .value("MATRIX", Dali::Property::MATRIX)
    .value("RECTANGLE", Dali::Property::RECTANGLE)
    .value("ROTATION", Dali::Property::ROTATION)
    .value("STRING", Dali::Property::STRING)
    .value("ARRAY", Dali::Property::ARRAY)
    .value("MAP", Dali::Property::MAP)
    .value("TYPE_COUNT", Dali::Property::TYPE_COUNT)
;

  enum_<Dali::GeometryType>("GeometryType")
    .value("GEOMETRY_TYPE_IMAGE", Dali::GEOMETRY_TYPE_IMAGE)
    .value("GEOMETRY_TYPE_UNTEXTURED_MESH", Dali::GEOMETRY_TYPE_UNTEXTURED_MESH)
    .value("GEOMETRY_TYPE_TEXTURED_MESH", Dali::GEOMETRY_TYPE_TEXTURED_MESH)
    .value("GEOMETRY_TYPE_LAST", Dali::GEOMETRY_TYPE_LAST)
;

  enum_<Dali::ShaderEffect::GeometryHints>("GeometryHints")
    .value("HINT_NONE", Dali::ShaderEffect::HINT_NONE)
    .value("HINT_GRID_X", Dali::ShaderEffect::HINT_GRID_X)
    .value("HINT_GRID_Y", Dali::ShaderEffect::HINT_GRID_Y)
    .value("HINT_GRID", Dali::ShaderEffect::HINT_GRID)
    .value("HINT_DEPTH_BUFFER", Dali::ShaderEffect::HINT_DEPTH_BUFFER)
    .value("HINT_BLENDING", Dali::ShaderEffect::HINT_BLENDING)
    .value("HINT_DOESNT_MODIFY_GEOMETRY", Dali::ShaderEffect::HINT_DOESNT_MODIFY_GEOMETRY)
;

  //
  // classes
  //
  class_<Dali::Property::Value>("PropertyValue")
    .constructor<>()
    .function("getType", &PropertyGetType)
    .function("getBoolean", &PropertyGetBoolean)
    .function("getFloat", &PropertyGetFloat)
    .function("getInteger", &PropertyGetInteger)
    .function("getVector2", &PropertyGetVector2)
    .function("getVector3", &PropertyGetVector3)
    .function("getVector4", &PropertyGetVector4)
    .function("getString", &PropertyGetString)
    .function("getMap", &PropertyGetMap)
    .function("getArray", &PropertyGetArray)
    .function("getMatrix", &PropertyGetMatrix)
    .function("getMatrix3", &PropertyGetMatrix3)
    .function("getEuler", &PropertyGetEuler)
    .function("getRotation", &PropertyGetRotation)
;

  class_<StageWrapper>("StageWrapper")
    .constructor<>()
    .function("__add", &StageWrapper::Add)
    .function("__remove", &StageWrapper::Remove)
    .function("__getRenderTaskList", &StageWrapper::GetRenderTaskList)
    .function("__getRootLayer", &StageWrapper::GetRootLayer)
    .function("__setBackgroundColor", &StageWrapper::SetBackgroundColor)
    .function("getLayerCount", &StageWrapper::GetLayerCount)
    .function("__getLayer", &StageWrapper::GetLayer)
    .function("getDpi", &StageWrapper::GetDpi)
    .function("getSize", &StageWrapper::GetSize)
;

  class_<TypeInfoWrapper>("TypeInfo")
    .function("getActions", &TypeInfoWrapper::GetActions)
    .function("getSignals", &TypeInfoWrapper::GetSignals)
    .function("getProperties", &TypeInfoWrapper::GetProperties)
    .function("getPropertyIndices", &TypeInfoWrapper::GetPropertyIndices)
    .function("getName", &TypeInfoWrapper::GetName)
    .function("getBaseName", &TypeInfoWrapper::GetBaseName)
;

  class_<TypeRegistryWrapper>("TypeRegistry")
    .function("getTypeNames", &TypeRegistryWrapper::GetTypeNames)
    .function("getTypeInfo", &TypeRegistryWrapper::GetTypeInfo)
;

  class_<HandleWrapper>("HandleWrapper")
    .constructor<Dali::Handle>()
    .function("ok", &HandleWrapper::Ok)
    .function("setProperty", &HandleWrapper::SetProperty)
    .function("getProperty", &HandleWrapper::GetProperty)
    .function("getPropertyIndex", &HandleWrapper::GetPropertyIndex)
    .function("getPropertyName", &HandleWrapper::GetPropertyName)
    .function("getTypeInfo", &HandleWrapper::GetTypeInfo)
    .function("getPropertyIndices", &HandleWrapper::GetPropertyIndices)
    .function("getProperties", &HandleWrapper::GetProperties)
    .function("getPropertyType", &HandleWrapper::GetPropertyType)
    .function("__registerProperty", &HandleWrapper::RegisterProperty)
    .function("__registerAnimatedProperty", &HandleWrapper::RegisterAnimatedProperty)
    .function("isPropertyAnimatable", &HandleWrapper::IsPropertyAnimatable)
    .function("isPropertyWriteable", &HandleWrapper::IsPropertyWritable)
    .function("isPropertyAConstraintInput", &HandleWrapper::IsPropertyAConstraintInput)
;

  class_<ShaderEffectWrapper, base<HandleWrapper>>("ShaderEffectWrapper")
    .constructor<std::string, std::string, int, int>()
    .function("__setUniform", &ShaderEffectWrapper::SetUniform)
;

  class_<ActorWrapper, base<HandleWrapper>>("ActorWrapper")
    .constructor<Dali::Actor>()
    .function("setImage", &ActorWrapper::SetImage)
    .function("__setImageRawRGBA", &ActorWrapper::SetImageRawRGBA)
    .function("__setImageRawRGB", &ActorWrapper::SetImageRawRGB)
    .function("ok", &ActorWrapper::Ok)
    .function("add", &ActorWrapper::Add)
    .function("remove", &ActorWrapper::Remove)
    .function("__getParent", &ActorWrapper::GetParent)
    .function("__findChildById", &ActorWrapper::FindChildById)
    .function("__findChildByName", &ActorWrapper::FindChildByName)
    .function("__getChildAt", &ActorWrapper::GetChildAt)
    .function("getChildCount", &ActorWrapper::GetChildCount)
    .function("getId", &ActorWrapper::GetId)
    .function("__connectSignal", &ActorWrapper::ConnectSignal)
    .function("__screenToLocal", &ActorWrapper::ScreenToLocal)
    .function("insert", &ActorWrapper::Insert)
    .function("addressOf", &ActorWrapper::AddressOf)
    .function("setShaderEffect", &ActorWrapper::SetShaderEffect)
    .function("__getShaderEffect", &ActorWrapper::GetShaderEffect)
;

  class_<LayerWrapper, base<ActorWrapper>>("LayerWrapper")
    .constructor<Dali::Layer>()
    .function("raise", &LayerWrapper::Raise)
    .function("lower", &LayerWrapper::Lower)
;

  class_<MaterialWrapper>("MaterialWrapper")
    .constructor<std::string>()
    .function("setAmbientColor", &MaterialWrapper::SetAmbientColor)
    .function("setDiffuseColor", &MaterialWrapper::SetDiffuseColor)
    .function("setSpecularColor", &MaterialWrapper::SetSpecularColor)
    .function("setEmissiveColor", &MaterialWrapper::SetEmissiveColor)
    .function("setShininess", &MaterialWrapper::SetShininess)
    .function("setDiffuseTextureRGBData", &MaterialWrapper::SetDiffuseTextureRGBData)
    .function("setOpacityTextureRGBData", &MaterialWrapper::SetOpacityTextureRGBData)
    .function("setDiffuseTextureRGBAData", &MaterialWrapper::SetDiffuseTextureRGBAData)
    .function("setOpacityTextureRGBAData", &MaterialWrapper::SetOpacityTextureRGBAData)
;

  class_<MeshDataWrapper>("MeshDataWrapper")
    .constructor<>()
    .function("setData", &MeshDataWrapper::SetData)
    .function("setLineData", &MeshDataWrapper::SetLineData)
    .function("setFaceIndices", &MeshDataWrapper::SetFaceIndices)
    .function("setVertices", &MeshDataWrapper::SetVertices)
    .function("setHasNormals", &MeshDataWrapper::SetHasNormals)
    .function("setHasTextureCoords", &MeshDataWrapper::SetHasTextureCoords)
    .function("setHasColor", &MeshDataWrapper::SetHasColor)
;

  class_<AnimationWrapper>("AnimationWrapper")
    .constructor<>()
    .function("setDuration", &AnimationWrapper::SetDuration)
    .function("setLooping", &AnimationWrapper::SetLooping)
    .function("setEndAction", &AnimationWrapper::SetEndAction)
    .function("SetDisconnectAction", &AnimationWrapper::SetDisconnectAction)
    .function("setCurrentProgress", &AnimationWrapper::SetCurrentProgress)
    .function("setSpeedFactor", &AnimationWrapper::SetSpeedFactor)
    .function("setPlayRange", &AnimationWrapper::SetPlayRange)
    .function("getCurrentProgress", &AnimationWrapper::GetCurrentProgress)
    .function("play", &AnimationWrapper::Play)
    .function("pause", &AnimationWrapper::Pause)
    .function("stop", &AnimationWrapper::Stop)
    .function("clear", &AnimationWrapper::Clear)
    .function("__animateTo", &AnimationWrapper::AnimateTo)
    .function("__animateBy", &AnimationWrapper::AnimateBy)
    .function("__animatePath", &AnimationWrapper::AnimatePath)
;

  class_<RenderTaskListWrapper>("RenderTaskListWrapper")
    .function("__createTask", &RenderTaskListWrapper::CreateTask)
    .function("removeTask", &RenderTaskListWrapper::RemoveTask)
    .function("getTaskCount", &RenderTaskListWrapper::GetTaskCount)
    .function("__getTask", &RenderTaskListWrapper::GetTask)
;

  class_<RenderTaskWrapper>("RenderTaskWrapper")
    .function("__getCameraActor", &RenderTaskWrapper::GetCameraActor)
    .function("setCameraActor", &RenderTaskWrapper::SetCameraActor)
    .function("setSourceActor", &RenderTaskWrapper::SetSourceActor)
    .function("setExclusive", &RenderTaskWrapper::SetExclusive)
    .function("setInputEnabled", &RenderTaskWrapper::SetInputEnabled)
    .function("setViewportPosition", &RenderTaskWrapper::SetViewportPosition)
    .function("setViewportSize", &RenderTaskWrapper::SetViewportSize)
    .function("getCurrentViewportPosition", &RenderTaskWrapper::GetCurrentViewportPosition)
    .function("getCurrentViewportSize", &RenderTaskWrapper::GetCurrentViewportSize)
    .function("setClearColor", &RenderTaskWrapper::SetClearColor)
    .function("getClearColor", &RenderTaskWrapper::GetClearColor)
    .function("setClearEnabled", &RenderTaskWrapper::SetClearEnabled)
    .function("getClearEnabled", &RenderTaskWrapper::GetClearEnabled)
    .function("screenToLocal", &RenderTaskWrapper::ScreenToLocal)
    .function("worldToScreen", &RenderTaskWrapper::WorldToScreen)
;


}

}; // namespace Emscripten
}; // namespace Internal
}; // namespace Dali

