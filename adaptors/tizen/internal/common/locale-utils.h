#ifndef __DALI_INTERNAL_LOCALE_UTILS_H__
#define __DALI_INTERNAL_LOCALE_UTILS_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// EXTERNAL INCLUDES
#include <string>
#include <dali/public-api/adaptor-framework/common/virtual-keyboard.h>

// INTERNAL INCLUDES

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace Locale
{

Dali::VirtualKeyboard::TextDirection GetTextDirection( std::string locale );

} // namespace Locale

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_LOCALE_UTILS_H__
