#ifndef __DALI_PLUGIN_BULLET_DYNAMICS_BODY_H__
#define __DALI_PLUGIN_BULLET_DYNAMICS_BODY_H__

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
#include <dali/integration-api/dynamics/dynamics-body-intf.h>

class btCollisionObject;

namespace Dali
{

namespace Integration
{

struct DynamicsBodySettings;
class DynamicsShape;

} // namespace Integration

namespace Plugin
{

class BulletDynamicsWorld;

/*
 *
 */
class BulletDynamicsBody : public Integration::DynamicsBody
{
public:
  BulletDynamicsBody();
  virtual ~BulletDynamicsBody();

public: // From Dali::Integration::DynamicsBody

  ///@ copydoc Dali::DynamicsBody::Initialize
  void Initialize( const Integration::DynamicsBodySettings& bodySettings, Integration::DynamicsShape* shape,
                   Integration::DynamicsWorld& dynamicsWorld,
                   const Vector3& startPosition, const Quaternion& startRotation );

  ///@ copydoc Dali::DynamicsBody::SetMass
  void SetMass( float mass );

  ///@ copydoc Dali::DynamicsBody::SetElasticity
  void SetElasticity( float elasticity );

  /// @copydoc Dali::DynamicsBody::SetLinearVelocity
  void SetLinearVelocity( const Vector3& velocity );

  /// @copydoc Dali::DynamicsBody::GetLinearVelocity
  Vector3 GetLinearVelocity() const;

  /// @copydoc Dali::DynamicsBody::SetAngularVelocity
  void SetAngularVelocity( const Vector3& velocity );

  /// @copydoc Dali::DynamicsBody::GetAngularVelocity
  Vector3 GetAngularVelocity() const;

  /// @copydoc Dali::DynamicsBody::SetKinematic
  void SetKinematic( bool flag );

  /// @copydoc Dali::DynamicsBody::IsKinematic
  bool IsKinematic() const;

  /// @copydoc Dali::DynamicsBody::SetSleepEnabled
  void SetSleepEnabled( bool flag );

  /// @copydoc Dali::DynamicsBody::WakeUp
  void WakeUp();

  /// @copydoc Dali::DynamicsBody::AddAnchor
  void AddAnchor( unsigned int index, const Integration::DynamicsBody* anchorBody, bool collisions );

  /// @copydoc Dali::DynamicsBody::ConserveVolume
  void ConserveVolume( bool flag );

  /// @copydoc Dali::DynamicsBody::ConserveShape
  void ConserveShape( bool flag );

  /// @copydoc Dali::DynamicsBody::GetCollisionGroup
  short int GetCollisionGroup() const;

  /// @copydoc Dali::DynamicsBody::SetCollisionGroup
  void SetCollisionGroup( short int collisionGroup );

  /// @copydoc Dali::DynamicsBody::GetCollisionMask
  short int GetCollisionMask() const;

  /// @copydoc Dali::DynamicsBody::SetCollisionMask
  void SetCollisionMask( short int collisionMask );

  int GetType() const;

  /// @copydoc Dali::DynamicsBody::IsActive
  bool IsActive() const;

  /// @copydoc Dali::Integration::DynamicsBody::SetTransform
  void SetTransform( const Vector3& position, const Quaternion& rotation );

  /// @copydoc Dali::Integration::GetTransform
  void GetTransform( Vector3& position, Quaternion& rotation );

  /// @copydoc Dali::Integration::GetSoftVertices
  void GetSoftVertices( MeshData::VertexContainer& vertices ) const;

public:

  btCollisionObject* GetBody() const;

private:
  /**
   * Create a rigid body
   */
  void CreateRigidBody( const Integration::DynamicsBodySettings& bodySettings, Dali::Integration::DynamicsShape* shape,
                        const Vector3& startPosition, const Quaternion& startRotation );

  /**
   * Create a soft body
   */
  void CreateSoftBody( const Integration::DynamicsBodySettings& bodySettings, Dali::Integration::DynamicsShape* shape,
                       const Vector3& startPosition, const Quaternion& startRotation );

private:
  Dali::Integration::DynamicsBodySettings* mSettings;
  btCollisionObject* mBody;
  bool mConserveVolume:1;
  bool mConserveShape:1;
  bool mKinematic:1;
  bool mActivationState:1;

  short int mCollisionGroup;
  short int mCollisionMask;

  BulletDynamicsWorld* mWorld;

}; // class BulletDynamicsBody

} // namespace Plugin

} // namespace Dali

#endif // __DALI_PLUGIN_BULLET_DYNAMICS_BODY_H__
