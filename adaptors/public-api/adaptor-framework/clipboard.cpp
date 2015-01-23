/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

const char* const Clipboard::SIGNAL_PASTE_DATA( "clipboard-data-selected" );

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
void Clipboard::Copy( MediaDataType& data )
{
  Internal::Adaptor::Clipboard::GetImplementation(*this).Copy( data );
}

void Clipboard::Paste()
{
  Internal::Adaptor::Clipboard::GetImplementation(*this).Paste();
}

void Clipboard::PasteWithUI()
{
  Internal::Adaptor::Clipboard::GetImplementation(*this).PasteWithUI();
}
bool Clipboard::IsEmpty()
{
  return Internal::Adaptor::Clipboard::GetImplementation(*this).IsEmpty();
}

Clipboard::ClipboardPasteDataSignalType& Clipboard::PasteDataSignal()
{
  return Internal::Adaptor::Clipboard::GetImplementation(*this).PasteDataSignal();
}

} // namespace Dali
