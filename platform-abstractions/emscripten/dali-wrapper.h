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

#include "emscripten/emscripten.h"
#include "emscripten/bind.h"

// INTERNAL INCLUDES
class TypeInfoWrapper;

////////////////////////////////////////////////////////////////////////////////
//
// To try to make the Javascript interface convenient and natural for the end
// user script written in the browser environment objects are wrapped and
// marshalled with a wrapper layer written in cpp and javascript.
//
// Unfortunately there is no finalize/destruction in javascript so any cpp
// objects created must be explicitly deleted. Sometimes this wrapper can
// hide the details and some simple types can be immediately marshalled to and
// from javascript values by embind directly. Otherwise objects created must be
// object.delete()
//
////////////////////////////////////////////////////////////////////////////////

//
// HandleWrapper
//
class HandleWrapper
{
public:
  HandleWrapper() {};
  explicit HandleWrapper(Dali::Handle handle);
  ~HandleWrapper();

  void SetProperty(const std::string& name, const Dali::Property::Value& value);

  Dali::Property::Value GetProperty(const std::string& name) const;

  std::string GetPropertyType(const std::string& name) const;

  int GetPropertyIndex(const std::string& name) const;

  std::string GetPropertyName(int index) const;

  Dali::Handle GetHandle() { return mHandle ; }

  TypeInfoWrapper GetTypeInfo() const;

  int GetPropertyCount() const;

  std::vector<int> GetPropertyIndices() const;

  std::vector<std::string> GetProperties() const;

  Dali::Property::Index RegisterProperty(const std::string& name, const Dali::Property::Value& propertyValue);

  Dali::Property::Index RegisterAnimatedProperty(const std::string& name, const Dali::Property::Value& propertyValue);

public:
  HandleWrapper( const HandleWrapper&o ) { mHandle = o.mHandle ; };
  HandleWrapper& operator=( const HandleWrapper&o ) { mHandle = o.mHandle; return *this; };

protected:
  Dali::Handle mHandle;
};

//
// ShaderEffectWrapper
//
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

//
// ActorWrapper
//
// - ConnectionTrackerInterface to add signals only to actors so that we can
//   return the actor (80/90%% use case)
//
class ActorWrapper : public HandleWrapper, public Dali::ConnectionTrackerInterface
{
public:
  ActorWrapper() {};
  explicit ActorWrapper(Dali::Actor actor);
  ~ActorWrapper();

  Dali::Actor GetActor() const;

  void SetImage(const std::string& data);

  void SetImageRawRGBA(unsigned int width, unsigned int height, const std::string& data);

  void SetImageRawRGB(unsigned int width, unsigned int height, const std::string& data);

  void Add(ActorWrapper child);

  void Remove(ActorWrapper child);

  void Insert(int index, ActorWrapper child);

  ActorWrapper GetParent() const;

  ActorWrapper FindChildById(const unsigned int id);

  ActorWrapper FindChildByName(const std::string& actorAlias);

  ActorWrapper GetChildAt(unsigned int index) const;

  unsigned int GetChildCount() const;

  unsigned int GetId() const;

  std::vector<std::string> GetProperties() const;

  std::vector<float> ScreenToLocal(float screenX, float screenY) const;

  bool Ok();

  bool ConnectSignal( const std::string& signalName, const emscripten::val& javascriptFunction );

  unsigned int AddressOf();

public: // ConnectionTrackerInterface

  virtual void SignalConnected( Dali::SlotObserver* slotObserver, Dali::CallbackBase* callback );

public: // SignalObserver
  virtual void SignalDisconnected( Dali::SlotObserver* slotObserver, Dali::CallbackBase* callback );

public: // ConnectionTrackerInterface forces this to be defined
  ActorWrapper( const ActorWrapper&o ) { mActor = o.mActor ; mHandle = o.mHandle ; };
  ActorWrapper& operator=( const ActorWrapper&o ) { mActor = o.mActor; mHandle = o.mHandle ; return *this; };

private:
  Dali::Actor mActor;
};

//
// RenderTaskWrapper
//
class RenderTaskWrapper : public HandleWrapper
{
public:
  explicit RenderTaskWrapper(Dali::RenderTask renderTask);
  ~RenderTaskWrapper();

  Dali::RenderTask Get() { return mRenderTask ; }

  void SetSourceActor( ActorWrapper actor );

  void SetExclusive( bool exclusive );

  void SetInputEnabled( bool enabled );

  void SetCameraActor(  ActorWrapper cameraActor  );

  ActorWrapper GetCameraActor( );

