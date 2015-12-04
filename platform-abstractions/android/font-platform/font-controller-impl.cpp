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

// CLASS HEADER
#include "font-controller-impl.h"
#include <dali/integration-api/debug.h>

// EXTERNAL HEADERS
// #include <fontconfig/fontconfig.h>


namespace Dali
{

namespace SlpPlatform
{

#if defined(DEBUG_ENABLED)
namespace
{
Dali::Integration::Log::Filter* gLogFilter = Dali::Integration::Log::Filter::New(Debug::Concise, false, "LOG_FONT_CONTROLLER");
} // unnamed namespace
#endif

namespace
{
const std::string SETTING_FONT_PRELOAD_FONT_PATH( FONT_PRELOADED_PATH );
const std::string SETTING_FONT_DOWNLOADED_FONT_PATH( FONT_DOWNLOADED_PATH );
// const std::string SETTING_FONT_APP_FONT_PATH( FONT_APPLICATION_PATH );

const uint32_t UNICODE_CR_LF = 0x85;
const uint32_t UNICODE_CHAR_START = 0x20;       // start range of unicode characters (below are control chars)


bool CheckFontInstallPath( FontController::FontListMode listMode, const std::string& fileName )
{
  return false;
}

} // unnamed namespace

FontController::FontController()
{
}

FontController::~FontController()
{
}

std::string FontController::GetFontPath( const StyledFontFamily& styledFontFamily )
{
  StyledFontFamily closestMachedStyledFontFamily;
  bool isDefault( false );

  // if the font was not found, the path will be an empty string
  return "";
}

FontList FontController::GetFontList( FontListMode fontListMode )
{
  return FontList();
}

bool FontController::ValidateFontFamilyName( const StyledFontFamily& styledFontFamily, bool& isDefaultSystemFont, StyledFontFamily& closestStyledFontFamilyMatch )
{
  return false;
}

FontController::StyledFontFamily FontController::GetFontFamilyForChars(const Integration::TextArray& charsRequested)
{
  // return empty string
  return StyledFontFamily();
}


bool FontController::AllGlyphsSupported(const StyledFontFamily& styledFontFamily, const Integration::TextArray& text)
{
}

void FontController::SetDefaultFontFamily( const StyledFontFamily& styledFontFamily )
{
}

} // namespace SlpPlatform

} // namespace Dali

// Implementation of Dali::Platform::FontController::New()
Dali::Platform::FontController* Dali::Platform::FontController::New()
{
  return new Dali::SlpPlatform::FontController();
}
