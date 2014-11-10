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

#define TIME_FMT "%0.2f ms" // 2 decimal places, e.g. 5.34 ms
#define TOTAL_TIME_FMT "%0.1f secs" // 1 decimal place, e.g. 4.5 seconds


namespace
{

const unsigned int DEFAULT_LOG_FREQUENCEY = 2;        ///< default log frequency = 2
const unsigned int MILLISECONDS_PER_SECOND = 1000;    ///< 1000 milliseconds per second
const unsigned int MICROSECONDS_PER_SECOND = 1000000; ///< 1000000 microseconds per second

const char* UPDATE_CONTEXT_NAME = "Update";
const char* RENDER_CONTEXT_NAME = "Render";
const char* EVENT_CONTEXT_NAME = "Event";

} // Anonymous namespace


PerformanceServer::PerformanceServer( AdaptorInternalServices& adaptorServices,
                                      const EnvironmentOptions& environmentOptions)
:mLoggingEnabled( false ),
 mLogFunctionInstalled( false ),
 mNextContextId( 0 ),
 mPlatformAbstraction( adaptorServices.GetPlatformAbstractionInterface() ),
 mEnvironmentOptions( environmentOptions ),
 mKernelTrace( adaptorServices.GetKernelTraceInterface() )
{
  // Add defaults
  mUpdateStats = AddContext( UPDATE_CONTEXT_NAME );
  mRenderStats = AddContext( RENDER_CONTEXT_NAME );
  mEventStats = AddContext( EVENT_CONTEXT_NAME );

  SetLogging( mEnvironmentOptions.GetPerformanceLoggingLevel(), mEnvironmentOptions.GetFrameRateLoggingFrequency());
}

PerformanceServer::~PerformanceServer()
{
  if( mLogFunctionInstalled )
  {
    mEnvironmentOptions.UnInstallLogFunction();
  }

  for( StatContexts::iterator it = mStatContexts.begin(), itEnd = mStatContexts.end(); it != itEnd; ++it )
  {
    StatContext* context = it->second;
    delete context;
  }
}

void PerformanceServer::SetLogging( unsigned int level, unsigned int logFrequency)
{
  if( level == 0 )
  {
    mLoggingEnabled = false;
    return;
  }
  mLogLevel = level;

  EnableLogging( mLogLevel & LOG_UPDATE_RENDER, mUpdateStats );
  EnableLogging( mLogLevel & LOG_UPDATE_RENDER, mRenderStats );
  EnableLogging( mLogLevel & LOG_EVENT_PROCESS, mEventStats );

  SetLoggingFrequency( logFrequency, mUpdateStats );
  SetLoggingFrequency( logFrequency, mRenderStats );
  SetLoggingFrequency( logFrequency, mEventStats );

  mLoggingEnabled = true;
}

void PerformanceServer::SetLoggingFrequency( unsigned int logFrequency, ContextId contextId )
{
  StatContext* context = GetContext( contextId );
  if( context )
  {
    unsigned int logFrequencyMicroseconds = logFrequency * MICROSECONDS_PER_SECOND;

    if( logFrequencyMicroseconds == 0 )
    {
      logFrequencyMicroseconds = DEFAULT_LOG_FREQUENCEY * MICROSECONDS_PER_SECOND;
    }

    context->logFrequencyMicroseconds = logFrequencyMicroseconds;
  }
}

void PerformanceServer::EnableLogging( bool enable, ContextId contextId )
{
  StatContext* context = GetContext( contextId );
  if( context )
  {
    context->log = enable;
  }
}

PerformanceServer::StatContext::StatContext()
: name( NULL ),
  logFrequencyMicroseconds( DEFAULT_LOG_FREQUENCEY * MICROSECONDS_PER_SECOND ),
  log( true )
{
}

PerformanceServer::StatContext::StatContext( const char* contextName )
: name( contextName ),
  logFrequencyMicroseconds( DEFAULT_LOG_FREQUENCEY * MICROSECONDS_PER_SECOND ),
  log( true )
{
}

PerformanceInterface::ContextId PerformanceServer::AddContext( const char* name )
{
  unsigned int contextId = mNextContextId++;

  DALI_ASSERT_DEBUG( !GetContext( contextId ) );

  mStatContexts[ contextId ] = new StatContext( name );

  return contextId;
}

void PerformanceServer::RemoveContext( ContextId contextId )
{
  StatContexts::iterator it = mStatContexts.find( contextId );
  if( it != mStatContexts.end() )
  {
    StatContext* context = it->second;
    delete context;

    mStatContexts.erase( it );
  }
}

PerformanceServer::StatContext* PerformanceServer::GetContext( ContextId contextId )
{
  StatContexts::iterator it = mStatContexts.find( contextId );
  if( it != mStatContexts.end() )
  {
    return it->second;
  }

  return NULL;
}

