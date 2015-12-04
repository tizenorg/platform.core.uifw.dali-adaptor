/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include "environment-options.h"

// EXTERNAL INCLUDES
#include <cstdlib>
#include <dali/integration-api/render-controller.h>
#include <dali/public-api/math/math-utils.h>

// INTERNAL INCLUDES
#include <base/environment-variables.h>

#include <cstdlib>
#include <string>

// For Android:
#if defined(ANDROID)
#include <sys/system_properties.h>
#endif


namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace
{
const unsigned int DEFAULT_STATISTICS_LOG_FREQUENCY = 2;

std::string GetEnv( const char* variable )
{
  std::string valueStr;

#if defined(ANDROID)
  char value[PROP_VALUE_MAX]="\0";
  __system_property_get(variable, value);
  if( strlen(value) > 0 )
  {
    valueStr = value;
  }
#else
  char* value = getenv(variable);
  if( value != NULL )
  {
    valueStr = value;
  }
#endif

  return valueStr;
}


unsigned int GetIntegerEnvironmentVariable( const char* variable, unsigned int defaultValue )
{
  unsigned int value = defaultValue;

  std::string variableParameter = GetEnv(variable);

  // if the parameter exists convert it to an integer, else return the default value
  if( ! variableParameter.empty() )
  {
    value = atoi(variableParameter.c_str());
  }

  return value;
}

bool GetIntegerEnvironmentVariable( const char* variable, int& intValue )
{
  std::string variableParameter = GetEnv(variable);

  if( ! variableParameter.empty() )
  {
    intValue = atoi(variableParameter.c_str());
    return true;
  }
  return false;
}

bool GetFloatEnvironmentVariable( const char* variable, float& floatValue )
{
  std::string variableParameter = GetEnv(variable);

  if( ! variableParameter.empty() )
  {
    floatValue = atof(variableParameter.c_str());
    return true;
  }
  return false;
}

} // unnamed namespace

EnvironmentOptions::EnvironmentOptions()
: mWindowName(),
  mWindowClassName(),
  mNetworkControl(1),
  mFpsFrequency(0),
  mUpdateStatusFrequency(0),
  mObjectProfilerInterval( 0 ),
  mPerformanceStatsLevel(0),
  mPerformanceStatsFrequency( DEFAULT_STATISTICS_LOG_FREQUENCY ),
  mPerformanceTimeStampOutput(0),
  mPanGestureLoggingLevel(0),
  mPanGesturePredictionMode(-1),
  mPanGesturePredictionAmount(-1), ///< only sets value in pan gesture if greater than 0
  mPanGestureMaxPredictionAmount(-1),
  mPanGestureMinPredictionAmount(-1),
  mPanGesturePredictionAmountAdjustment(-1),
  mPanGestureSmoothingMode(-1),
  mPanGestureSmoothingAmount(-1.0f),
  mPanMinimumDistance(-1),
  mPanMinimumEvents(-1),
  mGlesCallTime(0),
  mThreadingMode( ThreadingMode::SEPARATE_UPDATE_RENDER ),
  mLogFunction( NULL )
{
  ParseEnvironmentOptions();
}

EnvironmentOptions::~EnvironmentOptions()
{
}

void EnvironmentOptions::SetLogFunction( const Dali::Integration::Log::LogFunction& logFunction )
{
  mLogFunction = logFunction;
}

void EnvironmentOptions::InstallLogFunction() const
{
  Dali::Integration::Log::InstallLogFunction( mLogFunction );
}

void EnvironmentOptions::UnInstallLogFunction() const
{
  Dali::Integration::Log::UninstallLogFunction();
}

unsigned int EnvironmentOptions::GetNetworkControlMode() const
{
  return mNetworkControl;
}
unsigned int EnvironmentOptions::GetFrameRateLoggingFrequency() const
{
  return mFpsFrequency;
}

unsigned int EnvironmentOptions::GetUpdateStatusLoggingFrequency() const
{
  return mUpdateStatusFrequency;
}

unsigned int EnvironmentOptions::GetObjectProfilerInterval() const
{
  return mObjectProfilerInterval;
}

