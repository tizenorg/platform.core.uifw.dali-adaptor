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
#include <dali/public-api/adaptor-framework/common/clipboard.h>

// INTERNAL INCLUDES
#include <internal/common/clipboard-impl.h>

namespace Dali
{

namespace Clipboard
{

bool SetItem( std::string &itemData)
{
  return Internal::Adaptor::Clipboard::SetItem( itemData );
}

std::string GetItem( unsigned int index )
{
  return Internal::Adaptor::Clipboard::GetItem( index );
}

unsigned int NumberOfItems()
{
  return Internal::Adaptor::Clipboard::NumberOfItems();
}

void ShowClipboard()
{
  Internal::Adaptor::Clipboard::ShowClipboard();
}

} // namespace Clipboard

} // namespace Dali
