#ifndef __DALI_TEXT_ABSTRACTION_TEXT_SEGMENTATION_INTERFACE_H__
#define __DALI_TEXT_ABSTRACTION_TEXT_SEGMENTATION_INTERFACE_H__

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

// EXTERNAL INCLUDES
#include <string>
#include <dali/public-api/common/vector-wrapper.h>

namespace Dali
{

namespace Platform
{


class TextSegmentationInterface
{

public:

  /**
   * Retrieves the line break positions.
   *
   * @param[in] text text coded in utf32.
   * @param[in] startIndex Start index of the text to be shaped.
   * @param[in] count number of characters to be shaped
   * @param[out] positions Vector with possible indices where to break the paragraph in lines ( must be at least count long).
   */
  virtual void GetLineBreakPositions(  const uint32_t* const text,
                                       uint32_t startCharacter,
                                       uint32_t count,
                                       uint32_t* positions) = 0;



private:

  // Undefined copy constructor.
  TextSegmentationInterface( const TextSegmentationInterface& );

  // Undefined assignment operator.
  TextSegmentationInterface& operator=( const TextSegmentationInterface& );
};

} // namespace TextAbstraction

} // namespace Dali

#endif // __DALI_TEXT_ABSTRACTION_TEXT_SEGMENTATION_INTERFACE_H__
