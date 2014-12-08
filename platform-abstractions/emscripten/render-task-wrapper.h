#ifndef __DALI_RENDER_TASK_WRAPPER_H__
#define __DALI_RENDER_TASK_WRAPPER_H__

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
#include <dali/public-api/dali-core.h>
#include "emscripten/emscripten.h"
#include "emscripten/bind.h"

// INTERNAL INCLUDES
#include "actor-wrapper.h"

namespace Dali
{
namespace Internal
{
namespace Emscripten
{


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
  Dali::Vector4 GetClearColor();
  void SetClearEnabled( bool enabled );
  bool GetClearEnabled() const;

  Dali::Vector2 ScreenToLocal(ActorWrapper actor, float screenX, float screenY) const;
  Dali::Vector2 WorldToScreen(const Dali::Vector3 &position) const;

protected:
  Dali::RenderTask mRenderTask;
};


}; // namespace Emscripten
}; // namespace Internal
}; // namespace Dali

#endif // header
