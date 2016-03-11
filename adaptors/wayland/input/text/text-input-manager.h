#ifndef __DALI_WAYLAND_TEXT_INPUT_MANAGER_H__
#define __DALI_WAYLAND_TEXT_INPUT_MANAGER_H__

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

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include "text-input-interface.h"
#include <virtual-keyboard.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

/**
 *
 *
 */
class TextInputManager : private TextInputInterface
{

public:

  /**
   * @brief Constructor
   */
  TextInputManager();

  /**
   * @brief Destructor
   */
  ~TextInputManager();

  /**
   * Get the global TextInputManager
   */
  static TextInputManager& Get();

private: //TextInputInterface


  /**
   * @copydoc TextInputInterface::Enter
   */
  virtual void Enter( Seat* seat, WlSurface* surface );

  /**
   * @copydoc TextInputInterface::Leave
   */
  virtual void Leave( Seat* seat );

  /**
   * @copydoc TextInputInterface::ModifiersMap
   */
  virtual void ModifiersMap( Seat* seat, WlArray *map );

  /**
   * @copydoc TextInputInterface::InputPanelState
   */
  virtual void InputPanelState( Seat* seat, uint32_t state );

  /**
   * @copydoc TextInputInterface::PreeditString
   */
  virtual void PreeditString( Seat* seat, uint32_t serial, const char *text, const char *commit );

  /**
   * @copydoc TextInputInterface::PreeditStyling
   */
  virtual void PreeditStyling( Seat* seat, uint32_t index, uint32_t length, uint32_t style );


  /**
   * @copydoc TextInputInterface::PreeditCursor
   */
  virtual void PreeditCursor( Seat* seat, uint32_t index );

  /**
   * @copydoc TextInputInterface::CommitString
   */
  virtual void CommitString( Seat* seat, uint32_t serial, const char *text );

  /**
   * @copydoc TextInputInterface::CursorPosition
   */
  virtual void CursorPosition( Seat* seat, int32_t index, int32_t anchor);

  /**
   * @copydoc TextInputInterface::DeleteSurroundingText
   */
  virtual void DeleteSurroundingText( Seat* seat, int32_t index,  uint32_t length );


  /**
   * @copydoc TextInputInterface::Keysym
   */
  virtual void Keysym( Seat* seat,
                       uint32_t serial,
                       uint32_t time,
                       uint32_t sym,
                       uint32_t state,
                       uint32_t modifiers);

  /**
   * @copydoc TextInputInterface::Language
   */
  virtual void Language( Seat* seat, uint32_t serial, const char *language );

  /**
   * @copydoc TextInputInterface::TextDirection
   */
  virtual void TextDirection( Seat* seat, uint32_t serial, uint32_t direction );

  /**
   * @copydoc TextInputInterface::SelectionRegion
   */
  virtual void SelectionRegion( Seat* seat, uint32_t serial, int32_t start, int32_t end);


  /**
   * @copydoc TextInputInterface::PrivateCommand
   */
  virtual void PrivateCommand( Seat* seat, uint32_t serial, const char *command);

  /**
   * @copydoc TextInputInterface::InputPanelGeometry
   */
  virtual void InputPanelGeometry( Seat* seat,
                                   uint32_t x,
                                   uint32_t y,
                                   uint32_t width,
                                   uint32_t height);

public:

  void ShowInputPanel();

  void HideInputPanel();

  bool IsInputPanelVisible();

  void SetReturnKeyType( const InputMethod::ActionButton type );


  /**
   * @copydoc Dali::VirtualKeyboard::StatusChangedSignal
   */
  Dali::VirtualKeyboard::StatusSignalType& StatusChangedSignal();

  /**
   * @copydoc Dali::VirtualKeyboard::ResizedSignal
   */
  Dali::VirtualKeyboard::StatusSignalType& ResizedSignal()

  /**
   * @copydoc Dali::VirtualKeyboard::LanguageChangedSignal
   */
  Dali::VirtualKeyboard::StatusSignalType& LanguageChangedSignal();



public:

  /**
   * Store text input data for each seat.
   */
  struct SeatInfo
  {
    SeatInfo()
    : mTextDirection( Dali::VirtualKeyboard::LeftToRight ),
      mSeat( NULL ),
      mInputPanelDimensions( 0,0,0,0 ),
      mVisible( false )
    {

    }
    Dali::VirtualKeyboard::TextDirection mTextDirection;
    Seat* mSeat;
    Dali::Rect<int> mInputPanelDimensions;  ///< size of the virtual keybaord panel
    std::string mLanguage;
    bool mVisible;
  };

public:

  SeatInfo& GetLastActiveSeat();

public:

  Seat* mLastActiveSeat;    ///< Seat that was last used
  Dali::Vector< SeatInfo > mSeats;
  Dali::VirtualKeyboard::StatusSignalType mKeyboardStatusSignal;
  Dali::VirtualKeyboard::VoidSignalType   mKeyboardResizeSignal;
  Dali::VirtualKeyboard::VoidSignalType   mKeyboardLanguageChangedSignal;


};


} // Internal
} // Adaptor
} // Dali

#endif
