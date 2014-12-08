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

emscripten::val JavascriptValue( const Dali::Property::Value& v );

struct BaseSignalSlot : public Dali::ConnectionTracker
{
};

// generic signal callback

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

// touched signal callback
struct EmscriptenTouchedSignal : public Dali::Internal::Emscripten::BaseSignalSlot
{
  EmscriptenTouchedSignal(const emscripten::val& callback, unsigned int id)
    : mCallback(callback), mId(id)  {};
  emscripten::val mCallback;
  unsigned int mId;

  bool OnTouched(Dali::Actor actor, const Dali::TouchEvent& event)
  {
    Dali::Property::Map map;
    Dali::Property::Array points;

    map["pointCount"] = static_cast<int>(event.GetPointCount());
    map["time"] = static_cast<int>(event.time);

    for(TouchPointContainer::const_iterator iter = event.points.begin();
        iter != event.points.end(); ++iter) {
      const Dali::TouchPoint& pt = *iter;
      Dali::Property::Map pointMap;
      pointMap["deviceId"] = pt.deviceId;
      pointMap["hitActorId"] = static_cast<int>(pt.hitActor.GetId());
      pointMap["local"] = pt.local;
      pointMap["screen"]= pt.screen;

      switch(pt.state)
      {
        case TouchPoint::Down:        { pointMap["state"] = "Down"; break; }
        case TouchPoint::Up:          { pointMap["state"] = "Up"; break; }
        case TouchPoint::Motion:      { pointMap["state"] = "Motion"; break; }
        case TouchPoint::Leave:       { pointMap["state"] = "Leave"; break; }
        case TouchPoint::Stationary:  { pointMap["state"] = "Stationary"; break; }
        case TouchPoint::Interrupted: { pointMap["state"] = "Interrupted"; break; }
        case TouchPoint::Last:        { pointMap["state"] = "Last"; break; }
      };

      points.PushBack(pointMap);
    }

    map["points"] = points;


    Dali::Property::Value value(map);
    emscripten::val ret = mCallback( Dali::Internal::Emscripten::ActorWrapper(actor),
                                     JavascriptValue(value) );
    return true;
  }

  bool OnHovered(Dali::Actor actor, const Dali::HoverEvent& event)
  {
    Dali::Property::Map map;
    Dali::Property::Array points;

    map["pointCount"] = static_cast<int>(event.GetPointCount());
    map["time"] = static_cast<int>(event.time);

    for(TouchPointContainer::const_iterator iter = event.points.begin();
        iter != event.points.end(); ++iter) {
      const Dali::TouchPoint& pt = *iter;
      Dali::Property::Map pointMap;
      pointMap["deviceId"] = pt.deviceId;
      pointMap["hitActorId"] = static_cast<int>(pt.hitActor.GetId());
      pointMap["local"] = pt.local;
      pointMap["screen"]= pt.screen;

      switch(pt.state)
      {
        case TouchPoint::Down:        { pointMap["state"] = "Down"; break; }
        case TouchPoint::Up:          { pointMap["state"] = "Up"; break; }
        case TouchPoint::Motion:      { pointMap["state"] = "Motion"; break; }
        case TouchPoint::Leave:       { pointMap["state"] = "Leave"; break; }
        case TouchPoint::Stationary:  { pointMap["state"] = "Stationary"; break; }
        case TouchPoint::Interrupted: { pointMap["state"] = "Interrupted"; break; }
        case TouchPoint::Last:        { pointMap["state"] = "Last"; break; }
      };

      points.PushBack(pointMap);
    }

    map["points"] = points;

    Dali::Property::Value value(map);
    emscripten::val ret = mCallback( Dali::Internal::Emscripten::ActorWrapper(actor),
                                     JavascriptValue(value) );
    return true;
  }

};


struct EmscriptenNotifiy
{
  EmscriptenNotifiy(const emscripten::val& callback )
    : mCallback(callback) {};
  emscripten::val mCallback;

  void operator()(PropertyNotification& /* source */)
  {
    emscripten::val ret = mCallback();
  }
};


