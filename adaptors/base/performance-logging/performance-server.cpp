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

// CLASS HEADER
#include "performance-server.h"

// INTERNAL INCLUDES
#include <base/environment-options.h>
#include <dali/integration-api/platform-abstraction.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

PerformanceServer::PerformanceServer( AdaptorInternalServices& adaptorServices,
                                      const EnvironmentOptions& environmentOptions)
:mPlatformAbstraction( adaptorServices.GetPlatformAbstractionInterface() ),
 mEnvironmentOptions( environmentOptions ),
 mKernelTrace( adaptorServices.GetKernelTraceInterface() ),
 mStatisticsLogBitmask( 0 ),
 mLoggingEnabled( false ),
 mLogFunctionInstalled( false )
{
  SetLogging( mEnvironmentOptions.GetPerformanceStatsLoggingOptions(),
              mEnvironmentOptions.GetPerformanceTimeStampOutput(),
              mEnvironmentOptions.GetFrameRateLoggingFrequency());
}

PerformanceServer::~PerformanceServer()
{
  if( mLogFunctionInstalled )
  {
    mEnvironmentOptions.UnInstallLogFunction();
  }
}

void PerformanceServer::SetLogging( unsigned int statisticsLogOptions,
                                    unsigned int timeStampOutput,
                                    unsigned int logFrequency )
{
  if( ( statisticsLogOptions == 0) && ( timeStampOutput == 0 ))
  {
    mLoggingEnabled = false;
    return;
  }

  mStatisticsLogBitmask = statisticsLogOptions;
  mPerformanceOutputBitmask = timeStampOutput;


  mStatContextManager.SetLoggingLevel( mStatisticsLogBitmask, logFrequency);

  mLoggingEnabled = true;
}

void PerformanceServer::SetLoggingFrequency( unsigned int logFrequency, ContextId contextId )
{
  mStatContextManager.SetLoggingFrequency( logFrequency, contextId );
}

void PerformanceServer::EnableLogging( bool enable, ContextId contextId )
{
  mStatContextManager.EnableLogging( enable, contextId );
}

PerformanceInterface::ContextId PerformanceServer::AddContext( const char* name )
{
  // for adding custom contexts
  return mStatContextManager.AddContext( name, PerformanceMarker::CUSTOM_EVENTS );
}

void PerformanceServer::RemoveContext( ContextId contextId )
{
  mStatContextManager.RemoveContext( contextId );
}

void PerformanceServer::AddMarker( MarkerType markerType, ContextId contextId )
{
  // is only called from main event thread, but may overlap with internal AddMarker calls
  // from other threads ( update, render etc).
  boost::mutex::scoped_lock sharedDatalock( mDataMutex );

  // Add Marker can be called from any thread
  // Get the time
  unsigned int seconds = 0;
  unsigned int microseconds = 0;
  mPlatformAbstraction.GetTimeMicroseconds( seconds, microseconds );

  // Create a marker
  PerformanceMarker marker( markerType, FrameTimeStamp( 0, seconds, microseconds ) );

  mStatContextManager.AddCustomMarker( marker, contextId );

  // Make a string, that looks like  SIZE_NEGOTIATION_START
  std::string description;
  description.reserve(30);

  description.append( mStatContextManager.GetContextName( contextId ));
  description.append( "_");
  description.append( marker.GetName() );

  // log to kernel trace if enabled
  if( mPerformanceOutputBitmask & OUTPUT_KERNEL_TRACE )
  {
     mKernelTrace.Trace( description );
  }

  // log to the dali log
  if ( mPerformanceOutputBitmask & OUTPUT_DALI_LOG )
  {
    Integration::Log::LogMessage( Dali::Integration::Log::DebugInfo,
                                    "%d.%06d (seconds), %s\n",
                                    marker.GetTimeStamp().seconds,
                                    marker.GetTimeStamp().microseconds,
                                    description.c_str() );

  }

}
void PerformanceServer::AddMarker( MarkerType markerType )
{
  if( !mLoggingEnabled )
  {
    return;
  }
  // Only log on the v-sync thread, so we have less impact on update/render
  if( markerType == VSYNC )
  {
    // make sure log function is installed, note this will be called only from v-sync thread
    // if the v-sync thread has already installed one, it won't make any difference.
    if( ! mLogFunctionInstalled )
    {
      mEnvironmentOptions.InstallLogFunction();
      mLogFunctionInstalled = true;
    }
  }
  // Add Marker can be called from any thread
  // Get the time
  unsigned int seconds = 0;
  unsigned int microseconds = 0;
  mPlatformAbstraction.GetTimeMicroseconds( seconds, microseconds );

  // Create a marker
  PerformanceMarker marker( markerType, FrameTimeStamp( 0, seconds, microseconds ) );

  // process it
  ProcessMarker(marker);

}

void PerformanceServer::ProcessMarker( const PerformanceMarker& marker )
{


  // ProcessMarker can be called from multiple threads, to avoid problems
  // with updating contexts and the kernel trace, lock here.
  boost::mutex::scoped_lock sharedDatalock( mDataMutex );

  // log to kernel trace
  if( mPerformanceOutputBitmask & OUTPUT_KERNEL_TRACE )
  {
    mKernelTrace.Trace(marker.GetName());
  }

  // log to Dali log
  if ( mPerformanceOutputBitmask & OUTPUT_DALI_LOG )
  {
    Integration::Log::LogMessage( Dali::Integration::Log::DebugInfo,
                                    "%d.%06d (seconds), %s\n",
                                    marker.GetTimeStamp().seconds,
                                    marker.GetTimeStamp().microseconds,
                                    marker.GetName());

  }

  mStatContextManager.AddInternalMarker( marker );
}

} // namespace Internal

} // namespace Adaptor

} // namespace Dali
