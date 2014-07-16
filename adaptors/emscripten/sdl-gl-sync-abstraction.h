#ifndef __DALI_INTEGRATION_SDL_GL_SYNC_ABSTRACTION_H__
#define __DALI_INTEGRATION_SDL_GL_SYNC_ABSTRACTION_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <dali/integration-api/gl-sync-abstraction.h>

namespace Dali
{

/**
 * This abstraction defines an API for syncing CPU with GPU.
 * A typical use case is to determine when GL draw calls have finished drawing
 * to a framebuffer.
 */
class DALI_IMPORT_API SdlGlSyncAbstraction : public Dali::Integration::GlSyncAbstraction
{
public:

  virtual ~SdlGlSyncAbstraction();

  /**
   * Create a synchronisation object based on the resource id, typically that of
   * a framebuffer texture. It can then be polled using the same resource id.
   * @return A pointer to an opaque sync object
   */
  virtual Integration::GlSyncAbstraction::SyncObject* CreateSyncObject();

  /**
   * Destroy the synchronisation object.
   * @param[in] syncObject The sync object to destroy
   */
  virtual void DestroySyncObject(Integration::GlSyncAbstraction::SyncObject* syncObject);
};

} // namespace Dali

#endif // __DALI_INTEGRATION_SDL_GL_SYNC_ABSTRACTION_H__
