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
#include "base/performance-logging/resource-tracking/resource-tracking.h"

// EXTERNAL HEADERS
#include <dali/integration-api/debug.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

// Constructor for the per-thread tracking object
ResourceTracking::ResourceTracking( ResourceTrackingManager* resourceTrackingManager )
: mReadIndex( 0 ),
  mWriteIndex( 0 ),
  mBufferSize( INITIAL_RESOURCE_MESSAGE_SIZE )
{
  // Create the message buffer for this thread
  mBuffer = CreateBuffer();
  mThread = resourceTrackingManager->AddThread( this );
  mUnknownString = std::string("unknown");
}

void ResourceTracking::AddMessage(const ResourceTrackingMessage& message )
{
  if ( !mBuffer[ mWriteIndex ].GetMessage() )
  {
    void* ptr = &mBuffer[ mWriteIndex ];
    new ( ptr ) ResourceTrackingMessage( message );
    if ( mWriteIndex++ == mBufferSize )
    {
      mWriteIndex = 0;
    }
  }
  else
  {
    ResizeBuffer();
    void* ptr = &mBuffer[ mWriteIndex++ ];
    new ( ptr ) ResourceTrackingMessage( message );
  }
}

void ResourceTracking::AddMessage( const Dali::ResourceTrackMessage::Type msg,
                                   const void* ptr,
                                   const unsigned int parameter1,
                                   const unsigned int parameter2,
                                   const unsigned int parameter3,
                                   const unsigned int parameter4 )
{
  ResourceTrackingMessage message( msg, ptr, parameter1, parameter2, parameter3, parameter4 );
  AddMessage( message );
}

// Static method to pass logging message onto a thread specific tracking object
void ResourceTracking::AddNewMessage( const Dali::ResourceTrackMessage::Type msg,
                                      const void* ptr,
                                      const unsigned int parameter1,
                                      const unsigned int parameter2,
                                      const unsigned int parameter3,
                                      const unsigned int parameter4 )
{
  ResourceTrackingManager::ResourceThread* threadTracker = ResourceThreadLocal.get();
  if ( threadTracker )
  {
    ResourceTrackingMessage message( msg,
                                     ptr,
                                     parameter1,
                                     parameter2,
                                     parameter3,
                                     parameter4 );
    threadTracker->resourceTracker->AddMessage( message );
  }
}

ResourceTrackingMessage ResourceTracking::ReadNextMessage()
{

  ResourceTrackingMessage newMessage = mBuffer[ mReadIndex ];
  if ( mReadIndex != mWriteIndex && newMessage.GetMessage() )
  {
    mBuffer[ mReadIndex ].ClearMessage();
    if ( mReadIndex++ == mBufferSize )
    {
      mReadIndex = 0;
    }
    return newMessage;
  }
  return ResourceTrackingMessage();
}

ResourceTrackingMessage* ResourceTracking::CreateBuffer()
{
  // Allocate a buffer to handle the resource tracking messages
  return new ResourceTrackingMessage[ mBufferSize ];
}

void ResourceTracking::ResizeBuffer()
{
  // Expand the message buffer by 25%
  unsigned int oldSize = mBufferSize;
  mBufferSize += mBufferSize >> 2;
  ResourceTrackingMessage* newBuffer = CreateBuffer();
  if ( mReadIndex == mWriteIndex )
  {
    delete[] mBuffer;
    mBuffer = newBuffer;
    mReadIndex = mWriteIndex = 0;
    return;
  }

  unsigned int count = 0;
  if ( mWriteIndex > mReadIndex )
  {
    for ( unsigned int i = mReadIndex; i < mWriteIndex; i++ )
    {
      newBuffer[ count++ ] = mBuffer[ mReadIndex++ ];
    }
  }
  else
  {
    for ( unsigned int i = 0; i < oldSize - mReadIndex; i++ )
    {
      newBuffer[ count++ ] = mBuffer[ mReadIndex++ ];
    }
    for ( unsigned int i = 0; i < mWriteIndex; i++ )
    {
      newBuffer[ count++ ] = mBuffer[ i ];
    }
  }
  mReadIndex = 0;
  mWriteIndex = count;
  delete[] mBuffer;
  mBuffer = newBuffer;
}

std::string ResourceTracking::ReadString( const unsigned int hash )
{
  if ( hash )
  {
    return mHashString.ReadString( hash );
  }
  else
  {
    return mUnknownString;
  }
}

const unsigned int ResourceTracking::GetResourceHash( const std::string& string )
{
  ResourceTrackingManager::ResourceThread* threadTracker = ResourceThreadLocal.get();
  if ( threadTracker )
  {
    return threadTracker->resourceTracker->GetHash( string );
  }
  else
  {
    return 0;
  }
}

const unsigned int ResourceTracking::GetHash( const std::string& string )
{
  return mHashString.Insert( string );
}

ResourceTracking::~ResourceTracking()
{
  if ( mBuffer )
  {
    delete[] mBuffer;
    mBuffer = NULL;
  }
}
} // namespace Adaptor

} // namespace Internal

} // namespace Dali

