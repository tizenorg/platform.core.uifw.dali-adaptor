#ifndef __DALI_STYLE_CHANGE_H__
#define __DALI_STYLE_CHANGE_H__

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

/**
 * @addtogroup CAPI_DALI_FRAMEWORK
 * @{
 */

namespace Dali DALI_IMPORT_API
{

/**
 * This structure is used when any style changes occur and contains information about what exactly
 * has changed.
 */
struct StyleChange
{
  // Data

  bool defaultFontChange:1;     ///< Denotes that the default font has changed.
  bool defaultFontSizeChange:1; ///< Denotes that the default font size has changed.
  bool themeChange:1;           ///< Denotes that the theme has changed.
  std::string themeFilePath;    ///< Contains the path to the new theme file.

  // Construction

  /**
   * Default Constructor.
   */
  StyleChange()
  : defaultFontChange(false),
    defaultFontSizeChange(false),
    themeChange(false)
  {
  }
};

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_STYLE_CHANGE_H__
