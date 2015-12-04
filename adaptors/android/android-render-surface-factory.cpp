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

// INTERNAL INCLUDES
#include <android-render-surface.h>

namespace Dali
{

RenderSurface* CreateDefaultSurface(
  Integration::Framework* framework,
  RenderSurface::SurfaceType type,
  PositionSize positionSize,
  const std::string& name )
{
  // create a window by default
  Any surface;
  Any display;

  RenderSurface* renderSurface = new Internal::Adaptor::Android::AndroidRenderSurface( framework, type, positionSize, surface, display, name );

  return renderSurface;
}

} // namespace Dali
