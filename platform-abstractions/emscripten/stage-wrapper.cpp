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

#include "stage-wrapper.h"

// EXTERNAL INCLUDES

// INTERNAL INCLUDES

namespace Dali
{
namespace Internal
{
namespace Emscripten
{

StageWrapper::StageWrapper()
{

}

void StageWrapper::Add( ActorWrapper actorWrapper )
{
  Dali::Stage stage = Dali::Stage::GetCurrent();
  Dali::Actor actor = actorWrapper.GetActor();
  stage.Add(actor);
}

void StageWrapper::Remove( ActorWrapper actorWrapper )
{
  Dali::Stage stage = Dali::Stage::GetCurrent();
  Dali::Actor actor = actorWrapper.GetActor();
  stage.Remove(actor);
}

LayerWrapper StageWrapper::GetRootLayer() const
{
  Dali::Stage stage = Dali::Stage::GetCurrent();
  return LayerWrapper( stage.GetRootLayer() );
}

RenderTaskListWrapper StageWrapper::GetRenderTaskList() const
{
  Dali::Stage stage = Dali::Stage::GetCurrent();
  return RenderTaskListWrapper( stage.GetRenderTaskList() );
}

void StageWrapper::SetBackgroundColor(Dali::Vector4& color)
{
  Dali::Stage stage = Dali::Stage::GetCurrent();
  return stage.SetBackgroundColor( color );
}

unsigned int StageWrapper::GetLayerCount() const
{
  return Dali::Stage::GetCurrent().GetLayerCount();
}

LayerWrapper StageWrapper::GetLayer(unsigned int depth) const
{
  return LayerWrapper( Dali::Stage::GetCurrent().GetLayer(depth) );
}

Dali::Vector2 StageWrapper::GetDpi() const
{
  return Dali::Stage::GetCurrent().GetDpi();
}

Dali::Vector2 StageWrapper::GetSize() const
{
  return Dali::Stage::GetCurrent().GetSize();
}

}; // namespace Emscripten
}; // namespace Internal
}; // namespace Dali
