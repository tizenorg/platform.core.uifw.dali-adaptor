/*
Copyright (c) 2000-2012 Samsung Electronics Co., Ltd All Rights Reserved

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
  // HACK: any font will be valid, any request will use Roboto-thin.
  return true;
}

FontController::StyledFontFamily FontController::GetFontFamilyForChars(const TextArray& charsRequested)
{
  // return empty string
  return StyledFontFamily("Roboto", "Dodgers");
}


bool FontController::AllGlyphsSupported(const StyledFontFamily& styledFontFamily, const TextArray& text)
{
  return false;
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
