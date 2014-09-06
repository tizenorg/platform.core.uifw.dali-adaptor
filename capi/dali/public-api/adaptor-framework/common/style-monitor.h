#ifndef __DALI_STYLE_MONITOR_H__
#define __DALI_STYLE_MONITOR_H__

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

// EXTERNAL INCLUDES
#include <boost/function.hpp>
#include <string>

#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/signals/dali-signal-v2.h>

// INTERNAL INCLUDES
#include <dali/public-api/adaptor-framework/common/style-change.h>

namespace Dali DALI_IMPORT_API
{

namespace Internal DALI_INTERNAL
{
namespace Adaptor
{
class StyleMonitor;
}
}

/**
 * This is a handle to the adaptor's style monitor which holds the platform's style information.
 * It provides a signal when any aspect of the default style changes on the device.
 * @see Adaptor::GetStyleMonitor
 */
class StyleMonitor : public BaseHandle
{
public: // Typedefs

  // StyleChange Signal
  typedef SignalV2< void (StyleMonitor, StyleChange) > StyleChangeSignalV2;

public: // Creation & Destruction

  /**
   * Create an uninitialized StyleMonitor handle; this can be set by
   * retrieving the style monitor from the Adaptor or the Application
   * classes.  Calling member functions when uninitialized is not
   * allowed.
   */
  StyleMonitor();

  /**
   * Creates a copy of the handle.  The copy will point to the same implementation as the original.
   * @param[in]  monitor  The Style Monitor to copy from.
   */
  StyleMonitor(const StyleMonitor& monitor);

  /**
   * Retrieve the initialized instance of the StyleMonitor.
   * @return Handle to StyleMonitor.
   */
  static StyleMonitor Get();

  /**
   * Destructor.
   */
  virtual ~StyleMonitor();

  /**
   * Downcast an Object handle to StyleMonitor handle. If handle
   * points to a StyleMonitor object the downcast produces a valid
   * handle. If not the returned handle is left uninitialized.
   * @param[in] handle to An object @return handle to a Timer object
   * or an uninitialized handle
   */
  static StyleMonitor DownCast( BaseHandle handle );

  /**
   * @copydoc Dali::BaseHandle::operator=
   */
  using BaseHandle::operator=;

public: // Style Information

  /**
   * Retrieves the default font family.
   * @return The default font family.
   */
  std::string GetDefaultFontFamily() const;

  /**
   * Retrieves the default font size
   * @return The default font size.
   */
  float GetDefaultFontSize() const;

  /**
   * Retrieves the user defined Theme.
   * @return The user defined Theme.
   */
  const std::string& GetTheme() const;

  /**
   * Sets an user defined Theme.
   */
  void SetTheme(const std::string& themeFilePath);

public: // Signals

  /**
   * This signal is emitted whenever the style changes on the device.
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName(StyleMonitor styleMonitor, StyleChange change);
   * @endcode
   * @return The signal to connect to.
   */
  StyleChangeSignalV2& StyleChangeSignal();

public: // Operators

  /**
   * Assignment operator.
   * The handle points to the same implementation as the one being copied from.
   * @param[in]  monitor  The Style Monitor to copy from.
   */
  StyleMonitor& operator=(const StyleMonitor& monitor);


public: // Not intended for application developers
  /**
   * This constructor is used internally to create a handle from an object pointer.
   * @param [in] styleMonitor A pointer the internal style monitor.
   */
  explicit DALI_INTERNAL StyleMonitor(Internal::Adaptor::StyleMonitor* styleMonitor);
};

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_STYLE_MONITOR_H__
