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
#include "pure-wayland-input.h"

// EXTERNAL INCLUDES
#include <stdio.h>
#include <cstring>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace
{

/*******************
 *
 * Pointer event callbacks. See wl_pointer_listener in Wayland API for more information.
 *
 ******************/

/**
 * @brief Called when a seat's pointer has entered a surface
 *
 * When focus enters a surface, the pointer image is
 * undefined and a client should respond to this event by setting
 * an appropriate pointer image with the set_cursor request.
 *
 * @param[in] data user data
 * @param[in] pointer wayland pointer interface
 * @param[in] serial  serial id
 * @param[in] surface surface
 * @param[in] surface_x x coordinate in surface-relative coordinates
 * @param[in] surface_y y coordinate in surface-relative coordinates
 */
void PointerEnter( void *data, WlPointer* pointer, unsigned int serial, WlSurface* surface, wl_fixed_t surface_x, wl_fixed_t surface_y)
{
  printf("pointer enter \n");
}

/**
 * @brief Called when a seat's pointer leaves a surface
 * @param[in] data user data
 * @param[in] pointer wayland pointer interface
 * @param[in] serial serial id
 * @param[in] surface surface
 *
 * The leave notification is sent before the enter notification for
 * the new focus.
 */
void PointerLeave( void *data, WlPointer* pointer, unsigned int serial, WlSurface* surface )
{
  printf("pointer leave \n");
}
/**
 * @brief pointer motion event
 * @param[in] data user data
 * @param[in] pointer wayland pointer interface
 * @param[in] timestamp timestamp with millisecond granularity
 * @param[in] surface_x x coordinate in surface-relative coordinates
 * @param[in] surface_y y coordinate in surface-relative coordinates
 *
 */
void PointerMotion( void *data, WlPointer* pointer, unsigned int timestamp, wl_fixed_t x, wl_fixed_t y )
{
  printf("pointer motion %d, %d \n",x,y);
}

/**
 * @brief pointer click and release events
 * @param[in] data user data
 * @param[in] pointer wayland pointer interface
 * @param[in] serial serial id
 * @param[in] timestamp timestamp with millisecond granularity
 * @param[in] button pointer button pressed
 * @param[in] state button state
 *
 * The location of the click is given by the last motion or enter
 * event.
 */
void PointerButton( void *data, WlPointer* pointer,  unsigned int serial, unsigned int timestamp, unsigned int button, unsigned int state )
{
  printf("pointer button %d state= %d \n",button,state);
}
/**
 * @brief Pointer scroll and other axis notifications.
 * @param[in] data user data
 * @param[in] pointer wayland pointer interface
 * @param[in] timestamp timestamp with millisecond granularity
 * @param[in] axis pointer axis
 * @param[in] value length of a vector along the specified axis in a coordinate space identical to those of motion event
*/
void PointerAxis( void *data, WlPointer* pointer, unsigned int timestamp, unsigned int axis, wl_fixed_t value )
{
  // mouse wheel etc
  printf("pointer axis %d state= %d \n",axis,value);
}

/*******************
 *
 * Key event callbacks. See wl_keyboard_listener in Wayland API for more information.
 *
 ******************/
/**
 * @brief keyboard mapping
 * Event provides a file descriptor to which can be memory-mapped to
 * provide a keyboard mapping description
 * @param[in] data user data
 * @param[in] pointer wayland keyboard interface
 * @param[in] format see wl_keyboard_keymap_format
 * @param[in] fd  file descriptor
 * @param[in] size size of the memory mapped region in bytes
*/
void KeyboardKeymap( void *data, WlKeyboard* keyboard, unsigned int format, int fd, unsigned int size )
{
  // current formats defined in wayland-client-protocol.h
  // WL_KEYBOARD_KEYMAP_FORMAT_NO_KEYMAP =0, WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1=1
  // @todo
  printf("Keyboard map, format = %d \n",format);

}

/**
 * @brief The seat's keyboard focus has entered a surface
 * @param[in] data user data
 * @param[in] pointer wayland keyboard interface
 * @param[in] serial serial id
 * @param[in] surface surface
 * @param[in] keys the currently pressed keys
 */
void KeyFocusEnter( void *data, WlKeyboard* keyboard, unsigned int serial, WlSurface * surface, WlArray* keys )
{
  printf(" key focus enter\n");
}

/**
 * @brief The seat's keyboard focus has left a surface
 * The leave notification is sent before the enter notification for the new focus.
 * @param[in] data user data
 * @param[in] pointer wayland keyboard interface
 * @param[in] serial serial id
 */
void KeyFocusLeave( void *data, WlKeyboard* keyboard, unsigned int serial, WlSurface * surface )
{
  printf(" key focus leave \n");
}

/**
 * @brief Key has been pressed or released
 * @param[in] data user data
 * @param[in] pointer wayland keyboard interface
 * @param[in] timestamp timestamp with millisecond granularity
 * @param[in] serial serial id
 * @param[in] keycode key code
 * @param[in] state
 */
void KeyEvent( void *data, WlKeyboard* keyboard, unsigned int serial, unsigned int timestamp, unsigned int keycode, unsigned int state)
{
  printf(" keycode = %d %c \n",keycode, (char)keycode);
}
/**
 * @brief Key modifier state has changed
 * @param[in] data user data
 * @param[in] pointer wayland keyboard interface
 * @param[in] timestamp timestamp with millisecond granularity
 * @param[in] serial serial id
 * @param[in] depressed modifiers depressed
 * @param[in] latched modifiers latched
 * @param[in] locked modifiers locked
 * @param[in[ group group
 */
void KeyModifiers( void *data, WlKeyboard* keyboard, unsigned int serial, unsigned int depressed, unsigned int latched, unsigned int locked, unsigned int group)
{
  printf(" key modifiers\n");
}

/**
 * @brief Key repeat rate and delay.
 * Informs DALi of the keyboard's repeat rate and delay.
 *
 * Event sent when wl_keyboard object created, before any key events
 * If rate == zero, repeating is disabled
 *
 * @param[in] data user data
 * @param[in] pointer wayland keyboard interface
 * @param[in] rate rate of repeating keys in characters per second
 * @param[in] delay delay in milliseconds since key down until repeating starts
 */
void KeyRepeatInfo(void *data,
          struct wl_keyboard *wl_keyboard,
          int32_t rate,
          int32_t delay)
{
  printf("Key repeat rate = %d delay = %d \n",rate, delay);
}


/*******************
 *
 * Touch event callbacks. See wl_touch_listener in Wayland API for more information.
 *
 ******************/
/**
 * @brief Touch down event on a surface.
 * Future events from this touch point will have the same id which is valid
 * until a touch up event.
 * @param[in] data user data
 * @param[in] pointer wayland touch interface
 * @param[in] serial serial id
 * @param[in] timestamp timestamp with millisecond granularity
 * @param[in] surface surface
 * @param[in] touchId unique touch id valid until touch is released
 * @param[in] x coordinate in surface-relative coordinates
 * @param[in[ y coordinate in surface-relative coordinates
 */
void TouchDown( void *data, WlTouch* touch, unsigned int serial, unsigned int timestamp, WlSurface* surface, int touchId, wl_fixed_t x, wl_fixed_t y)
{
  printf(" touch down %d, %d \n", x,y );
}

/**
 * @brief Touch up event on a surface.
 * The touch id is released and maybe used for future touch events.
 *
 * @param[in] data user data
 * @param[in] pointer wayland touch interface
 * @param[in] serial serial id
 * @param[in] timestamp timestamp with millisecond granularity
 * @param[in] touchId unique touch id valid until touch is released
 */
void TouchUp( void *data, WlTouch* touch, unsigned int serial, unsigned int timestamp, int touchId )
{
  printf(" touch up \n" );
}

/**
 * @brief Touch Motion event on a surface.
 * The touch id is released and maybe used for future touch events.
 *
 * @param[in] data user data
 * @param[in] pointer touch keyboard interface
 * @param[in] timestamp timestamp with millisecond granularity
 * @param[in] touchId unique touch id valid until touch is released
 * @param[in] x coordinate in surface-relative coordinates
 * @param[in[ y coordinate in surface-relative coordinates
 */
void TouchMotion( void *data, WlTouch* touch, unsigned int timestamp, int touchId, wl_fixed_t x, wl_fixed_t y)
{
  printf(" touch motion  %d, %d \n", x,y );
}

/**
 * @brief  end of touch frame event
 * Indicates the end of a contact point list.
 * -Not sure what this is for
 * @param[in] data user data
 * @param[in] pointer wayland touch interface
 */
void TouchFrame( void *data, WlTouch* touch )
{
  printf(" touch frame \n");
}
/**
 * @brief  Cancel current touch session
 * If Wayland Compositor decides current touch stream is a global
 * gesture we have to cancel any touch / gesture detection.
 *
 * Touch cancellation applies to all touch points currently active on
 * our surface.
 *
 * @param[in] data user data
 * @param[in] pointer wayland touch interface
 */
void TouchCancel( void *data, WlTouch* touch )
{
  printf(" touch cancel\n");
}

/**
 * pointer interface
 */
static const struct wl_pointer_listener PointerListener =
{
   PointerEnter,
   PointerLeave,
   PointerMotion,
   PointerButton,
   PointerAxis,
};

/**
 * Keyboard listener interface
 */
const struct wl_keyboard_listener KeyboardListener =
{
   KeyboardKeymap,
   KeyFocusEnter,
   KeyFocusLeave,
   KeyEvent,
   KeyModifiers,
   KeyRepeatInfo
};

/**
 * Touch listener interface
 */
const struct wl_touch_listener TouchListener =
{
   TouchDown,
   TouchUp,
   TouchMotion,
   TouchFrame,
   TouchCancel
};

/**
 * @brief emitted whenever a seat gains or loses the pointer, keyboard or touch capabilities.
 * @param[in] data user data
 * @param[in] seatInterface seat interface
 * @param[in] caps enum containing the complete set of capabilities this seat has.
 */
void SeatHandleCapabilities( void* data, WlSeat* seatInterface, unsigned int caps)
{
  printf("SeatHandleCapabilities\n");

  WaylandInput* input = static_cast< WaylandInput* >( data );

  WaylandInput::Seat* seat =  input->GetSeat( seatInterface );

  bool pointerAvailable  = caps & WL_SEAT_CAPABILITY_POINTER;
  bool keyboardAvailable = caps & WL_SEAT_CAPABILITY_KEYBOARD;
  bool touchAvailable    = caps & WL_SEAT_CAPABILITY_TOUCH;

  // go through either adding or removing pointer/keyboard/touch capabilities
  // most devices are hot pluggable so this function may be called many times

  if( pointerAvailable && ( !seat->mPointer ) )
  {
    // new pointer found
    seat->mPointer = wl_seat_get_pointer( seatInterface );
    wl_pointer_set_user_data( seat->mPointer, input);
    wl_pointer_add_listener( seat->mPointer, &PointerListener, input);
    printf("has pointer \n");
  }
  else if( !pointerAvailable && seat->mPointer )
  {
    // pointer removed
    wl_pointer_destroy( seat->mPointer );
    seat->mPointer = NULL;
    printf("pointer removed \n");
  }
  if( keyboardAvailable && ( !seat->mKeyboard ) )
  {
    // new keyboard found
    seat->mKeyboard = wl_seat_get_keyboard( seatInterface );
    wl_keyboard_set_user_data( seat->mKeyboard, input );
    wl_keyboard_add_listener( seat->mKeyboard, &KeyboardListener, input);
    printf("has keyboard\n");
  }
  else if( !keyboardAvailable && seat->mKeyboard )
  {
    // keyboard removed
    wl_keyboard_destroy( seat->mKeyboard );
    seat->mKeyboard = NULL;
    printf("keyboard removed \n");
  }
  if( touchAvailable && ( !seat->mTouch ) )
  {
    // new touch device found
    printf("touch found \n");
    seat->mTouch = wl_seat_get_touch( seatInterface );
    wl_touch_set_user_data( seat->mTouch, input);
    wl_touch_add_listener( seat->mTouch, &TouchListener, input);
  }
  else if( !touchAvailable && seat->mTouch )
  {
    // touch device removed
    printf("touch device removed\n");
    wl_touch_destroy( seat->mTouch );
    seat->mTouch = NULL;
  }

}

void SeatName(void* data, WlSeat* seatInterface, const char *name)
{
 // WaylandInput* input = static_cast< WaylandInput* >( data );
//  WaylandInput::Seat* seat =  input->GetSeat( seatInterface );
 // seat->mName = std::string(name);
  printf("seat name = %s \n",name);
}

const struct wl_seat_listener SeatListener =
{
    SeatHandleCapabilities, //emitted whenever a seat gains or loses the pointer, keyboard or touch capabilities.
    SeatName, // used to help identify seat in multi-seat configurations
};

}
WaylandInput::WaylandInput()
{

}
WaylandInput::~WaylandInput()
{
  // clear up seat and input interfaces

  for( Dali::Vector< Seat >::Iterator iter = mSeats.Begin(); iter != mSeats.End() ; ++iter )
  {
    Seat& seat(*iter);
    wl_pointer_destroy( seat.mPointer );
    wl_keyboard_destroy( seat.mKeyboard );
    wl_touch_destroy( seat.mTouch );
    wl_seat_destroy(seat.mSeatInterface);
  }
  mSeats.Clear();

}

WaylandInput::Seat* WaylandInput::GetSeat( WlSeat* seatInterface)
{
  for( Dali::Vector< Seat >::Iterator iter = mSeats.Begin(); iter != mSeats.End() ; ++iter )
  {
    if( (*iter).mSeatInterface == seatInterface )
    {
      return &(*iter);
    }
  }
  return NULL;
}

void WaylandInput::AddSeat( WlRegistry* wlRegistry, uint32_t seatName, uint32_t version)
{
  Seat seat;

  printf(" adding seat %d \n",seatName);
  // get the seat interface from the registry
  seat.mSeatInterface = static_cast<Dali::WlSeat*>(wl_registry_bind( wlRegistry, seatName,  &wl_seat_interface, version));

  mSeats.PushBack( seat );

  wl_seat_add_listener( seat.mSeatInterface, &SeatListener, this );

  wl_seat_set_user_data( seat.mSeatInterface, this);


}

}
}
}
