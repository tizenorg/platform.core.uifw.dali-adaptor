#ifndef __DALI_WATCH_TIME_H__
#define __DALI_WATCH_TIME_H__

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali/public-api/dali-core.h>

namespace Dali
{
/**
 * @addtogroup dali_adaptor_framework
 * @{
 */

class DALI_IMPORT_API WatchTime
{
public:

  /**
   * @brief Construct an empty handle
   * @SINCE_1_1.36
   */
  WatchTime();

  /**
   * @brief return current hour
   *
   * @SINCE_1_1.36
   * @pre The WatchTime needs to be initialized.
   */
  int GetHour();

  /**
   * @brief return current hour24
   *
   * @SINCE_1_1.36
   * @pre The WatchTime needs to be initialized.
   */
  int GetHour24();

  /**
   * @brief return current minute
   *
   * @SINCE_1_1.36
   * @pre The WatchTime needs to be initialized.
   */
  int GetMinute();

  /**
   * @brief return current second
   *
   * @SINCE_1_1.36
   * @pre The WatchTime needs to be initialized.
   */
  int GetSecond();

public: // Not intended for application developers
  DALI_INTERNAL WatchTime(void *time_handle);

private:
  void *mTimeHandle;    // Internal pointer
};

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_WATCH_TIME_H__
