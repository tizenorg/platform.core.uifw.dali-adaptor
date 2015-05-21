#ifndef __DALI_INPUT_MEHTOD_H__
#define __DALI_INPUT_MEHTOD_H__

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
#include <dali/public-api/common/dali-common.h>

namespace Dali
{

namespace DALI_IMPORT_API InputMethod
{

/**
 * @brief Mapping of Action Button functions to platform specific functions.
 *
 */

  /**
   * @brief Specifies what the Input Method "action" button functionality is set to.
   *
   * The 'Action' button is traditionally the [RETURN] or [DONE] button.
   *
   * Not all these actions are supported by all systems
   *
   * Setting a custom label will still require one of these actions to be set.
   */
  enum ActionButton
  {
    ACTION_DEFAULT,
    ACTION_DONE,
    ACTION_GO,
    ACTION_JOIN,
    ACTION_LOGIN,
    ACTION_NEXT,
    ACTION_PREVIOUS,
    ACTION_SEARCH,
    ACTION_SEND,
    ACTION_SIGNIN,
    ACTION_UNSPECIFIED,
    ACTION_NONE
  };

  /**
   * @brief Settings that can be changed in the system Input Method
   *
   * Not all these settings are supported by all systems
   */
  enum Settings
  {
    ACTION_BUTTON_FUNCTION, // ActionButton
    ENABLE_PREDICATION,     // true/false
    ROTATE_TO,              // int
    AUTO_CAPITALISE,        // true/false
    AUTO_COMPLETE,          // true/false
    AUTO_CORRECT            // true/false
  };

} // namespace InputMethod

} // namespace Dali

#endif // __DALI_INPUT_MEHTOD_H__
