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

// CLASS HEADER
#include <dali/public-api/text-abstraction/reordering.h>

// INTERNAL INCLUDES
#include <dali/internal/text-abstraction/reordering-impl.h>

namespace Dali
{

namespace TextAbstraction
{

Reordering::Reordering()
{
}
Reordering::~Reordering()
{
}
Reordering::Reordering(Dali::Internal::TextAbstraction::Reordering *impl)
  : BaseHandle(impl)
{
}

Reordering Reordering::Get()
{
  return Dali::Internal::TextAbstraction::Reordering::Get();
}

Index Reordering::CreateBidirectionalInfo( const Character* const paragraph,
                                           Length numberOfCharacters )
{
  return 0u;
}

void Reordering::DestroyBidirectionalInfo( Index bidiInfoIndex )
{
}

void Reordering::Reorder( Index bidiInfoIndex,
                          Index firstCharacterIndex,
                          Length numberOfCharacters,
                          Index* visualToLogicalMap )
{
}

} // namespace TextAbstraction

} // namespace Dali
