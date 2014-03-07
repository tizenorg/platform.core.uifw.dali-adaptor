#ifndef __DALI_INTERNAL_ADAPTOR_RESOURCE_TRACKING_MESSAGE_H__
#define __DALI_INTERNAL_ADAPTOR_RESOURCE_TRACKING_MESSAGE_H__

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

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

/**
 * ResourceTrackingMessage
 */
class ResourceTrackingMessage
{
public:

  /**
   * @brief Default constructor, creates an empty message
   */
  ResourceTrackingMessage();

  /**
   * @brief Constructor
   *
   * @param message message type ( see enum )
   * @param ptr address that message references
   * @param parameter1
   * @param parameter2
   * @param parameter3
   * @param parameter4
   */
  ResourceTrackingMessage(  const ResourceTrackMessage::Type message,
                            const void* ptr,
                            const unsigned int parameter1,
                            const unsigned int parameter2,
                            const unsigned int parameter3,
                            const unsigned int parameter4 );

  ~ResourceTrackingMessage();

  /**
   * @brief Get the message type of this message
   * @return message
   */
  const ResourceTrackMessage::Type GetMessage() const
  {
    return mMessage;
  }

  /**
   * @brief Get the address referred to by this message
   * @return address
   */
  const void* GetPointer() const
  {
    return mPtr;
  }

  /**
   * @brief Get parameter from parameter array
   *
   * @param parameter index to fetch
   * @return value of parameter
   */
  const unsigned int GetParameter( const unsigned int parameter ) const
  {
    DALI_ASSERT_ALWAYS( parameter < 4 );
    return mParameters[ parameter ];
  }

  /**
   * @brief Mark a message as clear
   */
  void ClearMessage()
  {
    mMessage = ResourceTrackMessage::NONE;
  }

private:

  ResourceTrackMessage::Type mMessage;   // Message type ( see enum )
  const void* mPtr;                         // Address referred to by message
  unsigned int mParameters[ 4 ];            // Arbitrary parameters that can be used in message
};


} // namespace Internal

} // namespace Adaptor

} // namespace Dali


#endif // __DALI_INTERNAL_ADAPTOR_RESOURCE_TRACKING_H__