unsigned int EnvironmentOptions::GetPerformanceStatsLoggingOptions() const
{
  return mPerformanceStatsLevel;
}
unsigned int EnvironmentOptions::GetPerformanceStatsLoggingFrequency() const
{
  return mPerformanceStatsFrequency;
}
unsigned int EnvironmentOptions::GetPerformanceTimeStampOutput() const
{
  return mPerformanceTimeStampOutput;
}

unsigned int EnvironmentOptions::GetPanGestureLoggingLevel() const
{
  return mPanGestureLoggingLevel;
}

int EnvironmentOptions::GetPanGesturePredictionMode() const
{
  return mPanGesturePredictionMode;
}

int EnvironmentOptions::GetPanGesturePredictionAmount() const
{
  return mPanGesturePredictionAmount;
}

int EnvironmentOptions::GetPanGestureMaximumPredictionAmount() const
{
  return mPanGestureMaxPredictionAmount;
}

int EnvironmentOptions::GetPanGestureMinimumPredictionAmount() const
{
  return mPanGestureMinPredictionAmount;
}

int EnvironmentOptions::GetPanGesturePredictionAmountAdjustment() const
{
  return mPanGesturePredictionAmountAdjustment;
}

int EnvironmentOptions::GetPanGestureSmoothingMode() const
{
  return mPanGestureSmoothingMode;
}

float EnvironmentOptions::GetPanGestureSmoothingAmount() const
{
  return mPanGestureSmoothingAmount;
}

int EnvironmentOptions::GetMinimumPanDistance() const
{
  return mPanMinimumDistance;
}

int EnvironmentOptions::GetMinimumPanEvents() const
{
  return mPanMinimumEvents;
}


void EnvironmentOptions::SetPanGesturePredictionMode( unsigned int mode )
{
  mPanGesturePredictionMode = mode;
}

void EnvironmentOptions::SetPanGesturePredictionAmount( unsigned int amount )
{
  mPanGesturePredictionAmount = amount;
}

void EnvironmentOptions::SetPanGestureMaximumPredictionAmount( unsigned int amount )
{
  mPanGestureMaxPredictionAmount = amount;
}

void EnvironmentOptions::SetPanGestureMinimumPredictionAmount( unsigned int amount )
{
  mPanGestureMinPredictionAmount = amount;
}

void EnvironmentOptions::SetPanGesturePredictionAmountAdjustment( unsigned int amount )
{
  mPanGesturePredictionAmountAdjustment = amount;
}

void EnvironmentOptions::SetPanGestureSmoothingMode( unsigned int mode )
{
  mPanGestureSmoothingMode = mode;
}

void EnvironmentOptions::SetPanGestureSmoothingAmount( float amount )
{
  mPanGestureSmoothingAmount = amount;
}

void EnvironmentOptions::SetMinimumPanDistance( int distance )
{
  mPanMinimumDistance = distance;
}

void EnvironmentOptions::SetMinimumPanEvents( int events )
{
  mPanMinimumEvents = events;
}

void EnvironmentOptions::SetGlesCallTime( int time )
{
  mGlesCallTime = time;
}

int EnvironmentOptions::GetGlesCallTime() const
{
  return mGlesCallTime;
}


ThreadingMode::Type EnvironmentOptions::GetThreadingMode() const
{
  return mThreadingMode;
}

bool EnvironmentOptions::PerformanceServerRequired() const
{
  return ( ( GetPerformanceStatsLoggingOptions() > 0) ||
           ( GetPerformanceTimeStampOutput() > 0 ) ||
           ( GetNetworkControlMode() > 0) );
}

