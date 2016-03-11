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
}


TextInputManager::TextInputManager()
{
  gTextInputManager = this;
}

TextInputManager::~TextInputManager()
{
  gTextInputManager = NULL;
}

TextInputManager& TextInputManager::Get()
{
  return *gTextInputManager;
}

void TextInputManager::TextInputManager::Enter( Seat* seat, WlSurface* surface )
{
  mLastActiveSeat = seat;
}

void TextInputManager::TextInputManager::Leave( Seat* seat )
{
  mLastActiveSeat = seat;
}

void TextInputManager::ModifiersMap( Seat* seat, WlArray *map )
{
  mLastActiveSeat = seat;
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

}

void TextInputManager::PreeditStyling( Seat* seat, uint32_t index, uint32_t length, uint32_t style )
{
  mLastActiveSeat = seat;

}

void TextInputManager::PreeditCursor( Seat* seat, uint32_t index )
{
  mLastActiveSeat = seat;

}

void TextInputManager::CommitString( Seat* seat, uint32_t serial, const char *text )
{
  mLastActiveSeat = seat;

}

void TextInputManager::CursorPosition( Seat* seat, int32_t index, int32_t anchor)
{
  mLastActiveSeat = seat;

}

void TextInputManager::DeleteSurroundingText( Seat* seat, int32_t index,  uint32_t length )
{
  mLastActiveSeat = seat;

}


void TextInputManager::Keysym( Seat* seat,
                     uint32_t serial,
                     uint32_t time,
                     uint32_t sym,
                     uint32_t state,
                     uint32_t modifiers)
{
  mLastActiveSeat = seat;

}

void TextInputManager::Language( Seat* seat, uint32_t serial, const char *language )
{
  mLastActiveSeat = seat;
}

void TextInputManager::TextDirection( Seat* seat, uint32_t serial, uint32_t direction )
{
  mLastActiveSeat = seat;
  SeatInfo& info = GetLastActiveSeat();


  // direction can be auto, left to right, or right to left
  // DALi only supports ltr or trl
  if( direction == WL_TEXT_INPUT_TEXT_DIRECTION_RTL )
  {
    info.mTextDirection =  Dali::VirtualKeyboard::TextDirection::RightToLeft;
  }
  else
  {
    info.mTextDirection =  Dali::VirtualKeyboard::TextDirection::LeftToRight;
  }


  };


}

void TextInputManager::SelectionRegion( Seat* seat, uint32_t serial, int32_t start, int32_t end)
{
  mLastActiveSeat = seat;

}

void TextInputManager::PrivateCommand( Seat* seat, uint32_t serial, const char *command)
{

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


  if( info.mInputPanelDimensions != newDimensions )
  {
    info.mInputPanelDimensions =  newDimensions;
    mKeyboardResizeSignal.Emit();
  }
}

TextInputManager::SeatInfo& TextInputManager::GetLastActiveSeat()
{
  SeatInfo* currentSeat = &mSeats[ 0 ];

  for( Dali::Vector< SeatInfo >::Iterator iter = mSeats.Begin(); iter != mSeats.End() ; ++iter )
  {
      if( (*iter).mSeat == mLastActiveSeat )
      {
        currentSeat = (*iter);
        break;
      }

  }
  return *currentSeat;
}

void TextInputManager::ShowInputPanel()
{
  SeatInfo& info = GetLastActiveSeat();
  info.mVisible = true;

  info.mSeat->mTextInput->show_input_panel();
  wl_flush();
}

void TextInputManager::HideInputPanel()
{
  SeatInfo& info = GetLastActiveSeat();
  info.mVisible = false;

  info.mSeat->mTextInput->hide_input_panel();
  wl_flush();

}

bool TextInputManager::IsInputPanelVisible()
{
  SeatInfo& info = GetLastActiveSeat();
  return info.mVisible;
}


void TextInputManager::SetReturnKeyType( const InputMethod::ActionButton type )
{






}





// Current DALi Virtual Keyboard devel API has no knowledge of Seats
// so we use the most recently 'active' seat. This is only really needed if
// there's two seats in use at the same time. Not sure if
namespace VirtualKeyboard
{


Dali::VirtualKeyboard::TextDirection GetTextDirection()
{
  TextInputManager::SeatInfo& info = TextInputManager::Get().GetLastActiveSeat();
  return info.mTextDirection;
}
Dali::Rect< int > GetSizeAndPosition()
{
  TextInputManager::SeatInfo& info = TextInputManager::Get().GetLastActiveSeat();
  return info.mInputPanelDimensions;
}


Dali::VirtualKeyboard::VoidSignalType& StatusChangedSignal()
{
  return TextInputManager::Get().StatusChangedSignal();
}

Dali::VirtualKeyboard::VoidSignalType& ResizedSignal()
{
  return TextInputManager::Get().ResizedSignal();
}

Dali::VirtualKeyboard::VoidSignalType& LanguageChangedSignal()
{
  return TextInputManager::Get().LanguageChangedSignal();
}

void Show()
{
  TextInputManager::Get().ShowInputPanel();
}

void Hide()
{
  TextInputManager::Get().HideInputPanel();
}

bool IsVisible()
{
  TextInputManager::Get().IsInputPanelVisible();
}

void SetReturnKeyType( const InputMethod::ActionButton type )
{
  TextInputManager::Get().SetReturnKeyType( type );
}

} // namespace VirtualKeyboard

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
