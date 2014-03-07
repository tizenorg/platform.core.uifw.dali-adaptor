#ifndef __DALI_INTERNAL_VSYNC_NOTIFIER_H__
#define __DALI_INTERNAL_VSYNC_NOTIFIER_H__

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

// EXTERNAL HEADERS
#include <dali/integration-api/resource-tracking.h>

namespace boost
{

class thread;

} // namespace boost

namespace Dali
{

namespace Integration
{

class Core;
class PlatformAbstraction;

} // namespace Integration

namespace Internal
{

namespace Adaptor
{

class VSyncMonitorInterface;
class UpdateRenderSynchronization;
class LogOptions;
class AdaptorInternalServices;

/**
 * Implements a simple class that monitors vertical blanks from libdrm
 * and sends a notification to Core.
 */
class VSyncNotifier
{
public:

  /**
   * Create the vsync notification thread; this will not start to monitor vsync and
   * send notifications until Start() is called.
   * @param[in] sync         An object used to synchronize update, render and vsync threads.
   * @param[in] adaptorInterfaces base adaptor interface
   * @param[in] logOption    Log options
   */
  VSyncNotifier( UpdateRenderSynchronization& sync,
                 AdaptorInternalServices& adaptorInterfaces,
                 const LogOptions& logOptions);

  /**
   * Non-virtual destructor; VSyncNotifier is not suitable as a base class.
   */
  ~VSyncNotifier();

  /**
   * Starts the thread
   */
  void Start();

  /**
   * Stops the thread
   */
  void Stop();

private:

  /**
   * The main thread loop. The system thread will be destroyed on
   * exit from this function.
   */
  void Run();

private:

  UpdateRenderSynchronization&        mUpdateRenderSync;    ///< Used to synchronize the update, render & vsync threads
  Dali::Integration::Core&            mCore;                ///< Dali core reference
  Integration::PlatformAbstraction&   mPlatformAbstraction; ///< The platform abstraction for retrieving the current time etc.
  VSyncMonitorInterface*              mVSyncMonitor;        ///< VSyncMonitor interface
  boost::thread*                      mThread;              ///< The actual thread.
  const LogOptions&                   mLogOptions;          ///< Log options

}; // class VSyncNotifier

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_VSYNC_NOTIFIER_H__
