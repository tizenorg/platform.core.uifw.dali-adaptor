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

namespace TextAbstraction
{

namespace Internal
{

Shaping::Shaping()
: mPlugin( NULL )
{

}

Shaping::~Shaping()
{

}

TextAbstraction::Shaping Shaping::Get()
{
  TextAbstraction::Shaping shapingHandle;

  SingletonService service( SingletonService::Get() );
  if( service )
  {
    // Check whether the singleton is already created
    Dali::BaseHandle handle = service.GetSingleton( typeid( TextAbstraction::Shaping ) );
    if( handle )
    {
      // If so, downcast the handle
      Shaping* impl = dynamic_cast< Internal::Shaping* >( handle.GetObjectPtr() );
      shapingHandle = TextAbstraction::Shaping( impl );
    }
    else // create and register the object
    {
      shapingHandle = TextAbstraction::Shaping( new Shaping );
      service.Register( typeid( shapingHandle ), shapingHandle );
    }
  }

  return shapingHandle;
}

Length Shaping::Shape( const Character* const text,
                       Length numberOfCharacters,
                       FontId fontId,
                       Script script )
{
  return 0u;
}

void Shaping::GetGlyphs( GlyphInfo* glyphInfo,
                         CharacterIndex* glyphToCharacterMap )
{
}

} // namespace Internal

} // namespace TextAbstraction

} // namespace Dali
