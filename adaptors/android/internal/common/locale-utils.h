#ifndef __DALI_INTERNAL_LOCALE_UTILS_H__
#define __DALI_INTERNAL_LOCALE_UTILS_H__

/*
Copyright (c) 2000-2012 Samsung Electronics Co., Ltd All Rights Reserved

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
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/adaptor-framework/common/virtual-keyboard.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace Locale
{

Dali::VirtualKeyboard::TextDirection GetTextDirection( std::string locale );

} // namespace Locale

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_LOCALE_UTILS_H__
