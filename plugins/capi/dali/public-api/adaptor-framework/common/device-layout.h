#ifndef __DALI_DEVICE_LAYOUT_H__
#define __DALI_DEVICE_LAYOUT_H__

/*
Copyright (c) 2000-2012 Samsung Electronics Co., Ltd All Rights Reserved

This file is part of Dali Adaptor

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
#include <dali/public-api/math/vector2.h>

namespace Dali DALI_IMPORT_API
{

/**
 * The attributes of the screen on the device.
 * An application can specify the base layout that they used by inputting the values in this
 * structure and passing it to the Application or Adaptor class.
 * @see Dali::Application::Application(* argc, char **argv[], DeviceLayout baseLayout)
 * @see Dali::Adaptor::Adaptor(RenderSurface& surface, DeviceLayout baseLayout)
 */
struct DeviceLayout
{
public: // Construction & Destruction

  /**
   * Default Constructor
   */
  DeviceLayout();

  /**
   * Create a DeviceLayout with specific parameters.
   * @param[in]  resolution       The resolution of the screen the application is based upon.
   * @param[in]  screenSize       The size of the screen the application is based upon.
   * @param[in]  dpi              The DPI of the screen the application is based upon.
   * @param[in]  viewingDistance  The default viewing distance of the screen the application is based upon.
   */
  DeviceLayout(Vector2 resolution, float screenSize, float dpi, float viewingDistance);

  /**
   * Destructor
   */
  ~DeviceLayout();

public: // Data

  Vector2 resolution;      ///< Resolution (width and height) of the screen.
  float   screenSize;      ///< Size of the screen in inches (diagonal size).
  float   dpi;             ///< DPI (Dots per Inch) of the screen on the device.
  float   viewingDistance; ///< Average distance between the user and the device.

public: // Defaults Layouts

  /**
   * This is the default base layout that Dali will assume if no layout is passed in from the
   * application.
   * Resolution:        720.0f x 1280.0f
   * Screen Size:       4.65f
   * DPI:               316.0f
   * Viewing Distance:  30.0f
   */
  static const DeviceLayout DEFAULT_BASE_LAYOUT;
};

} // namespace Dali

#endif // __DALI_DEVICE_LAYOUT_H__
