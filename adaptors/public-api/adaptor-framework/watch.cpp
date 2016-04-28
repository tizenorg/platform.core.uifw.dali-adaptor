/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <watch.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <watch-impl.h>

namespace Dali
{

Watch Watch::New()
{
  return New( NULL, NULL );
}

Watch Watch::New( int* argc, char **argv[] )
{
  Internal::Adaptor::WatchPtr internal = Internal::Adaptor::Watch::New( argc, argv, "", OPAQUE );
  return Watch(internal.Get());
}

Watch Watch::New( int* argc, char **argv[], const std::string& stylesheet )
{
  Internal::Adaptor::WatchPtr internal = Internal::Adaptor::Watch::New( argc, argv, stylesheet, OPAQUE );
  return Watch(internal.Get());
}

Watch::~Watch()
{
}

Watch::Watch()
{
}

Watch::Watch(const Watch& implementation)
: Application(implementation)
{
}

Watch& Watch::operator=(const Watch& application)
{
  if( *this != application )
  {
    BaseHandle::operator=( application );
  }
  return *this;
}

Watch::WatchPtrSignal& Watch::TimeTickSignal()
{
  return Internal::Adaptor::GetImplementation(*this).mTickSignal;
}

Watch::WatchPtrSignal& Watch::AmbientTickSignal()
{
  return Internal::Adaptor::GetImplementation(*this).mAmbientTickSignal;
}

Watch::WatchBoolSignal& Watch::AmbientChangedSignal()
{
  return Internal::Adaptor::GetImplementation(*this).mAmbientChangeSignal;
}

Watch::Watch(Internal::Adaptor::Watch* implementation)
: Application(implementation)
{
}


} // namespace Dali
