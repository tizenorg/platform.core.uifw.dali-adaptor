
#ifndef __DALI_INTERNAL_ADAPTOR_RESOURCE_TRACKING_MANAGER_H__
#define __DALI_INTERNAL_ADAPTOR_RESOURCE_TRACKING_MANAGER_H__

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

// INTERNAL INCLUDES
#include "resource-tracking.h"
#include "resource-tracking-message.h"

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <boost/thread/mutex.hpp>
#include <boost/thread/tss.hpp>
#include <map>
#include <vector>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

class ResourceTracking;

/**
 * Resource Tracking Manager
 *
 * Consolidates and formats the data from the individual trackers
 */
class ResourceTrackingManager
{
public:

  /**
   * Iterators to 'friendly' thread numbers and tracking objects' pointers
  */
  typedef std::vector< unsigned int >::iterator ResourceThreadIter;
  typedef std::vector< ResourceTracking* >::iterator ResourceTrackerIter;
  typedef std::vector< ResourceTracking* >::const_iterator ResourceTrackerConstIter;

  struct CPUResourceSlot
  {
    unsigned int id;
    bool CPUStatus;
    bool GPUStatus;
  };

  struct GPUResourceSlot
  {
    const void* ptr;
    std::size_t size;
  };

  typedef std::map< unsigned int, GPUResourceSlot > GPUResourcesMap;
  typedef GPUResourcesMap::iterator GPUResourcesIter;
  typedef GPUResourcesMap::const_iterator GPUResourcesConstIter;

  typedef std::map< const void*, CPUResourceSlot > CPUResourcesMap;
  typedef CPUResourcesMap::iterator CPUResourcesIter;
  typedef CPUResourcesMap::const_iterator CPUResourcesConstIter;

  struct ResourceThread
  {
    ResourceThread( ResourceTracking* rt)
    : resourceTracker( rt )
    {
    }
    ResourceTracking* resourceTracker;
  };

  /**
   * Default constructor
   */
  ResourceTrackingManager();

  /**
   * Destructor
   */
   ~ResourceTrackingManager();

  /**
   * @brief Convenience constructor to register logging function at instantiation
   *
   * @param resourceLoggingFunction Logging function to do export to DaliLog
   */
  ResourceTrackingManager( void( *resourceLoggingFunction )() );

  /**
   * @brief Register logging function
   *
   * @param resourceLoggingFunction Logging function to do export to DaliLog
   */
  void InstallLogFunction( void( *resourceLoggingFunction )() )
  {
    mLoggingFunction = resourceLoggingFunction;
    mLogFunctionInstalled = true;
  }

  /**
   * @brief Public entry for logging function ( if installed )
   */
  void LogFunction()
  {
    if ( mLogFunctionInstalled )
    {
      mLoggingFunction();
    }
  }

  /**
   * @brief Get start of the list of trackers
   *
   * @return Iterator to start of list
   */
  ResourceTrackerConstIter GetStartTrackerList()
  {
    return mTrackerList.begin();
  }

  /**
   * @brief Get end of the list of trackers
   *
   * @return Iterator to end of list
   */

  ResourceTrackerConstIter GetEndTrackerList()
  {
    return mTrackerList.end();
  }

  /**
   * @brief Add to CPU used counter
   *
   * @param value to add
   */
  void AddToCPUTracked( unsigned int value )
  {
    mCPUTracked += value;
    if ( mCPUTracked > mCPUPeak )
    {
      mCPUPeak = mCPUTracked;
    }
  }

  /**
   * @brief Add to GPU used counter
   *
   * @param value to add
   */
  void AddToGPUTracked( unsigned int value )
  {
    mGPUTracked += value;
    if ( mGPUTracked > mGPUPeak )
    {
      mGPUPeak = mGPUTracked;
    }
  }

  /**
   * @brief Take from CPU used counter
   *
   * @param value to subtract
   */
  void TakeFromCPUTracked( unsigned int value )
  {
    mCPUTracked -= value;
  }

