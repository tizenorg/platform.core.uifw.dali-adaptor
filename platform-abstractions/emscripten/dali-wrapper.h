#ifndef __DALI_WRAPPER_H__
#define __DALI_WRAPPER_H__

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

// EXTERNAL INCLUDES
#include <dali/public-api/dali-core.h>

// INTERNAL INCLUDES
class TypeInfoWrapper;

////////////////////////////////////////////////////////////////////////////////
//
// One way to get emscripten/embind handle the memory deallocs (There is no
// guarranteed destructor in Javascript) is to use a shared pointer.
//
// You could also (with embind) setup some type traits to inform about your own
// smart pointer, but...
//
// Dali has a handle hierarchy which hold smart pointers. We also have data
// marshalling issues handled by a combination of the dali-wrapper.cpp and
// dali-wrapper.js.
//
// Because of these problems we end up with wrapper objects, managed by
// smart pointers, holding handles, holding intrusive pointers to
// actual Dali objects.
//
// Nothing can't be solved by one more level of indirection.
// Even so, it seems excessive. ....one more waifer thin mint...
//
////////////////////////////////////////////////////////////////////////////////


// !!
//
// No thats not true.... You still have to delete - so removing smart pointers
//
//



/* class ShaderEffectWrapper; */
/* class HandleWrapper; */
/* class ActorWrapper; */
/* class TypeInfoWrapper; */
/* class LayerWrapper; */
/* class RenderTask; */
/* class RenderTaskList; */
/* class CameraWrapper; */

/* typedef std::shared_ptr<ShaderEffectWrapper> ShaderEffectPtr; */
/* /\* typedef std::shared_ptr<HandleWrapper> HandlePtr; *\/ */
/* typedef std::shared_ptr<ActorWrapper> ActorPtr; */
/* typedef std::shared_ptr<TypeInfoWrapper> TypeInfoPtr; */
/* typedef std::shared_ptr<LayerWrapper> LayerPtr; */
/* typedef std::shared_ptr<RenderTaskWrapper> RenderTaskPtr; */
/* typedef std::shared_ptr<RenderTaskListWrapper> RenderTaskListPtr; */
/* typedef std::shared_ptr<CameraWrapper> CameraPtr; */

class HandleWrapper
{
public:
  HandleWrapper() {};
  explicit HandleWrapper(Dali::Handle handle);
  ~HandleWrapper();

  void SetProperty(const std::string& name, const Dali::Property::Value& value);

  Dali::Property::Value GetProperty(const std::string& name) const;

  int GetPropertyIndex(const std::string& name) const;

  Dali::Handle GetHandle() { return mHandle ; }

  TypeInfoWrapper GetTypeInfo() const;

  int GetPropertyCount() const;

  std::vector<int> GetPropertyIndices() const;

  std::vector<std::string> GetProperties() const;

protected:
  Dali::Handle mHandle;
};

class ShaderEffectWrapper : public HandleWrapper
{
public:
  ShaderEffectWrapper(Dali::ShaderEffect shaderEffect);
  ~ShaderEffectWrapper();

  void SetEffectImage(const std::string& data);

  Dali::ShaderEffect GetShaderEffect() { return mShaderEffect ; }

protected:
  Dali::ShaderEffect mShaderEffect;
};

class CameraActorWrapper
{
public:
  CameraActorWrapper(Dali::CameraActor);

  Dali::CameraActor Get();

private:
  Dali::CameraActor mCamera;
};

class ActorWrapper : public HandleWrapper
{
public:
  ActorWrapper() {};
  explicit ActorWrapper(Dali::Actor actor);
  ~ActorWrapper();

  Dali::Actor GetActor() const;

  void SetImage(const std::string& data);

  /* void SetShaderEffect( ShaderEffectWrapper shaderEffect ); */

  /* void RemoveShaderEffect(); */

  bool Ok();

private:
  Dali::Actor mActor;
};


class RenderTaskWrapper : public HandleWrapper
{
public:
  explicit RenderTaskWrapper(Dali::RenderTask renderTask);
  ~RenderTaskWrapper();

  Dali::RenderTask Get() { return mRenderTask ; }

  void SetSourceActor( ActorWrapper actor );

  void SetExclusive( bool exclusive );

  void SetInputEnabled( bool enabled );

  void SetCameraActor( CameraActorWrapper cameraActor );

protected:
  Dali::RenderTask mRenderTask;
};

class RenderTaskListWrapper : public HandleWrapper
{
public:
  explicit RenderTaskListWrapper(Dali::RenderTaskList renderTaskList);
  ~RenderTaskListWrapper();

