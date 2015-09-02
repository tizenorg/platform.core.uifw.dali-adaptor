#ifndef __DALI_PURE_WAYLAND_INPUT_H__
#define __DALI_PURE_WAYLAND_INPUT_H__

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
#include <wl-types.h>
#include <dali/public-api/common/dali-vector.h>
#include <base/interfaces/window-event-interface.h>
#include <xkbcommon/xkbcommon.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

/**
 * @brief Used to attached to input devices and receive input events
 * @todo needs a  tidy up and modularising into pointer / touch / key handling
 */
class WaylandInput
{

public:


  /**
   * @brief Constructor
   */
  WaylandInput();

  /**
   * @brief Destructor
   */
  ~WaylandInput();


  /**
   * @brief Add a new seat ( collection of input devices)
   * @param[in] wlRegistry
   * @param[in] seatName seat name / id
   * @param[in] version interface version
   */
  void AddSeat( WlRegistry* wlRegistry, uint32_t seatName, uint32_t version );


  void AssignWindowEventInterface( WindowEventInterface* eventInterface);

public:

  struct XkbData
  {
    struct xkb_context *mContext;
    struct xkb_keymap* mKeymap;
    struct xkb_state* mState;
    xkb_mod_mask_t mControlMask;
    xkb_mod_mask_t mAltMask;
    xkb_mod_mask_t mShiftMask;
    bool mControlDown:1;
    bool mShiftDown:1;
    bool mAltDown:1;


  } ;


  /*
  * Seat is a group of input devices ( pointer, touch, keyboard)
  * Seat = seat around a computer or a in a car.
  * So each person with a seat can use their own input devices.
  * On a multi-screen device, they may also have their own display.
  * E.g. in a car everyone has their own touchscreen device, which are running
  * of a central computer.
  */
 struct Seat
 {
   Seat()
   :mSeatInterface(NULL),
   mPointer(NULL),
   mKeyboard(NULL),
   mTouch(NULL)
   {
     mXkbData.mContext = NULL;

   };
   WlSeat *mSeatInterface;    ///< Wayland Seat interface
   WlPointer* mPointer;       ///< Wayland Pointer interface
   WlKeyboard* mKeyboard;     ///< Wayland Keyboard interface
   WlTouch* mTouch;           ///< Wayland Touch interface
   float mPointerX;
   float mPointerY;
   XkbData mXkbData;
 };


 Seat* GetSeat( WlSeat* seatInterface );

private:

 /**
  * retrieve the seat that contains the pointer
  */
 Seat* GetSeatFromPointer(WlPointer* pointer );


public:

 /**
  * @brief Update the pointer position
  * We have to cache the pointer position internally because
  * the pointer button events don't contain a position.
  * @param[in] pointer wayland pointer interface
  * @param[in] x x-position relative to surface
  * @param[in] y y-position relative to surface
  * @param[in] timestamp time stamp
  */
 void UpdatePointerPosition( WlPointer* pointer, float x, float y, unsigned int timestamp);

 /**
  * @brief Update pointer button
  * @param[in] pointer wayland pointer interface
  * @param[in] timestamp time stamp
  * @param[in] button button code
  * @param[in] state  0 = up, 1 = down
  */
 void UpdatePointerButton( WlPointer* pointer,  unsigned int timestamp, unsigned int button, unsigned int state);

 void PointerEnter();

 Seat* GetSeatFromKeyboard(WlKeyboard* keyboard);

 void PointerLeave();

 void KeyDown( unsigned int keyCode, const char* keyName, unsigned int timestamp );

private:

 Dali::Vector< Seat > mSeats;
 WindowEventInterface* mWindowEventInterface;


};
} // Internal
} // Adaptor
} // Dali

#endif  //__DALI_PURE_WAYLAND_INPUT_H__
