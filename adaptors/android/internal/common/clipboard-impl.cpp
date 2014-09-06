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

// CLASS HEADER
#include "clipboard-impl.h"

// EXTERNAL INCLUDES

#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace Clipboard
{
 const char* const CBHM_WINDOW = "CBHM_XWIN";
 const char* const CBHM_MSG = "CBHM_MSG";
 const char* const CBHM_ITEM = "CBHM_ITEM";
 const char* const CBHM_cCOUNT = "CBHM_cCOUNT";
 const char* const CBHM_ERROR = "CBHM_ERROR";
 const char* const SET_ITEM = "SET_ITEM";
 const char* const SHOW = "show0";

/**
 * Send string to clipboard
 * Function to send messages to the Clipboard (cbhm) as no direct API available
 * Reference elementary/src/modules/ctxpopup_copypasteUI/cbhm_helper.c
 */
bool SetItem( std::string &itemData )
{
  return false;
}

/*
 * Get string at given index of clipboard
 */
std::string GetItem( unsigned int index )  // change string to a Dali::Text object.
{
  std::string emptyString( "" );
  return emptyString;
}

/*
 * Get number of items in clipboard
 */
unsigned int NumberOfItems()
{
  int count = -1;
  return count;
}

/**
 * Show clipboard window
 * Function to send message to show the Clipboard (cbhm) as no direct API available
 * Reference elementary/src/modules/ctxpopup_copypasteUI/cbhm_helper.c
 */
void ShowClipboard()
{
}

} // namespace Clipboard

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
