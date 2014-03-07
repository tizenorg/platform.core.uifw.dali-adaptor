
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
   * Structures to define CPU and GPU records
   */
  struct CPUResourceSlot
  {
    unsigned int id;      // resource ID
    bool CPUStatus;       // resource occupies CPU memory
    bool GPUStatus;       // resource occupies GPU memory
  };

  struct GPUResourceSlot
  {
    const void* ptr;      // CPU memory that resource refers to
    std::size_t size;     // size of the texture
  };

  /**
   * Iterators to 'friendly' thread numbers and tracking objects' pointers
  */
  typedef std::vector< unsigned int >::iterator ResourceThreadIter;
  typedef std::vector< ResourceTracking* >::iterator ResourceTrackerIter;
  typedef std::vector< ResourceTracking* >::const_iterator ResourceTrackerConstIter;

  typedef std::map< unsigned int, GPUResourceSlot > GPUResourcesMap;
  typedef GPUResourcesMap::iterator GPUResourcesIter;
  typedef GPUResourcesMap::const_iterator GPUResourcesConstIter;

  typedef std::map< const void*, CPUResourceSlot > CPUResourcesMap;
  typedef CPUResourcesMap::iterator CPUResourcesIter;
  typedef CPUResourcesMap::const_iterator CPUResourcesConstIter;

  /**
   * Local thread storage structure to hold resource tracking object
   */
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
  explicit ResourceTrackingManager( void( *resourceLoggingFunction )() );

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

  /**
   * @brief Insert an entry for a CPU resource allocation
   *
   * @param ptr pointer to the resource buffer being logged
   * @param int resourceID to associate with the buffer
   */
  void InsertCPUResource( const void* ptr, const unsigned int id );

  /**
   * @brief Insert an entry for GPU resource allocation
   *
   * @param handle GL Handle that has been assigned to texture
   * @param ptr pointer to the resource buffer that texture is created from
   * @param size size of the texture in bytes
   */
  void InsertGPUResource( const unsigned int handle, const void* ptr, const std::size_t size );

  /**
   * @brief Get an iterator to the GPU entry with this hash
   *
   * @param hash hash value to lookup
   * @return iterator
   */
  GPUResourcesIter GetGPUResource( const unsigned int hash )
  {
    return mGPUResources.find( hash );
  }

  /**
   * @brief Get an iterator to the CPU entry with this hash
   *
   * @param ptr key to use for hash
   * @return iterator
   */
  CPUResourcesIter GetCPUResource( const void* ptr )
  {
    return mCPUResources.find( ptr );
  }

  /**
   * @brief Get the end of the GPU resource records
   *
   * @return GPU resource records end
   */
  GPUResourcesIter GetGPUResourcesEnd()
  {
    return mGPUResources.end();
  }

  /**
   * @brief Get the end of the CPU resource records
   *
   * @return CPU resource records end
   */
  CPUResourcesIter GetCPUResourcesEnd()
  {
    return mCPUResources.end();
  }

  /**
   * @brief Set the state of CPU memory use for this resource
   *
   * @param CIter iterator pointing to resource entry
   * @param status true if resource is occupying CPU memory
   */
  void SetResourceCPUStatus( CPUResourcesIter CIter, bool status )
  {
    CIter->second.CPUStatus = status;
  }

  /**
   * @brief Set the state of GPU memory use for this resource
   *
   * @param CIter iterator pointing to resource entry
   * @param status true if resource is occupying GPU memory
   */
  void SetResourceGPUStatus( CPUResourcesIter CIter, bool status )
  {
    CIter->second.GPUStatus = status;
  }

  /**
   * @brief If resource is no longer being used then erase the record
   *
   * @param CIter iterator to the record
   */
  void EraseCPUResource( CPUResourcesIter CIter )
  {
    // Check to see if both CPU and GPU references are clear
    if ( !(CIter->second.CPUStatus || CIter->second.GPUStatus) )
    {
      // TODO transfer hash to CPUResources Structure and erase string hash entry
      mCPUResources.erase( CIter );
    }
  }

  /**
   * @brief Erase the GPU resource record
   *
   * @param GIter iterator to the GPU record
   */
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

// Externs for globals
extern boost::thread_specific_ptr< Dali::Internal::Adaptor::ResourceTrackingManager::ResourceThread > ResourceThreadLocal;
extern ResourceTrackingManager* gResourceTrackingManager;

} // namespace Adaptor

} // namespace Internal

} // namespace Dali


#endif // __DALI_INTERNAL_ADAPTOR_RESOURCE_TRACKING_MANAGER_H__