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

// CLASS HEADER
#include <dali/public-api/adaptor-framework/common/render-surface.h>

// INTERNAL INCLUDES
#include <internal/common/render-surface-impl.h>

namespace Dali
{

RenderSurface::RenderSurface()
{
}

RenderSurface::~RenderSurface()
{
}

boost::any RenderSurface::GetSurface(SurfaceAPI /*api*/)
{
  return GetSurface();
}

boost::any RenderSurface::GetDisplay(SurfaceAPI /*api*/)
{
  return GetDisplay();
}

RenderSurface* CreateDefaultSurface( RenderSurface::SurfaceType type, PositionSize positionSize, const std::string& name )
{
  // create a Ecore X11 window by default
  boost::any surface;
  boost::any display;
  // FIXME: return new Internal::Adaptor::X11::RenderSurface( type, positionSize, surface, display, name );
  return NULL;
}

} // namespace Dali
