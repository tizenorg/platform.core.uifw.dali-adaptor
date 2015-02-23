#ifndef __DALI_WINDOW_EXTENSION_H__
#define __DALI_WINDOW_EXTENSION_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <vector>
#include <string>

namespace Dali
{
class Window;

namespace WindowExtensions
{
typedef std::vector< std::string > EffectContainer;

/**
 * @brief Set the supported window effect from the platform.
 *
 * The supported window effect can be combined.
 * The enabled window effect will be shown when the application is launched, quit, shown and hiden.
 *
 * @param[in] effect is the name of one of the supported effects.
 */
void SetEffect( Window window, const std::string& effect );

/**
 * @brief Remove the enabled window effect.
 * @param[in] effect is the name of one of the supported effects.
 */
void RemoveEffect( Window window, const std::string& effect );

/**
 * @brief Query the supported window effect from the platform.
 * @return Returns the effects supported by the platform.
 */
EffectContainer GetSupportedEffects();

/**
 * @brief Query the applied window effects from the current window.
 * @return Returns the applied effects.
 */
EffectContainer GetAppliedEffects( Window window );
}

}

#endif // __DALI_WINDOW_EXTENSION_H__
