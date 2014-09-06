#ifndef __DALI_INTERNAL_CLIPBOARD_H__
#define __DALI_INTERNAL_CLIPBOARD_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/adaptor-framework/common/clipboard.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

/**
 * Implementation of the Clip Board
 */
namespace Clipboard
{

/**
 * @copydoc Dali::Clipboard::SetItem()
 */
bool SetItem( std::string &itemData);

/**
 * @copydoc Dali::Clipboard::GetItem()
 */
std::string GetItem( unsigned int index );

/**
 * @copydoc Dali::Clipboard::NumberOfClipboardItems()
 */
unsigned int NumberOfItems();

/**
 * @copydoc Dali::Clipboard::ShowClipboard()
 */
void ShowClipboard();

} // namespace Clipboard

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_CLIPBOARD_H__
