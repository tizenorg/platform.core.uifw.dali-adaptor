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

// CLASS HEADER
#include "file-descriptor-monitor.h"

// EXTERNAL INCLUDES
#include <android/looper.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

/**
 * Using Impl to hide away Android specific members
 */
struct FileDescriptorMonitor::Impl
{
  // Construction
  Impl( int fileDescriptor, CallbackBase* callback )
  : mFileDescriptor( fileDescriptor ),
    mCallback( callback )
  {
  }

  ~Impl()
  {
    delete mCallback;
  }

  // Data
  int mFileDescriptor;
  CallbackBase* mCallback;

  // Static Methods

  /**
   * Called when the file descriptor receives an event.
   */
  static int EventDispatch( int fd, int events, void* data )
  {
    Impl* impl = reinterpret_cast<Impl*>(data);

    CallbackBase::Execute( *impl->mCallback );

    return 1; // Continue receiving callbacks
  }
};

FileDescriptorMonitor::FileDescriptorMonitor( int fileDescriptor, CallbackBase* callback )
{
  mImpl = new Impl(fileDescriptor, callback);

  if (fileDescriptor >= 0)
  {
    ALooper* looper = ALooper_forThread();
    if( looper )
    {
      ALooper_addFd( looper, fileDescriptor, ALOOPER_POLL_CALLBACK, ALOOPER_EVENT_INPUT, &Impl::EventDispatch, mImpl );
    }
  }
}

FileDescriptorMonitor::~FileDescriptorMonitor()
{
  if( mImpl->mFileDescriptor )
  {
    ALooper* looper = ALooper_forThread();
    if( looper )
    {
      ALooper_removeFd( looper, mImpl->mFileDescriptor );
    }
  }

  delete mImpl;
  mImpl = NULL;
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
