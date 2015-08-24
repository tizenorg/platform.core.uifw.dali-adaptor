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
    ULTRA_CONDENSED,
    EXTRA_CONDENSED,
    CONDENSED,
    SEMI_CONDENSED,
    NORMAL,
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
    THIN,
    ULTRA_LIGHT,
    EXTRA_LIGHT = ULTRA_LIGHT,
    LIGHT,
    DEMI_LIGHT,
    SEMI_LIGHT = DEMI_LIGHT,
    BOOK,
    NORMAL,
    REGULAR = NORMAL,
    MEDIUM,
    DEMI_BOLD,
    SEMI_BOLD = DEMI_BOLD,
    BOLD,
    ULTRA_BOLD,
    EXTRA_BOLD = ULTRA_BOLD,
    BLACK,
    HEAVY = BLACK,
    EXTRA_BLACK = BLACK
  };
}

namespace FontSlant
{
  enum Type
  {
    NORMAL,
    ROMAN = NORMAL,
    ITALIC,
    OBLIQUE
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
