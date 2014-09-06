#ifndef  __DALI_CLIPBOARD_H__
#define  __DALI_CLIPBOARD_H__

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
#include <dali/public-api/signals/dali-signal-v2.h>
#include <dali/public-api/math/rect.h>

namespace Dali DALI_IMPORT_API
{

/**
 * This namespace is provided for application developers to copy text to clipboard and paste text from clipboard
 */
namespace Clipboard
{

// Functions

/**
 * Send the given string to the clipboard
 * @param[in] itemData string to send to clip board
 * @return bool true if the internal clip board sending was successful.
 */
bool SetItem( std::string& itemData );

/**
 * Retreive the string at the given index in the clip board.
 * @param[in] index item in clipboard list to retrieve
 * @return string the text item at the current index.
 */
std::string GetItem( unsigned int index );

/**
 * Returns the number of item currently in the clipboard
 * @return unsigned int number of clipboard items
 */
unsigned int NumberOfItems();

/**
 * Show the clipboard window
 */
void ShowClipboard();

} // namespace Clipboard

} // namespace Dali

#endif // __DALI_CLIPBOARD_H__
