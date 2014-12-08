/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
