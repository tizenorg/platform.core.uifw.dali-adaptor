#ifndef __DALI_INTERNAL_TEXT_ABSTRACTION_BIDIRECTIONAL_SUPPORT_IMPL_H__
#define __DALI_INTERNAL_TEXT_ABSTRACTION_BIDIRECTIONAL_SUPPORT_IMPL_H__

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
#include <dali/public-api/object/base-object.h>

// INTERNAL INCLUDES
#include <dali/public-api/text-abstraction/bidirectional-support.h>

namespace Dali
{

namespace TextAbstraction
{

namespace Internal
{

/**
 * Implementation of the BidirectionalSupport
 */
class BidirectionalSupport : public Dali::BaseObject
{
public:

  /**
   * Constructor
   */
  BidirectionalSupport();

  /**
   * Destructor
   */
  ~BidirectionalSupport();

  /**
   * @copydoc Dali::BidirectionalSupport::Get()
   */
  static TextAbstraction::BidirectionalSupport Get();

  /**
   * @copydoc Dali::BidirectionalSupport::CreateInfo()
   */
  BidiInfoIndex CreateInfo( const Character* const paragraph,
                            Length numberOfCharacters );

  /**
   * @copydoc Dali::BidirectionalSupport::DestroyInfo()
   */
  void DestroyInfo( BidiInfoIndex bidiInfoIndex );

  /**
   * @copydoc Dali::BidirectionalSupport::Reorder()
   */
  void Reorder( BidiInfoIndex bidiInfoIndex,
                CharacterIndex firstCharacterIndex,
                Length numberOfCharacters,
                CharacterIndex* visualToLogicalMap );

private:

  // Undefined copy constructor.
  BidirectionalSupport( const BidirectionalSupport& );

  // Undefined assignment constructor.
  BidirectionalSupport& operator=( BidirectionalSupport& );

  void* mPlugin; ///< TODO replace this with bidirectional support plugin

private:

  /**
   * Stores bidirectional info per paragraph.
   */
  struct BidirectionalInfo;

  Vector<BidirectionalInfo*> mParagraphBidirectionalInfo; ///< Stores the bidirectional info per paragraph.
  Vector<BidiInfoIndex>      mFreeIndices;                ///< Stores indices of free positions in the bidirectional info vector.

}; // class BidirectionalSupport

} // namespace Internal

} // namespace TextAbstraction

inline static TextAbstraction::Internal::BidirectionalSupport& GetImplementation( TextAbstraction::BidirectionalSupport& bidirectionalSupport )
{
  DALI_ASSERT_ALWAYS( bidirectionalSupport && "bidirectional support handle is empty" );
  BaseObject& handle = bidirectionalSupport.GetBaseObject();
  return static_cast<TextAbstraction::Internal::BidirectionalSupport&>(handle);
}

inline static const  TextAbstraction::Internal::BidirectionalSupport& GetImplementation( const TextAbstraction::BidirectionalSupport& bidirectionalSupport )
{
  DALI_ASSERT_ALWAYS( bidirectionalSupport && "bidirectional support handle is empty" );
  const BaseObject& handle = bidirectionalSupport.GetBaseObject();
  return static_cast<const TextAbstraction::Internal::BidirectionalSupport&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_TEXT_ABSTRACTION_BIDIRECTIONAL_SUPPORT_IMPL_H__
