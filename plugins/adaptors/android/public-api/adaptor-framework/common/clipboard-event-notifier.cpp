/*
Copyright (c) 2000-2013 Samsung Electronics Co., Ltd All Rights Reserved

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
#include <dali/public-api/adaptor-framework/common/clipboard-event-notifier.h>

// INTERNAL INCLUDES
#include <internal/common/clipboard-event-notifier-impl.h>

namespace Dali
{

const char* const ClipboardEventNotifier::SIGNAL_CONTENT_SELECTED( "content-selected" );

ClipboardEventNotifier::ClipboardEventNotifier()
{
}

ClipboardEventNotifier::~ClipboardEventNotifier()
{
}

const std::string& ClipboardEventNotifier::GetContent() const
{
  return Internal::Adaptor::ClipboardEventNotifier::GetImplementation(*this).GetContent();
}

//ClipboardEventNotifier::ClipboardEventSignal ClipboardEventNotifier::SignalContentSelected()
//{
//  return Internal::Adaptor::ClipboardEventNotifier::GetImplementation(*this).SignalContentSelected();
//}

ClipboardEventNotifier::ClipboardEventSignalV2& ClipboardEventNotifier::ContentSelectedSignal()
{
  return Internal::Adaptor::ClipboardEventNotifier::GetImplementation(*this).ContentSelectedSignal();
}

ClipboardEventNotifier::ClipboardEventNotifier( Internal::Adaptor::ClipboardEventNotifier* notifier )
: BaseHandle( notifier )
{
}

} // namespace Dali

