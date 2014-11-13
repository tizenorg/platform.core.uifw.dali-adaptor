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

#include "resource-collector.h"
#include <dali/integration-api/debug.h>

namespace Dali
{
namespace Internal
{
namespace Platform
{
using namespace Dali::Integration;

ResourceCollector::ResourceCollector( LoadCompletionMonitor * const completionMonitor ) :
  mLoadCompletionMonitor( completionMonitor ),
  mGrandTotalCompletions( 0 )
{
}

ResourceCollector::~ResourceCollector() {}

void ResourceCollector::LoadResponse( Dali::Integration::ResourceId id, Dali::Integration::ResourceTypeId type, Dali::Integration::ResourcePointer resource, Dali::Integration::LoadStatus status )
{
  mCompletionStatuses[id] = true;
  if( status == RESOURCE_COMPLETELY_LOADED )
  {
    DALI_ASSERT_DEBUG( mCompletionCounts.find(id) == mCompletionCounts.end() && "A resource can only complete once." );
    ++mCompletionCounts[id];
    ++mSuccessCounts[id];
    mCompletionSequence.push_back( id );
    ++mGrandTotalCompletions;

    if( mLoadCompletionMonitor )
    {
       mLoadCompletionMonitor->LoadSucceeded( *this, id, type, resource );
    }
  }
}

void ResourceCollector::LoadFailed( Dali::Integration::ResourceId id, Dali::Integration::ResourceFailure failure )
{
  mCompletionStatuses[id] = false;
  ++mFailureCounts[id];
  mCompletionSequence.push_back( id );
  ++mGrandTotalCompletions;

  if( mLoadCompletionMonitor )
  {
     mLoadCompletionMonitor->LoadFailed( *this, id, failure );
  }
}


} /* namespace Platform */
} /* namespace Internal */
} /* namespace Dali */
