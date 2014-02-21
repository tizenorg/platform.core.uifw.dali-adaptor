#ifndef __DALI_INTERNAL_UPDATE_THREAD_H__
#define __DALI_INTERNAL_UPDATE_THREAD_H__

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

// EXTERNAL INCLUDES
#include <dali/integration-api/core.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/common/vector-wrapper.h>

namespace boost
{

class thread;

} // namespace boost

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

class TriggerEvent;
class UpdateRenderSynchronization;

/**
 * The update-thread is responsible for calling Core::Update(), and
 * for triggering the render-thread after each update.
 */
class UpdateThread
{
public:

  /**
   * Create the update-thread; this will not do anything until Start() is called.
   * @param[in] sync An object used to synchronize update & render threads.
   * @param[in] core The Dali core.
   * @param[in] logFunction The thread-specific logging function.
   * @param[in] logOpts options for logging function
   * @param[in] fpsTrackingSeconds The frames-per-second will be tracked for this duration.
   * @param[in] statusLogInterval The number of frames between status debug prints.
   * @param[in] notificationTrigger Used to send a notification event to core.
   */
  UpdateThread(UpdateRenderSynchronization& sync,
                Dali::Integration::Core& core,
                Dali::Integration::Log::LogFunction& logFunction,
                int logOpts,
                unsigned int fpsTrackingSeconds,
                unsigned int statusLogInterval,
                TriggerEvent& notificationTrigger );

  /**
   * Non-virtual destructor; UpdateThread is not suitable as a base class.
   */
  ~UpdateThread();

  /**
   * Starts the update-thread
   */
  void Start();

  /**
   * Stops the update-thread
   */
  void Stop();

private:

  /**
   * This method is used by the update-thread for calling Core::Update().
   * @return true, if the thread finishes properly.
   */
  bool Run();

  /**
   * When DALI_FPS_TRACKING is enabled, this method calculates the frame rates for the specified time period
   */
  void FPSTracking(float secondsFromLastFrame);

  /**
   * Output the FPS information
   * when the FSP tracking is enabled,
   * it is called when the specified tracking period is elapsed or in the destructor when the process finished beforehand
   */
  void OutputFPSRecord();

  /**
   * Optionally output the update thread status.
   * @param[in] keepUpdatingStatus Whether the update-thread requested further updates.
   * @param[in] renderNeedsUpdate Whether the render-thread requested another update.
   */
  void UpdateStatusLogging( unsigned int keepUpdatingStatus, bool renderNeedsUpdate );

private: // Data

  UpdateRenderSynchronization&        mUpdateRenderSync;    ///< Used to synchronize the update & render threads

  Dali::Integration::Core&            mCore;                ///< Dali core reference
  Dali::Integration::Log::LogFunction mLogFunction;         ///< log function
  unsigned int                        mLogOpts;             ///< logging options

  unsigned int                        mFpsTrackingSeconds;  ///< fps tracking time length in seconds
  std::vector<float>                  mFpsRecord;           ///< Record of frame rate
  float                               mElapsedTime;         ///< time elapsed within current second
  unsigned int                        mElapsedSeconds;      ///< seconds elapsed since the fps tracking started

  unsigned int                        mStatusLogInterval;   ///< Interval in frames between status debug prints
  unsigned int                        mStatusLogCount;      ///< Used to count frames between status debug prints

  TriggerEvent&                       mNotificationTrigger; ///< Reference to notification event trigger

  boost::thread*                      mThread;              ///< The actual update-thread.

}; // class UpdateThread

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_UPDATE_THREAD_H__
