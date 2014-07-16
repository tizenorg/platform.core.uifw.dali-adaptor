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
#include <imf-manager-impl.h>

// EXTERNAL INCLUDES
#include <boost/bind.hpp>
#include <dali/public-api/events/key-event.h>
#include <dali/public-api/object/type-registry.h>
#include <adaptor.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <window-render-surface.h>
#include <adaptor-impl.h>
#include <singleton-service-impl.h>
#include <virtual-keyboard-impl.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace
{

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_IMF_MANAGER");
#endif

BaseHandle Create()
{
  BaseHandle handle( ImfManager::Get() );

  Dali::SingletonService service( SingletonService::Get() );
  if ( !handle && Adaptor::IsAvailable() && service )
  {
    Dali::ImfManager manager = Dali::ImfManager( new ImfManager( ) );
    service.Register( typeid( manager ), manager );
    handle = manager;
  }

  return handle;
}

TypeRegistration IMF_MANAGER_TYPE( typeid(Dali::ImfManager), typeid(Dali::BaseHandle), Create, true /* Create Instance At Startup */ );

} // unnamed namespace

Dali::ImfManager ImfManager::Get()
{
  Dali::ImfManager manager;

  Dali::SingletonService service( SingletonService::Get() );
  if ( service )
  {
    // Check whether the singleton is already created
    Dali::BaseHandle handle = service.GetSingleton( typeid( Dali::ImfManager ) );
    if(handle)
    {
      // If so, downcast the handle
      manager = Dali::ImfManager( dynamic_cast< ImfManager* >( handle.GetObjectPtr() ) );
    }
  }

  return manager;
}

ImfManager::ImfManager( )
: mIMFCursorPosition( 0 ),
  mSurroundingText(""),
  mRestoreAfterFocusLost( false )
{
}

ImfManager::~ImfManager()
{
}


void ImfManager::Activate()
{
}

void ImfManager::Deactivate()
{
}

void ImfManager::Reset()
{
}

ImfContext* ImfManager::GetContext()
{
  return NULL;
}

bool ImfManager::RestoreAfterFocusLost() const
{
  return mRestoreAfterFocusLost;
}

void ImfManager::SetRestoreAferFocusLost( bool toggle )
{
  mRestoreAfterFocusLost = toggle;
}


void ImfManager::NotifyCursorPosition()
{
}

int ImfManager::GetCursorPosition()
{
  return mIMFCursorPosition;
}

void ImfManager::SetCursorPosition( unsigned int cursorPosition )
{
  mIMFCursorPosition = ( int )cursorPosition;
}

void ImfManager::SetSurroundingText( std::string text )
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "ImfManager::SetSurroundingText\n" );

  mSurroundingText = text;
}

std::string ImfManager::GetSurroundingText()
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "ImfManager::GetSurroundingText\n" );

  return mSurroundingText;
}

} // Adaptor

} // Internal

} // Dali
