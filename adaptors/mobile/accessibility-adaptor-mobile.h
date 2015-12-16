#ifndef __DALI_INTERNAL_ACCESSIBILITY_ADAPTOR_H__
#define __DALI_INTERNAL_ACCESSIBILITY_ADAPTOR_H__

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

// INTERNAL INCLUDES
#include <accessibility-adaptor.h>
#include <accessibility-action-handler.h>
#include <accessibility-gesture-handler.h>
#include <indicator-interface.h>
#include <accessibility-gesture-detector.h>
#include <accessibility-adaptor-impl.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

/**
 * This class detects to accessibility action
 */
class AccessibilityAdaptorMobile : public AccessibilityAdaptor
{
public:

  /**
   * Constructor.
   */
  AccessibilityAdaptor();

  /**
   * @copydoc Dali::AccessibilityAdaptor::HandleActionNextEvent()
   */
  bool HandleActionNextEvent( bool allowEndFeedback );

  /**
   * @copydoc Dali::AccessibilityAdaptor::HandleActionPreviousEvent()
   */
  bool HandleActionPreviousEvent( bool allowEndFeedback );

  /**
   * @copydoc Dali::AccessibilityAdaptor::HandleActionActivateEvent()
   */
  bool HandleActionActivateEvent();

  /**
   * @copydoc Dali::AccessibilityAdaptor::HandleActionReadEvent()
   */
  bool HandleActionReadEvent( unsigned int x, unsigned int y, bool allowReadAgain );

  /**
   * @copydoc Dali::AccessibilityAdaptor::HandleActionReadNextEvent()
   */
  bool HandleActionReadNextEvent( bool allowEndFeedback );

  /**
   * @copydoc Dali::AccessibilityAdaptor::HandleActionReadPreviousEvent()
   */
  bool HandleActionReadPreviousEvent( bool allowEndFeedback );

  /**
   * @copydoc Dali::AccessibilityAdaptor::HandleActionUpEvent()
   */
  bool HandleActionUpEvent();

  /**
   * @copydoc Dali::AccessibilityAdaptor::HandleActionDownEvent()
   */
  bool HandleActionDownEvent();

private:

  /**
   * Destructor.
   */
  virtual ~AccessibilityAdaptor();

  // Undefined
  AccessibilityAdaptor( const AccessibilityAdaptor& );
  AccessibilityAdaptor& operator=( AccessibilityAdaptor& );

public:

  // Helpers for public-api forwarding methods

  inline static Internal::Adaptor::AccessibilityAdaptor& GetImplementation(Dali::AccessibilityAdaptor& adaptor)
  {
    DALI_ASSERT_ALWAYS( adaptor && "AccessibilityAdaptor handle is empty" );

    BaseObject& handle = adaptor.GetBaseObject();

    return static_cast<Internal::Adaptor::AccessibilityAdaptor&>(handle);
  }

  inline static const Internal::Adaptor::AccessibilityAdaptor& GetImplementation(const Dali::AccessibilityAdaptor& adaptor)
  {
    DALI_ASSERT_ALWAYS( adaptor && "AccessibilityAdaptor handle is empty" );

    const BaseObject& handle = adaptor.GetBaseObject();

    return static_cast<const Internal::Adaptor::AccessibilityAdaptor&>(handle);
  }

};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_ACCESSIBILITY_ADAPTOR_H__
