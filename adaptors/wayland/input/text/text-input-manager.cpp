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
#include "text-input-manager.h"

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
//#include <wayland-extension/protocol/text-client-protocol.h>

// INTERNAL INCLUDES

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_TEXT_INPUT");
#endif

TextInputManager* gTextInputManager = NULL;


struct ReturnKeyInfo
{
  const uint32_t TizenReturnKey;
  const InputMethod::ActionButton ReturnKey;
};

const ReturnKeyInfo RETURN_KEY_TABLE[] =
{
  { WL_TEXT_INPUT_RETURN_KEY_TYPE_DEFAULT,  InputMethod::ACTION_DEFAULT },
  { WL_TEXT_INPUT_RETURN_KEY_TYPE_DONE,     InputMethod::ACTION_DONE },
  { WL_TEXT_INPUT_RETURN_KEY_TYPE_GO,       InputMethod::ACTION_GO },
  { WL_TEXT_INPUT_RETURN_KEY_TYPE_JOIN,     InputMethod::ACTION_JOIN },
  { WL_TEXT_INPUT_RETURN_KEY_TYPE_LOGIN,    InputMethod::ACTION_LOGIN },
  { WL_TEXT_INPUT_RETURN_KEY_TYPE_NEXT,     InputMethod::ACTION_NEXT },
  { WL_TEXT_INPUT_RETURN_KEY_TYPE_SEARCH,   InputMethod::ACTION_SEARCH },
  { WL_TEXT_INPUT_RETURN_KEY_TYPE_SEND,     InputMethod::ACTION_SEND }
};

const unsigned int RETURN_KEY_TABLE_COUNT = sizeof( RETURN_KEY_TABLE ) / sizeof( RETURN_KEY_TABLE[0] );

int GetTizenReturnKeyType( InputMethod::ActionButton returnKey )
{
  for( unsigned int i = 0; i < RETURN_KEY_TABLE_COUNT ; i ++ )
  {
    const ReturnKeyInfo& info = RETURN_KEY_TABLE[ i ];
    if( info.ReturnKey == returnKey )
    {
      return info.TizenReturnKey;
    }
  }
  DALI_LOG_ERROR("No mapping for InputMethod::ReturnKey %d \n", returnKey );

  return WL_TEXT_INPUT_RETURN_KEY_TYPE_DEFAULT;

};

} // unnamed namespace

TextInputManager::TextInputManager()
: mDisplay( NULL ),
  mLastActiveSeat( NULL ),
  mWindowEventInterface( NULL )
{
  gTextInputManager = this;
}

void TextInputManager::AssignWindowEventInterface( WindowEventInterface* eventInterface )
{
  mWindowEventInterface = eventInterface;
}

void TextInputManager::AssignDisplay( WlDisplay* display )
{
  mDisplay = display;
}

void TextInputManager::AddSeat( Seat* seat )
{
  SeatInfo info;
  info.mSeat = seat;
  mLastActiveSeat = seat;

  mSeats.push_back( info );
}

TextInputManager::~TextInputManager()
{
  gTextInputManager = NULL;
}

TextInputManager& TextInputManager::Get()
{
  return *gTextInputManager;
}

void TextInputManager::Enter( Seat* seat, WlSurface* surface )
{
  // focus received typically in response to an activate request
  mLastActiveSeat = seat;
  SeatInfo& info = GetLastActiveSeat();
  info.mFocused = true;
  printf(" TextInputManager seat entereed \n");
}

void TextInputManager::Leave( Seat* seat )
{
  mLastActiveSeat = seat;
  SeatInfo& info = GetLastActiveSeat();

  // Focus has been lost either in response
  // to a deactivate request or when the assigned surface lost focus or was destroyed.
  info.mFocused = false;
  printf(" TextInputManager seat left \n");
}

