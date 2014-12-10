#ifndef __DYNAMICS_FACTORY_H__
#define __DYNAMICS_FACTORY_H__

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
#include <dali/integration-api/dynamics/dynamics-factory-intf.h>

// INTERNAL INCLUDES

namespace Dali
{

namespace SlpPlatform
{

/**
 *
 */
class DynamicsFactory : public Integration::DynamicsFactory
{
public:
  /**
   * Constructor
   */
  DynamicsFactory();

  /**
   * Destructor
   */
  virtual ~DynamicsFactory();

  /**
   * @copydoc Dali::Integration::DynamicsFactory::InitializeDynamics
   */
  bool InitializeDynamics( const Integration::DynamicsWorldSettings& worldSettings );

  /**
   * @copydoc Dali::Integration::DynamicsFactory::TerminateDynamics
   */
  void TerminateDynamics();

  /**
   * @copydoc Dali::Integration::DynamicsFactory::CreateDynamicsWorld
   */
  Integration::DynamicsWorld* CreateDynamicsWorld();

  /**
   * @copydoc Dali::Integration::DynamicsFactory::CreateDynamicsBody
   */
  virtual Integration::DynamicsBody* CreateDynamicsBody();

  /**
   * @copydoc Dali::Integration::DynamicsFactory::CreateDynamicsJoint
   */
  virtual Integration::DynamicsJoint* CreateDynamicsJoint();

  /**
   * @copydoc Dali::Integration::DynamicsFactory::CreateDynamicsShape
   */
  virtual Integration::DynamicsShape* CreateDynamicsShape();

private:
  void* mHandle;    ///< The handle to the open shared object library

  typedef Integration::DynamicsWorld* (*CreateDynamicsWorldFunction)();
  typedef Integration::DynamicsBody* (*CreateDynamicsBodyFunction)();
  typedef Integration::DynamicsJoint* (*CreateDynamicsJointFunction)();
  typedef Integration::DynamicsShape* (*CreateDynamicsShapeFunction)();

  CreateDynamicsWorldFunction mCreateDynamicsWorld;
  CreateDynamicsBodyFunction mCreateDynamicsBody;
  CreateDynamicsJointFunction mCreateDynamicsJoint;
  CreateDynamicsShapeFunction mCreateDynamicsShape;

}; // class DynamicsFactory

} // namespace SlpPlatform

} // namespace Dali

#endif // __DYNAMICS_FACTORY_H__
