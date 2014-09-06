#ifndef __DALI_INTERNAL_ORIENTATION_H__
#define __DALI_INTERNAL_ORIENTATION_H__

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
#include <cmath>
#include <dali/public-api/common/constants.h>
#include <dali/public-api/object/base-object.h>

// INTERNAL INCLUDES
#include <dali/public-api/adaptor-framework/common/orientation.h>
#include <internal/common/rotation-observer.h>

namespace Dali
{
class Adaptor;

namespace Internal
{

namespace Adaptor
{
class Window;
class Orientation;

typedef Dali::IntrusivePtr<Orientation> OrientationPtr;

class Orientation : public BaseObject, public RotationObserver
{
public:

  typedef Dali::Orientation::OrientationSignalV2 OrientationSignalV2;

  static Orientation* New(Window* window);

  /**
   * Constructor
   */
  Orientation(Window* window);

protected:
  /**
   * Destructor
   */
  virtual ~Orientation();

public:
  /**
   * Set the adaptor for basic setup
   * @param[in] adaptor The adaptor
   */
  void SetAdaptor(Dali::Adaptor& adaptor);

  /**
   * Returns the actual orientation in degrees
   * @return The device's orientation
   */
  int GetDegrees() const;

  /**
   * Returns the actual orientation in radians
   * @return The device's orientation
   */
  float GetRadians() const;

public: // Signals

  /**
   * @copydoc Dali::Orientation::ChangedSignal()
   */
  OrientationSignalV2& ChangedSignal();

private:
  /**
   * @copydoc Dali::Internal::Adaptor::RotationObserver::OnRotationPrepare()
   */
  virtual void OnRotationPrepare( const RotationEvent& rotation );

  /**
   * @copydoc Dali::Internal::Adaptor::RotationObserver::OnRotationRequest()
   */
  virtual void OnRotationRequest( );

  // Undefined
  Orientation(const Orientation&);
  Orientation& operator=(Orientation&);

private:
  /**
   * Signals and sends event of orientation change.
   */
  void EmitOrientationChange();

private:

  Window*                                  mWindow;

  OrientationSignalV2 mChangedSignal;

  int                                      mOrientation;
  int                                      mWindowWidth;
  int                                      mWindowHeight;
};

inline Orientation& GetImplementation (Dali::Orientation& orientation)
{
  DALI_ASSERT_ALWAYS(orientation && "Orientation handle is empty");

  BaseObject& handle = orientation.GetBaseObject();

  return static_cast<Orientation&>(handle);
}

inline const Orientation& GetImplementation(const Dali::Orientation& orientation)
{
  DALI_ASSERT_ALWAYS(orientation && "Orientation handle is empty");

  const BaseObject& handle = orientation.GetBaseObject();

  return static_cast<const Orientation&>(handle);
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_ORIENTATION_H__
