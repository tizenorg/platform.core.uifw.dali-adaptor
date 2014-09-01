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
#include <singleton-service-impl.h>

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include <application-impl.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

Dali::SingletonService SingletonService::New()
{
  Dali::SingletonService service = Dali::SingletonService( new SingletonService );
  return service;
}

Dali::SingletonService SingletonService::Get()
{
  Dali::SingletonService singletonService;

  if ( Application::IsAvailable() )
  {
    singletonService = GetImplementation( Application::Get() ).GetSingletonService();
  }

  return singletonService;
}

void SingletonService::Register( const std::type_info& info, BaseHandle singleton )
{
  if( singleton )
  {
    mSingletonContainer.insert(SingletonPair(info.name(), singleton));
  }
}

BaseHandle SingletonService::GetSingleton( const std::type_info& info ) const
{
  BaseHandle object;

  SingletonConstIter iter = mSingletonContainer.find(info.name());
  if( iter != mSingletonContainer.end() )
  {
    object = ( *iter ).second;
  }

  return object;
}

SingletonService::SingletonService()
: mSingletonContainer()
{
}

SingletonService::~SingletonService()
{
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
