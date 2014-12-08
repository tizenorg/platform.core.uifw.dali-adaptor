#ifndef __DALI_STAGE_WRAPPER_H__
#define __DALI_STAGE_WRAPPER_H__

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
