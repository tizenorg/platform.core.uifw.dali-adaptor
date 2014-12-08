#ifndef __DALI_RENDER_TASK_LIST_WRAPPER_H__
#define __DALI_RENDER_TASK_LIST_WRAPPER_H__

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
#include "handle-wrapper.h"
#include "render-task-wrapper.h"

namespace Dali
{
namespace Internal
{
namespace Emscripten
{

//
// RenderTaskListWrapper
//
class RenderTaskListWrapper : public HandleWrapper
{
public:
  explicit RenderTaskListWrapper(Dali::RenderTaskList renderTaskList);
  ~RenderTaskListWrapper();

  RenderTaskWrapper CreateTask();
  void RemoveTask( RenderTaskWrapper task );
  unsigned int GetTaskCount() const;
  RenderTaskWrapper GetTask( unsigned int index ) const;

protected:
  Dali::RenderTaskList mRenderTaskList;
};


}; // namespace Emscripten
}; // namespace Internal
}; // namespace Dali

#endif // header
