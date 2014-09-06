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

// CLASS HEADER
#include "egl-factory.h"

// INTERNAL INCLUDES
#include <internal/common/gl/egl-implementation.h>
#include <internal/common/gl/egl-image-extensions.h>
#include <internal/common/gl/egl-sync-implementation.h>

namespace Dali
{
namespace Internal
{
namespace Adaptor
{

EglFactory::EglFactory()
: mEglImplementation(NULL),
  mEglImageExtensions(NULL),
  mEglSync(new EglSyncImplementation) // Created early, as needed by Core constructor
{
}

EglFactory::~EglFactory()
{
  // Ensure the EGL implementation is destroyed
  delete mEglImageExtensions;
  delete mEglImplementation;
  delete mEglSync;
}

EglInterface* EglFactory::Create()
{
  // Created by RenderThread (After Core construction)
  mEglImplementation = new EglImplementation();
  mEglImageExtensions = new EglImageExtensions(mEglImplementation);

  mEglSync->Initialize(mEglImplementation); // The sync impl needs the EglDisplay
  return mEglImplementation;
}

void EglFactory::Destroy()
{
  delete mEglImageExtensions;
  mEglImageExtensions = NULL;
  delete mEglImplementation;
  mEglImplementation = NULL;
}

EglInterface* EglFactory::GetImplementation()
{
  return mEglImplementation;
}

EglImageExtensions* EglFactory::GetImageExtensions()
{
  return mEglImageExtensions;
}

EglSyncImplementation* EglFactory::GetSyncImplementation()
{
  return mEglSync;
}

} // Adaptor
} // Internal
} // Dali
