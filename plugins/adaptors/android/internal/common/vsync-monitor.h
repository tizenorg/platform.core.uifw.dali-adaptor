#ifndef __DALI_INTERNAL_VSYNC_MONITOR_IMPL_H__
#define __DALI_INTERNAL_VSYNC_MONITOR_IMPL_H__

/*
 Copyright (c) 2000-2013 Samsung Electronics Co., Ltd All Rights Reserved

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

// INTERNAL INCLUDES
#include <base/interfaces/vsync-monitor-interface.h>

class AndroidVSync;

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

/**
 * Tizen interface for monitoring VSync
 */
class VSyncMonitor : public VSyncMonitorInterface
{
public:
  /**
   * Default constructor
   */
  VSyncMonitor();

  /**
   * Destructor
   */
  virtual ~VSyncMonitor();

public:

  /**
   * Initialize the vsync monitor.
   */
  virtual bool Initialize();

  /**
   * Terminate the vsync monitor
   */
  virtual void Terminate();

  /**
   * copydoc Dali::Internal::Adaptor::VSyncMonitorInterface::UseHardware
   */
  virtual bool UseHardware();

  /**
   * Wait for vertical blank sync.
   * @param[out] seconds      The timestamp (seconds) when the vsync occured
   * @param[out] microseconds The timestamp (microseconds) when the vsync occured
   * @return true if a valid sync was detected, false on error
   */
  virtual bool DoSync( unsigned int& frameNumber, unsigned int& seconds, unsigned int& microseconds );

private:
  AndroidVSync* mVSync;
};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_VSYNC_MONITOR_IMPL_H__
