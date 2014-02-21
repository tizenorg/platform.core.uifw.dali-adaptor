#ifndef __DALI_INTERNAL_ADAPTOR_PERFORMANCE_SERVER_H__
#define __DALI_INTERNAL_ADAPTOR_PERFORMANCE_SERVER_H__

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
#include "performance-interface.h"
#include "frame-time-stats.h"
#include <dali/public-api/common/dali-vector.h>
#include <dali/integration-api/platform-abstraction.h>
#include <dali/integration-api/debug.h>

// EXTERNAL INCLUDES
#include <boost/thread/mutex.hpp>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

/**
 * Concrete implementation of performance interface.
 * Adaptor classes should never include this file, they
 * just need to include the abstract class performance-interface.h
 */
class PerformanceServer : public PerformanceInterface
{

public:

  /**
   * Constructor
   * @param platformAbstraction platform abstraction
   * @param logFunction log function
   * @param logOpts log options
   */
  PerformanceServer( Integration::PlatformAbstraction& platformAbstraction,
                     Integration::Log::LogFunction& logFunction,
                     int logOpts );

  /**
   * Destructor
   */
  virtual ~PerformanceServer();

  /**
   * @copydoc PerformanceInterface::AddMarker()
   */
  virtual void AddMarker( PerformanceMarker::MarkerType markerType);

  /**
   * @copydoc PerformanceInterface::SetLogging()
   */
  virtual void SetLogging( unsigned int level, unsigned int interval);


private:

  /**
   * Helper
   */
  void AddMarkerToLog( PerformanceMarker marker );

  /**
   * Helper
   */
  void LogMarker(const char* name, const FrameTimeStats& frameStats);

  /**
   * log markers out
   */
  void LogMarkers();

private:

  bool mLoggingEnabled:1;         ///< whether logging update / render to a log is enabled
  bool mLogFunctionInstalled:1;   ///< whether the log function is installed
  unsigned int mLogLevel;         ///< log level
  unsigned int mlogOpts;          ///< log options
  unsigned int mLogFrequencyMicroseconds; ///< if logging is enabled, what frequency to log out at in micro-seconds

  FrameTimeStats mUpdateStats;    ///< update time statistics
  FrameTimeStats mRenderStats;    ///< render time statistics

  Integration::Log::LogFunction mLogFunction;             ///< log function
  Integration::PlatformAbstraction& mPlatformAbstraction; ///< platform abstraction

  typedef Dali::Vector<PerformanceMarker > MarkerVector;
  MarkerVector mMarkers;              ///< vector of markers
  boost::mutex mDataMutex;            ///< mutex

};


} // namespace Internal

} // namespace Adaptor

} // namespace Dali

#endif
