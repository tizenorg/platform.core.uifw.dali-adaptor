#ifndef __DALI_INTERNAL_ROTATION_OBSERVER_H__
#define __DALI_INTERNAL_ROTATION_OBSERVER_H__

/*
Copyright (c) 2000-2013 Samsung Electronics Co., Ltd All Rights Reserved

This file is part of Dali Adaptor

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
rotations suffered by licensee as a result of using, modifying or
distributing this software or its derivatives.
*/

// EXTERNAL INCLUDES
#include <vector>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

struct RotationEvent
{
  int angle;     ///< one of 0, 90, 180, 270
  int winResize; ///< true if the window should be resized
  int width;     ///< new window width
  int height;    ///< new window height
};


/**
 * The RotationObserver can be overridden in order to listen to rotation events.
 */
class RotationObserver
{
public:

  /**
   * Deriving classes should override this to be notified when we receive a RotationPrepare event.
   * @param[in]  area  The area that has been rotationd.
   */
  virtual void OnRotationPrepare( const RotationEvent& rotation ) = 0;

  /**
   * Deriving classes should override this to be notifed when a RotationRequest event is received
   */
  virtual void OnRotationRequest( ) = 0;

protected:

  /**
   * Protected Constructor.
   */
  RotationObserver()
  {
  }

  /**
   * Protected virtual destructor.
   */
  virtual ~RotationObserver()
  {
  }
};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_ROTATION_OBSERVER_H__
