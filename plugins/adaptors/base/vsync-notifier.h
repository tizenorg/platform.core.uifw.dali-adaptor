#ifndef __DALI_INTERNAL_VSYNC_NOTIFIER_H__
#define __DALI_INTERNAL_VSYNC_NOTIFIER_H__

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

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <base/interfaces/adaptor-internal-services.h>
 
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

  /**
   * Fallback main thread loop, uses sleep to simulate vsyncs at the correct frame rate.
   * The system thread will be destroyed on exit from this function.
   */
  void RunTimed();

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