  /**
   * @brief Take from GPU used counter
   *
   * @param value to subtract
   */
  void TakeFromGPUTracked( const std::size_t value )
  {
    mGPUTracked -= value;
  }

  /**
   * @brief Get CPU memory counter
   *
   * @return number of bytes being tracked
   */
  const unsigned int GetCPUTracked() const
  {
    return mCPUTracked;
  }

  /**
   * @brief Get GPU memory counter
   *
   * @return number of bytes being tracked
   */
  const unsigned int GetGPUTracked() const
  {
    return mGPUTracked;
  }

  /**
   * @brief Get CPU memory use peak
   *
   * @return peak
   */
  const unsigned int GetCPUPeak() const
  {
    return mCPUPeak;
  }

  /**
   * @brief Get GPU memory use peak
   *
   * @return peak
   */
  const unsigned int GetGPUPeak() const
  {
    return mGPUPeak;
  }


  void InsertCPUResource( const void* ptr, const unsigned int id );
  void InsertGPUResource( const unsigned int handle, const void* ptr, const std::size_t size );

  GPUResourcesIter GetGPUResource( const unsigned int hash )
  {
    return mGPUResources.find( hash );
  }

  CPUResourcesIter GetCPUResource( const void* ptr )
  {
    return mCPUResources.find( ptr );
  }

  GPUResourcesIter GetGPUResourcesEnd()
  {
    return mGPUResources.end();
  }

  CPUResourcesIter GetCPUResourcesEnd()
  {
    return mCPUResources.end();
  }

  void SetResourceCPUStatus( CPUResourcesIter CIter, bool status )
  {
    CIter->second.CPUStatus = status;
  }

  void SetResourceGPUStatus( CPUResourcesIter CIter, bool status )
  {
    CIter->second.GPUStatus = status;
  }

  void EraseCPUResource( CPUResourcesIter CIter )
  {
    // Check to see if both CPU and GPU references are clear
    if ( !(CIter->second.CPUStatus || CIter->second.GPUStatus) )
    {
      // TODO transfer hash to CPUResources Structure and erase string hash entry
      mCPUResources.erase( CIter );
    }
  }

  void EraseGPUResource( GPUResourcesIter GIter )
  {
    mGPUResources.erase( GIter );
  }

private:

  /**
   * @brief Register a tracker with the tracking manager
   *
   * @param resourceTracking Pointer to a tracking object
   *
   * @return 'Friendly' thread number
   */
  unsigned int AddThread( ResourceTracking* resourceTracking );

  /**
   * @brief Get 'Friendly' thread number for the current thread
   * @return 'Friendly' thread number
   */
  const unsigned int GetCurrentThread();

  std::vector< unsigned int > mThreadList;               // 'Friendly' thread numbers
  std::vector< ResourceTracking* > mTrackerList;         // Pointers to tracking objects
  boost::mutex mMutex;                                   // Mutex used for tracking object registration
  unsigned int mCPUTracked;                              // Total CPU memory being tracked
  unsigned int mGPUTracked;                              // Total GPU memory being tracked
  unsigned int mCPUPeak;                                 // Max CPU memory usage recorded
  unsigned int mGPUPeak;                                 // Max GPU memory usage recorded
  GPUResourcesMap mGPUResources;                         // Resources Map for GPU events
  CPUResourcesMap mCPUResources;                         // Resources Map for CPU events
  void (*mLoggingFunction)();                            // Logging function
  bool mLogFunctionInstalled;                            // Is Logging function installed

  // Resource Tracking objects need to access AddThread method
  friend class ResourceTracking;
};

extern boost::thread_specific_ptr< Dali::Internal::Adaptor::ResourceTrackingManager::ResourceThread > ResourceThreadLocal;
extern ResourceTrackingManager* g_ResourceTrackingManager;

} // namespace Adaptor

} // namespace Internal

} // namespace Dali


#endif // __DALI_INTERNAL_ADAPTOR_RESOURCE_TRACKING_MANAGER_H__