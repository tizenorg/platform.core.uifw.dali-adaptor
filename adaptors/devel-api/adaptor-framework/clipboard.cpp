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
#include <clipboard.h>

// INTERNAL INCLUDES
#include <clipboard-impl.h>

namespace Dali
{

Clipboard::Clipboard()
{
}
Clipboard::~Clipboard()
{
}
Clipboard::Clipboard(Internal::Adaptor::Clipboard *impl)
  : BaseHandle(impl)
{
}

Clipboard Clipboard::Get()
{
  return Internal::Adaptor::Clipboard::Get();
}

bool Clipboard::SetItem( const std::string &itemData)
{
  return Internal::Adaptor::Clipboard::GetImplementation(*this).SetItem( itemData );
}

std::string Clipboard::GetItem( unsigned int index )
{
  return Internal::Adaptor::Clipboard::GetImplementation(*this).GetItem( index );
}

unsigned int Clipboard::NumberOfItems()
{
  return Internal::Adaptor::Clipboard::GetImplementation(*this).NumberOfItems();
}

void Clipboard::ShowClipboard()
{
  Internal::Adaptor::Clipboard::GetImplementation(*this).ShowClipboard();
}

void Clipboard::HideClipboard()
{
  Internal::Adaptor::Clipboard::GetImplementation(*this).HideClipboard(false);
}

bool Clipboard::IsVisible()
{
  return Internal::Adaptor::Clipboard::GetImplementation(*this).IsVisible();
}

char *Clipboard::ExcuteBuffered( int type, void *event )
{
  return Internal::Adaptor::Clipboard::GetImplementation(*this).ExcuteBuffered(type, event);
}

} // namespace Dali
