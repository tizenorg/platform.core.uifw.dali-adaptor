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

// Currently this code is internal to dali/dali/internal/event/text/utf8.h but should be made Public and used from there instead.
size_t Utf8SequenceLength(const unsigned char leadByte)
{
  size_t length = 0;

  if ((leadByte & 0x80) == 0 )          //ASCII character (lead bit zero)
  {
    length = 1;
  }
  else if (( leadByte & 0xe0 ) == 0xc0 ) //110x xxxx
  {
    length = 2;
  }
  else if (( leadByte & 0xf0 ) == 0xe0 ) //1110 xxxx
  {
    length = 3;
  }
  else if (( leadByte & 0xf8 ) == 0xf0 ) //1111 0xxx
  {
    length = 4;
  }

  return length;
}

// Static function calls used by ecore 'c' style callback registration
void Commit( void *data, Ecore_IMF_Context *imfContext, void *event_info )
{
  if ( data )
  {
    ImfManager* imfManager = reinterpret_cast< ImfManager* > ( data );
    imfManager->CommitReceived( data, imfContext, event_info );
  }
}

void PreEdit( void *data, Ecore_IMF_Context *imfContext, void *event_info )
{
  if ( data )
  {
    ImfManager* imfManager = reinterpret_cast< ImfManager* > ( data );
    imfManager->PreEditChanged( data, imfContext, event_info );
  }
}

Eina_Bool ImfRetrieveSurrounding(void *data, Ecore_IMF_Context *imfContext, char** text, int* cursorPosition )
{
  if ( data )
  {
    ImfManager* imfManager = reinterpret_cast< ImfManager* > ( data );
    return imfManager->RetrieveSurrounding( data, imfContext, text, cursorPosition );
  }
  else
  {
    return false;
  }
}

/**
 * Called when an IMF delete surrounding event is received.
 * Here we tell the application that it should delete a certain range.
 */
void ImfDeleteSurrounding( void *data, Ecore_IMF_Context *imfContext, void *event_info )
{
  if ( data )
  {
    ImfManager* imfManager = reinterpret_cast< ImfManager* > ( data );
    imfManager->DeleteSurrounding( data, imfContext, event_info );
  }
}

BaseHandle Create()
{
  BaseHandle handle( ImfManager::Get() );

  if ( !handle && Adaptor::IsAvailable() )
  {
    Adaptor& adaptorImpl( Adaptor::GetImplementation( Adaptor::Get() ) );

    // The Ecore_X_Window needs to use the ImfManager.
    // Only when the render surface is window, we can get the Ecore_X_Window.
    Ecore_X_Window ecoreXwin( 0 );
    Dali::RenderSurface& surface( adaptorImpl.GetSurface() );
    if( surface.GetType() == Dali::RenderSurface::WINDOW )
    {
      ecoreXwin = AnyCast< Ecore_X_Window >( adaptorImpl.GetSurface().GetSurface() );
    }

    // If we fail to get Ecore_X_Window, we can't use the ImfManager correctly.
    // Thus you have to call "ecore_imf_context_client_window_set" somewhere.
    // In EvasPlugIn, this function is called in EvasPlugin::ConnectEcoreEvent().

    Dali::ImfManager manager = Dali::ImfManager( new ImfManager( ecoreXwin ) );
    adaptorImpl.RegisterSingleton( typeid( manager ), manager );
    handle = manager;
  }

  return handle;
}

TypeRegistration IMF_MANAGER_TYPE( typeid(Dali::ImfManager), typeid(Dali::BaseHandle), Create, true /* Create Instance At Startup */ );

} // unnamed namespace

Dali::ImfManager ImfManager::Get()
{
  Dali::ImfManager manager;

  if ( Adaptor::IsAvailable() )
  {
    // Check whether the singleton is already created
    Dali::BaseHandle handle = Dali::Adaptor::Get().GetSingleton( typeid( Dali::ImfManager ) );
    if(handle)
    {
      // If so, downcast the handle
      manager = Dali::ImfManager( dynamic_cast< ImfManager* >( handle.GetObjectPtr() ) );
    }
  }

  return manager;
}

ImfManager::ImfManager( Ecore_X_Window ecoreXwin )
ImfManager::ImfManager( Ecore_X_Window ecoreXwin )
: mIMFContext(),
  mIMFCursorPosition( 0 ),
  mSurroundingText(""),
  mRestoreAfterFocusLost( false ),
  mIdleCallbackConnected( false ),
  mKeyEvents()
{
  ecore_imf_init();
  CreateContext( ecoreXwin );

  ConnectCallbacks();
  VirtualKeyboard::ConnectCallbacks( mIMFContext );
}

