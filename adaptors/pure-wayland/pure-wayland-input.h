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


namespace Dali
{

namespace Internal
{

namespace Adaptor
{

/**
 * Client used to talk to Wayland server over a UNIX domain stream socket.
 *
 * Brief overview of Wayland:
 *
 * Transport mechanism = Socket
 * Display = handles all the data sent from and to the compositor
 * Display has a file descriptor that can be monitored for read / write events
 *
 * wl client function calls will place messages in a queue
 * Calling wl_display_flush() will flush the messages to  the server
 *
 * Incoming data is handled in two steps: queueing and dispatching.
 * In the queue step, the data coming from the display fd is interpreted and
 * added to a queue. On the dispatch step, the handler for the incoming event is called.
 *
 * default queue is dispatched by calling wl_display_dispatch().
 *
 * The compositor sends out the frame event every time it draws a frame.
 * wl_display_frame_callback() to schedule a callback per frame.
 *
 *
 * wl_display_dispatch(). This will dispatch any events queued on the default queue and
 * attempt to read from the display fd if it's empty.
 * Events read are then queued on the appropriate queues according to the proxy assignment.
 *
 *
 */
class WaylandInput
{

public:


  /**
   * @brief Constructor
   */
  WaylandInput();

  void SetDisplay(  WlDisplay* display );
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
  * On a multi-screen device, they may also have their own display
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
   WlSeat *mSeatInterface;
   WlPointer* mPointer;
   WlKeyboard* mKeyboard;
   WlTouch* mTouch;
 };

 Seat* GetSeat( WlSeat* seatInterface );

private:

 Dali::Vector< Seat > mSeats;

 WlDisplay* mDisplay;

};
} // Internal
} // Adaptor
} // Dali

#endif  //__DALI_PURE_WAYLAND_INPUT_H__