  void SetViewportPosition( Dali::Vector2 position );
  void SetViewportSize( Dali::Vector2 size );

  Dali::Vector2 GetCurrentViewportPosition() const;
  Dali::Vector2 GetCurrentViewportSize() const;

  void SetClearColor( const Dali::Vector4& color );
  void SetClearEnabled( bool enabled );
  bool GetClearEnabled() const;

  Dali::Vector2 ScreenToLocal(ActorWrapper actor, float screenX, float screenY) const;
  Dali::Vector2 WorldToScreen(const Dali::Vector3 &position) const;

protected:
  Dali::RenderTask mRenderTask;
};

//
// RenderTaskListWrapper
//
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

//
// AnimationWrapper
//
class AnimationWrapper
{
public:
  AnimationWrapper();
  AnimationWrapper(const AnimationWrapper& copy)
    : mAnimation(copy.mAnimation) {};

  ~AnimationWrapper();

  void SetDuration(float seconds);

  void SetLooping(bool looping);

  void SetEndAction(const std::string& action);

  void SetDisconnectAction(const std::string& action);

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

private:
  const AnimationWrapper& operator=(const AnimationWrapper& assign) = delete;

  Dali::Animation mAnimation;
};


class LayerWrapper;

//
// StageWrapper
//
class StageWrapper
{
public:
  StageWrapper();

  void Add( ActorWrapper actor);

  void Remove( ActorWrapper actor);

  LayerWrapper GetRootLayer() const;

  StageWrapper GetStage() const;

  RenderTaskListWrapper GetRenderTaskList() const;

  void SetBackgroundColor(Dali::Vector4& color);

  unsigned int GetLayerCount() const;

  LayerWrapper GetLayer(unsigned int depth) const;

  Dali::Vector2 GetDpi() const;

private:

};

class LayerWrapper : public ActorWrapper
{
public:
  explicit LayerWrapper(Dali::Layer);

  void Raise();

  void Lower();

private:
  Dali::Layer mLayer;
};

//
// PropertyMapWrapper
//
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

//
// TypeInfoWrapper
//
class TypeInfoWrapper
{
public:
  TypeInfoWrapper() {};
  explicit TypeInfoWrapper(Dali::TypeInfo typeInfo);

  std::vector<std::string> GetProperties() const;

  std::vector<std::string> GetSignals() const;

  std::vector<std::string> GetActions() const;

  std::vector<int> GetPropertyIndices() const;

  std::string GetName() const;

  std::string GetBaseName() const;

private:
  Dali::TypeInfo mInfo;
};

//
// TypeRegistryWrapper
//
class TypeRegistryWrapper
{
public:
  TypeRegistryWrapper();

  Dali::TypeRegistry::NameContainer GetTypeNames();

  TypeInfoWrapper GetTypeInfo(const std::string& typeName);

};

//
// MaterialWrapper
//
class MaterialWrapper
{
public:
  MaterialWrapper(const std::string name);

  void SetOpacity(const float opacity);
  void SetShininess(const float shininess);
  void SetAmbientColor(const Dali::Vector4& color);
  void SetDiffuseColor(const Dali::Vector4& color);
  void SetEmissiveColor(const Dali::Vector4& color);
  void SetSpecularColor(const Dali::Vector4& color);

  void SetDiffuseTextureRGBAData(unsigned int width, unsigned int height, const std::string& data);
  void SetDiffuseTextureRGBData(unsigned int width, unsigned int height, const std::string& data);

  void SetOpacityTextureRGBAData(unsigned int width, unsigned int height, const std::string& data);
  void SetOpacityTextureRGBData(unsigned int width, unsigned int height, const std::string& data);

  Dali::Material GetMaterial() const { return mMaterial; };
private:
  Dali::Material mMaterial;
};

//
// MeshDataWrapper
//
class MeshDataWrapper
{
public:
  Dali::MeshData meshData;

  void SetData( const Dali::MeshData::VertexContainer& vertices,
                const Dali::MeshData::FaceIndices& faceIndices,
                const Dali::BoneContainer&   bones,
                const MaterialWrapper  material );

  void SetLineData( const Dali::MeshData::VertexContainer& vertices,
                    const Dali::MeshData::FaceIndices& faceIndices,
                    const MaterialWrapper  material );

  void SetFaceIndices( const Dali::MeshData::FaceIndices& faceIndices );
  void SetVertices( const Dali::MeshData::VertexContainer& vertices );
  void SetHasNormals(bool hasNormals);
  void SetHasTextureCoords(bool hasTexCoords);
  void SetHasColor(bool hasColor);
};

#endif // header
