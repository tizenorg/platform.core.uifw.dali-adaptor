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

// CLASS HEADER
#include "pure-wayland-seat.h"

// INTERNAL INCLUDES
#include "pure-wayland-input-listeners.h"

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace Wayland
{

Seat::Seat( InputInterface* inputInterface )
:mPointer( NULL ),
 mKeyboard( NULL ),
 mTouch( NULL ),
 mInputInterface( inputInterface )
{
}

Seat::~Seat()
{
  DestroyPointerInterface();
  DestroyTouchInterface();
  DestroyKeyboardInterface();
}

void Seat::SetPointerInterface( InterfaceStatus status )
{
  if( status == INTERFACE_AVAILABLE )
  {
    if( ! mPointer )
    {
      WlPointer* pointerInterface = wl_seat_get_pointer( mWaylandSeat );

      // store the interface and add an event listener
      wl_pointer_add_listener( pointerInterface, GetPointerListener(), mInputInterface );

      mPointer = pointerInterface;
    }
  }
  else
  {
    DestroyPointerInterface();
  }
}

void Seat::SetTouchInterface( InterfaceStatus status )
{
  if( status == INTERFACE_AVAILABLE )
  {
    // check if it's configured already
    if( ! mTouch )
    {
      WlTouch* touchInterface = wl_seat_get_touch( mWaylandSeat );

      wl_touch_add_listener( touchInterface, GetTouchListener(), mInputInterface );

      // store the interface and add an event listener
      mTouch = touchInterface;
    }
  }
  else
  {
    DestroyTouchInterface();
  }

}

void Seat::SetKeyboardInterface( InterfaceStatus status )
{
  if( status == INTERFACE_AVAILABLE )
  {
    // check if it's configured already
    if( ! mKeyboard )
    {
      WlKeyboard* keyboardInterface = wl_seat_get_keyboard( mWaylandSeat );

      wl_keyboard_add_listener( keyboardInterface, GetKeyboardListener(), mInputInterface );

      // store the interface and add an event listener
      mKeyboard = keyboardInterface;
    }
  }
  else
  {
    DestroyPointerInterface();
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

void Seat::DestroyPointerInterface()
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

} // Wayland
} // Internal
} // Adaptor
} // Dali

#endif  //__DALI_PURE_WAYLAND_INPUT_H__
