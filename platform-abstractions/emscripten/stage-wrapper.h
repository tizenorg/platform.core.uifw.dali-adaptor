#ifndef __DALI_STAGE_WRAPPER_H__
#define __DALI_STAGE_WRAPPER_H__

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
#include "layer-wrapper.h"
#include "render-task-list-wrapper.h"

namespace Dali
{
namespace Internal
{
namespace Emscripten
{

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

  Dali::Vector2 GetSize() const;

private:

};

}; // namespace Emscripten
}; // namespace Internal
}; // namespace Dali

#endif // header
