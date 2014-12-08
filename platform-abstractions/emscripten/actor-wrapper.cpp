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

#include "actor-wrapper.h"

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include "emscripten-utils.h"


namespace Dali
{
namespace Internal
{
namespace Emscripten
{

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
      emscripten::val ret = mCallback( Dali::Internal::Emscripten::ActorWrapper(a) );
    }
    else
    {
      emscripten::val ret = mCallback();
    }
    return true;
  }
};

}; // anon namespace


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



}; // namespace Emscripten
}; // namespace Internal
}; // namespace Dali
