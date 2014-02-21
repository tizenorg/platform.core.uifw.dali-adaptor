#ifndef __DALI_SLP_PLATFORM_RESOURCE_THREAD_BASE_H__
#define __DALI_SLP_PLATFORM_RESOURCE_THREAD_BASE_H__

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

#include "resource-loader.h"
#include <deque>
#include <boost/thread.hpp>

#include <dali/integration-api/platform-abstraction.h>
#include <dali/integration-api/resource-cache.h>

namespace Dali
{

namespace SlpPlatform
{

/**
 * Resource loader worker thread
 */
class ResourceThreadBase
{
public:
  // typedefs and enums
  enum RequestType
  {
    RequestLoad,
    RequestSave
  };

  typedef std::pair<Integration::ResourceRequest, RequestType>  RequestInfo;
  typedef std::deque<RequestInfo>                               RequestQueue;
  typedef RequestQueue::iterator                                RequestQueueIter;

public:
  // C'tors and D'tors
  ResourceThreadBase(ResourceLoader& resourceLoader);
  virtual ~ResourceThreadBase();

protected:
  void TerminateThread();

public:
  /**
   * Add a resource request to the back of the queue
   * @param[in] request The requested resource/file url and attributes
   * @param[in] type    Load or save flag
   */
  void AddRequest(const Integration::ResourceRequest& request, const RequestType type);

  /**
   * Cancel a resource request. Removes the request from the queue.
   * @param[in] resourceId ID of the resource to be canceled
   */
  void CancelRequest(Integration::ResourceId  resourceId);

  /**
   * Pause starting new work in the background, but keep that work queued.
   */
  void Pause();

  /**
   * Resume processing tasks on background thread.
   */
  void Resume();


protected:
  /**
   * Main control loop for the thread.
   * The thread is terminated when this function exits
   */
  void ThreadLoop();

  /**
   * Wait for an incoming resource request or termination
   */
  void WaitForRequests();

  /**
   * Process the resource request at the head of the queue
   */
  void ProcessNextRequest();

  /**
   * Install a logging function in to core for this thread.
   */
  void InstallLogging();

  /**
   * Uninstall a logging function.
   */
  void UninstallLogging();

  /**
   * Load a resource
   * @param[in] request  The requested resource/file url and attributes
   */
  virtual void Load(const Integration::ResourceRequest& request) = 0;

  /**
   * Save a resource
   * @param[in] request  The requested resource/file url and attributes
   */
  virtual void Save(const Integration::ResourceRequest& request) = 0;

protected:
  ResourceLoader& mResourceLoader;
  boost::thread* mThread;                       ///< thread instance
  boost::condition_variable mCondition;         ///< condition variable
  boost::mutex mMutex;                          ///< used to protect mQueue
  RequestQueue mQueue;                          ///< Request queue
  bool mPaused;                                 ///< Whether to process work in mQueue

#if defined(DEBUG_ENABLED)
public:
  Integration::Log::Filter* mLogFilter;
#endif
}; // class ResourceThreadBase

} // namespace SlpPlatform

} // namespace Dali

#endif // __DALI_SLPPLATFORM_RESOURCE_THREAD_BASE_H__
