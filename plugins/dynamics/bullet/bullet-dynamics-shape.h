#ifndef __DALI_PLUGIN_BULLET_DYNAMICS_SHAPE_H__
#define __DALI_PLUGIN_BULLET_DYNAMICS_SHAPE_H__

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

// INTERNAL INCLUDES
#include <dali/integration-api/dynamics/dynamics-shape-intf.h>

#include <dali/public-api/geometry/mesh-data.h>

class btCollisionShape;

namespace Dali
{

struct Vector3;

namespace Plugin
{

/*
 *
 */
class BulletDynamicsShape : public Integration::DynamicsShape
{
public:
  BulletDynamicsShape();
  virtual ~BulletDynamicsShape();

public:
  // @copydoc Dali::Integration::DynamicsShape::Initialize(int,const Vector3&)
  void Initialize( int type, const Vector3& dimensions );

  // @copydoc Dali::Integration::DynamicsShape::Initialize(int, const MeshData&)
  void Initialize( int type, const MeshData::VertexContainer& vertices, const MeshData::FaceIndices& faceIndices );

public:
  btCollisionShape* GetShape();

private:

  btCollisionShape*          mShape;

public:

  MeshData::VertexContainer* mVertices;
  MeshData::FaceIndices*     mFaces;

}; // class BulletDynamicsShape

} // namespace Plugin

} // namespace Dali

#endif // __DALI_PLUGIN_BULLET_DYNAMICS_SHAPE_H__