void PerformanceServer::AddMarker( MarkerType markerType )
{
  switch( markerType )
  {
    case V_SYNC:
    {
      for( StatContexts::iterator it = mStatContexts.begin(), itEnd = mStatContexts.end(); it != itEnd; ++it )
      {
        ContextId contextId = it->first;
        AddMarker( V_SYNC, contextId );
      }
      break;
    }
    case UPDATE_START:
    {
      AddMarker( START, mUpdateStats );
      break;
    }
    case UPDATE_END:
    {
      AddMarker( END, mUpdateStats );
      break;
    }
    case RENDER_START:
    {
      AddMarker( START, mRenderStats );
      break;
    }
    case RENDER_END:
    {
      AddMarker( END, mRenderStats );
      break;
    }
    case PROCESS_EVENTS_START:
    {
      AddMarker( START, mEventStats );
      break;
    }
    case PROCESS_EVENTS_END:
    {
      AddMarker( END, mEventStats );
      break;
    }
    default:
    {
      break;
    }
  }
}

void PerformanceServer::AddMarker( MarkerType markerType, ContextId contextId )
{
  if( !mLoggingEnabled )
  {
    return;
  }

  // Get the time
  unsigned int seconds = 0;
  unsigned int microseconds = 0;
  mPlatformAbstraction.GetTimeMicroseconds( seconds, microseconds );

  // Create a marker and add it to the log
  PerformanceMarker marker( markerType, FrameTimeStamp( 0, seconds, microseconds ) );
  AddMarkerToLog( marker, contextId );
}

void PerformanceServer::AddMarkerToLog( const PerformanceMarker& marker, ContextId contextId )
{
  StatContext* context = GetContext( contextId );
  if( context )
  {
    // Add Marker can be called from any thread
    boost::mutex::scoped_lock sharedDatalock( context->dataMutex );

    // store the marker
    context->markers.PushBack( marker );

    if( mLogLevel & LOG_EVENTS_TO_KERNEL )
    {
      std::stringstream ss;
      ss << GetMarkerName( marker.GetType() ) << ":" << ( ( context->name ) ? context->name : "" );
      mKernelTrace.Trace( ss.str() );
    }

    // Only log on the v-sync thread, so we have less impact on update/render
    if( marker.GetType() == V_SYNC )
    {
      // log out every mLogFrequency.
      // check difference between first and last frame
      unsigned int microseconds = PerformanceMarker::MicrosecondDiff( context->markers[0], marker );
      if( microseconds >= context->logFrequencyMicroseconds )
      {
        LogMarkers( context );

        if( context->log )
        {
          LogMarker( context->name, context->stats );
        }

        context->stats.Reset();     // reset data for statistics
      }
    }
  }
}

void PerformanceServer::LogMarker(const char* name, const FrameTimeStats& frameStats)
{
  // make sure log function is installed, note this will be called only from v-sync thread
  // if the v-sync thread has already installed one, it won't make any difference.
  if( !mLogFunctionInstalled )
  {
    mEnvironmentOptions.InstallLogFunction();
    mLogFunctionInstalled = true;
  }

  float mean, standardDeviation;
  frameStats.CalculateMean( mean, standardDeviation );

  // this will always log regardless of debug / release mode
  Integration::Log::LogMessage( Dali::Integration::Log::DebugInfo,
                                    "%s, min " TIME_FMT ", max " TIME_FMT ", total (" TOTAL_TIME_FMT "), avg " TIME_FMT ", std dev " TIME_FMT "\n",
                                     ( name ) ? name : "",
                                     frameStats.GetMinTime() * MILLISECONDS_PER_SECOND,
                                     frameStats.GetMaxTime() * MILLISECONDS_PER_SECOND,
                                     frameStats.GetTotalTime(),
                                     mean * MILLISECONDS_PER_SECOND,
                                     standardDeviation * MILLISECONDS_PER_SECOND);
}

void PerformanceServer::LogMarkers( StatContext* context )
{
  // insert time stamps into a frame-time-stats object, based on type
  for( MarkerVector::SizeType i = 0; i < context->markers.Size(); ++i)
  {
    const PerformanceMarker& marker = context->markers[i];
    switch( marker.GetType() )
    {
      case START:
      {
        context->stats.StartTime( marker.GetTimeStamp() );
        break;
      }
      case END:
      {
        context->stats.EndTime( marker.GetTimeStamp() );
        break;
      }
      default:
      {
        break;
      }
    }
  }

  context->markers.Clear();
}

const char* PerformanceServer::GetMarkerName( MarkerType type ) const
{
  switch( type )
  {
    case V_SYNC:
    {
      return "V_SYNC";
    }

    case START:
    {
      return "START";
    }

    case END:
    {
      return "END";
    }

    default:
    {
      DALI_ASSERT_DEBUG( !"Unsupported MarkerType" );
      return "";
    }
  }
}

} // namespace Internal

} // namespace Adaptor

} // namespace Dali
