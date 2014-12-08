#ifndef __DALI_EMSCRIPTEN_UTILS_H__
#define __DALI_EMSCRIPTEN_UTILS_H__

/*
Copyright (c) 2000-2013 Samsung Electronics Co., Ltd All Rights Reserved

This file is part of Dali

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

Dali::Image CreateImageRGBA(unsigned int width, unsigned int height, const std::string& data);
Dali::Image CreateImageRGB(unsigned int width, unsigned int height, const std::string& data);

std::string PropertyNameToJavaScriptName(const std::string& hyphenatedName);

Dali::Image GetImage(const std::string& data);

}; // namespace Emscripten
}; // namespace Internal
}; // namespace Dali

#endif // header
