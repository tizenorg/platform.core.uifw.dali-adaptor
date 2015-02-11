#ifndef __DALI_INTERNAL_ECORE_X_DIPLAY_CONNECTION_H__
#define __DALI_INTERNAL_ECORE_X_DIPLAY_CONNECTION_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <Ecore_Wayland.h>
#include <ecore-x-types.h>

// INTERNAL INCLUDES
#include <dali/public-api/object/base-object.h>
#include <gl/egl-implementation.h>

namespace Dali
{

class RenderSurface;
class DisplayConnection;

namespace Internal
{

namespace Adaptor
{

/**
 * DisplayConnection implementation
 */
class DisplayConnection : public Dali::BaseObject
{
public:

  /**
   * Create a new DisplayConnection object.
   * @return A pointer to the newly allocated object.
   */
  static DisplayConnection* New();

public:

  /**
   * @copydoc Dali::DisplayConnection::GetDpi
   */
  void GetDpi(unsigned int& dpiHorizontal, unsigned int& dpiVertical) const;

  /**
   * @copydoc Dali::DisplayConnection::ConsumeEvents
   */
  void ConsumeEvents();

  /**
   * @copydoc Dali::DisplayConnection::InitializeEgl
   */
  bool InitializeEgl(EglInterface& egl);

  /**
   * Replace EGL surface
   */
  bool ReplaceEGL(EglInterface& egl, RenderSurface* renderSurface);

public:

  /**
   * Destructor
   */
  virtual ~DisplayConnection();

protected:

  /**
   * Private constructor; see also DisplayConnection::New()
   */
  DisplayConnection();

  // Undefined
  DisplayConnection(const DisplayConnection&);

  // Undefined
  DisplayConnection& operator=(const DisplayConnection& rhs);

private:

};

} // namespace Adaptor

} // namespace internal

} // namespace Dali

#endif // __DALI_INTERNAL_ECORE_X_DIPLAY_CONNECTION_H__
