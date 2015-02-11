#ifndef __DALI_DISPLAY_CONNECTION_H__
#define __DALI_DISPLAY_CONNECTION_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/object/handle.h>
#include <dali/public-api/object/any.h>

namespace Dali
{

class EglInterface;

namespace Internal
{
  namespace Adaptor
  {
    class DisplayConnection;
  }
}

class DALI_IMPORT_API DisplayConnection : public BaseHandle
{
public:

  /**
   * Create a DisplayConnection object.
   *
   * @return A handle to a newly allocated object.
   */
  static DisplayConnection New();

  /**
   * @brief Create a DisplayConnection handle; this can be initialised with DisplayConnection::New().
   *
   * Calling member functions with an uninitialised handle is not allowed.
   */
  DisplayConnection();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~DisplayConnection();

  /**
   * This copy constructor is required for (smart) pointer semantics.
   *
   * @param [in] handle A reference to the copied handle
   */
  DisplayConnection(const DisplayConnection& handle);

  /**
   * This assignment operator is required for (smart) pointer semantics.
   *
   * @param [in] rhs  A reference to the copied handle
   * @return A reference to this
   */
  DisplayConnection& operator=(const DisplayConnection& rhs);

  /**
   * Get DPI
   * @param[out] dpiHorizontal set to the horizontal dpi
   * @param[out] dpiVertical set to the vertical dpi
   */
  void GetDpi(unsigned int& dpiHorizontal, unsigned int& dpiVertical) const;

  /**
   * Consumes any possible events on the queue so that there is no leaking between frames
   */
  void ConsumeEvents();

  /**
   * Initialize EGL surface
   *
   * @param egl implementation to use for the creation
   */
  bool InitializeEgl(EglInterface& egl);

public:

  /**
   * This constructor is used by DisplayConnection New() methods.
   *
   * @param [in] handle A pointer to a newly allocated DisplayConnection resource
   */
  explicit DALI_INTERNAL DisplayConnection(Internal::Adaptor::DisplayConnection* impl);
};

}

#endif // __DALI_DISPLAY_CONNECTION_H__