ImfManager::~ImfManager()
{
  VirtualKeyboard::DisconnectCallbacks( mIMFContext );
  DisconnectCallbacks();

  DeleteContext();
  ecore_imf_shutdown();
}

ImfManager::ImfManager( Ecore_X_Window ecoreXwin )
void ImfManager::CreateContext( Ecore_X_Window ecoreXwin )
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "ImfManager::CreateContext\n" );

  const char *contextId = ecore_imf_context_default_id_get();
  if( contextId )
  {
    mIMFContext = ecore_imf_context_add( contextId );

    if( mIMFContext )
    {
      if( ecoreXwin )
      {
        ecore_imf_context_client_window_set( mIMFContext, reinterpret_cast<void*>( ecoreXwin ) );
      }
    }
    else
    {
      DALI_LOG_WARNING("IMF Unable to get IMF Context\n");
    }
  }
  else
  {
    DALI_LOG_WARNING("IMF Unable to get IMF Context\n");
  }
}

void ImfManager::DeleteContext()
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "ImfManager::DeleteContext\n" );

  if ( mIMFContext )
  {
    mIMFContext = NULL;
  }
}

// Callbacks for predicitive text support.
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

void ImfManager::Activate()
{
  // Reset mIdleCallbackConnected
  mIdleCallbackConnected = false;

  if ( mIMFContext )
  {
    DALI_LOG_INFO( gLogFilter, Debug::General, "ImfManager::Activate\n" );

    ecore_imf_context_focus_in( mIMFContext );

    // emit keyboard activated signal
    Dali::ImfManager handle( this );
    mActivatedSignalV2.Emit( handle );
  }
}

void ImfManager::Deactivate()
{
  if( mIMFContext )
  {
    DALI_LOG_INFO( gLogFilter, Debug::General, "ImfManager::Deactivate\n" );

    Reset();
    ecore_imf_context_focus_out( mIMFContext );
  }

  // Reset mIdleCallbackConnected
  mIdleCallbackConnected = false;
}

void ImfManager::Reset()
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "ImfManager::Reset\n" );

  if ( mIMFContext )
  {
    ecore_imf_context_reset( mIMFContext );
  }
}

Ecore_IMF_Context* ImfManager::GetContext()
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "ImfManager::GetContext\n" );

  return mIMFContext;
}

bool ImfManager::RestoreAfterFocusLost() const
{
  return mRestoreAfterFocusLost;
}

void ImfManager::SetRestoreAferFocusLost( bool toggle )
{
  mRestoreAfterFocusLost = toggle;
}

/**
 * Called when an IMF Pre-Edit changed event is received.
 * We are still predicting what the user is typing.  The latest string is what the IMF module thinks
 * the user wants to type.
 */
void ImfManager::PreEditChanged( void *, Ecore_IMF_Context *imfContext, void *event_info )
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "ImfManager::PreEditChanged\n" );

  char *preEditString( NULL );
  int cursorPosition( 0 );
  Eina_List *attrs = NULL;
  Eina_List *l = NULL;

  Ecore_IMF_Preedit_Attr *attr;

  // Retrieves attributes as well as the string the cursor position offset from start of pre-edit string.
  // the attributes (attrs) is used in languages that use the soft arrows keys to insert characters into a current pre-edit string.
  ecore_imf_context_preedit_string_with_attributes_get( imfContext, &preEditString, &attrs, &cursorPosition );

  if ( attrs )
  {
    // iterate through the list of attributes getting the type, start and end position.
    for ( l = attrs, (attr =  (Ecore_IMF_Preedit_Attr*)eina_list_data_get(l) ); l; l = eina_list_next(l), ( attr = (Ecore_IMF_Preedit_Attr*)eina_list_data_get(l) ))
    {
      if ( attr->preedit_type == ECORE_IMF_PREEDIT_TYPE_SUB4 ) // (Ecore_IMF)
      {
        // check first byte so know how many bytes a character is represented by as keyboard returns cursor position in bytes. Which is different for some languages.

        size_t visualCharacterIndex = 0;
        size_t byteIndex = 0;

        // iterate through null terminated string checking each character's position against the given byte position ( attr->end_index ).
        while ( preEditString[byteIndex] != '\0' )
        {
          // attr->end_index is provided as a byte position not character and we need to know the character position.
          size_t currentSequenceLength = Utf8SequenceLength(preEditString[byteIndex]); // returns number of bytes used to represent character.
          if ( byteIndex == attr->end_index )
          {
            cursorPosition = visualCharacterIndex;
            break;
            // end loop as found cursor position that matches byte position
          }
          else
          {
            byteIndex += currentSequenceLength; // jump to next character
            visualCharacterIndex++;  // increment character count so we know our position for when we get a match
          }

          DALI_ASSERT_DEBUG( visualCharacterIndex < strlen( preEditString ));
        }
      }
    }
  }

  if ( Dali::Adaptor::IsAvailable() )
  {
    std::string keyString ( preEditString );
    int numberOfChars( 0 );

    Dali::ImfManager handle( this );
    Dali::ImfManager::ImfEventData imfEventData ( Dali::ImfManager::PREEDIT, keyString, cursorPosition, numberOfChars );
    Dali::ImfManager::ImfCallbackData callbackData = mEventSignalV2.Emit( handle, imfEventData );

    if ( callbackData.update )
    {
      SetCursorPosition( callbackData.cursorPosition );
      SetSurroundingText( callbackData.currentText );

      NotifyCursorPosition();
    }

    if ( callbackData.preeditResetRequired )
    {
      Reset();
    }
  }
  free( preEditString );
}

