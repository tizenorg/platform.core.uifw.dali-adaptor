#ifndef __DALI_PURE_WAYLAND_KEYBOARD_H__
#define __DALI_PURE_WAYLAND_KEYBOARD_H__

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


namespace Dali
{

namespace Internal
{

namespace Adaptor
{

/**
 * @brief Used to attached to input devices and receive input events
 *
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

public:
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
   };
   WlSeat *mSeatInterface;    ///< Wayland Seat interface
   WlPointer* mPointer;       ///< Wayland Pointer interface
   WlKeyboard* mKeyboard;     ///< Wayland Keyboard interface
   WlTouch* mTouch;           ///< Wayland Touch interface
 };

 Seat* GetSeat( WlSeat* seatInterface );

private:

 Dali::Vector< Seat > mSeats;
};
} // Internal
} // Adaptor
} // Dali

#endif  //__DALI_PURE_WAYLAND_INPUT_H__
