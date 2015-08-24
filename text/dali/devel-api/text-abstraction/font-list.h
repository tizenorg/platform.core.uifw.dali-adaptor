#ifndef __DALI_TEXT_ABSTRACTION_FONT_LIST_H__
#define __DALI_TEXT_ABSTRACTION_FONT_LIST_H__

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
#include <iostream>
#include <stdint.h>
#include <string>
#include <dali/public-api/common/vector-wrapper.h>


namespace Dali
{

namespace TextAbstraction
{
typedef std::string FontPath;
typedef std::string FontFamily;
typedef std::string FontStyle;

namespace FontWidth
{
  enum Type
  {
    ULTRA_CONDENSED  = 75,
    EXTRA_CONDENSED  = ULTRA_CONDENSED,
    CONDENSED        = 87,
    SEMI_CONDENSED   = CONDENSED,
    NORMAL           = 100,
    SEMI_EXPANDED,
    EXPANDED,
    EXTRA_EXPANDED,
    ULTRA_EXPANDED,
  };
} // namespace FontWidth

namespace FontWeight
{
  enum Type
  {
    THIN        = 40,
    ULTRA_LIGHT = 45,
    LIGHT       = 50,
    BOOK        = 75,
    NORMAL      = 80,
    MEDIUM      = 100,
    SEMI_BOLD   = 180,
    BOLD        = 200,
    ULTRA_BOLD  = 205,
    BLACK       = 210,
    EXTRA_BLACK
  };
}

namespace FontSlant
{
  enum Type
  {
    NORMAL  = 0,
    ITALIC  = 100,
    OBLIQUE = 110
  };
} // namespace FontSlant

struct FontDescription
{
  FontPath         path;
  FontFamily       family;
  FontStyle        style;
  FontWidth::Type  width;
  FontWeight::Type weight;
  FontSlant::Type  slant;
};

typedef std::vector<FontDescription> FontList;

DALI_IMPORT_API std::ostream& operator<<( std::ostream& o, const FontList& fontList );

} // namespace TextAbstraction

} // namespace Dali

#endif // __DALI_TEXT_ABSTRACTION_FONT_LIST_H__
