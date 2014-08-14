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

// CLASS HEADER
#include "file-descriptor-monitor.h"

// EXTERNAL INCLUDES
#include <Ecore.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

/**
 * Using Impl to hide away EFL specific members
 */
struct FileDescriptorMonitor::Impl
{
  // Construction
  Impl(int fileDescriptor, Dali::Callback functor)
  : mFileDescriptor(fileDescriptor),
    mFunctor(functor),
    mHandler(NULL)
  {
  }

  // Data
  int mFileDescriptor;
  Dali::Callback mFunctor;
  Ecore_Fd_Handler* mHandler;

  // Static Methods

  /**
   * Called when the file descriptor receives an event.
   */
  static Eina_Bool EventDispatch(void* data, Ecore_Fd_Handler *handler)
  {
    Impl* impl = reinterpret_cast<Impl*>(data);

    Dali::Callback::Execute(impl->mFunctor);

    return ECORE_CALLBACK_RENEW;
  }
};

FileDescriptorMonitor::FileDescriptorMonitor(int fileDescriptor, Dali::Callback functor)
{
  mImpl = new Impl(fileDescriptor, functor);

  if (fileDescriptor >= 0)
  {
    mImpl->mHandler = ecore_main_fd_handler_add(fileDescriptor, ECORE_FD_READ, &Impl::EventDispatch, mImpl, NULL, NULL);
  }
}

FileDescriptorMonitor::~FileDescriptorMonitor()
{
  if (mImpl->mHandler)
  {
    ecore_main_fd_handler_del(mImpl->mHandler);
  }

  delete mImpl;
  mImpl = NULL;
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
