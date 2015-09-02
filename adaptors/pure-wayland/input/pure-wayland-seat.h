#ifndef __DALI_PURE_WAYLAND_INPUT_SEAT_H__
#define __DALI_PURE_WAYLAND_INPUT_SEAT_H__

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


namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace Wayland
{

class InputInterface;

/**
 *
 * A single seat is a group of keyboards, pointers and touch devices.
 *
 * For example you can have a car with 4 seats each with their own touch screen that
 * is run by a central computer with a multi-display output.
 *
 * Or you may have two people / two seats in an office sharing the same screen at the same time.
 *
 *
 * There is a single wl_pointer / wl_touch and wl_keyboard interface per seat.
 * But each interface can have multiple devices. E.g. wl_pointer interface may have 2
 * mouse pointers attached.
 *
 * Input devices can be hot plugged. However wl_pointer / wl_touch and wl_keyboard interface
 * will only be destroyed when the interface has no devices associated with it.
 * E.g. if a seat has a single mouse, when you unplug the mouse the interface is deleted.
 * If a seat has a two mice, only when you unplug both mice is the interface deleted.
 *
 */
class Seat
{

public:

  /**
   * Seat interface status
   */
  enum InterfaceStatus
  {
    INTERFACE_AVAILABLE,
    INTERFACE_NOT_AVAILABLE,
  };

  /**
   * @brief constructor
   * @param[in] inputInterface input interface
   */
  Seat( InputInterface* inputInterface );

  /**
   * @brief non virtual destructor, not intended as base class
   */
  ~Seat();

  /**
   * @brief Set the pointer interface
   * @param[in] pointer Wayland pointer interface
   * @param[in] status of the interface
   */
  void SetPointerInterface( InterfaceStatus status );

  /**
   * @brief Set the touch interface
   * @param[in] pointer Wayland pointer interface
   * @param[in] status of the interface
   */
  void SetTouchInterface( InterfaceStatus status );

  /**
   * @brief Set the keyboard interface
   * @param[in] pointer Wayland pointer interface
   * @param[in] status of the interface
   */
  void SetKeyboardInterface( InterfaceStatus status );

  /**
   * @brief Get the pointer interface
   * @return Wayland pointer interface
   */
  WlPointer* GetPointerInterface();

  /**
   * @brief Get the touch interface
   * @return Wayland touch interface
   */
  WlTouch* GetTouchInterface();

  /**
   * @brief Get the keyboard interface
   * @return Wayland keyboard interface
   */
  WlKeyboard* GetKeyboardInterface();

  /**
   * @brief calls wl_pointer_destroy on the pointer interface
   */
  void DestroyPointerInterface();

  /**
   * @brief calls wl_touch_destroy on the touch interface
   */
  void DestroyTouchInterface();

  /**
   * @brief calls wl_touch_keyboard on the keyboard interface
   */
  void DestroyKeyboardInterface();

  /**
   * @brief place holder store the seat name
   * Currently we don't store the name as DALi core isn't seat name aware
   * @todo think about adding Seat name to touch / key events.
   */
  void SetName( const char* name );

private:

  WlPointer* mPointer;       ///< Wayland Pointer interface
  WlKeyboard* mKeyboard;     ///< Wayland Keyboard interface
  WlTouch* mTouch;           ///< Wayland Touch interface
  WlSeat* mWaylandSeat;      ///< Wayland Seat interface
  InputInterface* mInputInterface;  ///< DALi Wayland Input interface

};

} // Wayland
} // Internal
} // Adaptor
} // Dali

#endif  //__DALI_PURE_WAYLAND_INPUT_SEAT_H__
