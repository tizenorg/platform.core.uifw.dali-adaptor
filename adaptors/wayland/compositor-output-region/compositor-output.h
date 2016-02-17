#ifndef __DALI_INTERNAL_ADAPTOR_COMPOSITOR_OUTPUT_H__
#define __DALI_INTERNAL_ADAPTOR_COMPOSITOR_OUTPUT_H__

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <xkbcommon/xkbcommon.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/events/key-event.h>

// INTERNAL INCLUDES
#include <wl-types.h>


namespace Dali
{

namespace Internal
{

namespace Adaptor
{

/**
 *
 * An output is typically a monitor.
 * The output is required to view a region of the compositor space.
 * E.g. a monitor may be display the entire compositor space, or just a
 * region.
 * The output interface is published as global during start up, or when a monitor is
 * hotplugged.
 *
 * Information available from the output includes
 * - refresh rate
 * - monitor dimensions in millimetres
 * - monitor resolution
 */

class CompositorOutput
{

public:



  /**
   * @brief constructor
   * @param[in] inputInterface input interface
   * @param[in] seatInterface Wayland seat interface
   */
  CompositorOutput();

  /**
   * @brief non virtual destructor, not intended as base class
   */
  ~CompositorOutput();

  /**
   * @brief listen to output callbacks
   * @param outputInterface[in] output interface
   */
  void AddListener( WlOutput* outputInterface );

  /**
   * @brief Get DPI
   * @param[out] dpiHorizontal set to the horizontal dpi
   * @param[out] dpiVertical set to the vertical dpi
   */
  void GetDpi( unsigned int& dpiHorizontal, unsigned int& dpiVertical ) const;


  /**
   * @brief return true if all information about the compositor output has been received
   * and is ready to be read
   */
  bool DataReady() const;


  /**
   * @brief Set monitor size in millimetres
   * @param[in] width width
   * @param[in] height height
   */
  void SetMonitorDimensions( unsigned int width, unsigned int height );

  /**
   * @brief Set monitor resolution in pixels
   * @param[in] width width
   * @param[in] height height
   */
  void SetMonitorResolution( unsigned int width, unsigned int height );


  /**
   * @brief Called when all callbacks have finished
   * at this point we calculate dpi
   */
  void CallbacksDone();

private:


  WlOutput* mOutput;///< Compositor output ( info for attached monitor)
  unsigned int mDpiHorizontal;
  unsigned int mDpiVertical;
  unsigned int mXresolution;            ///< Monitor resolution in pixel
  unsigned int mYresolution;           ///< Monitor resolution in pixel
  float mMonitorWidth;     ///< width in inches
  float mMonitorHeight;    ///< height in inches
  bool mDataReady:1;

};

} // Internal
} // Adaptor
} // Dali

#endif  //__DALI_INTERNAL_ADAPTOR_COMPOSITOR_OUTPUT_H__
