#include "dali-wrapper.h"
#include <sstream>
#include <dali/public-api/scripting/scripting.h>
#include "emscripten/emscripten.h"
#include "emscripten/bind.h"
#include "emscripten/val.h"
#include "sdl-application.h"
#include "emscripten-callbacks.h"

namespace EmscriptenCallbacks
{

// Javascript callbacks
extern emscripten::val JSGetGlyphImage;
extern emscripten::val JSGetImage;
extern emscripten::val JSGetImageMetaData;
extern emscripten::val JSRenderFinished;

};

using namespace emscripten;

extern void EmscriptenUpdateOnce();
extern void EmscriptenRenderOnce();


namespace
{

struct EmscriptenSignal
{
  EmscriptenSignal(const emscripten::val& callback, unsigned int id)
    : mCallback(callback), mId(id) {};
  emscripten::val mCallback;
  unsigned int mId;
  bool operator()()
  {
    Dali::Actor a = Dali::Stage::GetCurrent().GetRootLayer().FindChildById(mId);
    if(a)
    {
      emscripten::val ret = mCallback( ActorWrapper(a) );
    }
    else
    {
      emscripten::val ret = mCallback();
    }
    return true;
  }
};

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

Dali::Image GetImage(const std::string& data)
{
  Dali::ImageAttributes imageAttributes;
  const uint8_t* const ptr = reinterpret_cast<const uint8_t * const>(data.c_str());
  return Dali::EncodedBufferImage::New( ptr, data.size(), imageAttributes, Dali::Image::Never );
}

Dali::Animation::EndAction GetEndAction(const std::string& action)
{
  Dali::Animation::EndAction end = Dali::Animation::Discard;

  if( Dali::Scripting::CompareEnums( action.c_str(), "Bake" ) )
  {
    end = Dali::Animation::Bake;
  }
  else if( Dali::Scripting::CompareEnums( action.c_str(), "Discard" ) )
  {
    end = Dali::Animation::Discard;
  }
  else if( Dali::Scripting::CompareEnums( action.c_str(), "BakeFinal" ) )
  {
    end = Dali::Animation::BakeFinal;
  }

  return end;
}

Dali::AlphaFunction GetAlphaFunction(const std::string& name)
{
  Dali::AlphaFunction func = Dali::AlphaFunctions::Linear;

  if( Dali::Scripting::CompareEnums( name.c_str(), "Default") )
  {
    func = Dali::AlphaFunctions::Linear;
  }
  else if( Dali::Scripting::CompareEnums( name.c_str(), "Linear") )
  {
    func = Dali::AlphaFunctions::Linear;
  }
  else if( Dali::Scripting::CompareEnums( name.c_str(), "Square") )
  {
    func = Dali::AlphaFunctions::Square;
  }
  else if( Dali::Scripting::CompareEnums( name.c_str(), "Reverse") )
  {
    func = Dali::AlphaFunctions::Reverse;
  }
  else if( Dali::Scripting::CompareEnums( name.c_str(), "EaseIn") )
  {
    func = Dali::AlphaFunctions::EaseIn;
  }
  else if( Dali::Scripting::CompareEnums( name.c_str(), "EaseOut") )
  {
    func = Dali::AlphaFunctions::EaseOut;
  }
  else if( Dali::Scripting::CompareEnums( name.c_str(), "EaseInOut") )
  {
    func = Dali::AlphaFunctions::EaseInOut;
  }
  else if( Dali::Scripting::CompareEnums( name.c_str(), "EaseInSine") )
  {
    func = Dali::AlphaFunctions::EaseInSine;
  }
  else if( Dali::Scripting::CompareEnums( name.c_str(), "EaseOutSine") )
  {
    func = Dali::AlphaFunctions::EaseOutSine;
  }
  else if( Dali::Scripting::CompareEnums( name.c_str(), "EaseInOutSine") )
  {
    func = Dali::AlphaFunctions::EaseInOutSine;
  }
  else if( Dali::Scripting::CompareEnums( name.c_str(), "EaseInSine33") )
  {
    func = Dali::AlphaFunctions::EaseInSine33;
  }
  else if( Dali::Scripting::CompareEnums( name.c_str(), "EaseOutSine33") )
  {
    func = Dali::AlphaFunctions::EaseOutSine33;
  }
  else if( Dali::Scripting::CompareEnums( name.c_str(), "EaseInOutSine33") )
  {
    func = Dali::AlphaFunctions::EaseInOutSine33;
  }
  else if( Dali::Scripting::CompareEnums( name.c_str(), "EaseInOutSine50") )
  {
    func = Dali::AlphaFunctions::EaseInOutSine50;
  }
  else if( Dali::Scripting::CompareEnums( name.c_str(), "EaseInOutSine60") )
  {
    func = Dali::AlphaFunctions::EaseInOutSine60;
  }
  else if( Dali::Scripting::CompareEnums( name.c_str(), "EaseInOutSine70") )
  {
    func = Dali::AlphaFunctions::EaseInOutSine70;
  }
  else if( Dali::Scripting::CompareEnums( name.c_str(), "EaseInOutSine80") )
  {
    func = Dali::AlphaFunctions::EaseInOutSine80;
  }
  else if( Dali::Scripting::CompareEnums( name.c_str(), "EaseInOutSine90") )
  {
    func = Dali::AlphaFunctions::EaseInOutSine90;
  }
  else if( Dali::Scripting::CompareEnums( name.c_str(), "DoubleEaseInOutSine60") )
  {
    func = Dali::AlphaFunctions::DoubleEaseInOutSine60;
  }
  else if( Dali::Scripting::CompareEnums( name.c_str(), "EaseOutQuint50") )
  {
    func = Dali::AlphaFunctions::EaseOutQuint50;
  }
  else if( Dali::Scripting::CompareEnums( name.c_str(), "EaseOutQuint80") )
  {
    func = Dali::AlphaFunctions::EaseOutQuint80;
  }
  else if( Dali::Scripting::CompareEnums( name.c_str(), "Bounce") )
  {
    func = Dali::AlphaFunctions::Bounce;
  }
  else if( Dali::Scripting::CompareEnums( name.c_str(), "BounceBack") )
  {
    func = Dali::AlphaFunctions::BounceBack;
  }
  else if( Dali::Scripting::CompareEnums( name.c_str(), "EaseInBack") )
  {
    func = Dali::AlphaFunctions::EaseInBack;
  }
  else if( Dali::Scripting::CompareEnums( name.c_str(), "EaseOutBack") )
  {
    func = Dali::AlphaFunctions::EaseOutBack;
  }
  else if( Dali::Scripting::CompareEnums( name.c_str(), "EaseInOutBack") )
  {
    func = Dali::AlphaFunctions::EaseInOutBack;
  }
  else if( Dali::Scripting::CompareEnums( name.c_str(), "Sin") )
  {
    func = Dali::AlphaFunctions::Sin;
  }
  else if( Dali::Scripting::CompareEnums( name.c_str(), "Sin2x") )
  {
    func = Dali::AlphaFunctions::Sin2x;
 }
  return func;
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


Dali::Image CreateImageRGBA(unsigned int width, unsigned int height, const std::string& data)
{
  Dali::BitmapImage b = Dali::BitmapImage::New( width, height, Dali::Pixel::RGBA8888 );

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
  Dali::BitmapImage b = Dali::BitmapImage::New( width, height, Dali::Pixel::RGB888 );

  const Dali::PixelBuffer* from = reinterpret_cast<const Dali::PixelBuffer*>( data.c_str() );
  Dali::PixelBuffer* to = b.GetBuffer();

  unsigned int len = std::max( width * height * 3, data.size() );
  for(int i = 0; i < len; i++)
  {
    *to++ = *from++;
  }
  return b;
}



} // anon namespace

namespace
{
const unsigned int MAX_BORDER_SIZE( 9 );
}

ActorWrapper CreateMeshActor(MeshDataWrapper& wrapper)
{
  Dali::Mesh mesh = Dali::Mesh::New( wrapper.meshData );

  return ActorWrapper( Dali::MeshActor::New(mesh) );
}

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

