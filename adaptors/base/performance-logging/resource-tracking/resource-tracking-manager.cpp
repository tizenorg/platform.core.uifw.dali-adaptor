//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// CLASS HEADER
#include "resource-tracking-manager.h"

// EXTERNAL HEADERS
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

// Globals
boost::thread_specific_ptr< Dali::Internal::Adaptor::ResourceTrackingManager::ResourceThread > ResourceThreadLocal;
ResourceTrackingManager* gResourceTrackingManager = NULL;

ResourceTrackingManager::ResourceTrackingManager()
: mCPUTracked( 0 ),
  mGPUTracked( 0 ),
  mCPUPeak( 0 ),
  mGPUPeak( 0 ),
  mLogFunctionInstalled( false )
{
  gResourceTrackingManager = this;
}

ResourceTrackingManager::ResourceTrackingManager( void( *resourceLoggingFunction )() )
: mCPUTracked( 0 ),
  mGPUTracked( 0 ),
  mCPUPeak( 0 ),
  mGPUPeak( 0 ),
  mLogFunctionInstalled( false )
{
  InstallLogFunction( resourceLoggingFunction );
  gResourceTrackingManager = this;
}

ResourceTrackingManager::~ResourceTrackingManager()
{
  // Interate through the tracking object list and delete those objects
  for ( ResourceTrackerIter rIt = mTrackerList.begin(); rIt != mTrackerList.end(); ++rIt )
  {
    delete *rIt;
  }
  ResourceThreadLocal.reset();
  gResourceTrackingManager = NULL;
}

unsigned int ResourceTrackingManager::AddThread( ResourceTracking* resourceTracking )
{
  // Add a mutex lock just while the tracking objects are registering themselves
  boost::mutex::scoped_lock lock( mMutex );

  unsigned int thread = GetCurrentThread();

  // Check to see if there's an entry for this thread and exit with existing threadID if so
  unsigned int threadIndex = 0;

  for ( ResourceThreadIter rIt = mThreadList.begin(); rIt != mThreadList.end(); ++rIt, threadIndex++ )
  {
    if ( *rIt == thread )
    {
      return threadIndex;
    }
  }

  // Create a new entry for this thread's tracker
  mThreadList.push_back( thread );
  mTrackerList.push_back( resourceTracking );

  // Create thread local storage for this tracker
  ResourceThread* resourceThread = new ResourceThread( resourceTracking );
  ResourceThreadLocal.reset( resourceThread );
  return threadIndex;
}

const unsigned int ResourceTrackingManager::GetCurrentThread()
{
   // Resolve the boost thread number into a usable index
  std::string threadId = boost::lexical_cast<std::string>( boost::this_thread::get_id() );
  unsigned long thread = 0;
  sscanf( threadId.c_str(), "%lx", &thread);
  return static_cast< unsigned int >( thread );
}

void ResourceTrackingManager::InsertCPUResource( const void* ptr, const unsigned int id )
{
  CPUResourceSlot crs;
  crs.id = id;
  crs.CPUStatus = true;  // start resource loaded
  crs.GPUStatus = false;
  mCPUResources.insert( std::make_pair( ptr, crs ) );
}

void ResourceTrackingManager::InsertGPUResource( const unsigned int handle, const void* ptr, const std::size_t size )
{
  GPUResourceSlot grs;
  grs.size = size;
  grs.ptr = ptr;
  mGPUResources.insert( std::make_pair( handle, grs ) );
}


} // namespace Adaptor

} // namespace Internal

} // namespace Dali
