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
#include "resource-thread-base.h"

// INTERNAL HEADERS
#include <platform-abstractions/android/android-logging.h>

// EXTERNAL HEADERS
#include <dali/integration-api/debug.h>
#include <sys/prctl.h>
#include <memory>

using namespace std;
using namespace Dali::Integration;

namespace Dali
{

namespace SlpPlatform
{

ResourceThreadBase::ResourceThreadBase( ResourceLoader& resourceLoader )
: mResourceLoader( resourceLoader ),
  mThread( 0 ),
  mPaused( false )
{
#if defined(DEBUG_ENABLED)
  mLogFilter = Debug::Filter::New(Debug::Concise, false, "LOG_RESOURCE_THREAD_BASE");
#endif

  int error = pthread_create( &mThread, NULL, InternalThreadEntryFunc, this );
  DALI_ASSERT_ALWAYS( !error && "Error in pthread_create()" );
}

ResourceThreadBase::~ResourceThreadBase()
{
  TerminateThread();

#if defined(DEBUG_ENABLED)
  delete mLogFilter;
#endif
}

void ResourceThreadBase::TerminateThread()
{
  if (mThread)
  {
    // wake thread
    mCondition.Notify();

    // wait for thread to exit
    pthread_join( mThread, NULL );

    mThread = 0;
  }
}

void ResourceThreadBase::AddRequest(const ResourceRequest& request, const RequestType type)
{
  bool wasEmpty = false;
  bool wasPaused = false;

  {
    // Lock while adding to the request queue
    ConditionalWait::ScopedLock lock( mCondition );

    wasEmpty = mQueue.empty();
    wasPaused = mPaused;

    mQueue.push_back( std::make_pair(request, type) );
  }

  if( wasEmpty && !wasPaused )
  {
    // Wake-up the thread
    mCondition.Notify();
  }
}

// Called from outer thread.
void ResourceThreadBase::CancelRequest( const Integration::ResourceId resourceId )
{
  DALI_LOG_INFO( mLogFilter, Debug::Verbose, "%s: %u.\n", __FUNCTION__, unsigned(resourceId) );

  // Eliminate the cancelled request from the request queue if it is in there:
  {
    // Lock while searching and removing from the request queue:
    ConditionalWait::ScopedLock lock( mCondition );

    for( RequestQueueIter iterator = mQueue.begin();
         iterator != mQueue.end();
         ++iterator )
    {
      if( ((*iterator).first).GetId() == resourceId )
      {
        iterator = mQueue.erase( iterator );
        break;
      }
    }
  }
}

// Called from worker thread.
void ResourceThreadBase::InterruptionPoint() const
{
}

void* ResourceThreadBase::InternalThreadEntryFunc( void* This )
{
  ( static_cast<ResourceThreadBase*>( This ) )->ThreadLoop();
  return NULL;
}

void ResourceThreadBase::Pause()
{
  ConditionalWait::ScopedLock lock( mCondition );
  mPaused = true;
}

void ResourceThreadBase::Resume()
{
  // Clear the paused flag and if we weren't running already, also wake up the background thread:
  bool wasPaused = false;
  {
    ConditionalWait::ScopedLock lock( mCondition );
    wasPaused = mPaused;
    mPaused = false;
  }

  // If we were paused, wake up the background thread and give it a
  // chance to do some work:
  if( wasPaused )
  {
    mCondition.Notify();
  }
}

//----------------- Called from separate thread (mThread) -----------------

void ResourceThreadBase::ThreadLoop()
{
  prctl(PR_SET_NAME, "ResLoader");
  InstallLogging();

  while( !mResourceLoader.IsTerminating() )
  {
    WaitForRequests();

    if ( !mResourceLoader.IsTerminating() )
    {
      ProcessNextRequest();
    }
  }
}

void ResourceThreadBase::WaitForRequests()
{
  ConditionalWait::ScopedLock lock( mCondition );

  if( mQueue.empty() || mPaused == true )
  {
    // Waiting for a wake up from resource loader control thread
    // This will be to process a new request or terminate
    mCondition.Wait( lock );
  }
}

void ResourceThreadBase::ProcessNextRequest()
{
  ResourceRequest* request(NULL);
  RequestType type(RequestLoad);

  {
    // lock the queue and extract the next request
    ConditionalWait::ScopedLock lock( mCondition );

    if (!mQueue.empty())
    {
      const RequestInfo & front = mQueue.front();
      request = new ResourceRequest( front.first );
      type = front.second;
      mQueue.pop_front();
    }
  } // unlock the queue

  // process request outside of lock
  if ( NULL != request )
  {
    std::auto_ptr<ResourceRequest> deleter( request );
    switch( type )
    {
      case RequestLoad:
      {
        Load(*request);
      }
      break;

      case RequestDecode:
      {
        Decode(*request);
      }
      break;

      break;
    }
  }
}

void ResourceThreadBase::InstallLogging()
{
  // resource loading thread will send its logs to SLP Platform's LogMessage handler.
  Dali::Integration::Log::InstallLogFunction(Dali::SlpPlatform::LogMessage);
}

void ResourceThreadBase::UninstallLogging()
{
  // uninstall it on resource loading thread.
  Dali::Integration::Log::UninstallLogFunction();
}

void ResourceThreadBase::Decode(const Integration::ResourceRequest& request)
{
  DALI_LOG_TRACE_METHOD(mLogFilter);
  DALI_LOG_WARNING("Resource Decoding from a memory buffer not supported for this type.");
  ///! If you need this for a subclassed thread, look to ResourceThreadImage::Decode() for an example implementation.
}

} // namespace SlpPlatform

} // namespace Dali
