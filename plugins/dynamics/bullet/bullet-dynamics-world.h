#ifndef __DALI_PLUGIN_BULLET_DYNAMICS_WORLD_H__
#define __DALI_PLUGIN_BULLET_DYNAMICS_WORLD_H__

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
#include <dali/integration-api/dynamics/dynamics-world-intf.h>

// forward declarations
class btCollisionWorld;
class btConstraintSolver;
class btBroadphaseInterface;
class btCollisionDispatcher;
class btCollisionConfiguration;
class btSoftBodyWorldInfo;

namespace Dali
{

namespace Integration
{

struct DynamicsWorldSettings;
class DynamicsBody;
class DynamicsJoint;

} // namespace Integration

namespace Plugin
{

class BulletDebugDraw;

/*
 *
 */
class BulletDynamicsWorld : public Dali::Integration::DynamicsWorld
{
public:
  BulletDynamicsWorld();
  virtual ~BulletDynamicsWorld();

  /// @copydoc Dali::Integration::DynamicsWorld::InitializeWorld
  void Initialize( const Dali::Integration::DynamicsWorldSettings& worldSettings );

  /// @copydoc Dali::Integration::DynamicsWorld::AddBody
  void AddBody( Dali::Integration::DynamicsBody* body );

  /// @copydoc Dali::Integration::DynamicsWorld::RemoveBody
  void RemoveBody( Dali::Integration::DynamicsBody* body );

  /// @copydoc Dali::Integration::DynamicsWorld::AddJoint
  void AddJoint( Dali::Integration::DynamicsJoint* joint );

  /// @copydoc Dali::Integration::DynamicsWorld::RemoveJoint
  void RemoveJoint( Dali::Integration::DynamicsJoint* joint );

  /// @copydoc Dali::Integration::DynamicsWorld::SetGravity
  void SetGravity( const Vector3& gravity );

  /// @copydoc Dali::DynamicsWorld::SetDebugDrawMode
  void SetDebugDrawMode(int mode);

  /// @copydoc Dali::Integration::DynamicsWorld::DebugDraw
  const Integration::DynamicsDebugVertexContainer& DebugDraw();

  /// @copydoc Dali::Integration::DynamicsWorld::Update
  void Update( float elapsedSeconds );

  /// @copydoc Dali::Integration::DynamicsWorld::CheckForCollisions
  void CheckForCollisions( Integration::CollisionDataContainer& contacts );

public:

  float GetWorldScale() const;

  btSoftBodyWorldInfo* GetSoftBodyWorldInfo();

private:
  Dali::Integration::DynamicsWorldSettings* mSettings;
  btCollisionWorld* mDynamicsWorld;
  btConstraintSolver* mSolver;
  btBroadphaseInterface* mBroadphase;
  btCollisionDispatcher* mDispatcher;
  btCollisionConfiguration* mCollisionConfiguration;
  btSoftBodyWorldInfo* mSoftBodyWorldInfo;

  BulletDebugDraw* mDebugDraw;
}; // class BulletDynamicsWorld

} // namespace Plugin

} // namespace Dali

#endif // __DALI_PLUGIN_BULLET_DYNAMICS_WORLD_H__
