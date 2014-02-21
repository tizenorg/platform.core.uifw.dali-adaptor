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

// CLASS HEADER
#include "performance-server.h"


namespace Dali
{

namespace Internal
{

namespace Adaptor
{

#define TIME_FMT "%0.2f ms" // 2 decimal places, e.g. 5.34 ms

namespace
{
const unsigned int DEFAULT_LOG_FREQUENCEY = 2;        ///< default log frequency = 2
const unsigned int MILLISECONDS_PER_SECOND = 1000;    ///< 1000 milliseconds per second
const unsigned int MICROSECONDS_PER_SECOND = 1000000; ///< 1000000 microseconds per second
}


PerformanceServer::PerformanceServer( Integration::PlatformAbstraction& platformAbstraction,
                                      Integration::Log::LogFunction& logFunction,
                                      int logOpts)
:mLoggingEnabled( false),
 mLogFunctionInstalled( false ),
 mLogLevel(0),
 mlogOpts(logOpts),
 mLogFrequencyMicroseconds( 0 ),
 mLogFunction( logFunction ),
 mPlatformAbstraction( platformAbstraction )
{
}

PerformanceServer::~PerformanceServer()
{
}

void PerformanceServer::SetLogging( unsigned int level, unsigned int interval)
{
  if( level == 0)
  {
    mLoggingEnabled = false;
    return;
  }

  mLogFrequencyMicroseconds = interval * MICROSECONDS_PER_SECOND;

  if( mLogFrequencyMicroseconds == 0 )
  {
    mLogFrequencyMicroseconds = DEFAULT_LOG_FREQUENCEY * MICROSECONDS_PER_SECOND;
  }
  mLoggingEnabled = true;

}

void PerformanceServer::AddMarker( PerformanceMarker::MarkerType markerType )
{
  if( !mLoggingEnabled )
  {
    return;
  }

  unsigned int seconds(0);
  unsigned int microseconds(0);

  // get the time
  mPlatformAbstraction.GetTimeMicroseconds( seconds, microseconds );

  // create a marker
  PerformanceMarker marker( markerType, FrameTimeStamp( 0, seconds, microseconds ));

  AddMarkerToLog( marker );
}

void PerformanceServer::AddMarkerToLog( PerformanceMarker marker )
{
  // Add Marker can be called from any thread
  boost::mutex::scoped_lock sharedDatalock( mDataMutex );

  // store the marker
  mMarkers.PushBack( marker );

  // only log on the v-sync thread, so we have less impact on update/render
  if( marker.GetType() != PerformanceMarker::V_SYNC )
  {
    return;
  }

  // log out every mLogFrequency.
  // check difference between first and last frame
  unsigned int microseconds = PerformanceMarker::MicrosecondDiff( mMarkers[0], marker );

  if( microseconds  >=  mLogFrequencyMicroseconds )
  {
    LogMarkers( );
    mMarkers.Clear();

    // reset data for update / render statistics
    mUpdateStats.Reset();
    mRenderStats.Reset();
  }
}

void PerformanceServer::LogMarker(const char* name, const FrameTimeStats& frameStats)
{
  // make sure log function is installed, note this will be called only from v-sync thread
  // if the v-sync thread has already installed one, it won't make any difference.
  if(! mLogFunctionInstalled )
  {
    Dali::Integration::Log::InstallLogFunction(mLogFunction, mlogOpts);
    mLogFunctionInstalled = true;
  }

  // this will always log regardless of debug / release mode
  Integration::Log::LogMessage( Dali::Integration::Log::DebugInfo,
                                    "%s , min " TIME_FMT ", max " TIME_FMT ", avg " TIME_FMT "\n",
                                     name,
                                     frameStats.GetMinTime() * MILLISECONDS_PER_SECOND,
                                     frameStats.GetMaxTime() * MILLISECONDS_PER_SECOND,
                                     frameStats.GetRollingAverageTime() * MILLISECONDS_PER_SECOND);
}

void PerformanceServer::LogMarkers()
{
  // insert time stamps into a frame-time-stats object, based on type
  for( MarkerVector::SizeType i = 0; i < mMarkers.Size(); ++i)
  {
    const PerformanceMarker& marker = mMarkers[i];
    switch( marker.GetType() )
    {
      case PerformanceMarker::UPDATE_START:
      {
        mUpdateStats.StartTime( marker.GetTimeStamp() );
        break;
      }
      case PerformanceMarker::UPDATE_END:
      {
        mUpdateStats.EndTime( marker.GetTimeStamp() );
        break;
      }
      case PerformanceMarker::RENDER_START:
      {
        mRenderStats.StartTime( marker.GetTimeStamp() );
        break;
      }
      case PerformanceMarker::RENDER_END:
      {
        mRenderStats.EndTime( marker.GetTimeStamp() );
        break;
      }
      default:
      {
        break;
      }
    }
  }

  LogMarker("Update",mUpdateStats);
  LogMarker("Render",mRenderStats);

}

} // namespace Internal

} // namespace Adaptor

} // namespace Dali
