/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// CLASS HEADER
#include "egl-factory.h"

// INTERNAL INCLUDES
#include <gl/egl-implementation.h>
#include <gl/egl-image-extensions.h>
#include <gl/egl-sync-implementation.h>

namespace Dali
{
namespace Internal
{
namespace Adaptor
{

struct EglFactory::Impl
{
  EglImplementation* mEglImplementation;
  EglImageExtensions* mEglImageExtensions;
  EglSyncImplementation* mEglSync;

  Impl()
    : mEglImplementation(NULL),
      mEglImageExtensions(NULL),
      mEglSync(new EglSyncImplementation) // Created early, as needed by Core constructor
  {
  }
  ~Impl()
  {
    // Ensure the EGL implementation is destroyed
    delete mEglImageExtensions;
    delete mEglImplementation;
    delete mEglSync;
  }
};

EglFactory::EglFactory()
{
  mImpl = new Impl();
}

EglFactory::~EglFactory()
{
  delete mImpl;
}

EglInterface* EglFactory::Create()
{
  // Created by RenderThread (After Core construction)
  mImpl->mEglImplementation = new EglImplementation();
  mImpl->mEglImageExtensions = new EglImageExtensions(mEglImplementation);

  mImpl->mEglSync->Initialize(mEglImplementation); // The sync impl needs the EglDisplay
  return mImpl->mEglImplementation;
}

void EglFactory::Destroy()
{
  mImpl->Destroy();
  delete mImpl->mEglImageExtensions;
  mImpl->mEglImageExtensions = NULL;
  delete mImpl->mEglImplementation;
  mImpl->mEglImplementation = NULL;
}

EglInterface* EglFactory::GetImplementation()
{
  return mImpl->mEglImplementation;
}

EglImageExtensions* EglFactory::GetImageExtensions()
{
  return mImpl->mEglImageExtensions;
}

EglSyncImplementation* EglFactory::GetSyncImplementation()
{
  return mImpl->mEglSync;
}

} // Adaptor
} // Internal
} // Dali
