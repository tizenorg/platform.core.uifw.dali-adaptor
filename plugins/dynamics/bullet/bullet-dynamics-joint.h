#ifndef __DALI_PLUGIN_BULLET_DYNAMICS_JOINT_H__
#define __DALI_PLUGIN_BULLET_DYNAMICS_JOINT_H__

/*
 Copyright (c) 2000-2013 Samsung Electronics Co., Ltd All Rights Reserved

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

// EXTERNAL INCLUDES
// INTERNAL INCLUDES
#include <dali/integration-api/dynamics/dynamics-joint-intf.h>

class btGeneric6DofSpringConstraint;
class btTypedConstraint;

namespace Dali
{

namespace Plugin
{

/*
 *
 */
class BulletDynamicsJoint : public Integration::DynamicsJoint
{
public:
  BulletDynamicsJoint();
  virtual ~BulletDynamicsJoint();

public: // From Dali::Integration::DynamicsJoint

  /// @copydoc Dali::Integration::DynamicsJoint::Initialize
  void Initialize( Integration::DynamicsBody* bodyA, const Vector3& positionA, const Quaternion& rotationA, const Vector3& offsetA,
                   Integration::DynamicsBody* bodyB, const Vector3& positionB, const Quaternion& rotationB, const Vector3& offsetB );

  /// @copydoc Dali::Integration::DynamicsJoint::SetLinearLimit
  void SetLimit( const int axisIndex, const float lowerLimit, const float upperLimit );

  /// @copydoc Dali::Integration::DynamicsJoint::EnableSpring
  void EnableSpring( int axisIndex, bool flag );

  /// @copydoc Dali::Integration::DynamicsJoint::SetSpringStiffness
  void SetSpringStiffness( int axisIndex, float stiffness );

  /// @copydoc Dali::Integration::DynamicsJoint::SetSpringDamping
  void SetSpringDamping( int axisIndex, float damping );

  /// @copydoc Dali::Integration::DynamicsJoint::SetSpringCenterPoint
  void SetSpringCenterPoint( int axisIndex, float ratio );

  /// @copydoc Dali::Integration::DynamicsJoint::EnableMotor
  void EnableMotor( int axisIndex, bool flag );

  /// @copydoc Dali::Integration::DynamicsJoint::SetMotorVelocity
  void SetMotorVelocity( int axisIndex, float velocity );

  /// @copydoc Dali::Integration::DynamicsJoint::SetMotorForce
  void SetMotorForce( int axisIndex, float force );

public:

  btTypedConstraint* GetConstraint() const;

private:

  btGeneric6DofSpringConstraint* mConstraint;

}; // class BulletDynamicsJoint

} // namespace Plugin

} // namespace Dali

#endif // __DALI_PLUGIN_BULLET_DYNAMICS_JOINT_H__
