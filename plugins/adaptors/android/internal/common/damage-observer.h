#ifndef __DALI_INTERNAL_DAMAGE_OBSERVER_H__
#define __DALI_INTERNAL_DAMAGE_OBSERVER_H__

/*
Copyright (c) 2000-2012 Samsung Electronics Co., Ltd All Rights Reserved

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

// EXTERNAL INCLUDES
#include <vector>

#include <dali/public-api/math/rect.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

typedef Rect<int> DamageArea;

/**
 * The DamageObserver can be overridden in order to listen to damage events.
 */
class DamageObserver
{
public:

  /**
   * Deriving classes should override this to be notified when we receive a damage event.
   * @param[in]  area  The area that has been damaged.
   */
  virtual void OnDamaged( const DamageArea& area ) = 0;

protected:

  /**
   * Protected Constructor.
   */
  DamageObserver()
  {
  }

  /**
   * Protected virtual destructor.
   */
  virtual ~DamageObserver()
  {
  }
};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_DAMAGE_OBSERVER_H__
