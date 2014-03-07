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
#include "log-options.h"

// INTERNAL HEADERS
#include <base/performance-logging/resource-tracking/resource-tracking.h>

namespace Dali
{
namespace Internal
{
namespace Adaptor
{

LogOptions::LogOptions()
: mFpsFrequency(0),
  mUpdateStatusFrequency(0),
  mPerformanceLoggingLevel(0),
  mLogFunction( NULL )
{
}

LogOptions::~LogOptions()
{

}

void LogOptions::SetOptions( const Dali::Integration::Log::LogFunction& logFunction,
                             unsigned int logFrameRateFrequency,
                             unsigned int logupdateStatusFrequency,
                             unsigned int logPerformanceLevel )
{
  mLogFunction = logFunction;
  mFpsFrequency = logFrameRateFrequency;
  mUpdateStatusFrequency = logupdateStatusFrequency;
  mPerformanceLoggingLevel = logPerformanceLevel;
}

void LogOptions::InstallLogFunction() const
{
  // Will be destroyed when ResourceTrackingManager shuts down
  if ( g_ResourceTrackingManager )
  {
    Dali::Internal::Adaptor::ResourceTracking* rt = new Dali::Internal::Adaptor::ResourceTracking( g_ResourceTrackingManager );
    Dali::Integration::Log::InstallLogFunction( mLogFunction, rt );
  }
  else
  {
    Dali::Integration::Log::InstallLogFunction( mLogFunction, NULL );
  }

}

void LogOptions::UnInstallLogFunction() const
{
  Dali::Integration::Log::UninstallLogFunction();
}

unsigned int LogOptions::GetFrameRateLoggingFrequency() const
{
  return mFpsFrequency;
}

unsigned int LogOptions::GetUpdateStatusLoggingFrequency() const
{
  return mUpdateStatusFrequency;
}

unsigned int LogOptions::GetPerformanceLoggingLevel() const
{
  return mPerformanceLoggingLevel;
}


} // Adaptor
} // Internal
} // Dali
