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
#include "input-manager.h"

// INTERNAL INCLUDES
#include <input/input-listeners.h>

#include <virtual-keyboard.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace
{

const unsigned int POINTER_DEVICE_ID = 2;
const unsigned int TOUCH_DEVICE_ID = 3;

} // unnamed namespace

InputManager::InputManager()
:mWindowEventInterface( NULL )
{

}
InputManager::~InputManager()
{
  for( Dali::Vector< Seat* >::Iterator iter = mSeats.Begin(); iter != mSeats.End() ; ++iter )
  {
    delete (*iter);
  }
  mSeats.Clear();
}

void InputManager::AssignWindowEventInterface( WindowEventInterface* eventInterface)
{
  mWindowEventInterface = eventInterface;
}

void InputManager::AddSeatListener( Dali::WlSeat* seatInterface )
{
  Seat* seat = new Seat( this, seatInterface );

  AddSeat( seat );

  // listen to seat events
  wl_seat_add_listener( seatInterface, Wayland::GetSeatListener(), this );

}

void AddTextInputManager( Dali::WlTextInputManager* textInputManager )
{
  for( Dali::Vector< Seat* >::Iterator iter = mSeats.Begin(); iter != mSeats.End() ; ++iter )
  {
    Seat* seat = (*iter);
    // Create a text input object for each seat
    WlTextInput* textInput = wl_text_input_manager_create_text_input( textInputManager );
    seat->SetTextInput( textInput );

    wl_text_input_add_listener( textInput, &GetTextInputListener(), this );
  }





}



Seat* InputManager::GetFirstSeat()
{
  return mSeats[0];
}

void InputManager::PointerEnter( Seat* seat, unsigned int serial, WlSurface* surface, float x, float y )
{
  if( mWindowEventInterface )
  {
    mWindowEventInterface->WindowFocusIn();
  }
}

void InputManager::PointerLeave( Seat* seat, unsigned int serial, WlSurface* surface )
{
  if( mWindowEventInterface )
  {
    mWindowEventInterface->WindowFocusOut();
  }

}

void InputManager::PointerMotion( Seat* seat, unsigned int timestamp, float x, float y )
{
  if( mWindowEventInterface )
  {
    TouchPoint point ( POINTER_DEVICE_ID, TouchPoint::Motion, x , y);
    mWindowEventInterface->TouchEvent( point, timestamp );
  }
}

void InputManager::PointerButton( Seat* seat, unsigned int serial, unsigned int timestamp, unsigned int button, unsigned int state )
{
  // think about handling multiple pointer button states, if DALi starts to support them
  if( mWindowEventInterface )
  {
    const Dali::Vector2& pointer( seat->GetLastPointerPosition() );

    TouchPoint point ( POINTER_DEVICE_ID, TouchPoint::Up,  pointer.x,  pointer.y );
    if( state == 1)
    {
      point.state = TouchPoint::Down;
    }
    mWindowEventInterface->TouchEvent( point, timestamp );
  }
}

void InputManager::PointerAxis( Seat* seat, unsigned int timestamp, unsigned int axis, float value )
{

  if( mWindowEventInterface )
  {
    WheelEvent wheelEvent( WheelEvent::MOUSE_WHEEL,
                           axis,
                           static_cast< int >( seat->GetDepressedKeyboardModifiers() ),
                           seat->GetLastPointerPosition(),
                           value,
                           timestamp );

    mWindowEventInterface->WheelEvent( wheelEvent );
  }
}

void InputManager::KeyboardKeymap( Seat* seat, unsigned int format, int fd, unsigned int size )
{
  printf("InputManager:: keyboard map \n");
  seat->KeyboardKeymap( format, fd, size );
}

void InputManager::KeyFocusEnter( Seat* seat, unsigned int serial, WlSurface* surface, WlArray* keys )
{
  printf(" InputManager::keyboard enter\n");

  // ignore for now
}

void InputManager::KeyFocusLeave( Seat* seat, unsigned int serial, WlSurface* surface )
{
  // ignore for now
}

