#ifndef __DALI_INTERNAL_WINDOW_VISIBILITY_OBSERVER_H__
#define __DALI_INTERNAL_WINDOW_VISIBILITY_OBSERVER_H__

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
damages suffered by licensee as a result of using, modifying or
distributing this software or its derivatives.
*/

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

/**
 * An interface used to observe when the application's window is shown/hidden.
 */
class WindowVisibilityObserver
{
public:

  /**
   * Called when the window becomes fully or partially visible.
   */
  virtual void OnWindowShown() = 0;

  /**
   * Called when the window is fully hidden.
   */
  virtual void OnWindowHidden() = 0;

protected:

  /**
   * Protected Constructor.
   */
  WindowVisibilityObserver()
  {
  }

  /**
   * Protected virtual destructor.
   */
  virtual ~WindowVisibilityObserver()
  {
  }
};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_WINDOW_VISIBILITY_OBSERVER_H__
