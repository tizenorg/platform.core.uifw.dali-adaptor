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
#include "font-client-impl.h"

// INTERNAL INCLUDES
#include <singleton-service-impl.h>

namespace Dali
{

namespace TextAbstraction
{

namespace Internal
{

struct FontClient::Impl
{
  FontId GetFontId( const std::string& path, FaceIndex face )
  {
    return 0; // TODO
  }
};

FontClient::FontClient()
: mImpl( NULL )
{
}

FontClient::~FontClient()
{
}

Dali::TextAbstraction::FontClient FontClient::Get()
{
  Dali::TextAbstraction::FontClient fontClientHandle;

  Dali::SingletonService service( SingletonService::Get() );
  if ( service )
  {
    // Check whether the singleton is already created
    Dali::BaseHandle handle = service.GetSingleton( typeid( Dali::TextAbstraction::FontClient ) );
    if(handle)
    {
      // If so, downcast the handle
      FontClient* impl = dynamic_cast< Dali::TextAbstraction::Internal::FontClient* >( handle.GetObjectPtr() );
      fontClientHandle = Dali::TextAbstraction::FontClient( impl );
    }
    else // create and register the object
    {
      fontClientHandle = Dali::TextAbstraction::FontClient( new FontClient );
      service.Register( typeid( fontClientHandle ), fontClientHandle );
    }
  }

  return fontClientHandle;
}

FontId FontClient::GetFontId( const std::string& path, FaceIndex face )
{
  CreateImpl();

  return mImpl->GetFontId( path, face );
}

void FontClient::CreateImpl()
{
  if( !mImpl )
  {
    mImpl = new Impl();
  }
}

} // namespace Internal

} // namespace FontClient

} // namespace Dali
