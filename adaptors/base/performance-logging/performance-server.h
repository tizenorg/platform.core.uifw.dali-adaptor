#ifndef __DALI_INTERNAL_ADAPTOR_PERFORMANCE_SERVER_H__
#define __DALI_INTERNAL_ADAPTOR_PERFORMANCE_SERVER_H__

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

// INTERNAL INCLUDES
#include <base/performance-logging/frame-time-stats.h>
#include <dali/public-api/common/dali-vector.h>
#include <base/interfaces/adaptor-internal-services.h>
#include <base/performance-logging/performance-marker.h>

// EXTERNAL INCLUDES
#include <boost/thread/mutex.hpp>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

class EnvironmentOptions;

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
   * @param adaptorServices adaptor internal services
   * @param environmentOptions environment options
   */
  PerformanceServer( AdaptorInternalServices& adaptorServices,
                     const EnvironmentOptions& environmentOptions );

  /**
   * Destructor
   */
  virtual ~PerformanceServer();

  /**
   * @copydoc PerformanceLogger::AddContext()
   */
  virtual ContextId AddContext( const char* name );

  /**
   * @copydoc PerformanceLogger::RemoveContext()
   */
  virtual void RemoveContext( ContextId contextId );

  /**
   * @copydoc PerformanceInterface::AddMarker()
   */
  virtual void AddMarker( MarkerType markerType );

  /**
   * @copydoc PerformanceLogger::AddMarker()
   */
  virtual void AddMarker( MarkerType markerType, ContextId contextId );

  /**
   * @copydoc PerformanceInterface::SetLogging()
   */
  virtual void SetLogging( unsigned int level, unsigned int logFrequency );

  /**
   * @copydoc PerformanceLogger::SetLoggingFrequency()
   */
  virtual void SetLoggingFrequency( unsigned int logFrequency, ContextId contextId );

  /**
   * @copydoc PerformanceLogger::EnableLogging()
   */
  virtual void EnableLogging( bool enable, ContextId contextId );

private:

  /**
   * Struct to hold a stat context
   */
  struct StatContext
  {
    typedef Dali::Vector< PerformanceMarker > MarkerVector;

    /**
     * Default constructor
     */
    StatContext();

    /**
     * Constructor
     *
     * @param[in] contextName Name of the context to print in console
     */
    StatContext( const char* contextName );

    const char* name;           ///< Name of the context
    FrameTimeStats stats;       ///< Frame time stats to accumulate

    MarkerVector markers;      ///< vector of markers
    boost::mutex dataMutex;    ///< mutex

    unsigned int logFrequencyMicroseconds; ///< if logging is enabled, what frequency to log out at in micro-seconds

    bool log;                  ///< Whether to print the log for this context or not
  };

  /**
   * Helper
   */
  void AddMarkerToLog( const PerformanceMarker& marker, ContextId contextId );

  /**
   * Helper
   */
  void LogMarker(const char* name, const FrameTimeStats& frameStats);

  /**
   * log markers out
   */
  void LogMarkers( StatContext* context );

  /**
   * Retrieve a stat context by ID
   *
   * @param[in] contextId The ID of the context to retrieve
   * @return Return the stat context if it exists or null
   */
  StatContext* GetContext( ContextId contextId );

  /**
   * Return a string representation of the marker type
   *
   * @param[in] type The marker type
   * @return Return the name of the given marker
   */
  const char* GetMarkerName( MarkerType type ) const;

private:

  struct StatContextIdPair
  {
    /**
     * Default constructor
     */
    StatContextIdPair() {}

    /**
     * Constructor
     */
    StatContextIdPair( StatContext* statContext, unsigned int contextId ) : context( statContext ), id( contextId ) {}

    StatContext* context;   ///< A raw pointer to a StateContext
    unsigned int id;        ///< The ID of the context
  };

  typedef Dali::Vector< StatContextIdPair > StatContexts;    // Using a map as contexts may be added/deleted at any time

  // The list of stat contexts
  StatContexts mStatContexts;

  Integration::PlatformAbstraction& mPlatformAbstraction; ///< platform abstraction
  const EnvironmentOptions& mEnvironmentOptions;          ///< environment options
  KernelTraceInterface& mKernelTrace;                     ///< kernel trace interface

  unsigned int mLogLevel;         ///< log level
  unsigned int mNextContextId;    ///< The next valid context ID

  // Some defaults
  ContextId mUpdateStats;    ///< update time statistics
  ContextId mRenderStats;    ///< render time statistics
  ContextId mEventStats;     ///< event time statistics

  bool mLoggingEnabled:1;         ///< whether logging update / render to a log is enabled
  bool mLogFunctionInstalled:1;   ///< whether the log function is installed
};


} // namespace Internal

} // namespace Adaptor

} // namespace Dali

#endif
