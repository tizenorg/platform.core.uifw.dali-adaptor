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
#include <Ecore_X.h>
#include <ecore-x-types.h>

// INTERNAL INCLUDES
#include <base/display-connection.h>
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
   * @brief Default constructor
   */
  DisplayConnection();

  /**
   * @brief Create a new DisplayConnection object.
   *
   * @return A pointer to the newly allocated object.
   */
  static DisplayConnection* New();

  /**
   * @brief Retrieve the initialized instance of the DisplayConnection.
   *
   * @return A pointer to the allocated object.
   */
  static DisplayConnection& Get();

public:

  /**
   * @copydoc Dali::DisplayConnection::GetDisplay
   */
  Any GetDisplay();

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

public:

  /**
   * Destructor
   */
  virtual ~DisplayConnection();

protected:

  // Undefined
  DisplayConnection(const DisplayConnection&);

  // Undefined
  DisplayConnection& operator=(const DisplayConnection& rhs);

private:
  XDisplay*   mDisplay;        ///< X-connection for rendering
};

} // namespace Adaptor

} // namespace internal

inline Internal::Adaptor::DisplayConnection& GetImpl()
{
  return Internal::Adaptor::DisplayConnection::Get();
}

} // namespace Dali

#endif // __DALI_INTERNAL_ECORE_X_DIPLAY_CONNECTION_H__
