#ifndef __DALI_TEXT_ABSTRACTION_SEGMENTATION_INTERFACE_H__
#define __DALI_TEXT_ABSTRACTION_SEGMENTATION_INTERFACE_H__

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

// INTERNAL INCLUDES
#include <public-api/text-abstraction/text-type-definitions.h>

namespace Dali
{

namespace TextAbstraction
{

/**
 *
 */
class SegmentationInterface
{

public:

  /**
   * Retrieves the line break info.
   *
   * @param[in] text Pointer to the first character of the text coded in UTF32.
   * @param[in] numberOfCharacters The number of characters.
   * @param[out] breakInfo The line break info.
   */
  virtual void GetLineBreakPositions( const Character* const text,
                                      Length numberOfCharacters,
                                      LineBreakInfo* breakInfo ) = 0;


  /**
   * Retrieves the word break info.
   *
   * @param[in] text Pointer to the first character of the text coded in UTF32.
   * @param[in] numberOfCharacters The number of characters.
   * @param[out] breakInfo The word break info.
   */
  virtual void GetWordBreakPositions( const Character* const text,
                                      Length numberOfCharacters,
                                      WordBreakInfo* breakInfo ) = 0;

protected:

  // constructor
  SegmentationInterface() {};

  // virtual destructor
  virtual ~SegmentationInterface() {};

private:

  // Undefined copy constructor.
  SegmentationInterface( const SegmentationInterface& );

  // Undefined assignment operator.
  SegmentationInterface& operator=( const SegmentationInterface& );
};

} // namespace TextAbstraction

} // namespace Dali

#endif // __DALI_TEXT_ABSTRACTION_SEGMENTATION_INTERFACE_H__
