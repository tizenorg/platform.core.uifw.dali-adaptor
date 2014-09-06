#ifndef __DALI_INTERNAL_ADAPTOR_EGL_FACTORY_IMPL_H__
#define __DALI_INTERNAL_ADAPTOR_EGL_FACTORY_IMPL_H__

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
#include <base/interfaces/egl-factory-interface.h>

namespace Dali
{
namespace Internal
{
namespace Adaptor
{
class EglImplementation;
class EglImageExtensions;
class EglSyncImplementation;

class EglFactory : public EglFactoryInterface
{
public:
  /**
   * Constructor
   */
  EglFactory();

  /**
   * Destructor
   */
  virtual ~EglFactory();

  /**
   * Create an EGL Implementation
   * @return[in] An implementation
   */
  EglInterface* Create();

  /**
   * Destroy the EGL Implementation
   */
  void Destroy();

  /**
   * Get an implementation if one has been created.
   * @return An implementation, or NULL if one has not yet been created.
   */
  EglInterface* GetImplementation();

  /**
   * Get the image extension
   */
  EglImageExtensions* GetImageExtensions();

  /**
   * Get the fence sync implementation
   * @return An implementation of fence sync
   */
  EglSyncImplementation* GetSyncImplementation();

private:
  /** Undefined */
  EglFactory(const EglFactory& rhs);
  EglFactory& operator=(const EglFactory& rhs);

private:
  EglImplementation* mEglImplementation;
  EglImageExtensions* mEglImageExtensions;
  EglSyncImplementation* mEglSync;
};

}
}
}

#endif //__DALI_INTERNAL_ADAPTOR_EGL_FACTORY_IMPL_H__
