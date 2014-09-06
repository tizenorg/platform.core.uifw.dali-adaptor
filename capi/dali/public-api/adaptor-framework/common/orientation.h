#ifndef __DALI_ORIENTATION_H__
#define __DALI_ORIENTATION_H__

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

#include <dali/public-api/signals/dali-signal-v2.h>
#include <dali/public-api/object/base-handle.h>

namespace Dali DALI_IMPORT_API
{

namespace Internal DALI_INTERNAL
{
namespace Adaptor
{
class Orientation;
}
}

/**
 * Orientation allows the user to determine the orientation of the device.
 * A signal is emitted whenever the orientation changes.
 * Dali applications have full control over visual layout when the device is rotated
 * i.e. the application developer decides which UI controls to rotate, if any.
 */
class Orientation : public BaseHandle
{
public:

  typedef SignalV2< void (Orientation) > OrientationSignalV2;

  //typedef boost::signals2::signal_type< void (Orientation), DummyMutex >::type OrientationSignalType;

  //typedef boost::function<void (Orientation)> OrientationCallbackType;

  //typedef Signal<OrientationSignalType, OrientationCallbackType> OrientationSignal;

  /**
   * Create an unintialized handle. This can be initialized by calling
   * Dali::Application::GetOrientation()
   */
  Orientation();

  /**
   * Virtual Destructor
   */
  virtual ~Orientation();

  /**
   * @copydoc Dali::BaseHandle::operator=
   */
  using BaseHandle::operator=;


  /**
   * Returns the orientation of the device in degrees.
   * This is one of four discrete values, in degrees clockwise: 0, 90, 180, & 270
   * For a device with a portrait form-factor:
   *   0 indicates that the device is in the "normal" portrait orientation.
   *   90 indicates that device has been rotated clockwise, into a landscape orientation.
   * @return The orientation in degrees clockwise.
   */
  int GetDegrees() const;

  /**
   * Returns the orientation of the device in radians.
   * This is one of four discrete values, in radians clockwise: 0, PI/2, PI, & 3xPI/2
   * For a device with a portrait form-factor:
   *   0 indicates that the device is in the "normal" portrait orientation.
   *   PI/2 indicates that device has been rotated clockwise, into a landscape orientation.
   * @return The orientation in radians clockwise.
   */
  float GetRadians() const;

  /**
   * The user should connect to this signal so that they can be notified whenever
   * the orientation of the device changes.
   * @return The orientation change signal.
   */
  //OrientationSignal SignalChange();
  OrientationSignalV2& ChangedSignal();

public: // Not intended for application developers
  /**
   * This constructor is used by Dali::Application::GetOrientation().
   * @param[in] orientation A pointer to the orientation object
   */
  explicit DALI_INTERNAL Orientation( Internal::Adaptor::Orientation* orientation );
};

} // namespace Dali

#endif // __DALI_ORIENTATION_H__
