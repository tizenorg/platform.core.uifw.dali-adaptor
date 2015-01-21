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
#include "text-abstraction-interface-impl.h"

// INTERNAL INCLUDES
#include <singleton-service-impl.h>
#include <dali/public-api/object/type-registry.h>

namespace Dali
{

namespace Internal
{

namespace TextAbstraction
{


TextAbstractionInterface::TextAbstractionInterface()
{

}

TextAbstractionInterface::~TextAbstractionInterface()
{

}

Dali::TextAbstraction::TextAbstractionInterface TextAbstractionInterface::Get()
{
  Dali::TextAbstraction::TextAbstractionInterface interface;

  Dali::SingletonService service( SingletonService::Get() );
  if ( service )
  {
     // Check whether the singleton is already created
     Dali::BaseHandle handle = service.GetSingleton( typeid( Dali::TextAbstraction::TextAbstractionInterface ) );
     if(handle)
     {
       // If so, downcast the handle
       TextAbstractionInterface* impl = dynamic_cast< Dali::Internal::TextAbstraction::TextAbstractionInterface* >( handle.GetObjectPtr() );
       interface = Dali::TextAbstraction::TextAbstractionInterface( impl );
     }
     else // create and register the object
     {
       interface = Dali::TextAbstraction::TextAbstractionInterface( new TextAbstractionInterface);
       service.Register( typeid( interface ), interface );
     }
   }

   return interface;
}

} // namespace TextAbstractionInterface
} // namespace Internal
} // namespace Dali
