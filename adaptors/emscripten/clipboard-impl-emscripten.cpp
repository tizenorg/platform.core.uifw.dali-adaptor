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
#include "clipboard-impl.h"

// EXTERNAL INCLUDES
#include <dali/public-api/dali-core.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <adaptor.h>
#include <dali/public-api/object/any.h>
#include <adaptor-impl.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
// Clipboard
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace
{
BaseHandle Create()
{
  BaseHandle handle( Clipboard::Get() );

  return handle;
}

TypeRegistration CLIPBOARD_TYPE( typeid(Dali::Clipboard), typeid(Dali::BaseHandle), Create, true /* Create Instance At Startup */ );

} // unnamed namespace

Clipboard::~Clipboard()
{
}

Dali::Clipboard Clipboard::Get()
{
  Dali::Clipboard clipboard;

  return clipboard;
}

bool Clipboard::SetItem(const std::string &itemData )
{
  return true;
}

std::string Clipboard::GetItem( unsigned int index )
{
  return std::string( "" );
}

unsigned int Clipboard::NumberOfItems()
{
  int count = -1;
  return count;
}

void Clipboard::ShowClipboard()
{
}

void Clipboard::HideClipboard()
{
}


} // namespace Adaptor

} // namespace Internal

} // namespace Dali
