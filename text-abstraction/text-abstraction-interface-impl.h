#ifndef __DALI_INTERNAL_TEXT_ABSTRACTION_INTERFACE_IMPL_H__
#define __DALI_INTERNAL_TEXT_ABSTRACTION_INTERFACE_IMPL_H__

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
#include "text-abstraction-interface.h"
#include <dali/public-api/object/base-object.h>



namespace Dali
{

namespace Internal
{

namespace TextAbstraction
{

/**
 * Implementation of the TextAbstractionInterface
 */

class TextAbstractionInterface :  public Dali::BaseObject
{
public:

  /**
   * Constructor
   */
  TextAbstractionInterface();

  /**
   * Destructor
   */
  ~TextAbstractionInterface();

  /**
   * @copydoc Dali::TextAbstraction::Get()
   */
  static Dali::TextAbstraction::TextAbstractionInterface Get();

private:

  TextAbstractionInterface( const TextAbstractionInterface& );
  TextAbstractionInterface& operator=( TextAbstractionInterface& );

}; // class TextAbstractionInterface


} // namespace TextAbstraction

} // namespace Internal

inline static Internal::TextAbstraction::TextAbstractionInterface& GetImplementation(Dali::TextAbstraction::TextAbstractionInterface& textAbstractionInterface)
{
  DALI_ASSERT_ALWAYS( textAbstractionInterface && "text abstraction interface handle is empty" );
  BaseObject& handle = textAbstractionInterface.GetBaseObject();
  return static_cast<Internal::TextAbstraction::TextAbstractionInterface&>(handle);
}

inline static const  Internal::TextAbstraction::TextAbstractionInterface& GetImplementation(const Dali::TextAbstraction::TextAbstractionInterface& textAbstractionInterface)
{
  DALI_ASSERT_ALWAYS( textAbstractionInterface && "text abstraction interface handle is empty" );
  const BaseObject& handle = textAbstractionInterface.GetBaseObject();
  return static_cast<const Internal::TextAbstraction::TextAbstractionInterface&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_TEXT_ABSTRACTION_INTERFACE_IMPL_H__