void ImfManager::CommitReceived( void *, Ecore_IMF_Context *imfContext, void *event_info )
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "ImfManager::CommitReceived\n" );

  if ( Dali::Adaptor::IsAvailable() )
  {
    const std::string keyString( (char *)event_info );
    const int cursorOffset( 0 );
    const int numberOfChars( 0 );

    Dali::ImfManager handle( this );
    Dali::ImfManager::ImfEventData imfEventData ( Dali::ImfManager::COMMIT, keyString, cursorOffset, numberOfChars );
    Dali::ImfManager::ImfCallbackData callbackData = mEventSignalV2.Emit( handle, imfEventData );

    if ( callbackData.update )
    {
      SetCursorPosition( callbackData.cursorPosition );
      SetSurroundingText( callbackData.currentText );

      NotifyCursorPosition();
    }
  }
}

/**
 * Called when an IMF retrieve surround event is received.
 * Here the IMF module wishes to know the string we are working with and where within the string the cursor is
 * We need to signal the application to tell us this information.
 */
Eina_Bool ImfManager::RetrieveSurrounding( void *data, Ecore_IMF_Context *imfContext, char** text, int* cursorPosition )
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "ImfManager::RetrieveSurrounding\n" );

  std::string keyString ( "" );
  int cursorOffset( 0 );
  int numberOfChars( 0 );

  Dali::ImfManager::ImfEventData imfData ( Dali::ImfManager::GETSURROUNDING , keyString, cursorOffset, numberOfChars );
  Dali::ImfManager handle( this );
  mEventSignalV2.Emit( handle, imfData );

  if ( text )
  {
    std::string surroundingText( GetSurroundingText() );

    if ( !surroundingText.empty() )
    {
      *text = strdup( surroundingText.c_str() );
    }
    else
    {
      *text = strdup( "" );
    }
  }

  if ( cursorPosition )
  {
    *cursorPosition = GetCursorPosition();
  }


  return EINA_TRUE;
}

/**
 * Called when an IMF delete surrounding event is received.
 * Here we tell the application that it should delete a certain range.
 */
void ImfManager::DeleteSurrounding( void *data, Ecore_IMF_Context *imfContext, void *event_info )
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "ImfManager::DeleteSurrounding\n" );

  if ( Dali::Adaptor::IsAvailable() )
  {
    Ecore_IMF_Event_Delete_Surrounding* deleteSurroundingEvent = (Ecore_IMF_Event_Delete_Surrounding*) event_info;

    const std::string keyString( "" );
    const int cursorOffset( deleteSurroundingEvent->offset );
    const int numberOfChars( deleteSurroundingEvent->n_chars );

    Dali::ImfManager::ImfEventData imfData ( Dali::ImfManager::DELETESURROUNDING , keyString, cursorOffset, numberOfChars );
    Dali::ImfManager handle( this );
    mEventSignalV2.Emit( handle, imfData );
  }
}

void ImfManager::NotifyCursorPosition()
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "ImfManager::NotifyCursorPosition\n" );

  if ( mIMFContext )
  {
    ecore_imf_context_cursor_position_set( mIMFContext, mIMFCursorPosition );
  }
}

int ImfManager::GetCursorPosition()
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "ImfManager::GetCursorPosition\n" );

  return mIMFCursorPosition;
}

void ImfManager::SetCursorPosition( unsigned int cursorPosition )
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "ImfManager::SetCursorPosition\n" );

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
