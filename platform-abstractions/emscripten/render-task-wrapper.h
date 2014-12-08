#ifndef __DALI_RENDER_TASK_WRAPPER_H__
#define __DALI_RENDER_TASK_WRAPPER_H__

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