  Dali::BitmapImage imageData;
  if(color.a != 1.0 || borderColor.a != 1.0)
  {
    imageData = Dali::BitmapImage::New( bitmapWidth, bitmapWidth, Dali::Pixel::RGBA8888 );

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
    imageData = Dali::BitmapImage::New( bitmapWidth, bitmapWidth, Dali::Pixel::RGB888 );

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


/**
 * The functor to be used in the hit-test algorithm to check whether the actor is hittable.
 */
bool IsActorHittableFunction(Dali::Actor actor, Dali::HitTestAlgorithm::TraverseType type)
{
  return actor.IsVisible();
};


ActorWrapper HitTest(float x, float y)
{
  Dali::HitTestAlgorithm::Results results;
  Dali::HitTestAlgorithm::HitTest( Dali::Stage::GetCurrent(), Dali::Vector2(x,y), results, IsActorHittableFunction );
  return ActorWrapper( results.actor );
}


//
// HandleWrapper
//
HandleWrapper::HandleWrapper(Dali::Handle handle)
  : mHandle(handle)
{

}

HandleWrapper::~HandleWrapper()
{

}

void HandleWrapper::SetProperty(const std::string& javascriptName, const Dali::Property::Value& value)
{
  std::string name = JavaScriptNameToPropertyName(javascriptName);

  DALI_ASSERT_ALWAYS(mHandle);
  if( mHandle )
  {
    Dali::Property::Index index = mHandle.GetPropertyIndex(name);
    if( Dali::Property::INVALID_INDEX != index )
    {
      mHandle.SetProperty(index, value);
    }
    else
    {
      printf("ERR Invalid property name:%s", name.c_str());
      EM_ASM( throw "Invalid property name (HandleWrapper::SetProperty)" );
    }
  }
  else
  {
    EM_ASM( throw "ActorWrapper has no actor" );
  }

}

Dali::Property::Value HandleWrapper::GetProperty(const std::string& javascriptName) const
{
  std::string name = JavaScriptNameToPropertyName(javascriptName);

  DALI_ASSERT_ALWAYS(mHandle);
  Dali::Property::Value ret;
  if( mHandle )
  {
    Dali::Property::Index index = mHandle.GetPropertyIndex(name);
    if( Dali::Property::INVALID_INDEX != index )
    {
      ret = mHandle.GetProperty(index);
      // printf("Get property(%d):%s=%s\n", (int)index, name.c_str(), PropertyValueToString( ret ).c_str());
    }
    else
    {
      printf("ERR Invalid property name:%s", name.c_str());
      EM_ASM( throw new Error("Invalid property name (HandleWrapper::GetProperty)") );
      EM_ASM( throw new Error("Invalid property name (HandleWrapper::GetProperty)") );
    }
  }
  else
  {
    EM_ASM( throw new Error("ActorWrapper has no actor") );
  }

  return ret;
}

int HandleWrapper::GetPropertyIndex(const std::string& javascriptName) const
{
  std::string name = JavaScriptNameToPropertyName(javascriptName);

  Dali::Property::Value ret;
  if( mHandle )
  {
    return (int)mHandle.GetPropertyIndex(name);
  }

  return -1;
}

std::string HandleWrapper::GetPropertyName(int index) const
{
  std::string ret;
  if(mHandle)
  {
    ret = mHandle.GetPropertyName(index);
  }
  return ret;
}

TypeInfoWrapper HandleWrapper::GetTypeInfo() const
{
  TypeInfoWrapper ret;
  if( mHandle )
  {
    Dali::TypeInfo info;
    if(mHandle.GetTypeInfo(info))
    {
      ret = TypeInfoWrapper( info );
    }
  }

  return ret;
}

int HandleWrapper::GetPropertyCount() const
{
  return mHandle.GetPropertyCount();
}

std::vector<int> HandleWrapper::GetPropertyIndices() const
{
  Dali::Property::IndexContainer indices;
  mHandle.GetPropertyIndices( indices );

  return std::vector<int>( indices.begin(), indices.end() );
}

std::vector<std::string> HandleWrapper::GetProperties() const
{
  Dali::Property::IndexContainer indices;
  mHandle.GetPropertyIndices( indices );
  std::vector<std::string> names;
  for(Dali::Property::IndexContainer::iterator iter(indices.begin()); iter != indices.end(); ++iter)
  {
    std::string name = PropertyNameToJavaScriptName( mHandle.GetPropertyName( *iter ) );

    names.push_back(name);
  }
  return names;
}

std::string HandleWrapper::GetPropertyType(const std::string& name) const
{
  if(mHandle)
  {
    Dali::Property::Index index = mHandle.GetPropertyIndex(name);
    if(Dali::Property::INVALID_INDEX != index)
    {
      return Dali::PropertyTypes::GetName(mHandle.GetPropertyType(index));
    }
  }

  // if we got here
  return Dali::PropertyTypes::GetName(Dali::Property::NONE);
}

Dali::Property::Index HandleWrapper::RegisterProperty(const std::string& name, const Dali::Property::Value& propertyValue)
{
  Dali::Property::Index ret = Dali::Property::INVALID_INDEX;

  Dali::Property::Type type = propertyValue.GetType();
  if(Dali::Property::ARRAY == type || Dali::Property::MAP == type)
  {
    // these types would need support in the javascript side of the wrapper
    EM_ASM( throw "Property type not supported" );
  }

  if(mHandle)
  {
    ret = mHandle.RegisterProperty(JavaScriptNameToPropertyName(name),
                                   propertyValue, Dali::Property::AccessMode::READ_WRITE);
  }
  return ret;
}

Dali::Property::Index HandleWrapper::RegisterAnimatedProperty(const std::string& name, const Dali::Property::Value& propertyValue)
{
  Dali::Property::Index ret = Dali::Property::INVALID_INDEX;

  Dali::Property::Type type = propertyValue.GetType();
  if(Dali::Property::ARRAY == type || Dali::Property::MAP == type)
  {
    // these types would need support in the javascript side of the wrapper
    EM_ASM( throw "Property type not supported" );
  }


  if(mHandle)
  {
    ret = mHandle.RegisterProperty(JavaScriptNameToPropertyName(name),
                                   propertyValue,  Dali::Property::AccessMode::ANIMATABLE);
  }
  return ret;
}

bool HandleWrapper::IsPropertyAnimatable(int index) const
{
  if(mHandle)
  {
    return mHandle.IsPropertyAnimatable(index);
  }
  else
  {
    return false;
  }
}

bool HandleWrapper::IsPropertyWritable(Dali::Property::Index index) const
{
  if(mHandle)
  {
    return mHandle.IsPropertyWritable(index);
  }
  else
  {
    return false;
  }
}

bool HandleWrapper::IsPropertyAConstraintInput(Dali::Property::Index index) const
{
  if(mHandle)
  {
    return mHandle.IsPropertyAConstraintInput(index);
  }
  else
  {
    return false;
  }
}

//
// ShaderEffectWrapper
//
ShaderEffectWrapper::ShaderEffectWrapper(Dali::ShaderEffect effect)
  : HandleWrapper(effect), mShaderEffect(effect)
{
}

ShaderEffectWrapper::ShaderEffectWrapper()
{
}

ShaderEffectWrapper::ShaderEffectWrapper( const std::string& vertexShader,
                                          const std::string& fragmentShader,
                                          Dali::GeometryType type,
                                          Dali::ShaderEffect::GeometryHints hints)

{
  mShaderEffect = Dali::ShaderEffect::New(vertexShader, fragmentShader, type, hints);
  mHandle = mShaderEffect; // possible?!
}


ShaderEffectWrapper::~ShaderEffectWrapper()
{

}

void ShaderEffectWrapper::SetEffectImage(const std::string& data)
{
  DALI_ASSERT_ALWAYS(mShaderEffect);
  if(mShaderEffect)
  {
    mShaderEffect.SetEffectImage( GetImage( data ) );
  }
}


//
// ActorWrapper
//
ActorWrapper::ActorWrapper(Dali::Actor actor)
  : HandleWrapper(actor), mActor(actor)
{
}


ActorWrapper::~ActorWrapper()
{
}

Dali::Actor ActorWrapper::GetActor() const
{
  return mActor;
}

void ActorWrapper::Add(ActorWrapper child)
{
  mActor.Add(child.mActor);
}

void ActorWrapper::Remove(ActorWrapper child)
{
  mActor.Remove(child.mActor);
}

ActorWrapper ActorWrapper::GetParent() const
{
  return ActorWrapper(mActor.GetParent());
}

ActorWrapper ActorWrapper::FindChildById(const unsigned int id)
{
  return ActorWrapper(mActor.FindChildById(id));
}

ActorWrapper ActorWrapper::FindChildByName(const std::string& actorAlias)
{
  // using alias which defaults to name
  return ActorWrapper(mActor.FindChildByAlias(actorAlias));
}

ActorWrapper ActorWrapper::GetChildAt(unsigned int index) const
{
  return ActorWrapper(mActor.GetChildAt(index));
}

unsigned int ActorWrapper::GetChildCount() const
{
  return mActor.GetChildCount();
}

unsigned int ActorWrapper::GetId() const
{
  return mActor.GetId();
}

void ActorWrapper::SetImage(const std::string& data)
{
  Dali::ImageActor imageActor = Dali::ImageActor::DownCast(mActor);

  if(imageActor)
  {
    imageActor.SetImage( GetImage( data ) );
  }
}

void ActorWrapper::SetImageRawRGBA(unsigned int width, unsigned int height, const std::string& data)
{
  if( Dali::ImageActor imageActor = Dali::ImageActor::DownCast(mActor) )
  {
    imageActor.SetImage( CreateImageRGBA(width, height, data) );
  }
}

void ActorWrapper::SetImageRawRGB(unsigned int width, unsigned int height, const std::string& data)
{
  if( Dali::ImageActor imageActor = Dali::ImageActor::DownCast(mActor) )
  {
    imageActor.SetImage( CreateImageRGB( width,height,data ) );
  }
}

bool ActorWrapper::Ok()
{
  if(mActor)
  {
    return true;
  }
  else
  {
    return false;
  }
}

void ActorWrapper::SetShaderEffect( ShaderEffectWrapper effect )
{
  if( Dali::RenderableActor renderable = Dali::RenderableActor::DownCast(mActor) )
  {
    renderable.SetShaderEffect( effect.GetShaderEffect() );
  }
}

ShaderEffectWrapper ActorWrapper::GetShaderEffect() const
{
  if( Dali::RenderableActor renderable = Dali::RenderableActor::DownCast(mActor) )
  {
    return ShaderEffectWrapper( renderable.GetShaderEffect() );
  }
  else
  {
    return ShaderEffectWrapper();
  }
}

void ActorWrapper::RemoveShaderEffect()
{
  if( Dali::RenderableActor renderable = Dali::RenderableActor::DownCast(mActor) )
  {
    renderable.RemoveShaderEffect();
  }
}


//
// AnimationWrapper
//
AnimationWrapper::AnimationWrapper()
{
  mAnimation = Dali::Animation::New(0);
}

AnimationWrapper::~AnimationWrapper()
{
}

void AnimationWrapper::SetDuration(float seconds)
{
  DALI_ASSERT_ALWAYS(mAnimation);
  mAnimation.SetDuration(seconds);
}

void AnimationWrapper::SetLooping(bool looping)
{
  DALI_ASSERT_ALWAYS(mAnimation);
  mAnimation.SetLooping(looping);
}

void AnimationWrapper::SetCurrentProgress( float progress )
{
  DALI_ASSERT_ALWAYS(mAnimation);
  mAnimation.SetCurrentProgress(progress);
}

float AnimationWrapper::GetCurrentProgress()
{
  DALI_ASSERT_ALWAYS(mAnimation);
  return mAnimation.GetCurrentProgress();
}

void AnimationWrapper::SetSpeedFactor( float factor )
{
  DALI_ASSERT_ALWAYS(mAnimation);
  mAnimation.SetSpeedFactor(factor);
}

void AnimationWrapper::SetPlayRange( const Dali::Vector2& range )
{
  DALI_ASSERT_ALWAYS(mAnimation);
  mAnimation.SetPlayRange(range);
}

void AnimationWrapper::Play()
{
  DALI_ASSERT_ALWAYS(mAnimation);
  mAnimation.Play();
}

void AnimationWrapper::Pause()
{
  DALI_ASSERT_ALWAYS(mAnimation);
  mAnimation.Pause();
}

void AnimationWrapper::Stop()
{
  DALI_ASSERT_ALWAYS(mAnimation);
  mAnimation.Stop();
}

void AnimationWrapper::Clear()
{
  DALI_ASSERT_ALWAYS(mAnimation);
  mAnimation.Clear();
}

void AnimationWrapper::AnimateTo(HandleWrapper handleWrapper,
                                 const std::string& property,
                                 Dali::Property::Value destinationValue,
                                 const std::string& alphaFunction,
                                 float delay, float duration)
{
  DALI_ASSERT_ALWAYS(mAnimation);

  Dali::AlphaFunction func = GetAlphaFunction(alphaFunction);

  Dali::Handle handle(handleWrapper.GetHandle());

  Dali::Property::Index propertyIndex = handle.GetPropertyIndex(JavaScriptNameToPropertyName(property));
  if( propertyIndex != Dali::Property::INVALID_INDEX )
  {
    Dali::Property target( handle, propertyIndex );
    mAnimation.AnimateTo( target, destinationValue, func, Dali::TimePeriod(delay,duration));
  }
  else
  {
    printf("Unable to add animation, bad property:%s\n", property.c_str());
  }

}

void AnimationWrapper::AnimateBy(HandleWrapper handleWrapper,
                                 const std::string& property,
                                 Dali::Property::Value destinationValue,
                                 const std::string& alphaFunction,
                                 float delay, float duration)
{
  DALI_ASSERT_ALWAYS(mAnimation);

  Dali::AlphaFunction func = GetAlphaFunction(alphaFunction);

  Dali::Handle handle(handleWrapper.GetHandle());

  Dali::Property::Index propertyIndex = handle.GetPropertyIndex(JavaScriptNameToPropertyName(property));
  if( propertyIndex != Dali::Property::INVALID_INDEX )
  {
    Dali::Property target( handle, propertyIndex );
    mAnimation.AnimateBy( target, destinationValue, func, Dali::TimePeriod(delay,duration));
  }
  else
  {
    printf("Unable to add animation, bad property:%s\n", property.c_str());
  }
}

void AnimationWrapper::AnimatePath(HandleWrapper targetHandle,
                                   HandleWrapper pathHandle,
                                   Dali::Vector3& forward,
                                   const std::string& alphaFunction,
                                   float delay, float duration)
{
  DALI_ASSERT_ALWAYS(mAnimation);

  Dali::AlphaFunction func = GetAlphaFunction(alphaFunction);

  Dali::Path path = Dali::Path::DownCast(pathHandle.GetHandle());

  if(!path)
  {
    printf("Unable to add animation, bad path object\n");
  }
  else
  {
    Dali::Actor actor = Dali::Actor::DownCast(targetHandle.GetHandle());
    if( !actor )
    {
      printf("Unable to add path animation, bad actor\n");
    }
    else
    {
      mAnimation.Animate( actor, path, forward, func, Dali::TimePeriod( delay, duration ) );
    }
  }
}

void AnimationWrapper::SetEndAction(const std::string& action)
{
  mAnimation.SetEndAction( GetEndAction(action) );
}

void AnimationWrapper::SetDisconnectAction(const std::string& action)
{
  mAnimation.SetDisconnectAction( GetEndAction(action) );
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

bool ActorWrapper::ConnectSignal( const std::string& signalName,
                                  const emscripten::val& javascriptFunction )
{
  return mHandle.ConnectSignal( this, signalName, EmscriptenSignal(javascriptFunction, mActor.GetId()) );
}

void ActorWrapper::SignalConnected( Dali::SlotObserver* slotObserver, Dali::CallbackBase* callback )
{
}

void ActorWrapper::SignalDisconnected( Dali::SlotObserver* slotObserver, Dali::CallbackBase* callback )
{
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

std::vector<float> ActorWrapper::ScreenToLocal(float screenX, float screenY) const
{
  std::vector<float> ret;
  float localX = 0.f;
  float localY = 0.f;
  bool ok = mActor.ScreenToLocal(localX, localY, screenX, screenY);
  if( ok )
  {
    ret.push_back(localX);
    ret.push_back(localY);
    ret.push_back(1.0);
  }
  else
  {
    ret.push_back(0.0);
    ret.push_back(0.0);
    ret.push_back(0.0);
  }
  return ret;
}


void ActorWrapper::Insert(int index, ActorWrapper child)
{
  if(Ok() && child.Ok())
  {
    mActor.Insert(index, child.GetActor());
  }
}

unsigned int ActorWrapper::AddressOf()
{
  if(Ok())
  {
    return (unsigned int)&mActor.GetBaseObject();
  }
  else
  {
    return 0;
  }
}

StageWrapper GetStage()
{
  return StageWrapper();
}


TypeRegistryWrapper GetTypeRegistry()
{
  return TypeRegistryWrapper();
}

StageWrapper::StageWrapper()
{

}

void StageWrapper::Add( ActorWrapper actorWrapper )
{
  Dali::Stage stage = Dali::Stage::GetCurrent();
  Dali::Actor actor = actorWrapper.GetActor();
  stage.Add(actor);
}

void StageWrapper::Remove( ActorWrapper actorWrapper )
{
  Dali::Stage stage = Dali::Stage::GetCurrent();
  Dali::Actor actor = actorWrapper.GetActor();
  stage.Remove(actor);
}

LayerWrapper StageWrapper::GetRootLayer() const
{
  Dali::Stage stage = Dali::Stage::GetCurrent();
  return LayerWrapper( stage.GetRootLayer() );
}

RenderTaskListWrapper StageWrapper::GetRenderTaskList() const
{
  Dali::Stage stage = Dali::Stage::GetCurrent();
  return RenderTaskListWrapper( stage.GetRenderTaskList() );
}

void StageWrapper::SetBackgroundColor(Dali::Vector4& color)
{
  Dali::Stage stage = Dali::Stage::GetCurrent();
  return stage.SetBackgroundColor( color );
}

unsigned int StageWrapper::GetLayerCount() const
{
  return Dali::Stage::GetCurrent().GetLayerCount();
}

LayerWrapper StageWrapper::GetLayer(unsigned int depth) const
{
  return LayerWrapper( Dali::Stage::GetCurrent().GetLayer(depth) );
}

Dali::Vector2 StageWrapper::GetDpi() const
{
  return Dali::Stage::GetCurrent().GetDpi();
}

Dali::Vector2 StageWrapper::GetSize() const
{
  return Dali::Stage::GetCurrent().GetSize();
}


//
// LayerWrapper
//
LayerWrapper::LayerWrapper(Dali::Layer layer)
  : ActorWrapper(layer), mLayer(layer)
{

}

void LayerWrapper::Raise()
{
  mLayer.Raise();
}

void LayerWrapper::Lower()
{
  mLayer.Lower();
}


//
// PropertyMapWrapper
//
PropertyMapWrapper::PropertyMapWrapper()
  : mOwner(true)
{
  mValue = new Dali::Property::Value(Dali::Property::MAP);
}

PropertyMapWrapper::PropertyMapWrapper(Dali::Property::Value* notOwnedValue)
  : mOwner(false)
{
  mValue = notOwnedValue;
}

PropertyMapWrapper::~PropertyMapWrapper()
{
  if(mOwner)
  {
    delete mValue;
  }
}

void PropertyMapWrapper::Add(const std::string& key, Dali::Property::Value& value)
{
  assert(mValue);
  assert(Dali::Property::MAP == mValue->GetType());
  mValue->SetValue(key, value);
}

void PropertyMapWrapper::Append(Dali::Property::Value& value)
{
  assert(mValue);
  assert(Dali::Property::ARRAY == mValue->GetType());
  mValue->AppendItem(value);
}

PropertyMapWrapper PropertyMapWrapper::AddMap(const std::string& key)
{
  assert(mValue);
  assert(Dali::Property::MAP == mValue->GetType());
  mValue->SetValue(key, Dali::Property::Value(Dali::Property::MAP));
  return PropertyMapWrapper(&mValue->GetValue(key));
}

PropertyMapWrapper PropertyMapWrapper::AddArray(const std::string& key)
{
  assert(mValue);
  assert(Dali::Property::MAP == mValue->GetType());
  mValue->SetValue(key, Dali::Property::Value(Dali::Property::ARRAY));
  return PropertyMapWrapper(&mValue->GetValue(key));
}


//
// TypeRegistryWrapper
//
TypeRegistryWrapper::TypeRegistryWrapper()
{
}


Dali::TypeRegistry::NameContainer TypeRegistryWrapper::GetTypeNames()
{
  return Dali::TypeRegistry::Get().GetTypeNames();
}

TypeInfoWrapper TypeRegistryWrapper::GetTypeInfo(const std::string& typeName)
{
  Dali::TypeInfo info = Dali::TypeRegistry::Get().GetTypeInfo(typeName);
  return TypeInfoWrapper( info );
}

//
// TypeInfoWrapper
//
TypeInfoWrapper::TypeInfoWrapper(Dali::TypeInfo typeInfo)
  :mInfo(typeInfo)
{
}

std::vector<int> TypeInfoWrapper::GetPropertyIndices() const
{
  Dali::Property::IndexContainer indices;
  mInfo.GetPropertyIndices( indices );

  std::vector<int> ret( indices.begin(), indices.end() );
  return ret;
}

std::string TypeInfoWrapper::GetName() const
{
  return mInfo.GetName();
}

std::string TypeInfoWrapper::GetBaseName() const
{
  return mInfo.GetBaseName();
}

std::vector<std::string> TypeInfoWrapper::GetProperties() const
{
  std::vector<std::string> names;

  // get the other properties
  if(Dali::Handle handle = Dali::Handle::DownCast( mInfo.CreateInstance() ) )
  {
    typedef Dali::Property::IndexContainer IndexContainer;

    Dali::Property::IndexContainer indices;
    handle.GetPropertyIndices( indices );

    for(IndexContainer::iterator iter(indices.begin()); iter != indices.end(); ++iter)
    {
      std::string name = PropertyNameToJavaScriptName( handle.GetPropertyName( *iter ) );

      names.push_back(name);
    }
  }
  else
  {
    // all we can do is get the event side properties
    // get the event side properties
    mInfo.GetProperties(names);
  }

  return names;
}

std::vector<std::string> TypeInfoWrapper::GetSignals() const
{
  std::vector<std::string> names;
  mInfo.GetSignals(names);
  return names;
}

std::vector<std::string> TypeInfoWrapper::GetActions() const
{
  std::vector<std::string> names;
  mInfo.GetActions(names);
  return names;
}

//
// RenderTaskList
//
RenderTaskListWrapper::RenderTaskListWrapper(Dali::RenderTaskList renderTaskList)
  : mRenderTaskList(renderTaskList)
{

}

RenderTaskListWrapper::~RenderTaskListWrapper()
{
}

RenderTaskWrapper RenderTaskListWrapper::CreateTask()
{
  return RenderTaskWrapper( mRenderTaskList.CreateTask() );
}

void RenderTaskListWrapper::RemoveTask( RenderTaskWrapper task )
{
  mRenderTaskList.RemoveTask( task.Get() );
}

unsigned int RenderTaskListWrapper::GetTaskCount() const
{
  return mRenderTaskList.GetTaskCount();
}

RenderTaskWrapper RenderTaskListWrapper::GetTask( unsigned int index ) const
{
  return RenderTaskWrapper( mRenderTaskList.GetTask(index) );
}

//
// RenderTask
//
RenderTaskWrapper::RenderTaskWrapper(Dali::RenderTask renderTask)
  : mRenderTask(renderTask)
{

}

RenderTaskWrapper::~RenderTaskWrapper()
{

}

void RenderTaskWrapper::SetSourceActor( ActorWrapper actor )
{
  mRenderTask.SetSourceActor(actor.GetActor());
}

void RenderTaskWrapper::SetExclusive( bool exclusive )
{
  mRenderTask.SetExclusive(exclusive);
}

void RenderTaskWrapper::SetInputEnabled( bool enabled )
{
  mRenderTask.SetInputEnabled( enabled );
}

void RenderTaskWrapper::SetCameraActor( ActorWrapper cameraActor )
{
  Dali::CameraActor camera = Dali::CameraActor::DownCast(cameraActor.GetActor());
  mRenderTask.SetCameraActor( camera );
}

ActorWrapper RenderTaskWrapper::GetCameraActor()
{
  return ActorWrapper( mRenderTask.GetCameraActor() );
}

void RenderTaskWrapper::SetViewportPosition( Dali::Vector2 position )
{
  mRenderTask.SetViewportPosition(position);
}

void RenderTaskWrapper::SetViewportSize( Dali::Vector2 size )
{
  mRenderTask.SetViewportSize(size);
}

Dali::Vector2 RenderTaskWrapper::GetCurrentViewportPosition() const
{
  return mRenderTask.GetCurrentViewportPosition();
}

Dali::Vector2 RenderTaskWrapper::GetCurrentViewportSize() const
{
  return mRenderTask.GetCurrentViewportSize();
}

void RenderTaskWrapper::SetClearColor( const Dali::Vector4& color )
{
  mRenderTask.SetClearColor(color);
}

Dali::Vector4 RenderTaskWrapper::GetClearColor()
{
  return mRenderTask.GetClearColor();
}

void RenderTaskWrapper::SetClearEnabled( bool enabled )
{
  mRenderTask.SetClearEnabled(enabled);
}

bool RenderTaskWrapper::GetClearEnabled() const
{
  return mRenderTask.GetClearEnabled();
}


Dali::Vector2 RenderTaskWrapper::ScreenToLocal(ActorWrapper actor, float screenX, float screenY) const
{
  float localX = 0;
  float localY = 0;
  mRenderTask.ScreenToLocal(actor.GetActor(), screenX, screenY, localX, localY);
  return Dali::Vector2(localX,localY);
}

Dali::Vector2 RenderTaskWrapper::WorldToScreen(const Dali::Vector3 &position) const
{
  float screenX = 0;
  float screenY = 0;
  mRenderTask.WorldToScreen(position, screenX, screenY);
  return Dali::Vector2(screenX, screenY);
}


//
// MeshDataWrapper
//
MaterialWrapper::MaterialWrapper(const std::string name)
{
  mMaterial = Dali::Material::New(name);
}

void MaterialWrapper::SetOpacity(const float opacity)
{
  mMaterial.SetOpacity(opacity);
}

void MaterialWrapper::SetShininess(const float shininess)
{
  mMaterial.SetShininess(shininess);
}

void MaterialWrapper::SetAmbientColor(const Dali::Vector4& color)
{
  mMaterial.SetAmbientColor(color);
}

void MaterialWrapper::SetDiffuseColor(const Dali::Vector4& color)
{
  mMaterial.SetDiffuseColor(color);
}

void MaterialWrapper::SetEmissiveColor(const Dali::Vector4& color)
{
  mMaterial.SetEmissiveColor(color);
}

void MaterialWrapper::SetSpecularColor(const Dali::Vector4& color)
{
  mMaterial.SetSpecularColor(color);
}


void MaterialWrapper::SetDiffuseTextureRGBAData(unsigned int width, unsigned int height, const std::string& data)
{
  mMaterial.SetDiffuseTexture( CreateImageRGBA(width, height, data) );
}

void MaterialWrapper::SetDiffuseTextureRGBData(unsigned int width, unsigned int height, const std::string& data)
{
  mMaterial.SetDiffuseTexture( CreateImageRGB(width, height, data) );
}

void MaterialWrapper::SetOpacityTextureRGBAData(unsigned int width, unsigned int height, const std::string& data)
{
  mMaterial.SetOpacityTexture( CreateImageRGBA(width, height, data) );
}

void MaterialWrapper::SetOpacityTextureRGBData(unsigned int width, unsigned int height, const std::string& data)
{
  mMaterial.SetOpacityTexture( CreateImageRGB(width, height, data) );
}

//
// MeshDataWrapper
//
void MeshDataWrapper::SetData( const Dali::MeshData::VertexContainer& vertices,
                               const Dali::MeshData::FaceIndices&     faceIndices,
                               const Dali::BoneContainer&   bones,
                               const MaterialWrapper  material )
{
  meshData.SetData(vertices, faceIndices, bones, material.GetMaterial());
}

void MeshDataWrapper::SetLineData( const Dali::MeshData::VertexContainer& vertices,
                                   const Dali::MeshData::FaceIndices& faceIndices,
                                   const MaterialWrapper  material )
{
  meshData.SetLineData(vertices,faceIndices,material.GetMaterial());
}



void MeshDataWrapper::SetFaceIndices( const Dali::MeshData::FaceIndices& faceIndices )
{
  meshData.SetFaceIndices(faceIndices);
}

void MeshDataWrapper::SetVertices( const Dali::MeshData::VertexContainer& vertices )
{
  meshData.SetVertices(vertices);
}

void MeshDataWrapper::SetHasNormals(bool hasNormals)
{
  meshData.SetHasNormals(hasNormals);
}

void MeshDataWrapper::SetHasTextureCoords(bool hasTexCoords)
{
  meshData.SetHasTextureCoords(hasTexCoords);
}

void MeshDataWrapper::SetHasColor(bool hasColor)
{
  meshData.SetHasColor(hasColor);
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
  EmscriptenCallbacks::JSGetGlyphImage = callback;
}

void SetCallbackGetImage(const emscripten::val& callback)
{
  EmscriptenCallbacks::JSGetImage = callback;
}

void SetCallbackGetImageMetadata(const emscripten::val& callback)
{
  EmscriptenCallbacks::JSGetImageMetaData = callback;
}

void SetCallbackRenderFinished(const emscripten::val& callback)
{
  EmscriptenCallbacks::JSRenderFinished = callback;
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

extern int INTERVAL;
extern "C" void SetInterval(int ms)
{
  INTERVAL = ms;
}

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

extern void EmscriptenMouseEvent(double x, double y, int mouseIsDown);

EMSCRIPTEN_BINDINGS(dali_wrapper)
{
  register_vector<std::string>("VectorString");
  register_vector<int>("VectorInt");
  register_vector<float>("VectorFloat");
  register_vector<ActorWrapper>("VectorActor");
  register_vector<Dali::MeshData::Vertex>("VectorVertex");
  register_vector<Dali::MeshData::FaceIndex>("VectorFaceIndex");

  emscripten::function("createAny", &CreateAny);
  emscripten::function("createActor", &CreateActor);
  emscripten::function("__getStage", &GetStage);
  emscripten::function("getTypeRegistry", &GetTypeRegistry);

  emscripten::function("setInterval", &SetInterval);

  emscripten::function("setUpdateFunction", &SetUpdateFunction);
  emscripten::function("getUpdateFunction", &GetUpdateFunction);

  emscripten::function("javascriptValue", &JavascriptValue);

  emscripten::function("setCallbackGetGlyphImage", &SetCallbackGetGlyphImage);
  emscripten::function("setCallbackGetImage", &SetCallbackGetImage);
  emscripten::function("setCallbackGetImageMetadata", &SetCallbackGetImageMetadata);
  emscripten::function("setCallbackRenderFinished", &SetCallbackRenderFinished);

  emscripten::function("sendMouseEvent", &EmscriptenMouseEvent);

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

  emscripten::function("getTypeRegistry", &GetTypeRegistry);

  emscripten::function("__createSolidColorActor", &CreateSolidColorActor);

  emscripten::function("__hitTest", &HitTest);

  emscripten::function("__createMeshActor", &CreateMeshActor);

  emscripten::function("__updateOnce", &EmscriptenUpdateOnce);
  emscripten::function("__renderOnce", &EmscriptenRenderOnce);

  value_array<EmscriptenCallbacks::Statistics>("Statistics")
    .element(&EmscriptenCallbacks::Statistics::on)
    .element(&EmscriptenCallbacks::Statistics::frameCount)
    .element(&EmscriptenCallbacks::Statistics::lastFrameDeltaSeconds)
    .element(&EmscriptenCallbacks::Statistics::lastSyncTimeMilliseconds)
    .element(&EmscriptenCallbacks::Statistics::nextSyncTimeMilliseconds)
    .element(&EmscriptenCallbacks::Statistics::keepUpdating)
    .element(&EmscriptenCallbacks::Statistics::needsNotification)
    .element(&EmscriptenCallbacks::Statistics::secondsFromLastFrame)
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

  class_<StageWrapper>("Stage")
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


  enum_<Dali::GeometryType>("GeometryType")
    .value("GEOMETRY_TYPE_IMAGE", Dali::GEOMETRY_TYPE_IMAGE)
    .value("GEOMETRY_TYPE_TEXT", Dali::GEOMETRY_TYPE_TEXT)
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


  class_<ShaderEffectWrapper, base<HandleWrapper>>("ShaderEffectWrapper")
    .constructor<std::string, std::string, Dali::GeometryType, Dali::ShaderEffect::GeometryHints>()
;

  class_<ActorWrapper, base<HandleWrapper>>("ActorWrapper")
    .constructor<Dali::Actor>()
    .function("setImage", &ActorWrapper::SetImage)
    .function("__setImageRawRGBA", &ActorWrapper::SetImageRawRGBA)
    .function("__setImageRawRGB", &ActorWrapper::SetImageRawRGB)
    .function("ok", &ActorWrapper::Ok)
    .function("add", &ActorWrapper::Add)
    .function("remove", &ActorWrapper::Remove)
    // emscripten::functions which return actors are adorned internal with '__'
    // we do more processing on the return in the js layer
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
