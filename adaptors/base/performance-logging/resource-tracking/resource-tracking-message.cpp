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
#include "resource-tracking-message.h"

namespace Dali
{

namespace Internal
{

namespace Adaptor
{


ResourceTrackingMessage::ResourceTrackingMessage()
: mMessage( ResourceTrackMessage::NONE ),
  mPtr( NULL )
{
  mParameters[ 0 ] = 0;
  mParameters[ 1 ] = 0;
  mParameters[ 2 ] = 0;
  mParameters[ 3 ] = 0;
}

ResourceTrackingMessage::ResourceTrackingMessage(  const ResourceTrackMessage::Type message,
                            const void* ptr,
                            const unsigned int parameter1,
                            const unsigned int parameter2,
                            const unsigned int parameter3,
                            const unsigned int parameter4 )
: mMessage( message ),
  mPtr( ptr )
{
  mParameters[ 0 ] = parameter1;
  mParameters[ 1 ] = parameter2;
  mParameters[ 2 ] = parameter3;
  mParameters[ 3 ] = parameter4;
}

ResourceTrackingMessage::~ResourceTrackingMessage()
{
}

} // namespace Internal

} // namespace Adaptor

} // namespace Dali