void TextInputManager::ModifiersMap( Seat* seat, WlArray *map )
{
  // Map contains an array of 0-terminated modifiers names. The
  // position in the array is the index of the modifier as used in
  // the modifiers bitmask in the keysym event
  // workout if we need to use this
}

void TextInputManager::InputPanelState( Seat* seat, uint32_t state )
{
  mLastActiveSeat = seat;
  SeatInfo& info = GetLastActiveSeat();

  info.mVisible = ( state == 1);

  // is true, then the keyboard has just shown,
  // state == 1 for show, 0 for hidden
  mKeyboardStatusSignal.Emit( info.mVisible );

}

void TextInputManager::PreeditString( Seat* seat, uint32_t serial, const char *text, const char *commit )
{
  mLastActiveSeat = seat;
  mPreEditStringSignal.Emit( serial, text, commit );
}

void TextInputManager::PreeditStyling( Seat* seat, uint32_t index, uint32_t length, uint32_t style )
{
  mLastActiveSeat = seat;
  mPreEditStylingSignal.Emit( index, length, style );
}

void TextInputManager::PreeditCursor( Seat* seat, int32_t index )
{
  mLastActiveSeat = seat;
  mPreEditCursorSignal.Emit( index  );
}

void TextInputManager::CommitString( Seat* seat, uint32_t serial, const char *text )
{
  mLastActiveSeat = seat;
  mCommitStringSignal.Emit( serial, text );

  if ( KeyLookup::IsDeviceButton( text ) )
  {
    // don't commit text like SHIFT_R
    return;
  }

  printf(" commit string %s\n", text );


  Dali::KeyEvent keyEvent;

  keyEvent.keyPressed = std::string ( text );
  keyEvent.keyPressedName = keyEvent.keyPressed;
  keyEvent.state = KeyEvent::Down;

  printf(" got a key from virtual keyboard %s %d %s \n", keyEvent.keyPressed.c_str(), keyEvent.keyCode, keyEvent.keyPressedName.c_str() );
  mWindowEventInterface->KeyEvent( keyEvent );


}

void TextInputManager::CursorPosition( Seat* seat, int32_t index, int32_t anchor)
{
  mLastActiveSeat = seat;
  mCursorPositionSignal.Emit( index, anchor );
}

void TextInputManager::DeleteSurroundingText( Seat* seat, int32_t index,  uint32_t length )
{
  mLastActiveSeat = seat;
  mDeleteSurroundingTextSignal.Emit( index, length );
}


void TextInputManager::Keysym( Seat* seat,
                     uint32_t serial,
                     uint32_t time,
                     uint32_t sym,
                     uint32_t state,
                     uint32_t modifiers)
{
  mLastActiveSeat = seat;

  Dali::KeyEvent keyEvent = seat->GetDALiKeyEventFromSymbol( serial, time, sym, state, modifiers);

  printf(" got a key from virtual keyboard %s %d %s \n", keyEvent.keyPressed.c_str(), keyEvent.keyCode, keyEvent.keyPressedName.c_str() );
  mWindowEventInterface->KeyEvent( keyEvent );
}

void TextInputManager::Language( Seat* seat, uint32_t serial, const char *language )
{
  mLastActiveSeat = seat;
  SeatInfo& info = GetLastActiveSeat();
  info.mLanguage =  language;
  mKeyboardLanguageChangedSignal.Emit();
}

void TextInputManager::TextDirection( Seat* seat, uint32_t serial, uint32_t direction )
{
  mLastActiveSeat = seat;
  SeatInfo& info = GetLastActiveSeat();


  // text-input direction can be auto, left to right, or right to left
  // DALi only supports ltr or rtl
  if( direction == WL_TEXT_INPUT_TEXT_DIRECTION_RTL )
  {
    info.mTextDirection =  Dali::VirtualKeyboard::RightToLeft;
  }
  else
  {
    info.mTextDirection =  Dali::VirtualKeyboard::LeftToRight;
  }
}

