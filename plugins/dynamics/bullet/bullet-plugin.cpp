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

#define LOG_TAG "DALI_DYNAMICS"

// EXTERNAL INCLUDES
#include <dlog.h>
// TODO: Change this to use #pragma GCC diagnostic push / pop when the compiler is updated to 4.6.0+
#pragma GCC diagnostic ignored "-Wfloat-equal"
# include <bullet/btBulletDynamicsCommon.h>
# include "BulletSoftBody/btSoftRigidDynamicsWorld.h"
# include "BulletSoftBody/btSoftBodyHelpers.h"
#pragma GCC diagnostic error "-Wfloat-equal"

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include "bullet-dynamics-body.h"
#include "bullet-dynamics-joint.h"
#include "bullet-dynamics-shape.h"
#include "bullet-dynamics-world.h"

#include <dali/integration-api/dynamics/dynamics-shape-intf.h>

#define DEBUG_PRINTF(fmt, arg...)  LOGD(fmt, ##arg)

extern "C" DALI_EXPORT_API Dali::Integration::DynamicsWorld* CreateDynamicsWorld()
{
  DEBUG_PRINTF( "%s\n", __func__ );

  return new Dali::Plugin::BulletDynamicsWorld();
}

extern "C" DALI_EXPORT_API Dali::Integration::DynamicsBody* CreateDynamicsBody()
{
  DEBUG_PRINTF( "%s\n", __func__ );

  return new Dali::Plugin::BulletDynamicsBody();
}

extern "C" DALI_EXPORT_API Dali::Integration::DynamicsJoint* CreateDynamicsJoint()
{
  DEBUG_PRINTF( "%s\n", __func__ );

  return new Dali::Plugin::BulletDynamicsJoint();
}

extern "C" DALI_EXPORT_API Dali::Integration::DynamicsShape* CreateDynamicsShape()
{
  DEBUG_PRINTF( "%s\n", __func__ );

  return new Dali::Plugin::BulletDynamicsShape();
}

