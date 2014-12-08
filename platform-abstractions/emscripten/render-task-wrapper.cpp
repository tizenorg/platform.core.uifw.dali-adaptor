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

#include "render-task-wrapper.h"

// EXTERNAL INCLUDES

// INTERNAL INCLUDES

namespace Dali
{
namespace Internal
{
namespace Emscripten
{

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


}; // namespace Emscripten
}; // namespace Internal
}; // namespace Dali
