#ifndef __DALI_TEXT_ABSTRACTION_REORDERING_INTERFACE_H__
#define __DALI_TEXT_ABSTRACTION_REORDERING_INTERFACE_H__

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



namespace Dali
{

namespace TextAbstraction
{

/**
 *Bidirectional Text Support
 */
class ReorderingInterface
{

public:


  /**
   * Creates bidirectional data for the whole paragraph.
   *
   * @param[in] paragraph The paragraph's text coded in utf32.
   *
   * @return An index to a table storing the bidirectional data.
   */
  virtual uint32_t CreateBidirectionalInfo( const uint32_t* const text,
                                            uint32_t startCharacter,
                                            uint32_t characterCount, ) = 0;

  /**
   * Reorders a line of a paragraph.
   *
   * @param[in] bidiInfoIndex The index to the table storing the bidirectional data for current paragraph.
   * @param[in] firstCharacterIndex The first character of the line within the whole paragraph.
   * @param[in] numberOfCharacters The number of characters of the line.
   * @param[out] visualToLogicalMap The visual to logical conversion map.
   */
  virtual void Reorder( uint32_t bidiInfoIndex,
                uint32_t startCharacter,
                uint32_t numberOfCharacters,
                uint32_t* visualToLogicalMap ) = 0;

protected:

  // constructor
  ReorderingInterface() {};

  // virtual destructor
  virtual ~ReorderingInterface() {};

private:

  // Undefined copy constructor.
  ReorderingInterface( const TextReorderingInterface& );

  // Undefined assignment operator.
  ReorderingInterface& operator=( const TextReorderingInterface& );
};

} // namespace TextAbstraction

} // namespace Dali

#endif // __DALI_TEXT_ABSTRACTION_REORDERING_INTERFACE_H__
