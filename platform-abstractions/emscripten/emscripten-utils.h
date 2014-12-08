#ifndef __DALI_EMSCRIPTEN_UTILS_H__
#define __DALI_EMSCRIPTEN_UTILS_H__

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
#include <dali/public-api/dali-core.h>

// INTERNAL INCLUDES

namespace Dali
{
namespace Internal
{
namespace Emscripten
{

std::string JavaScriptNameToPropertyName(const std::string& camelCase);
std::string PropertyNameToJavaScriptName(const std::string& hyphenatedName);

Dali::Image CreateImageRGBA(unsigned int width, unsigned int height, const std::string& data);
Dali::Image CreateImageRGB(unsigned int width, unsigned int height, const std::string& data);

Dali::Image GetImage(const std::string& data);

}; // namespace Emscripten
}; // namespace Internal
}; // namespace Dali

#endif // header
