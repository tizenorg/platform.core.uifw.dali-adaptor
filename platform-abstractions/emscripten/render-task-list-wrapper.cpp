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

#include "render-task-list-wrapper.h"

// EXTERNAL INCLUDES

// INTERNAL INCLUDES

namespace Dali
{
namespace Internal
{
namespace Emscripten
{

//
// RenderTaskList
//
RenderTaskListWrapper::RenderTaskListWrapper(Dali::RenderTaskList renderTaskList)
  : mRenderTaskList(renderTaskList)
{

}

RenderTaskListWrapper::~RenderTaskListWrapper()
{
}

RenderTaskWrapper RenderTaskListWrapper::CreateTask()
{
  return RenderTaskWrapper( mRenderTaskList.CreateTask() );
}

void RenderTaskListWrapper::RemoveTask( RenderTaskWrapper task )
{
  mRenderTaskList.RemoveTask( task.Get() );
}

unsigned int RenderTaskListWrapper::GetTaskCount() const
{
  return mRenderTaskList.GetTaskCount();
}

RenderTaskWrapper RenderTaskListWrapper::GetTask( unsigned int index ) const
{
  return RenderTaskWrapper( mRenderTaskList.GetTask(index) );
}


}; // namespace Emscripten
}; // namespace Internal
}; // namespace Dali