void TextInputManager::SelectionRegion( Seat* seat, uint32_t serial, int32_t start, int32_t end)
{
  mLastActiveSeat = seat;
  mSelectionRegionSignal.Emit( serial, start, end );
}

void TextInputManager::PrivateCommand( Seat* seat, uint32_t serial, const char *command)
{
  mLastActiveSeat = seat;
  // not required
}

void TextInputManager::InputPanelGeometry( Seat* seat,
                                 uint32_t x,
                                 uint32_t y,
                                 uint32_t width,
                                 uint32_t height)
{
  mLastActiveSeat = seat;
  SeatInfo& info = GetLastActiveSeat();

  Dali::Rect<int> newDimensions( x, y, width, height );

  printf("  TextInputManager::InputPanelGeometry %d %d %d %d \n",x,y,width,height);

  if( info.mInputPanelDimensions != newDimensions )
  {
    info.mInputPanelDimensions =  newDimensions;
    mKeyboardResizeSignal.Emit();
  }
}

TextInputManager::SeatInfo& TextInputManager::GetLastActiveSeat()
{
  SeatInfo* currentSeat = &mSeats[ 0 ];

  for( std::vector< SeatInfo >::iterator iter = mSeats.begin(); iter != mSeats.end() ; ++iter )
  {
      if( (*iter).mSeat == mLastActiveSeat )
      {
        currentSeat = &(*iter);
        break;
      }

  }
  return *currentSeat;
}

Seat* TextInputManager::GetSeat( const WlTextInput* textInput)
{
   for( std::vector< SeatInfo >::iterator iter = mSeats.begin(); iter != mSeats.end() ; ++iter )
   {
     Seat* seat = (*iter).mSeat;

     if( seat->GetTextInputInterface() == textInput )
     {
       return seat;
     }
  }
   printf("  TextInputManager::GetSeat got null seat\n");

   return NULL;
}


void TextInputManager::ShowInputPanel()
{
  SeatInfo& info = GetLastActiveSeat();
  info.mVisible = true;
  Seat* seat = info.mSeat;

  printf("  TextInputManager::ShowInputPanel() \n");

  wl_text_input_show_input_panel( seat->GetTextInputInterface() );

  // imf normally does this...
  wl_text_input_activate( seat->GetTextInputInterface(), seat->GetSeatInterface(), seat->GetSurface() );

  wl_display_flush( mDisplay );
}

void TextInputManager::HideInputPanel()
{
  SeatInfo& info = GetLastActiveSeat();
  info.mVisible = false;
  Seat* seat = info.mSeat;

  wl_text_input_deactivate( seat->GetTextInputInterface(), seat->GetSeatInterface() );

  wl_text_input_hide_input_panel( seat->GetTextInputInterface() );

  wl_display_flush( mDisplay );
}

bool TextInputManager::IsInputPanelVisible()
{
  SeatInfo& info = GetLastActiveSeat();
  return info.mVisible;
}


void TextInputManager::SetReturnKeyType( const InputMethod::ActionButton type )
{
  TextInputManager::SeatInfo& info = TextInputManager::Get().GetLastActiveSeat();

  uint32_t returnKey = GetTizenReturnKeyType( type );

  wl_text_input_set_return_key_type( info.mSeat->GetTextInputInterface(), returnKey);

  wl_display_flush( mDisplay );
}

Dali::VirtualKeyboard::StatusSignalType& TextInputManager::StatusChangedSignal()
{
  return mKeyboardStatusSignal;
}

Dali::VirtualKeyboard::VoidSignalType& TextInputManager::ResizedSignal()
{
  return mKeyboardResizeSignal;
}

Dali::VirtualKeyboard::VoidSignalType& TextInputManager::LanguageChangedSignal()
{
  return mKeyboardLanguageChangedSignal;
}


} // namespace Adaptor

} // namespace Internal

} // namespace Dali
