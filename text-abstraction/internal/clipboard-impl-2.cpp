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

// CLASS  HEADER
#include "clipboard-impl-2.h"

// INTERNAL INCLUDES
#include <singleton-service-impl.h>

namespace Dali
{

namespace Internal
{

namespace TextAbstraction
{


Clipboard::Clipboard()
:mPlugin(NULL)
{

}

Clipboard::~Clipboard()
{

}

Dali::TextAbstraction::Clipboard Clipboard::Get()
{
  Dali::TextAbstraction::Clipboard interface;

  Dali::SingletonService service( SingletonService::Get() );
  if ( service )
  {
     // Check whether the singleton is already created
     Dali::BaseHandle handle = service.GetSingleton( typeid( Dali::TextAbstraction::Clipboard ) );
     if(handle)
     {
       // If so, downcast the handle
       Clipboard* impl = dynamic_cast< Dali::Internal::TextAbstraction::Clipboard* >( handle.GetObjectPtr() );
       interface = Dali::TextAbstraction::Clipboard( impl );
     }
     else // create and register the object
     {
       interface = Dali::TextAbstraction::Clipboard( new Clipboard);
       service.Register( typeid( interface ), interface );
     }
   }

   return interface;
}

 bool Clipboard::SetItem( const std::string& itemData )
 {
   // plugin init
   // plugin -> SetItem();
   return true;
 }

std::string Clipboard::GetItem( unsigned int index )
{
  // plugin init
  // plugin -> GetItem();
  return "no item";
}

unsigned int Clipboard::NumberOfItems()
{
  // plugin init
  // plugin -> NumberOfItems();
  return 0;
}

void Clipboard::ShowClipboard()
{
  // plugin init
  // plugin -> ShowClipboard();
}

void Clipboard::HideClipboard()
{
  // plugin init
  // plugin -> HideClipboard();
}

} // namespace Clipboard
} // namespace Internal
} // namespace Dali
