#ifndef __DALI_INTERNAL_ADAPTOR_RESOURCE_TRACKING_H__
#define __DALI_INTERNAL_ADAPTOR_RESOURCE_TRACKING_H__

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

// INTERNAL INCLUDES
#include "resource-tracking-manager.h"
#include "resource-tracking-message.h"

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <map>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

class ResourceTrackingMessage;
class ResourceTrackingManager;

const unsigned int INITIAL_RESOURCE_MESSAGE_SIZE = 8;

/**
 *  ResourceTracking Class
 *
 * Instantiated once per thread
 */
class ResourceTracking : public Debug::ResourceTrackingInterface
{
public:

  /**
   * @brief Constructor and register this object with manager
   *
   * @param resourceTrackingManager Reference to the manager
   */
  explicit ResourceTracking( ResourceTrackingManager* resourceTrackingManager );
  ~ResourceTracking();

  /**
   * @brief Add a message to the message buffer
   *
   * @param message
   */
  void AddMessage( const ResourceTrackingMessage& message );

  /**
   * @brief Add a message to the message buffer from individual parts
   *
   * @param msg Resource Tracking Message type
   * @param ptr Pointer relating to the message ( contextual )
   * @param parameter1 1st parameter
   * @param parameter2 2nd parameter
   * @param parameter3 3rd parameter
   * @param parameter4 4th parameter
   */
  virtual void AddMessage( const Dali::ResourceTrackMessage::Type msg,
                           const void* ptr,
                           const unsigned int parameter1,
                           const unsigned int parameter2,
                           const unsigned int parameter3,
                           const unsigned int parameter4 );

  /**
   * @brief Pass a new message to a thread specific resource tracker
   *
   * @param msg Resource Tracking Message type
   * @param ptr Pointer relating to the message ( contextual )
   * @param parameter1 1st parameter
   * @param parameter2 2nd parameter
   * @param parameter3 3rd parameter
   * @param parameter4 4th parameter
   */
  static void AddNewMessage( const Dali::ResourceTrackMessage::Type msg,
                             const void* ptr,
                             const unsigned int parameter1,
                             const unsigned int parameter2,
                             const unsigned int parameter3,
                             const unsigned int parameter4 );

  /**
   * @brief Return a string associated with a hash
   *
   * @param hash
   * @return previously hashed string
   */
  std::string ReadString( const unsigned int hash );

  /**
   * @brief Get hash for string
   *
   * @param string [description]
   * @return [description]
   */
  const unsigned int GetHash( const std::string& string );

  static const unsigned int GetResourceHash( const std::string& string );

  /**
   * @brief Read the next available message fromt the buffer
   *
   * @return messsage ( blank message if none available )
   */
  ResourceTrackingMessage ReadNextMessage();

  /**
   * @brief Get the 'friendly' thread number for this tracker
   *
   * @return thread number
   */
  const unsigned int GetThread() const
  {
    return mThread;
  }

private:

  // Not implemented
  ResourceTracking( const ResourceTracking& );
  ResourceTracking& operator=( const ResourceTracking& );

  HashString mHashString;                           // Hashing object for this tracker
  std::string mUnknownString;                       // 'unknown' label
  unsigned int mThread;                             // Tracking Manager friendly thread number
  unsigned int mReadIndex;                          // Current position to read in buffer
  unsigned int mWriteIndex;                         // Current position to write to in buffer
  unsigned int mBufferSize;                         // Current buffer size
  ResourceTrackingMessage* mBuffer;                 // Message buffer

  /**
   * @brief Create a cyclic message buffer
   *
   * @return pointer to buffer
   */
  inline ResourceTrackingMessage* CreateBuffer();

  /**
   * @brief Expand buffer when it gets choked
   */
  void ResizeBuffer();

};

#define DALI_RESOURCE_TRACKING( message, ptr, p1, p2, p3, p4 ) if ( Dali::Internal::Adaptor::gResourceTrackingManager ) \
                                                               { Dali::Internal::Adaptor::ResourceTracking::AddNewMessage( message, ptr, p1, p2, p3, p4 ); }


} // namespace Adaptor

} // namespace Internal

} // namespace Dali


#endif // __DALI_INTERNAL_ADAPTOR_RESOURCE_TRACKING_H__