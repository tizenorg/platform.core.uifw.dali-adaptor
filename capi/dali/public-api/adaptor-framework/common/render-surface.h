#ifndef __DALI_RENDER_SURFACE_H__
#define __DALI_RENDER_SURFACE_H__

/*
Copyright (c) 2000-2013 Samsung Electronics Co., Ltd All Rights Reserved

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
#include <boost/any.hpp>
#include <string>
#include <dali/public-api/math/rect.h>

namespace Dali DALI_IMPORT_API
{

typedef Dali::Rect<int> PositionSize;

/**
 * Dali::Adaptor requires a render surface to draw on to. This is
 * usually a window in the native windowing system, or some other
 * mapped pixel buffer.
 *
 * Dali::Application will automatically create a render surface using a window.
 *
 * The implementation of the factory method below should choose an appropriate
 * implementation of RenderSurface for the given platform
 */
class RenderSurface
{
public:
  /**
   * enumeration of surface types
   */
  enum SurfaceType
  {
    NO_SURFACE,     ///< not configured
    PIXMAP,         ///< Pixmap
    WINDOW,         ///< Window
    NATIVE_BUFFER   ///< Native Buffer
  };

  /**
   * @deprecated, not supported
   */
  enum SurfaceAPI
  {
    X11,        ///< X types
    ECORE_X11   ///< EFL e-core x11 types
  };

  /**
   * When application uses pixmap surface, it can select rendering mode
   * RENDER_SYNC : application should call RenderSync() after posting the offscreen to onscreen
   * RENDER_#FPS : the maximum performance will be limited designated number of frame
   */
  enum RenderMode
  {
    RENDER_DEFAULT = -1,
    RENDER_SYNC = 0,
    RENDER_24FPS = 24,
    RENDER_30FPS = 30,
    RENDER_60FPS = 60
  };

  /**
   * Constructor
   * Application or Adaptor needs to create the appropriate concrete RenderSurface type.
   * @see CreateDefaultSurface
   */
  RenderSurface();

  /**
   * Virtual Destructor
  */
  virtual ~RenderSurface();

  /**
   * @returns the surface type
   */
  virtual SurfaceType GetType() = 0;

  /**
   * Returns the window or pixmap surface
   * @return surface
   */
  virtual boost::any GetSurface() = 0;

  /**
   * @deprecated, use GetSurface() instead
   * @param[in] api API for which to get surface
   */
  boost::any GetSurface( SurfaceAPI api );

  /**
   * Returns the display
   * @return display
   */
  virtual boost::any GetDisplay() = 0;

  /**
   * @deprecated, use GetDisplay() instead
   * @param[in] api API for which to get display
   */
  boost::any GetDisplay( SurfaceAPI api );

  /**
   * Return the size and position of the surface
   */
  virtual PositionSize GetPositionSize() const = 0;

  /**
   * Set frame update rate for pixmap surface type
   */
  virtual void SetRenderMode(RenderMode mode) = 0;

  /**
   * Get current fps for pixmap surface type
   */
  virtual RenderMode GetRenderMode() const = 0;

private:

  /**
   * Undefined copy constructor and assignment operator. RenderSurface cannot be copied
   */
  RenderSurface( const RenderSurface& rhs );
  RenderSurface& operator=( const RenderSurface& rhs );

};

/**
 * Default surface factory function.
 * A surface is created with the given type.
 *
 * @param [in] type the type of surface to create
 * @param [in] positionSize the position and size of the surface to create
 * @param [in] name optional name of surface passed in
 */
RenderSurface* CreateDefaultSurface( RenderSurface::SurfaceType type, PositionSize positionSize, const std::string& name = "" );

} // namespace Dali

#endif // __DALI_RENDER_SURFACE_H__