void EnvironmentOptions::ParseEnvironmentOptions()
{
  // get logging options
  mFpsFrequency = GetIntegerEnvironmentVariable( DALI_ENV_FPS_TRACKING, 0 );
  mUpdateStatusFrequency = GetIntegerEnvironmentVariable( DALI_ENV_UPDATE_STATUS_INTERVAL, 0 );
  mObjectProfilerInterval = GetIntegerEnvironmentVariable( DALI_ENV_OBJECT_PROFILER_INTERVAL, 0 );
  mPerformanceStatsLevel = GetIntegerEnvironmentVariable( DALI_ENV_LOG_PERFORMANCE_STATS, 0 );
  mPerformanceStatsFrequency = GetIntegerEnvironmentVariable( DALI_ENV_LOG_PERFORMANCE_STATS_FREQUENCY, 0 );
  mPerformanceTimeStampOutput = GetIntegerEnvironmentVariable( DALI_ENV_PERFORMANCE_TIMESTAMP_OUTPUT, 0 );
  mNetworkControl = GetIntegerEnvironmentVariable( DALI_ENV_NETWORK_CONTROL, 0 );
  mPanGestureLoggingLevel = GetIntegerEnvironmentVariable( DALI_ENV_LOG_PAN_GESTURE, 0 );

  int predictionMode;
  if( GetIntegerEnvironmentVariable(DALI_ENV_PAN_PREDICTION_MODE, predictionMode) )
  {
    mPanGesturePredictionMode = predictionMode;
  }
  int predictionAmount(-1);
  if( GetIntegerEnvironmentVariable(DALI_ENV_PAN_PREDICTION_AMOUNT, predictionAmount) )
  {
    if( predictionAmount < 0 )
    {
      // do not support times in the past
      predictionAmount = 0;
    }
    mPanGesturePredictionAmount = predictionAmount;
  }
  int minPredictionAmount(-1);
  if( GetIntegerEnvironmentVariable(DALI_ENV_PAN_MIN_PREDICTION_AMOUNT, minPredictionAmount) )
  {
    if( minPredictionAmount < 0 )
    {
      // do not support times in the past
      minPredictionAmount = 0;
    }
    mPanGestureMinPredictionAmount = minPredictionAmount;
  }
  int maxPredictionAmount(-1);
  if( GetIntegerEnvironmentVariable(DALI_ENV_PAN_MAX_PREDICTION_AMOUNT, maxPredictionAmount) )
  {
    if( minPredictionAmount > -1 && maxPredictionAmount < minPredictionAmount )
    {
      // maximum amount should not be smaller than minimum amount
      maxPredictionAmount = minPredictionAmount;
    }
    mPanGestureMaxPredictionAmount = maxPredictionAmount;
  }
  int predictionAmountAdjustment(-1);
  if( GetIntegerEnvironmentVariable(DALI_ENV_PAN_PREDICTION_AMOUNT_ADJUSTMENT, predictionAmountAdjustment) )
  {
    if( predictionAmountAdjustment < 0 )
    {
      // negative amount doesn't make sense
      predictionAmountAdjustment = 0;
    }
    mPanGesturePredictionAmountAdjustment = predictionAmountAdjustment;
  }
  int smoothingMode;
  if( GetIntegerEnvironmentVariable(DALI_ENV_PAN_SMOOTHING_MODE, smoothingMode) )
  {
    mPanGestureSmoothingMode = smoothingMode;
  }
  float smoothingAmount = 1.0f;
  if( GetFloatEnvironmentVariable(DALI_ENV_PAN_SMOOTHING_AMOUNT, smoothingAmount) )
  {
    smoothingAmount = Clamp(smoothingAmount, 0.0f, 1.0f);
    mPanGestureSmoothingAmount = smoothingAmount;
  }

  int minimumDistance(-1);
  if ( GetIntegerEnvironmentVariable(DALI_ENV_PAN_MINIMUM_DISTANCE, minimumDistance ))
  {
    mPanMinimumDistance = minimumDistance;
  }

  int minimumEvents(-1);
  if ( GetIntegerEnvironmentVariable(DALI_ENV_PAN_MINIMUM_EVENTS, minimumEvents ))
  {
    mPanMinimumEvents = minimumEvents;
  }

  int glesCallTime(0);
  if ( GetIntegerEnvironmentVariable(DALI_GLES_CALL_TIME, glesCallTime ))
  {
    SetGlesCallTime( glesCallTime );
  }

  int threadingMode(0);
  if ( GetIntegerEnvironmentVariable( DALI_THREADING_MODE, threadingMode ) )
  {
    switch( threadingMode )
    {
      case ThreadingMode::SEPARATE_UPDATE_RENDER:
      case ThreadingMode::COMBINED_UPDATE_RENDER:
      case ThreadingMode::SINGLE_THREADED:
      {
        mThreadingMode = static_cast< ThreadingMode::Type >( threadingMode );
        break;
      }
    }
  }
}

} // Adaptor

} // Internal

} // Dali
