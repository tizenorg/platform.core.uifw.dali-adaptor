#ifndef DALI_ANDROID_RENDER_SURFACE_FACTORY_H
#define DALI_ANDROID_RENDER_SURFACE_FACTORY_H

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
 */

namespace Dali
{
namespace Integration
{
class Framework;
}

/**
 * Surface factory for android
 * @param[in] type The type of surface to create
 * @param[in] positionSize The position and size of the surface
 * @param[in] name The name of the surface.
 */
RenderSurface* CreateDefaultSurface( Integration::Framework* framework,
                                     RenderSurface::SurfaceType type,
                                     PositionSize positionSize,
                                     const std::string& name );

} // namespace Dali

#endif // DALI_ANDROID_RENDER_SURFACE_FACTORY_H
