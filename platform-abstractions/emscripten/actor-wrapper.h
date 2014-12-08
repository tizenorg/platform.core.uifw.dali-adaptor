#ifndef __DALI_ACTOR_WRAPPER_H__
#define __DALI_ACTOR_WRAPPER_H__

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
