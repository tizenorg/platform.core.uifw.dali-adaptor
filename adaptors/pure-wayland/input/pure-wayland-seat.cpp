#ifndef __DALI_PURE_WAYLAND_INPUT_INTERFACE_H__
#define __DALI_PURE_WAYLAND_INPUT_INTERFACE_H__

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

// EXTERNAL INCLUDES
#include "pure-wayland-input-seat.h"

namespace Dali
{

namespace Internal
{

namespace Adaptor
{
Seat::Seat( WaylandInputInterface* inputInterface )
:mPointer( NULL ),
 mKeyboard( NULL ),
 mTouch( NULL ),
 mInputInterface( inputInterface )
{
}

Seat::~Seat()
{
  DestoryPointerInterface();
  DestroyTouchInterface();
  DestroyKeyboardInterface();
}

void Seat::SetPointerInterface( InterfaceState state)
{
  if( status == INTERFACE_AVAILABLE )
  {
    if( ! mPointer )
    {
      WlPointer* pointerInterface = wl_seat_get_pointer( mWaylandSeat );

      // store the interface and add an event listener
      wl_pointer_add_listener( pointerInterface, &Wayland::GetPointerListener(), mInputInterface );

      mPointer = pointerInterface;
    }
  }
  else
  {
    DestoryPointerInterface();
  }
}

void Seat::SetTouchInterface( WlTouch* touch, InterfaceStatus status )
{
  if( status == INTERFACE_AVAILABLE)
  {
    // check if it's configured already
    if( ! mTouch )
    {
      WlTouch* touchInterface = wl_seat_get_touch( mWaylandSeat );

      wl_touch_add_listener( touchInterface, &Wayland::GetTouchListener(), mInputInterface );

      // store the interface and add an event listener
      mTouch = touchInterface;
    }
  }
  else
  {
    DestoryTouchInterface();
  }

}

void Seat::SetKeyboardInterface( WlKeyboard* keyboard, InterfaceStatus status )
{
  if( status == INTERFACE_AVAILABLE)
  {
    // check if it's configured already
    if( ! mKeyboard )
    {
      WlKeyboard* keyboardInterface = wl_seat_get_keyboard( mWaylandSeat );

      wl_keyboard_add_listener( keyboardInterface, &Wayland::GetKeyboardListener(), mInputInterface );

      // store the interface and add an event listener
      mKeyboard = keyboardInterface;
    }
  }
  else
  {
    DestoryKeyboardInterface();
  }
}

WlPointer* Seat::GetPointerInterface()
{
  return mPointer;
}

WlTouch* Seat::GetTouchInterface()
{
  return mTouch;
}

WlKeyboard* Seat::GetKeyboardInterface()
{
  return mKeyboard;
}

void Seat::DestoryPointerInterface()
{
  if( mPointer )
  {
    wl_pointer_destroy( mPointer );
    mPointer = NULL;
  }
}

void Seat::DestroyTouchInterface()
{
  if( mTouch )
  {
    wl_touch_destroy( mTouch );
    mTouch = NULL;
  }
}

void Seat::DestroyKeyboardInterface()
{
  if( mKeyboard )
  {
    wl_keyboard_destroy( mKeyboard );
    mKeyboard = NULL;
  }
}

void SetName( const char* name )
{
}




} // Internal
} // Adaptor
} // Dali

#endif  //__DALI_PURE_WAYLAND_INPUT_H__
