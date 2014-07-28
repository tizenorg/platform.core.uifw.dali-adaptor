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
#include "performance-marker.h"

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>

// EXTERNAL INCLUDES
#include <string.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace
{

struct NamePair
{
  PerformanceMarker::MarkerType type;
  const char* name;
  PerformanceMarker::MarkerFilter group;
};

const NamePair MarkerLookup[] =
{
    { PerformanceMarker::V_SYNC       ,        "V_SYNC"               , PerformanceMarker::V_SYNC_EVENTS },
    { PerformanceMarker::UPDATE_START ,        "UPDATE_START"         , PerformanceMarker::UPDATE },
    { PerformanceMarker::UPDATE_END   ,        "UPDATE_END"           , PerformanceMarker::UPDATE },
    { PerformanceMarker::RENDER_START ,        "RENDER_START"         , PerformanceMarker::RENDER },
    { PerformanceMarker::RENDER_END   ,        "RENDER_END"           , PerformanceMarker::RENDER },
    { PerformanceMarker::SWAP_START   ,        "SWAP_START"           , PerformanceMarker::SWAP_BUFFERS  },
    { PerformanceMarker::SWAP_END     ,        "SWAP_END"             , PerformanceMarker::SWAP_BUFFERS  },
    { PerformanceMarker::PROCESS_EVENTS_START, "PROCESS_EVENT_START"  , PerformanceMarker::EVENT_PROCESS },
    { PerformanceMarker::PROCESS_EVENTS_END,   "PROCESS_EVENT_END"    , PerformanceMarker::EVENT_PROCESS },
    { PerformanceMarker::PAUSED       ,        "PAUSED"               , PerformanceMarker::LIFE_CYCLE_EVENTS },
    { PerformanceMarker::RESUME       ,        "RESUMED"              , PerformanceMarker::LIFE_CYCLE_EVENTS }
};
} // un-named name space

const unsigned int PerformanceMarker::SERIALIZED_SIZE = sizeof( PerformanceMarker );

PerformanceMarker::PerformanceMarker( MarkerType type )
:mType(type)
{
}

PerformanceMarker::PerformanceMarker( MarkerType type, FrameTimeStamp frameInfo )
:mType(type),
 mTimeStamp(frameInfo)
{
}

const char* PerformanceMarker::GetName( ) const
{
  return MarkerLookup[ mType ].name;
}

unsigned int PerformanceMarker::MicrosecondDiff( const PerformanceMarker& start,const PerformanceMarker& end )
{
  return FrameTimeStamp::MicrosecondDiff( start.mTimeStamp, end.mTimeStamp );
}

bool PerformanceMarker::IsFilterEnabled( MarkerFilter filter ) const
{
  return  (filter & MarkerLookup[ mType ].group);
}

void PerformanceMarker::Serialize( void *buffer, unsigned int lengthInBytes) const
{
  DALI_ASSERT_DEBUG( lengthInBytes == SERIALIZED_SIZE );

  memcpy( buffer, &mTimeStamp, SERIALIZED_SIZE );
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