  RenderTaskWrapper CreateTask();
  void RemoveTask( RenderTaskWrapper task );
  unsigned int GetTaskCount() const;
  RenderTaskWrapper GetTask( unsigned int index ) const;

protected:
  Dali::RenderTaskList mRenderTaskList;
};


class AnimationWrapper
{
public:
  AnimationWrapper();
  AnimationWrapper(const AnimationWrapper& copy)
    : mAnimation(copy.mAnimation) {};

  ~AnimationWrapper();

  /* enum AlphaFunctionEnum */
  /* { */
  /*   Default, */
  /*   Linear, */
  /*   Square, */
  /*   Reverse, */
  /*   EaseIn, */
  /*   EaseOut, */
  /*   EaseInOut, */
  /*   EaseInSine, */
  /*   EaseOutSine, */
  /*   EaseInOutSine, */
  /*   EaseInSine33, */
  /*   EaseOutSine33, */
  /*   EaseInOutSine33, */
  /*   EaseInOutSine50, */
  /*   EaseInOutSine60, */
  /*   EaseInOutSine70, */
  /*   EaseInOutSine80, */
  /*   EaseInOutSine90, */
  /*   DoubleEaseInOutSine60, */
  /*   EaseOutQuint50, */
  /*   EaseOutQuint80, */
  /*   Bounce, */
  /*   BounceBack, */
  /*   EaseInBack, */
  /*   EaseOutBack, */
  /*   EaseInOutBack, */
  /*   Sin, */
  /*   Sin2x, */
  /* }; */

  /* enum EndAction */
  /* { */
  /*   Bake,     ///< When the animation ends, the animated property values are saved. */
  /*   Discard,  ///< When the animation ends, the animated property values are forgotten. */
  /*   BakeFinal ///< If the animation is stopped, the animated property values are saved as if the animation had run to completion, otherwise behaves like Bake. */
  /* }; */

  void SetDuration(float seconds);

  void SetLooping(bool looping);

  void SetEndAction(const std::string& action);

  void SetDestroyAction(const std::string& action);

  /* void SetDefaultAlphaFunction(AlphaFunction alpha); */

  void Play();

  void Pause();

  void Stop();

  void Clear();

  void AnimateTo(HandleWrapper handleWrapper,
                 const std::string& property,
                 Dali::Property::Value destinationValue,
                 const std::string& alphaFunction,
                 float delay, float duration);

  void AnimateBy(HandleWrapper handleWrapper,
                 const std::string& property,
                 Dali::Property::Value destinationValue,
                 const std::string& alphaFunction,
                 float delay, float duration);

  /* void AnimateBy(Property target, Property::Value relativeValue, AlphaFunction alpha, TimePeriod period); */

private:
  const AnimationWrapper& operator=(const AnimationWrapper& assign) = delete;

  Dali::Animation mAnimation;
};


class LayerWrapper;

class StageWrapper
{
public:
  StageWrapper();

  void Add( ActorWrapper actor);

  void Remove( ActorWrapper actor);

  LayerWrapper GetRootLayer() const;

  StageWrapper GetStage() const;

  RenderTaskListWrapper GetRenderTaskList() const;
private:

};

class LayerWrapper
{
public:
  explicit LayerWrapper(Dali::Layer);

  void Raise();

  void Lower();

private:
  Dali::Layer mLayer;
};

class PropertyMapWrapper
{
public:
  PropertyMapWrapper();
  explicit PropertyMapWrapper(Dali::Property::Value* notOwnedValue);
  ~PropertyMapWrapper();

  void Add(const std::string& key, Dali::Property::Value& value);

  void Append(Dali::Property::Value& value);

  PropertyMapWrapper AddMap(const std::string& key);

  PropertyMapWrapper AddArray(const std::string& key);

private:
  bool mOwner;
  Dali::Property::Value *mValue;

};

class TypeInfoWrapper
{
public:
  TypeInfoWrapper() {};
  explicit TypeInfoWrapper(Dali::TypeInfo typeInfo);

  Dali::TypeInfo::NameContainer GetProperties();

  Dali::TypeInfo::NameContainer GetSignals();

  Dali::TypeInfo::NameContainer GetActions();

  std::vector<int> GetPropertyIndices() const;

private:
  Dali::TypeInfo mInfo;
};

class TypeRegistryWrapper
{
public:
  TypeRegistryWrapper();

  Dali::TypeRegistry::NameContainer GetTypeNames();

  TypeInfoWrapper GetTypeInfo(const std::string& typeName);

};


#endif // header
