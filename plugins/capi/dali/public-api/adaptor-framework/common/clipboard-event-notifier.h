#ifndef __DALI_CLIPBOARD_EVENT_NOTIFIER_H__
#define __DALI_CLIPBOARD_EVENT_NOTIFIER_H__

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

// EXTERNAL INCLUDES
#include <boost/function.hpp>

#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/signals/dali-signal-v2.h>

namespace Dali DALI_IMPORT_API
{

namespace Internal DALI_INTERNAL
{
namespace Adaptor
{
class ClipboardEventNotifier;
}
}

/**
 * The ClipboardEventNotifier provides signals when clipboard events are received from the device.
 */
class ClipboardEventNotifier : public BaseHandle
{
public:

  // Typedefs

  // Clipboard event

  typedef SignalV2< void ( ClipboardEventNotifier& ) > ClipboardEventSignalV2;

  //typedef boost::signals2::signal_type< void ( ClipboardEventNotifier& ), DummyMutex >::type ClipboardEventSignalType;
  //typedef boost::function<void ( ClipboardEventNotifier& )> ClipboardEventCallbackType;
  //typedef Signal<ClipboardEventSignalType, ClipboardEventCallbackType> ClipboardEventSignal;

  // Signal Names
  static const char* const SIGNAL_CONTENT_SELECTED;

  /**
   * Create an uninitialized handle.
   * This can be initialized by getting the notifier from Dali::Adaptor.
   */
  ClipboardEventNotifier();

  /**
   * Virtual Destructor.
   */
  virtual ~ClipboardEventNotifier();

  /**
   * Returns the selected content.
   * @return A reference to the string representing the selected content.
   */
  const std::string& GetContent() const;

public:  // Signals

  /**
   * This is emitted when content is selected from the clipboard
   * A callback of the following type may be connected:
   * @code
   *   void YourCallback( ClipboardEventNotifier& notifier );
   * @endcode
   * @return The signal to connect to.
   */
  //ClipboardEventSignal SignalContentSelected();
  ClipboardEventSignalV2& ContentSelectedSignal();

public: // Not intended for application developers

  /**
   * This constructor is used by ClipboardEventNotifier::Get().
   * @param[in] notifier A pointer to the drag and drop notifier.
   */
  ClipboardEventNotifier( Internal::Adaptor::ClipboardEventNotifier* notifier );
};

} // namespace Dali

#endif // __DALI_CLIPBOARD_EVENT_NOTIFIER_H__

