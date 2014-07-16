#include "dali-wrapper.h"
#include <sstream>
#include "emscripten/emscripten.h"
#include "emscripten/bind.h"
#include "sdl-application.h"

namespace EmscriptenCallbacks
{

// Javascript callbacks
extern emscripten::val JSGetGlyphImage;
extern emscripten::val JSGetImage;
extern emscripten::val JSGetImageMetaData;
};

using namespace emscripten;
using namespace Dali;

namespace
{

// //
// // embind smart pointer specialization to hold handles
// //
// template<typename PointeeType>
// struct smart_ptr_trait<Dali::Handle> {
//   typedef Dali::Handle PointerType;
//   typedef typename PointerType element_type;

//   static element_type* get(const PointerType& ptr) {
//     return &ptr;
//   }

//   static sharing_policy get_sharing_policy() {
//     return sharing_policy::BY_EMVAL;
//   }

//   static std::shared_ptr<PointeeType>* share(PointeeType* p, internal::EM_VAL v) {
//     return new PointerType(*p);
//   }

//   static PointerType* construct_null() {
//     return new PointerType;
//   }

// private:
//   // class val_deleter {
//   // public:
//   //   val_deleter() = delete;
//   //   explicit val_deleter(val v)
//   //     : v(v)
//   //   {}
//   //   void operator()(void const*) {
//   //     v();
//   //     // eventually we'll need to support emptied out val
//   //     v = val::undefined();
//   //   }
//   // private:
//   //   val v;
//   // };
// };



template <typename T>
std::string ToString(const T& value)
{
  std::stringstream ss;
  ss << value;
  return ss.str();
}

template <>
std::string ToString(const Rect<int>& value)
{
  std::stringstream ss;
  ss << value.x << "," << value.y << "," << value.width << "," << value.height;
  return ss.str();
}


std::string PropertyValueToString( const Property::Value& value )
{
  std::string ret;

  switch( value.GetType() )
  {
    case Property::NONE:
    {
      ret = "NONE";
      break;
    }            ///< No type
    case Property::BOOLEAN:
    {
      ret = value.Get<bool>() ? "True" : "False";
      break;
    }
    case Property::FLOAT:
    {
      ret = ToString( value.Get<float>() );
      break;
    }
    case Property::INTEGER:
    {
      ret = ToString( value.Get<int>() );
      break;
    }
    case Property::UNSIGNED_INTEGER:
    {
      ret = ToString( value.Get<unsigned int>() );
      break;
    }
    case Property::VECTOR2:
    {
      ret = ToString( value.Get<Vector2>() );
      break;
    }
    case Property::VECTOR3:
    {
      ret = ToString( value.Get<Vector3>() );
      break;
    }
    case Property::VECTOR4:
    {
      ret = ToString( value.Get<Vector4>() );
      break;
    }
    case Property::MATRIX3:
    {
      ret = ToString( value.Get<Matrix3>() );
      break;
    }
    case Property::MATRIX:
    {
      ret = ToString( value.Get<Matrix>() );
      break;
    }
    case Property::RECTANGLE:
    {
      ret = ToString( value.Get< Rect<int> >() );
      break;
    }
    case Property::ROTATION:
    {
      break;
    }
    case Property::STRING:
    {
      ret = value.Get<std::string>();
      break;
    }
    case Property::ARRAY:
    {
      ret = std::string("Array Size=") + ToString( value.Get<Property::Array>().size() );
      break;
    }
    case Property::MAP:
    {
      ret = std::string("Map Size=") + ToString( value.Get<Property::Map>().size() );
      break;
    }
    case Property::TYPE_COUNT:
    {
      ret = "";
      break;
    }
  }

  return ret;
}

// Image GetImage(int emscriptenPointerToUint8, int length)
// {
//   ImageAttributes imageAttributes;
//   const uint8_t* const ptr = reinterpret_cast<const uint8_t * const>(emscriptenPointerToUint8);
//   return EncodedBufferImage::New( ptr, length, imageAttributes, Image::Never );
// }

Image GetImage(const std::string& data)
{
  ImageAttributes imageAttributes;
  const uint8_t* const ptr = reinterpret_cast<const uint8_t * const>(data.c_str());
  return EncodedBufferImage::New( ptr, data.size(), imageAttributes, Image::Never );
}

Animation::EndAction GetEndAction(const std::string& action)
{
  Animation::EndAction end = Animation::Discard;

  if( action == "Bake" )
  {
    end = Animation::Bake;
  }
  else if( action == "Discard" )
  {
    end = Animation::Discard;
  }
  else if( action == "BakeFinal" )
  {
    end = Animation::BakeFinal;
  }

  return end;
}

AlphaFunction GetAlphaFunction(const std::string& name)
{
  AlphaFunction func = AlphaFunctions::Linear;

  if( name == "Default")
  {
    func = AlphaFunctions::Linear;
  }
  else if( name == "Linear")
  {
    func = AlphaFunctions::Linear;
  }
  else if( name == "Square")
  {
    func = AlphaFunctions::Square;
  }
  else if( name == "Reverse")
  {
    func = AlphaFunctions::Reverse;
  }
  else if( name == "EaseIn")
  {
    func = AlphaFunctions::EaseIn;
  }
  else if( name == "EaseOut")
  {
    func = AlphaFunctions::EaseOut;
  }
  else if( name == "EaseInOut")
  {
    func = AlphaFunctions::EaseInOut;
  }
  else if( name == "EaseInSine")
  {
    func = AlphaFunctions::EaseInSine;
  }
  else if( name == "EaseOutSine")
  {
    func = AlphaFunctions::EaseOutSine;
  }
  else if( name == "EaseInOutSine")
  {
    func = AlphaFunctions::EaseInOutSine;
  }
  else if( name == "EaseInSine33")
  {
    func = AlphaFunctions::EaseInSine33;
  }
  else if( name == "EaseOutSine33")
  {
    func = AlphaFunctions::EaseOutSine33;
  }
  else if( name == "EaseInOutSine33")
  {
    func = AlphaFunctions::EaseInOutSine33;
  }
  else if( name == "EaseInOutSine50")
  {
    func = AlphaFunctions::EaseInOutSine50;
  }
  else if( name == "EaseInOutSine60")
  {
    func = AlphaFunctions::EaseInOutSine60;
  }
  else if( name == "EaseInOutSine70")
  {
    func = AlphaFunctions::EaseInOutSine70;
  }
  else if( name == "EaseInOutSine80")
  {
    func = AlphaFunctions::EaseInOutSine80;
  }
  else if( name == "EaseInOutSine90")
  {
    func = AlphaFunctions::EaseInOutSine90;
  }
  else if( name == "DoubleEaseInOutSine60")
  {
    func = AlphaFunctions::DoubleEaseInOutSine60;
  }
  else if( name == "EaseOutQuint50")
  {
    func = AlphaFunctions::EaseOutQuint50;
  }
  else if( name == "EaseOutQuint80")
  {
    func = AlphaFunctions::EaseOutQuint80;
  }
  else if( name == "Bounce")
  {
    func = AlphaFunctions::Bounce;
  }
  else if( name == "BounceBack")
  {
    func = AlphaFunctions::BounceBack;
  }
  else if( name == "EaseInBack")
  {
    func = AlphaFunctions::EaseInBack;
  }
  else if( name == "EaseOutBack")
  {
    func = AlphaFunctions::EaseOutBack;
  }
  else if( name == "EaseInOutBack")
  {
    func = AlphaFunctions::EaseInOutBack;
  }
  else if( name == "Sin")
  {
    func = AlphaFunctions::Sin;
  }
  else if( name == "Sin2x")
  {
    func = AlphaFunctions::Sin2x;
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


} // anon namespace

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
    Property::Index index = mHandle.GetPropertyIndex(name);
    if( Property::INVALID_INDEX != index )
    {
      mHandle.SetProperty(index, value);
      //std::string s = PropertyValueToString( value );
      //printf("Set property(%d):%s=%s\n", (int)index, name.c_str(), s.c_str());
      //Property::Value ret = mHandle.GetProperty(index);
      //s = PropertyValueToString( ret );
      //printf("  Get property(%d):%s=%s\n", (int)index, name.c_str(), s.c_str());
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

  // else
}

Dali::Property::Value HandleWrapper::GetProperty(const std::string& javascriptName) const
{
  std::string name = JavaScriptNameToPropertyName(javascriptName);

  DALI_ASSERT_ALWAYS(mHandle);
  Property::Value ret;
  if( mHandle )
  {
    Property::Index index = mHandle.GetPropertyIndex(name);
    if( Property::INVALID_INDEX != index )
    {
      ret = mHandle.GetProperty(index);
      std::string s = PropertyValueToString( ret );
      // printf("Get property(%d):%s=%s\n", (int)index, name.c_str(), s.c_str());
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

  Property::Value ret;
  if( mHandle )
  {
    return (int)mHandle.GetPropertyIndex(name);
  }

  return -1;
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


//
//
//
ShaderEffectWrapper::ShaderEffectWrapper(Dali::ShaderEffect shaderEffect)
  : HandleWrapper(shaderEffect), mShaderEffect(shaderEffect)
{

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
// CameraActorWrapper
//

CameraActorWrapper::CameraActorWrapper(Dali::CameraActor camera)
  : mCamera(camera)
{

}

Dali::CameraActor CameraActorWrapper::Get()
{
  return mCamera;
}

//
// ActorWrapper
//

ActorWrapper::ActorWrapper(Actor actor)
  : HandleWrapper(actor), mActor(actor)
{
  printf("%s\n", "actor created");
}


ActorWrapper::~ActorWrapper()
{
  if(mActor)
    printf("%s\n", "actor destroyed");
}

Actor ActorWrapper::GetActor() const
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

// extern "C" void SetImage(ActorWrapper actorWrapper, int emscriptenPointerToUint8, int length)
// {
//   actorWrapper->SetImage(emscriptenPointerToUint8, length);
// }

void ActorWrapper::SetImage(const std::string& data)
{
  ImageActor imageActor = ImageActor::DownCast(mActor);

  if(imageActor)
  {
    imageActor.SetImage( GetImage( data ) );
  }
}


// void ActorWrapper::SetShaderEffect( ShaderEffectWrapper shaderEffect )
// {
//   DALI_ASSERT_ALWAYS(mActor);
//   if(mActor && shaderEffect.GetShaderEffect())
//   {
//     mActor.SetShaderEffect( shaderEffect.GetShaderEffect() );
//   }
// }

// void ActorWrapper::RemoveShaderEffect()
// {
//   DALI_ASSERT_ALWAYS(mActor);
//   if(mActor)
//   {
//     mActor.RemoveShaderEffect();
//   }
// }

bool ActorWrapper::Ok()
{
  return mActor;
}

// void ActorWrapper::SetEffectImage(int emscriptenPointerToUint8, int length)
// {
//   ImageActor imageActor = ImageActor::DownCast(mActor);

//   if(imageActor)
//   {
//     mActor.SetEffectImage( GetImage( emscriptenPointerToUint8, length ) );
//   }
// }



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

  AlphaFunction func = GetAlphaFunction(alphaFunction);

  Dali::Handle handle(handleWrapper.GetHandle());

  Property::Index propertyIndex = handle.GetPropertyIndex(property);
  if( propertyIndex != Property::INVALID_INDEX )
  {
    Property target( handle, propertyIndex );
    mAnimation.AnimateTo( target, destinationValue, func, TimePeriod(delay,duration));
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

  AlphaFunction func = GetAlphaFunction(alphaFunction);

  Dali::Handle handle(handleWrapper.GetHandle());

  Property::Index propertyIndex = handle.GetPropertyIndex(property);
  if( propertyIndex != Property::INVALID_INDEX )
  {
    Property target( handle, propertyIndex );
    mAnimation.AnimateBy( target, destinationValue, func, TimePeriod(delay,duration));
  }
  else
  {
    printf("Unable to add animation, bad property:%s\n", property.c_str());
  }
}

void AnimationWrapper::SetEndAction(const std::string& action)
{
  mAnimation.SetEndAction( GetEndAction(action) );
}

void AnimationWrapper::SetDestroyAction(const std::string& action)
{
  mAnimation.SetDestroyAction( GetEndAction(action) );
}


            // void _embind_register_class_property(
            //     TYPEID classType,
            //     const char* fieldName,
            //     TYPEID getterReturnType,
            //     const char* getterSignature,
            //     GenericFunction getter,
            //     void* getterContext,
            //     TYPEID setterArgumentType,
            //     const char* setterSignature,
            //     GenericFunction setter,
            //     void* setterContext);


            // auto getter = &MemberAccess<ClassType, FieldType>::template getWire<ClassType>;
            // _embind_register_class_property(
            //     TypeID<ClassType>::get(),
            //     fieldName,
            //     TypeID<FieldType>::get(),
            //     getSignature(getter),
            //     reinterpret_cast<GenericFunction>(getter),
            //     getContext(field),
            //     0,
            //     0,
            //     0,
            //     0);
            // return *this;

HandleWrapper CreateAny(const std::string& name)
{
  HandleWrapper ret;

  TypeRegistry registry = TypeRegistry::Get();

  TypeInfo typeInfo = registry.GetTypeInfo( name );

  if(!typeInfo)
  {
    EM_ASM( throw "Invalid type name" );
  }
  else
  {
    BaseHandle base = typeInfo.CreateInstance();

    Handle handle = Handle::DownCast(base);

    ret = HandleWrapper( handle );
  }

  return ret;
}

ActorWrapper CreateActor(const std::string& name)
{
  ActorWrapper ret;

  TypeRegistry registry = TypeRegistry::Get();

  TypeInfo typeInfo = registry.GetTypeInfo( name );

  if(!typeInfo)
  {
    EM_ASM( throw "Invalid type name" );
  }
  else
  {
    BaseHandle handle = typeInfo.CreateInstance();

    // printf("Name %s\n", name.c_str());

    if( Actor actor = Actor::DownCast(handle) )
    {
      ret = ActorWrapper( actor );
    }
  }

  // if( Animation anim = Animation::DownCast(handle) )
  // {
  //   return std::shared_ptr<AnimationWrapper>();
  // }

  //if here
  return ret;

}

// ActorWrapper CreateAnimation(const std::string& name)
// {
//   TypeRegistry registry = TypeRegistry::Get();

//   TypeInfo typeInfo = registry.GetTypeInfo( name );

//   BaseHandle handle = typeInfo.CreateInstance();
//   Actor actor = Actor::DownCast(handle);

//   // printf("Name %s\n", name.c_str());

//   if( actor )
//   {
//     return ActorWrapper( actor );
//   }
//   else
//   {
//     return ActorWrapper();
//   }
// }

StageWrapper GetStage()
{
  return StageWrapper();
}


TypeRegistryWrapper GetTypeRegistry()
{
  return TypeRegistryWrapper();
}

// BaseHandleWrapper::BaseHandleWrapper()
// {

// }

StageWrapper::StageWrapper()
{

}

void StageWrapper::Add( ActorWrapper actorWrapper )
{
  Stage stage = Stage::GetCurrent();
  Actor actor = actorWrapper.GetActor();
  stage.Add(actor);
}

void StageWrapper::Remove( ActorWrapper actorWrapper )
{
  Stage stage = Stage::GetCurrent();
  Actor actor = actorWrapper.GetActor();
  stage.Remove(actor);
}

LayerWrapper StageWrapper::GetRootLayer() const
{
  Stage stage = Stage::GetCurrent();
  return LayerWrapper( stage.GetRootLayer() );
}

RenderTaskListWrapper StageWrapper::GetRenderTaskList() const
{
  Stage stage = Stage::GetCurrent();
  return RenderTaskListWrapper( stage.GetRenderTaskList() );
}

//
//
//
LayerWrapper::LayerWrapper(Layer layer)
  : mLayer(layer)
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
//
//

PropertyMapWrapper::PropertyMapWrapper()
  : mOwner(true)
{
  mValue = new Property::Value(Property::MAP);
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
  assert(Property::MAP == mValue->GetType());
  mValue->SetValue(key, value);
}

void PropertyMapWrapper::Append(Dali::Property::Value& value)
{
  assert(mValue);
  assert(Property::ARRAY == mValue->GetType());
  mValue->AppendItem(value);
}

PropertyMapWrapper PropertyMapWrapper::AddMap(const std::string& key)
{
  assert(mValue);
  assert(Property::MAP == mValue->GetType());
  mValue->SetValue(key, Property::Value(Property::MAP));
  return PropertyMapWrapper(&mValue->GetValue(key));
}

PropertyMapWrapper PropertyMapWrapper::AddArray(const std::string& key)
{
  assert(mValue);
  assert(Property::MAP == mValue->GetType());
  mValue->SetValue(key, Property::Value(Property::ARRAY));
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

void TypeInfoWrapper::GetProperties(Dali::TypeInfo::NameContainer& names)
{
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
}

void TypeInfoWrapper::GetSignals(Dali::TypeInfo::NameContainer& names)
{
  mInfo.GetSignals(names);
}

void TypeInfoWrapper::GetActions(Dali::TypeInfo::NameContainer & names)
{
  mInfo.GetActions(names);
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

void RenderTaskWrapper::SetCameraActor( CameraActorWrapper cameraActor )
{
  mRenderTask.SetCameraActor( cameraActor.Get() );
}

//
//
//

// bool Update(int milliSeconds)
// {
//   extern Dali::SdlApplication *app;
//   if( app )
//   {
//     static int w = 0;
//     static int h = 0;

//     int _x = EM_ASM_INT_V({ return dali.canvas.width; });
//     int _y = EM_ASM_INT_V({ return dali.canvas.height; });

//     bool resize = false;
//     if( _x != w )
//     {
//       w = _x;
//       resize = true;
//     }
//     if( _y != h )
//     {
//       h = _y;
//       resize = true;
//     }
//     if( resize )
//     {
//       printf("resize:%d,%d\n", w,h );
//       app->SetSurfaceWidth(w, h);
//     }

//     return app->DoUpdate(milliSeconds);
//   }
//   else
//   {
//     EM_ASM( throw new Error("No App to update?") );
//   }
//   return false;
// }

// void Render()
// {
//   extern Dali::SdlApplication *app;
//   if( app )
//   {
//     app->DoRender();
//   }
//   else
//   {
//     EM_ASM( throw new Error("No App to render with?") );
//   }
// }

bool HasKey(const emscripten::val& value, const std::string& key)
{
  val keys = val::global("Object").call<val>("keys", value);
  int keyLength = keys["length"].as<int>();
  for( int j = 0; j < keyLength; ++j )
  {
    if( keys[j].as<std::string>() == key )
    {
      return true;
    }
  }
  return false;
}

void RecursiveSetProperty(Property::Value& propertyValue, const emscripten::val& fromVal)
{
  const std::string fromType( fromVal.typeof().as<std::string>() );

  if( fromType == "object" )
  {
    // val::global("Object").call<val>("IsArray", fromVal ).as<bool>() )
    if( HasKey(fromVal, "length") )
    {
      int length = fromVal["length"].as<int>();
      propertyValue = Property::Value(Property::ARRAY);
      for( int j = 0; j < length; ++j )
      {
        Property::Value add;
        propertyValue.AppendItem( add );

        std::stringstream ss;
        ss << j;

        emscripten::val val = fromVal[ ss.str() ];

        RecursiveSetProperty( propertyValue.GetItem(j), val );

        // fromVal[ ss.c_str() ]

        // RecursiveSetProperty( add, itemValue )
        // propertyValue.AppendItem(     SetValue( key, Property::Value(Property::ARRAY) );

        // propertyValue.SetValue( key, Property::Value(Property::ARRAY) );

        // RecursiveSetProperty( propertyValue.GetValue( key ), itemValue );
      }
    }
    else
    {
      propertyValue = Property::Value(Property::MAP);
      val keys = val::global("Object").call<val>("keys", fromVal);
      int keyLength = keys["length"].as<int>();
      for( int j = 0; j < keyLength; ++j )
      {
        Property::Value add;
        std::string key = keys[j].as<std::string>();
        propertyValue.SetValue( key , add );
        val keyVal = fromVal[ key ];
        RecursiveSetProperty( propertyValue.GetValue( key ), keyVal );
      }
      // propertyValue.SetValue( key, Property::Value(Property::MAP) );
      // RecursiveSetProperty( map.GetValue(key), itemValue );
    }
  }
  else if( fromType == "number" )
  {
    propertyValue = Property::Value( fromVal.as<float>() );
  }
  else if( fromType == "string" )
  {
    propertyValue = Property::Value( fromVal.as<std::string>() );
  }
  else
  {
    assert(0);
  }

}

emscripten::val JavascriptValue( const Property::Value& v )
{
  switch( v.GetType() )
  {
    case Property::BOOLEAN:
    {
      return emscripten::val(v.Get<bool>());
      break;
    }
    case Property::FLOAT:
    {
      return emscripten::val(v.Get<float>());
      break;
    }
    case Property::INTEGER:
    {
      return emscripten::val(v.Get<int>());
      break;
    }
    case Property::UNSIGNED_INTEGER:
    {
      return emscripten::val(v.Get<unsigned int>());
      break;
    }
    case Property::VECTOR2:
    {
      Dali::Vector2 in = v.Get<Dali::Vector2>();
      emscripten::val out = emscripten::val::array();
      out.set("0", emscripten::val(in.x) );
      out.set("1", emscripten::val(in.y) );
      return out;
      break;
    }
    case Property::VECTOR3:
    {
      Dali::Vector3 in = v.Get<Dali::Vector3>();
      emscripten::val out = emscripten::val::array();
      out.set("0", emscripten::val(in.x) );
      out.set("1", emscripten::val(in.y) );
      out.set("2", emscripten::val(in.z) );
      return out;
      break;
    }
    case Property::VECTOR4:
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
    case Property::MATRIX3:
    {
      return emscripten::val( v.Get<unsigned int>() );

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
    case Property::MATRIX:
    {
      return emscripten::val( v.Get<unsigned int>() );

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
    case Property::RECTANGLE:
    {
      Rect<int> in = v.Get<Rect<int> >();
      emscripten::val out = emscripten::val::array();
      out.set("0", emscripten::val(in.x) );
      out.set("1", emscripten::val(in.y) );
      out.set("2", emscripten::val(in.width) );
      out.set("3", emscripten::val(in.height) );
      return out;
      break;
    }
    case Property::ROTATION:
    {
      Dali::Quaternion in = v.Get<Quaternion>();
      emscripten::val out = emscripten::val::array();
      // // (x == pitch, y == yaw, z == roll)
      // Vector4 euler = in.EulerAngles();
      // out.set("0", emscripten::val((float)Dali::Degree(Dali::Radian(euler.x))) );
      // out.set("1", emscripten::val((float)Dali::Degree(Dali::Radian(euler.y))) );
      // out.set("2", emscripten::val((float)Dali::Degree(Dali::Radian(euler.z))) );

      Vector3 axis;
      float angle;
      in.ToAxisAngle(axis, angle);

      out.set("0", emscripten::val( axis.x ) );
      out.set("1", emscripten::val( axis.y ) );
      out.set("2", emscripten::val( axis.z ) );
      out.set("3", emscripten::val( static_cast<float>(Dali::Degree(Dali::Radian(angle)) ) ) );

      return out;
      break;
    }
    case Property::STRING:
    {
      return emscripten::val( v.Get<std::string>() );
      break;
    }
    case Property::ARRAY:
    {
      emscripten::val val = emscripten::val::array();

      for( int i = 0; i < v.GetSize(); ++i )
      {
        Property::Value& property = v.GetItem( i );
        std::stringstream key;
        key << 0;
        val.set( key.str(), JavascriptValue( property ) );
      }

      return val;
      break;
    }
    case Property::MAP:
    {
      emscripten::val val = emscripten::val::object();

      for( int i = 0; i < v.GetSize(); ++i )
      {
        std::string key;
        Property::Value& property = v.GetItem( i, key );
        val.set( key, JavascriptValue( property ) );
      }

      return val;
      break;
    }
    case Property::TYPE_COUNT:
    case Property::NONE:
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

//
// Creating property Value Objects
//   javascript can't select on type so we have renamed constructors
//
// Emscripten can convert some basic types and ones we've declared as value_array(s)
// (These are given member access offsets/functions and are for simple structs etc)
//
// The composite types need converting.
//
Property::Value PropertyBoolean(bool v)
{
  return Property::Value(v);
}

Property::Value PropertyFloat(float v)
{
  return Property::Value(v);
}

Property::Value PropertyVector2( const Dali::Vector2& v )
{
  return Property::Value(v);
}

Property::Value PropertyVector3( const Dali::Vector3& v )
{
  return Property::Value(v);
}

Property::Value PropertyVector4( const Dali::Vector4& v )
{
  return Property::Value(v);
}

Property::Value PropertyMatrix( const Dali::Matrix& v )
{
  return Property::Value(v);
}

Property::Value PropertyMatrix3( const Dali::Matrix3& v )
{
  return Property::Value(v);
}

Property::Value PropertyEuler( const Dali::Vector3& v )
{
  return Property::Value( Quaternion(
                            Radian(Degree(v.x)),
                            Radian(Degree(v.y)),
                            Radian(Degree(v.z)) ) );
}

Property::Value PropertyAxisAngle( const Dali::Vector4& v )
{
  Quaternion q( Quaternion::FromAxisAngle(v, Dali::Radian(Dali::Degree(v[3])) ) );
  return Property::Value( q );
}

Property::Value PropertyString( const std::string& v )
{
  return Property::Value(v);
}

Property::Value PropertyContainer( const emscripten::val& v )
{
  Property::Value ret;
  RecursiveSetProperty( ret, v );
  return ret;
}

//
// Property value accessors
//
//
bool PropertyGetBoolean(const Property::Value& v)
{
  return v.Get<bool>();
}

float PropertyGetFloat(const Property::Value& v)
{
  return v.Get<float>();
}

int PropertyGetInteger(const Property::Value& v)
{
  return v.Get<int>();
}

Dali::Vector2 PropertyGetVector2(const Property::Value& v)
{
  return v.Get<Dali::Vector2>();
}

Dali::Vector3 PropertyGetVector3(const Property::Value& v)
{
  return v.Get<Dali::Vector3>();
}

Dali::Vector4 PropertyGetVector4(const Property::Value& v)
{
  return v.Get<Dali::Vector4>();
}

std::string PropertyGetString(const Property::Value& v)
{
  return v.Get<std::string>();
}

emscripten::val PropertyGetMap(const Property::Value& v)
{
  return JavascriptValue(v);
}

emscripten::val PropertyGetArray(const Property::Value& v)
{
  return JavascriptValue(v);
}

emscripten::val PropertyGetMatrix(const Property::Value& v)
{
  return JavascriptValue(v);
}

emscripten::val PropertyGetMatrix3(const Property::Value& v)
{
  return JavascriptValue(v);
}

emscripten::val PropertyGetEuler(const Property::Value& v)
{
  return JavascriptValue(v);
}

emscripten::val PropertyGetRotation(const Property::Value& v)
{
  return JavascriptValue(v);
}

int PropertyGetType(Property::Value& v)
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

// I guess I could write template policies so emscrpten.element template handles a std::bind
// but then again, I'll be dead soon.
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

// class MatrixGetter
// {
// public:
//   explicit MatrixGetter(int index) : mIndex(index) {}
//   float operator()(Dali::Matrix& v)
//   {
//     return *(v.AsFloat()  + mIndex);
//   }
// private:
//   int mIndex;
// };

// class MatrixSetter
// {
// public:
//   explicit MatrixSetter(int index) : mIndex(index) {}
//   float operator()(Dali::Matrix& v, float a)
//   {
//     *(v.AsFloat()  + mIndex) = a;
//   }
// private:
//   int mIndex;
// };

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

extern void EmscriptenTouchEvent(double x, double y);

EMSCRIPTEN_BINDINGS(dali_wrapper)
{
//   class_<BaseHandleWrapper>("BaseHandle")
//     .constructor<BaseHandle>()
//     .smart_ptr<std::shared_ptr<BaseHandleWrapper>>()
//;

  register_vector<std::string>("VectorString");
  register_vector<int>("VectorInt");
  register_vector<ActorWrapper>("VectorActor");

  function("createAny", &CreateAny);
  function("createActor", &CreateActor);
  function("getStage", &GetStage);
  function("getTypeRegistry", &GetTypeRegistry);

  // function("Update", &Update);
  // function("Render", &Render);

  function("setInterval", &SetInterval);

  function("setUpdateFunction", &SetUpdateFunction);
  function("getUpdateFunction", &GetUpdateFunction);

  function("javascriptValue", &JavascriptValue);

  function("setCallbackGetGlyphImage", &SetCallbackGetGlyphImage);
  function("setCallbackGetImage", &SetCallbackGetImage);
  function("setCallbackGetImageMetadata", &SetCallbackGetImageMetadata);

  function("sendTouchEvent", &EmscriptenTouchEvent);

  function("PropertyBoolean", &PropertyBoolean);
  function("PropertyFloat", &PropertyFloat);
  function("PropertyString", &PropertyString);
  function("PropertyVector2", &PropertyVector2);
  function("PropertyVector3", &PropertyVector3);
  function("PropertyVector4", &PropertyVector4);
  function("PropertyMatrix", &PropertyMatrix);
  function("PropertyMatrix3", &PropertyMatrix3);
  function("PropertyMap", &PropertyContainer);
  function("PropertyArray", &PropertyContainer);
  function("PropertyEuler", &PropertyEuler);
  function("PropertyAxisAngle", &PropertyAxisAngle);

  function("getTypeRegistry", &GetTypeRegistry);

  // function("SetImage", &SetImage);

// EMSCRIPTEN_BINDING(overloads) {
//     class_<HasOverloadedMethods>("HasOverloadedMethods")
//         .function("foo", select_overload<void()>(&HasOverloadedMethods::foo))
//         .function("foo_int", select_overload<void(int)>(&HasOverloadedMethods::foo))
//         .function("foo_float", select_overload<void(float)const>(&HasOverloadedMethods::foo))
//;

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
    // .smart_ptr<std::shared_ptr<StageWrapper>>("StageWrapper")
    .function("addActor", &StageWrapper::Add)
    .function("removeActor", &StageWrapper::Remove)
    .function("getRootLayer", &StageWrapper::GetRootLayer)
    .function("getRenderTaskList", &StageWrapper::GetRenderTaskList)
;

  class_<TypeInfoWrapper>("TypeInfo")
    // .smart_ptr<TypeInfoWrapper>("TypeInfoWrapper")
    .function("getActions", &TypeInfoWrapper::GetActions)
    .function("getSignals", &TypeInfoWrapper::GetSignals)
    .function("getProperties", &TypeInfoWrapper::GetProperties)
    .function("getPropertyIndices", &TypeInfoWrapper::GetPropertyIndices)
;

  class_<TypeRegistryWrapper>("TypeRegistry")
    // .smart_ptr<std::shared_ptr<TypeRegistryWrapper>>("TypeRegistryWrapper")
    .function("getTypeNames", &TypeRegistryWrapper::GetTypeNames)
    .function("getTypeInfo", &TypeRegistryWrapper::GetTypeInfo)
;

  class_<HandleWrapper>("HandleWrapper")
    .constructor<Handle>()
    // .smart_ptr<Dali::Handle  std::shared_ptr<HandleWrapper>>("HandleWrapper")
    .function("setProperty", &HandleWrapper::SetProperty)
    .function("getProperty", &HandleWrapper::GetProperty)
    .function("getPropertyIndex", &HandleWrapper::GetPropertyIndex)
    .function("getTypeInfo", &HandleWrapper::GetTypeInfo)
    .function("getPropertyIndices", &HandleWrapper::GetPropertyIndices)
    .function("getProperties", &HandleWrapper::GetProperties)
;

  class_<ActorWrapper, base<HandleWrapper>>("ActorWrapper")
    .constructor<Actor>()
    // .smart_ptr<std::shared_ptr<ActorWrapper>>("ActorWrapper")
    .function("setImage", &ActorWrapper::SetImage)
    .function("ok", &ActorWrapper::Ok)
    .function("add", &ActorWrapper::Add)
    .function("remove", &ActorWrapper::Remove)
    // functions which return actors are adorned internal with '__'
    // we do more processing on the return in the js layer
    .function("__getParent", &ActorWrapper::GetParent)
    .function("__findChildById", &ActorWrapper::FindChildById)
    .function("__findChildByName", &ActorWrapper::FindChildByName)
    .function("__getChildAt", &ActorWrapper::GetChildAt)
    .function("getChildCount", &ActorWrapper::GetChildCount)
    .function("getId", &ActorWrapper::GetId)
    // .function("SetEffectImage", &ActorWrapper::SetEffectImage)
    // // property simulation    .property("x", &MyClass::getX, &MyClass::setX)
    // // static functions     .class_function("getStringFromInstance", &MyClass::getStringFromInstance) // static function
;

  class_<LayerWrapper>("LayerWrapper")
    .constructor<Layer>()
    // .smart_ptr<std::shared_ptr<LayerWrapper>>("LayerWrapperWrapper")
    .function("raise", &LayerWrapper::Raise)
    .function("lower", &LayerWrapper::Lower)
//     .function("SetProperty", &ActorWrapper::GetProperty)
// // property simulation    .property("x", &MyClass::getX, &MyClass::setX)
// // static functions     .class_function("getStringFromInstance", &MyClass::getStringFromInstance) // static function
;

//   class_<PropertyMapWrapper>("PropertyMap")
//     .constructor<PropertyMapWrapper>()
// //    .smart_ptr<std::shared_ptr<PropertyMapWrapper>>("LayerWrapperWrapper")
//     .function("Add", &PropertyMapWrapper::Add)
//     .function("Append", &PropertyMapWrapper::Append)
//     .function("AddMap", &PropertyMapWrapper::AddMap)
//     .function("AddArray", &PropertyMapWrapper::AddArray)
//;

  class_<AnimationWrapper>("AnimationWrapper")
    .constructor<>()
    // .smart_ptr<std::shared_ptr<AnimationWrapper>>("AnimationWrapper")
    .function("setDuration", &AnimationWrapper::SetDuration)
    .function("setLooping", &AnimationWrapper::SetLooping)
    .function("play", &AnimationWrapper::Play)
    .function("pause", &AnimationWrapper::Pause)
    .function("stop", &AnimationWrapper::Stop)
    .function("clear", &AnimationWrapper::Clear)
    .function("animateTo", &AnimationWrapper::AnimateTo)
    .function("animateBy", &AnimationWrapper::AnimateBy)
;

}
