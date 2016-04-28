#ifndef __DALI_WATCH_H__
#define __DALI_WATCH_H__

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
#include <string>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/signals/callback.h>


// INTERNAL INCLUDES
#include "application.h"

namespace Dali
{
/**
 * @addtogroup dali_adaptor_framework
 * @{
 */

namespace Internal DALI_INTERNAL
{
namespace Adaptor
{
class Watch;
}
}
class DALI_IMPORT_API Watch : public Application
{
public:
  typedef Signal< void (Application&, void *) > WatchPtrSignal; ///< Watch pointer signal callback type @SINCE_1_0.0
  typedef Signal< void (Application&, bool) > WatchBoolSignal; ///< Watch bool signal callback type @SINCE_1_0.0

public:

  /**
   * @brief This is the constructor for applications without an argument list.
   * @SINCE_1_0.0
   */
  static Watch New();

  /**
   * @brief This is the constructor for applications.
   *
   * @SINCE_1_0.0
   * @param[in,out]  argc        A pointer to the number of arguments
   * @param[in,out]  argv        A pointer the the argument list
   */
  static Watch New( int* argc, char **argv[] );

  /**
   * @brief This is the constructor for applications with a name
   *
   * @SINCE_1_0.0
   * @param[in,out]  argc        A pointer to the number of arguments
   * @param[in,out]  argv        A pointer the the argument list
   * @param[in]      stylesheet  The path to user defined theme file
   */
  static Watch New( int* argc, char **argv[], const std::string& stylesheet );

  /**
   * @brief Construct an empty handle
   * @SINCE_1_0.0
   */
  Watch();

  /**
   * @brief Copy Constructor
   * @SINCE_1_0.0
   */
  Watch( const Watch& implementation );

  /**
   * @brief Assignment operator
   * @SINCE_1_0.0
   */
  Watch& operator=( const Watch& applicaton );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~Watch();

public: // Stereoscopy
  /**
   * @brief This signal is emitted at every second
   * @param[in] watch_time The watch time handle. watch_time will not be available after returning this callback. It will be freed by the framework.
   * @SINCE_1_0.0
   */
  WatchPtrSignal& TimeTickSignal();

  /**
   * @brief This signal is emitted at every second in ambient mode
   * @param[in] watch_time The watch time handle. watch_time will not be available after returning this callback. It will be freed by the framework.
   * @SINCE_1_0.0
   */
  WatchPtrSignal& AmbientTickSignal();
  
  /**
   * @brief This signal is emitted when the device enters or exits ambient mode
   * @param[in] ambient_mode If true the device enters the ambient mode, otherwise false
   * @SINCE_1_0.0
   */
  WatchBoolSignal& AmbientChangedSignal();

public: // Not intended for application developers
  /**
   * @brief Internal constructor
   * @SINCE_1_0.0
   */
  explicit DALI_INTERNAL Watch(Internal::Adaptor::Watch* implementation);
};

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_WATCH_H__
