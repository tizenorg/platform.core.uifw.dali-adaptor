#ifndef __DALI_INTERNAL_CLIPBOARD_EVENT_NOTIFIER_H__
#define __DALI_INTERNAL_CLIPBOARD_EVENT_NOTIFIER_H__

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
#include <string>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/math/vector2.h>

// INTERNAL INCLUDES
#include <dali/public-api/adaptor-framework/common/clipboard-event-notifier.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

typedef Dali::IntrusivePtr< ClipboardEventNotifier > ClipboardEventNotifierPtr;

/**
 * This class listens to Clipboard events.
 */
class ClipboardEventNotifier : public Dali::BaseObject
{
public:

  typedef Dali::ClipboardEventNotifier::ClipboardEventSignalV2 ClipboardEventSignalV2;

  // Creation

  /**
   * Create a ClipboardEventNotifier.
   * This should only be called once by the Window class.
   * @return A newly allocated clipboard-event-notifier.
   */
  static Dali::ClipboardEventNotifier New();

  // Public API

  /**
   * @copydoc Dali::ClipboardEventNotifier::GetContent() const
   */
  const std::string& GetContent() const;

  /**
   * Sets the selected content.
   * @param[in] content  A string that represents the content that has been selected.
   */
  void SetContent( const std::string& content );

  /**
   * Clears the stored content.
   */
  void ClearContent();

  /**
   * Called when content is selected in the clipboard.
   */
  void EmitContentSelectedSignal();

public: // Signals

  /**
   * @copydoc Dali::ClipboardEventNotifier::ContentSelectedSignal
   */
  ClipboardEventSignalV2& ContentSelectedSignal()
  {
    return mContentSelectedSignalV2;
  }

private:

  // Construction & Destruction

  /**
   * Constructor.
   */
  ClipboardEventNotifier();

  /**
   * Destructor.
   */
  virtual ~ClipboardEventNotifier();

  // Undefined
  ClipboardEventNotifier( const ClipboardEventNotifier& );
  ClipboardEventNotifier& operator=( ClipboardEventNotifier& );

private:

  std::string mContent;    ///< The current selected content.

  ClipboardEventSignalV2 mContentSelectedSignalV2;

public:

  // Helpers for public-api forwarding methods

  inline static Internal::Adaptor::ClipboardEventNotifier& GetImplementation(Dali::ClipboardEventNotifier& detector)
  {
    DALI_ASSERT_ALWAYS( detector && "ClipboardEventNotifier handle is empty" );

    BaseObject& handle = detector.GetBaseObject();

    return static_cast<Internal::Adaptor::ClipboardEventNotifier&>(handle);
  }

  inline static const Internal::Adaptor::ClipboardEventNotifier& GetImplementation(const Dali::ClipboardEventNotifier& detector)
  {
    DALI_ASSERT_ALWAYS( detector && "ClipboardEventNotifier handle is empty" );

    const BaseObject& handle = detector.GetBaseObject();

    return static_cast<const Internal::Adaptor::ClipboardEventNotifier&>(handle);
  }

};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_CLIPBOARD_EVENT_NOTIFIER_H__
