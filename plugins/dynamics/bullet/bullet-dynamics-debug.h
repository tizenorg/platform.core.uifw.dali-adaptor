#ifndef __DALI_PLUGIN_BULLET_DYNAMICS_DEBUG_H__
#define __DALI_PLUGIN_BULLET_DYNAMICS_DEBUG_H__

/*
Copyright (c) 2000-2013 Samsung Electronics Co., Ltd All Rights Reserved

This file is part of Dali Demo

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

// EXTERNAL HEADERS
#include <dali/integration-api/dynamics/dynamics-debug-vertex.h>
// TODO: Change this to use #pragma GCC diagnostic push / pop when the compiler is updated to 4.6.0+
#pragma GCC diagnostic ignored "-Wfloat-equal"
#include <bullet/btBulletDynamicsCommon.h>
#pragma GCC diagnostic error "-Wfloat-equal"

namespace Dali
{

namespace Plugin
{

/**
 * Debug draw class, BulletPhysics engine will invoke methods on this object to render debug information.
 * Debug lines are collated into a container and then rendered as a single array during Core::Render
 */
class BulletDebugDraw : public btIDebugDraw
{
public:
  /**
   * Constructor
   */
  BulletDebugDraw();

  /**
   * Destructor
   */
  virtual ~BulletDebugDraw();

  virtual void drawLine(const btVector3& from,const btVector3& to,const btVector3& color);
  virtual void drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color);
  virtual void reportErrorWarning(const char* warningString);
  virtual void draw3dText(const btVector3& location,const char* textString);
  virtual void setDebugMode(int debugMode);
  virtual int  getDebugMode() const;

  void ClearVertices();
  const Integration::DynamicsDebugVertexContainer& GetVertices() const;

private:

  int mDebugMode;
  Integration::DynamicsDebugVertexContainer mVertices;

}; // class BulletDebugDraw

} // namespace Plugin

} // namespace Dali

#endif /* __DALI_PLUGIN_BULLET_DYNAMICS_DEBUG_H__ */
