#ifndef __DALI_EVENT_FEEDER_H_
#define __DALI_EVENT_FEEDER_H_

/*
Copyright (c) 2000-2014 Samsung Electronics Co., Ltd All Rights Reserved

This file is part of Dali

PROPRIETARY/CONFIDENTIAL

This software is the confidential and proprietary information of
SAMSUNG ELECTRONICS ("Confidential Information"). You shall not
disclose such Confidential Information and shall use it only in
accordance with the terms of the license agreement you entered
into with SAMSUNG ELECTRONICS.

SAMSUNG make no representations or warranties about the suitability
of the software, either express or implied, including but not limited
to the implied warranties of merchantability, fitness for a particular
purpose, or non-infringement. SAMSUNG shall not be liable for any
damages suffered by licensee as a result of using, modifying or
distributing this software or its derivatives.
*/

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali DALI_IMPORT_API
{

struct KeyEvent;
struct MouseWheelEvent;
struct TouchPoint;

namespace EventFeeder
{

/**
 * Feed a touch point to the adaptor.
 *
 * @param[in] point touch point
 * @param[in] timeStamp time value of event
 *
 * @note For testing/automation purposes only.
 */
void FeedTouchPoint( TouchPoint& point, int timeStamp );

/**
 * Feed a mouse wheel event to the adaptor.
 *
 * @param[in]  wheelEvent mouse wheel event
 *
 * @note For testing/automation purposes only.
 */
void FeedWheelEvent( MouseWheelEvent& wheelEvent );

/**
 * Feed a key event to the adaptor.
 *
 * @param[in] keyEvent The key event holding the key information.
 *
 * @note For testing/automation purposes only.
 */
void FeedKeyEvent( KeyEvent& keyEvent );

} // namespace EventFeeder

} // namespace Dali

#endif // __DALI_EVENT_FEEDER_H_
