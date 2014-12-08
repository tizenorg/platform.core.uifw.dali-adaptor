#ifndef __DALI_ACTOR_WRAPPER_H__
#define __DALI_ACTOR_WRAPPER_H__

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

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include "handle-wrapper.h"
#include "shader-effect-wrapper.h"

namespace Dali
{
namespace Internal
{
namespace Emscripten
{

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

  void Add(ActorWrapper child);

  void Remove(ActorWrapper child);

  void Insert(int index, ActorWrapper child);

  ActorWrapper GetParent() const;

  ActorWrapper FindChildById(const unsigned int id);

  ActorWrapper FindChildByName(const std::string& actorAlias);

  ActorWrapper GetChildAt(unsigned int index) const;

  unsigned int GetChildCount() const;

  unsigned int GetId() const;

  void SetImage(const std::string& data);

  void SetImageRawRGBA(unsigned int width, unsigned int height, const std::string& data);

  void SetImageRawRGB(unsigned int width, unsigned int height, const std::string& data);

  std::vector<std::string> GetProperties() const;

  std::vector<float> ScreenToLocal(float screenX, float screenY) const;

  bool Ok();

  bool ConnectSignal( const std::string& signalName, const emscripten::val& javascriptFunction );

  unsigned int AddressOf();

  void SetShaderEffect( ShaderEffectWrapper effect );

  ShaderEffectWrapper GetShaderEffect() const;

  void RemoveShaderEffect();

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


}; // namespace Emscripten
}; // namespace Internal
}; // namespace Dali

#endif // header
