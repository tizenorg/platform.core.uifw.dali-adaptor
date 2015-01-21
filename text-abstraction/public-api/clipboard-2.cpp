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
#include "clipboard-2.h"

// INTERNAL INCLUDES
#include <internal/clipboard-impl-2.h>

namespace Dali
{

namespace TextAbstraction
{

Clipboard::Clipboard()
{
}
Clipboard::~Clipboard()
{
}
Clipboard::Clipboard(Dali::Internal::TextAbstraction::Clipboard *impl)
  : BaseHandle(impl)
{
}

Clipboard Clipboard::Get()
{
  return Dali::Internal::TextAbstraction::Clipboard::Get();
}

bool Clipboard::SetItem( const std::string& itemData)
{
  return GetImplementation(*this).SetItem( itemData );
}

std::string Clipboard::GetItem( unsigned int index )
{
  return GetImplementation(*this).GetItem( index );
}

unsigned int Clipboard::NumberOfItems()
{
  return GetImplementation(*this).NumberOfItems();
}

void Clipboard::ShowClipboard()
{
  GetImplementation(*this).ShowClipboard();
}

void Clipboard::HideClipboard()
{
  GetImplementation(*this).HideClipboard();
}

} // namespace TextAbstraction

} // namespace Dali
