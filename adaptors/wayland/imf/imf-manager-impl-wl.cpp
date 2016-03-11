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

// CLASS HEADER
#include <imf-manager-impl.h>

// EXTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>
#include <singleton-service-impl.h>


namespace Dali
{

namespace Internal
{

namespace Adaptor
{

bool ImfManager::IsAvailable()
{
  return false;
}

Dali::ImfManager ImfManager::Get()
{
  Dali::ImfManager manager;

  return manager;
/*  Dali::SingletonService service( SingletonService::Get() );
  if( !service )
  {
    return manager;
  }

  // Check whether the singleton is already created
  Dali::BaseHandle handle = service.GetSingleton( typeid( Dali::ImfManager ) );
  if( handle )
  {
    // If so, downcast the handle
    manager = Dali::ImfManager( dynamic_cast< ImfManager* >( handle.GetObjectPtr() ) );
  }
  else if ( Adaptor::IsAvailable() )
  {
       // Create instance and register singleton only if the adaptor is available
       manager = Dali::ImfManager( new ImfManager() );
       service.Register( typeid( manager ), manager );
   }

   return manager;*/
}

ImfManager::ImfManager()
{

}

ImfManager::~ImfManager()
{
}

/*
void ImfManager::ConnectCallbacks()
{
  // Callbacks for predicitive text support.
 TextInputManager& textinputManager( TextInputManager::Get() );

  DALI_LOG_INFO( gLogFilter, Debug::General, "ImfManager::ConnectCallbacks\n" );

  textinputManager.t`



  void ImfManager::ConnectCallbacks()
  {

  if ( mIMFContext )
  {
      DALI_LOG_INFO( gLogFilter, Debug::General, "ImfManager::ConnectCallbacks\n" );

      ecore_imf_context_event_callback_add( mIMFContext, ECORE_IMF_CALLBACK_PREEDIT_CHANGED,    PreEdit,    this );
      ecore_imf_context_event_callback_add( mIMFContext, ECORE_IMF_CALLBACK_COMMIT,             Commit,     this );
      ecore_imf_context_event_callback_add( mIMFContext, ECORE_IMF_CALLBACK_DELETE_SURROUNDING, ImfDeleteSurrounding, this );

      ecore_imf_context_retrieve_surrounding_callback_set( mIMFContext, ImfRetrieveSurrounding, this);
    }
  }

  void ImfManager::DisconnectCallbacks()
  {
    if ( mIMFContext )
    {
      DALI_LOG_INFO( gLogFilter, Debug::General, "ImfManager::DisconnectCallbacks\n" );

      ecore_imf_context_event_callback_del( mIMFContext, ECORE_IMF_CALLBACK_PREEDIT_CHANGED,    PreEdit );
      ecore_imf_context_event_callback_del( mIMFContext, ECORE_IMF_CALLBACK_COMMIT,             Commit );
      ecore_imf_context_event_callback_del( mIMFContext, ECORE_IMF_CALLBACK_DELETE_SURROUNDING, ImfDeleteSurrounding );

      // We do not need to unset the retrieve surrounding callback.
    }


}

void ImfManager::DisconnectCallbacks()
{
}
 }*/
void ImfManager::Activate()
{
}

void ImfManager::Deactivate()
{
}

void ImfManager::Reset()
{
}

void* ImfManager::GetContext()
{
  return NULL;
}

bool ImfManager::RestoreAfterFocusLost() const
{
  return false;
}

void ImfManager::SetRestoreAfterFocusLost( bool toggle )
{
}

void ImfManager::NotifyCursorPosition()
{
}
unsigned int ImfManager::GetCursorPosition() const
{
  return 0;
}

void ImfManager::SetCursorPosition( unsigned int cursorPosition )
{
}

void ImfManager::SetSurroundingText(  const std::string& text )
{
}

const std::string& ImfManager::GetSurroundingText() const
{
  static std::string dummyString("");
  return dummyString;
}

} // Adaptor

} // Internal

} // Dali
