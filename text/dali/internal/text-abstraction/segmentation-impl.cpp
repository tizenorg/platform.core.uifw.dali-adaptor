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
#include "segmentation-impl.h"

// INTERNAL INCLUDES
#include <singleton-service-impl.h>

namespace Dali
{

namespace Internal
{

namespace TextAbstraction
{

Segmentation::Segmentation()
: mPlugin( NULL )
{

}

Segmentation::~Segmentation()
{

}

Dali::TextAbstraction::Segmentation Segmentation::Get()
{
  Dali::TextAbstraction::Segmentation segmentationHandle;

  Dali::SingletonService service( SingletonService::Get() );
  if ( service )
  {
     // Check whether the singleton is already created
     Dali::BaseHandle handle = service.GetSingleton( typeid( Dali::TextAbstraction::Segmentation ) );
     if(handle)
     {
       // If so, downcast the handle
       Segmentation* impl = dynamic_cast< Dali::Internal::TextAbstraction::Segmentation* >( handle.GetObjectPtr() );
       segmentationHandle = Dali::TextAbstraction::Segmentation( impl );
     }
     else // create and register the object
     {
       segmentationHandle = Dali::TextAbstraction::Segmentation( new Segmentation);
       service.Register( typeid( segmentationHandle ), segmentationHandle );
     }
   }

   return segmentationHandle;
}

void Segmentation::GetLineBreakPositions( const Dali::TextAbstraction::Character* const text,
                                          Dali::TextAbstraction::Length numberOfCharacters,
                                          Dali::TextAbstraction::LineBreakInfo* breakInfo )
{
}

void Segmentation::GetWordBreakPositions( const Dali::TextAbstraction::Character* const text,
                                          Dali::TextAbstraction::Length numberOfCharacters,
                                          Dali::TextAbstraction::WordBreakInfo* breakInfo )
{
}

} // namespace TextAbstraction

} // namespace Internal

} // namespace Dali
