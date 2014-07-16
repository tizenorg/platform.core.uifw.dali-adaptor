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
#include "server-connection.h"

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES

namespace Dali
{
namespace Internal
{
namespace Adaptor
{
#if defined(DEBUG_ENABLED)
extern Debug::Filter* gIndicatorLogFilter;
#endif

struct ServerConnection::Impl
{
};

ServerConnection::ServerConnection( const char*                 serviceName,
                                    int                         serviceNumber,
                                    bool                        isSystem,
                                    ServerConnection::Observer* observer)
  : mImpl(NULL)
{
}

ServerConnection::~ServerConnection()
{
}

bool ServerConnection::SendEvent( int event, const void *data, int size )
{
  return false;
}

bool ServerConnection::SendEvent( int event, int ref, int ref_to, const void *data, int size )
{
  return false;
}


} // Adaptor
} // Internal
} // Dali