namespace
{

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
  for(Slots::Iterator iter = mSlots.Begin(); iter != mSlots.End(); ++iter)
  {
    delete *iter;
  }
  mSlots.Clear();
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
  return ActorWrapper(mActor.FindChildByName(actorAlias));
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
  if( Dali::ImageActor actor = Dali::ImageActor::DownCast(mActor) )
  {
    actor.SetShaderEffect( effect.GetShaderEffect() );
  }
}

ShaderEffectWrapper ActorWrapper::GetShaderEffect() const
{
  if( Dali::ImageActor renderable = Dali::ImageActor::DownCast(mActor) )
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
  if( Dali::ImageActor renderable = Dali::ImageActor::DownCast(mActor) )
  {
    renderable.RemoveShaderEffect();
  }
}

bool ActorWrapper::ConnectSignal( const std::string& signalName,
                                  const emscripten::val& javascriptFunction )
{
  if(0 == signalName.compare("touched"))
  {
    EmscriptenTouchedSignal* slot = new EmscriptenTouchedSignal(javascriptFunction, mActor.GetId());
    mSlots.PushBack(slot);
    mActor.TouchedSignal().Connect(slot, &EmscriptenTouchedSignal::OnTouched);
    return true;
  }
  else if(0 == signalName.compare("hovered"))
  {
    EmscriptenTouchedSignal* slot = new EmscriptenTouchedSignal(javascriptFunction, mActor.GetId());
    mSlots.PushBack(slot);
    mActor.HoveredSignal().Connect(slot, &EmscriptenTouchedSignal::OnHovered);
    return true;
  }
  else
  {
    return mHandle.ConnectSignal( this, signalName, EmscriptenSignal(javascriptFunction, mActor.GetId()) );
  }
}

void ActorWrapper::SignalConnected( Dali::SlotObserver* slotObserver, Dali::CallbackBase* callback )
{
}

void ActorWrapper::SignalDisconnected( Dali::SlotObserver* slotObserver, Dali::CallbackBase* callback )
{
}

// api removed
// void ActorWrapper::Insert(int index, ActorWrapper child)
// {
//   if(Ok() && child.Ok())
//   {
//     mActor.Insert(index, child.GetActor());
//   }
// }

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

void ActorWrapper::SetPropertyNotification( Dali::Property::Index index, const std::string& propertyConditionType, float arg0, float arg1,
                                            const emscripten::val& javascriptFunction)
{
  DALI_ASSERT_ALWAYS(mActor);

  unsigned int i = FindEnumIndex( propertyConditionType.c_str(), ConditionTypeTable, ConditionTypeTableSize );

  if( i < ConditionTypeTableSize )
  {
    ConditionType type = static_cast<ConditionType>(ConditionTypeTable[i].value);
    Dali::PropertyNotification notification;
    switch(type)
    {
      case ConditionType::False:
      {
        notification = mActor.AddPropertyNotification( index, Dali::LessThanCondition(arg0) );
      }
      case ConditionType::LessThan:
      {
        notification = mActor.AddPropertyNotification( index, Dali::LessThanCondition(arg0) );
      }
      case ConditionType::GreaterThan:
      {
        notification = mActor.AddPropertyNotification( index, Dali::GreaterThanCondition(arg0) );
      }
      case ConditionType::Inside:
      {
        notification = mActor.AddPropertyNotification( index, Dali::InsideCondition(arg0, arg1) );
      }
      case ConditionType::Outside:
      {
        notification = mActor.AddPropertyNotification( index, Dali::OutsideCondition(arg0, arg1) );
      }
      case ConditionType::Step:
      {
        notification = mActor.AddPropertyNotification( index, Dali::StepCondition(arg0, arg1) );
      }
      case ConditionType::VariableStep:
      {
        notification = mActor.AddPropertyNotification( index, Dali::StepCondition(arg0, arg1) );
      }
    };

    if(notification)
    {
      notification.NotifySignal().Connect( this, FunctorDelegate::New( EmscriptenNotifiy(javascriptFunction)) );
    }
    else
    {
      EM_ASM( throw "Cannot set notification" );
    }
  }
}


}; // namespace Emscripten
}; // namespace Internal
}; // namespace Dali
