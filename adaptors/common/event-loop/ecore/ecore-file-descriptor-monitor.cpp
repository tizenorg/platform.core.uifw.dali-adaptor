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

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>

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
  Impl( int fileDescriptor, CallbackBase* callback, CallbackBase* preCallback, CallbackBase* awakeCallback, int eventsToMonitor )
  : mFileDescriptor( fileDescriptor ),
    mEventsToMonitor( eventsToMonitor ),
    mCallback( callback ),
    mPreCallback( preCallback ),
    mAwakeCallback( awakeCallback ),
    mHandler( NULL )
  {
  }

  ~Impl()
  {
    delete mCallback;
  }

  // Data
  int mFileDescriptor;
  int mEventsToMonitor;              ///< what file descriptor events to monitor
  CallbackBase* mCallback;
  CallbackBase* mPreCallback;
  CallbackBase* mAwakeCallback;
  Ecore_Fd_Handler* mHandler;

  // Static Methods

  /**
   * Called when the file descriptor receives an event.
   */
  static Eina_Bool EventDispatch(void* data, Ecore_Fd_Handler *handler)
  {
    Impl* impl = reinterpret_cast<Impl*>(data);

    // if we want read events, check to see if a read event is available
    int type = FileDescriptorMonitor::FD_NO_EVENT;

    if( ecore_main_fd_handler_active_get( handler, ECORE_FD_ERROR) )
    {
      CallbackBase::Execute( *impl->mCallback, FileDescriptorMonitor::FD_ERROR);
      DALI_LOG_ERROR("ECORE_FD_ERROR occurred on %d", impl->mFileDescriptor);

      return ECORE_CALLBACK_CANCEL;
    }

    if( impl->mEventsToMonitor & ECORE_FD_READ )
    {
      if (ecore_main_fd_handler_active_get( handler, ECORE_FD_READ))
      {
        type = FileDescriptorMonitor::FD_READABLE;
      }
    }
    // check if we want write events
    if( impl->mEventsToMonitor & ECORE_FD_WRITE )
    {
      if (ecore_main_fd_handler_active_get( handler, ECORE_FD_WRITE))
      {
        type |= FileDescriptorMonitor::FD_WRITABLE;
      }
    }

    // if there is an event, execute the callback
    if( type != FileDescriptorMonitor::FD_NO_EVENT )
    {
      CallbackBase::Execute( *impl->mCallback, static_cast< FileDescriptorMonitor::EventType >(type ) );
    }

    return ECORE_CALLBACK_RENEW;
  }

  /**
    * Called before entering the main loop select function.
    */
  static void PrepareEvent( void* data, Ecore_Fd_Handler* handler )
  {
    Impl* impl = reinterpret_cast<Impl*>(data);

    if( impl->mPreCallback )
    {
      CallbackBase::Execute( *impl->mPreCallback );
    }
  }

  static void AwakeCallback( void*data )
  {
    Impl* impl = reinterpret_cast<Impl*>(data);

    if( impl->mAwakeCallback )
    {
      bool awakenByMyFd = false;

      int events = 0;
      if( impl->mEventsToMonitor & FileDescriptorMonitor::FD_READABLE)
      {
        events = ECORE_FD_READ;
      }
      if( impl->mEventsToMonitor & FileDescriptorMonitor::FD_WRITABLE)
      {
        events |= ECORE_FD_WRITE;
      }

      if( ecore_main_fd_handler_active_get( impl->mHandler, static_cast< Ecore_Fd_Handler_Flags >( events ) ) )
      {
        awakenByMyFd = true;
      }

      CallbackBase::Execute( *impl->mAwakeCallback, awakenByMyFd );
    }
  }
};

FileDescriptorMonitor::FileDescriptorMonitor( int fileDescriptor, CallbackBase* callback, CallbackBase* preCallback, CallbackBase* awakeCallback, int eventBitmask)
{
  mImpl = new Impl(fileDescriptor, callback, preCallback, awakeCallback, eventBitmask);

  if (fileDescriptor < 1)
  {
    DALI_ASSERT_ALWAYS( 0 && "Invalid File descriptor");
    return;
  }

  int events = 0;
  if( eventBitmask & FD_READABLE)
  {
    events = ECORE_FD_READ;
  }
  if( eventBitmask & FD_WRITABLE)
  {
    events |= ECORE_FD_WRITE;
  }
  mImpl->mEventsToMonitor = events;
  mImpl->mHandler = ecore_main_fd_handler_add( fileDescriptor, static_cast<Ecore_Fd_Handler_Flags >( events ), &Impl::EventDispatch, mImpl, NULL, NULL );

  ecore_main_fd_handler_prepare_callback_set( mImpl->mHandler, &Impl::PrepareEvent, mImpl );

#ifndef DALI_PROFILE_UBUNTU
  ecore_main_awake_handler_add( &Impl::AwakeCallback, mImpl );
#endif
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
