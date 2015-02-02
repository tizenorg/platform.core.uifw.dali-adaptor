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
#include "shaping-impl.h"

// INTERNAL INCLUDES
#include <singleton-service-impl.h>

namespace Dali
{

namespace Internal
{

namespace TextAbstraction
{

Shaping::Shaping()
: mPlugin( NULL )
{

}

Shaping::~Shaping()
{

}

Dali::TextAbstraction::Shaping Shaping::Get()
{
  Dali::TextAbstraction::Shaping shapingHandle;

  Dali::SingletonService service( SingletonService::Get() );
  if ( service )
  {
     // Check whether the singleton is already created
     Dali::BaseHandle handle = service.GetSingleton( typeid( Dali::TextAbstraction::Shaping ) );
     if(handle)
     {
       // If so, downcast the handle
       Shaping* impl = dynamic_cast< Dali::Internal::TextAbstraction::Shaping* >( handle.GetObjectPtr() );
       shapingHandle = Dali::TextAbstraction::Shaping( impl );
     }
     else // create and register the object
     {
       shapingHandle = Dali::TextAbstraction::Shaping( new Shaping);
       service.Register( typeid( shapingHandle ), shapingHandle );
     }
   }

   return shapingHandle;
}

Dali::TextAbstraction::Length Shaping::Shape( const Dali::TextAbstraction::Character* const text,
                                              Dali::TextAbstraction::Length numberOfCharacters,
                                              Dali::TextAbstraction::FontId fontId,
                                              Dali::TextAbstraction::Script scriptId )
{
  return 0u;
}

void Shaping::GetGlyphs( Dali::TextAbstraction::GlyphInfo* glyphInfo,
                         Dali::TextAbstraction::CharacterIndex* glyphToCharacterMap )
{
}

} // namespace TextAbstraction

} // namespace Internal

} // namespace Dali