void InputManager::KeyEvent( Seat* seat, unsigned int serial, unsigned int timestamp, unsigned int keycode, unsigned int state )
{
  Dali::KeyEvent keyEvent = seat->GetDALiKeyEvent( serial, timestamp, keycode, state );
  printf(" InputManager::KeyEvent\n");
  mWindowEventInterface->KeyEvent( keyEvent);

}


void InputManager::KeyModifiers( Seat* seat,
                          unsigned int serial,
                          unsigned int depressed,
                          unsigned int latched,
                          unsigned int locked,
                          unsigned int group )
{
  seat->SetDepressedKeyboardModifiers( depressed );
}

void InputManager::KeyRepeatInfo( Seat* seat, int32_t rate, int32_t delay)
{
  if(( rate >= 0 ) && ( delay >= 0))
  {
    seat->SetKeyRepeatInfo( static_cast< unsigned int >( rate) , static_cast< unsigned int >(delay ));
  }
}

void InputManager::TouchDown( Seat* seat, unsigned int serial, unsigned int timestamp, WlSurface* surface, int touchId, float x, float y)
{
  // think about handling multiple pointer button states, if DALi starts to support them
  if( mWindowEventInterface )
  {
    const Dali::Vector2& pointer( seat->GetLastPointerPosition() );

    TouchPoint point ( touchId, TouchPoint::Down,  pointer.x,  pointer.y );
    mWindowEventInterface->TouchEvent( point, timestamp );
  }
}


void InputManager::TouchUp( Seat* seat, unsigned int serial, unsigned int timestamp, int touchId )
{
  if( mWindowEventInterface )
  {
    const Dali::Vector2& pointer( seat->GetLastPointerPosition() );

    TouchPoint point ( touchId, TouchPoint::Up,  pointer.x,  pointer.y );
    mWindowEventInterface->TouchEvent( point, timestamp );
  }
}

void InputManager::TouchMotion( Seat* seat, unsigned int timestamp, int touchId, float x, float y )
{
  if( mWindowEventInterface )
  {
    const Dali::Vector2& pointer( seat->GetLastPointerPosition() );

    TouchPoint point ( touchId, TouchPoint::Motion,  pointer.x,  pointer.y );
    mWindowEventInterface->TouchEvent( point, timestamp );
  }
}

void InputManager::TouchFrame( Seat* seat )
{
  // un-used
}

void InputManager::TouchCancel( Seat* seat )
{
  if( mWindowEventInterface )
  {
    const Dali::Vector2& pointer( seat->GetLastPointerPosition() );

    // it looks like DALi just checks the first touch point for interruption
    // so touchId can be zero
    TouchPoint point ( 0, TouchPoint::Interrupted,  pointer.x,  pointer.y );
    mWindowEventInterface->TouchEvent( point, 0 );
  }
}

Seat* InputManager::GetSeat( const WlKeyboard* keyboard )
{
  for( Dali::Vector< Seat *>::Iterator iter = mSeats.Begin(); iter != mSeats.End() ; ++iter )
  {
    if( (*iter)->GetKeyboardInterface() == keyboard )
    {
      return (*iter);
    }
  }
  return NULL;
}

Seat* InputManager::GetSeat( const WlPointer* pointer )
{
  for( Dali::Vector< Seat *>::Iterator iter = mSeats.Begin(); iter != mSeats.End() ; ++iter )
  {
    if( (*iter)->GetPointerInterface() == pointer )
    {
      return (*iter);
    }
  }
  return NULL;
}

Seat* InputManager::GetSeat( const WlTouch* touch )
{
  for( Dali::Vector< Seat* >::Iterator iter = mSeats.Begin(); iter != mSeats.End() ; ++iter )
  {
    if( (*iter)->GetTouchInterface() == touch )
    {
      return (*iter);
    }
  }
  return NULL;
}

Seat* InputManager::GetSeat( const WlSeat* seat)
{
  for( Dali::Vector< Seat* >::Iterator iter = mSeats.Begin(); iter != mSeats.End() ; ++iter )
  {
    if( (*iter)->GetSeatInterface() == seat )
    {
      return (*iter);
    }
  }
  return NULL;
}

void InputManager::AddSeat( Seat* seat )
{
  mSeats.PushBack( seat );
}


}
}
}
