#ifndef __DALI_SLP_PLATFORM_FONT_CONTROLLER_H__
#define __DALI_SLP_PLATFORM_FONT_CONTROLLER_H__

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

// INTERNAL INCLUDES
#include "../../interfaces/font-controller.h"

// EXTERNAL INCLUDES
#include <boost/thread.hpp>
#include <map>


namespace Dali
{

namespace SlpPlatform
{

typedef Dali::Platform::FontController::FontList FontList;

/**
 * concrete interface for the SLP font controller.
 *
 * Font controller currently caches the following:
 * - List of fonts on the system, if the user calls GetFontList()
 * - List of fonts used by the application with both its filename and character set
 *
 * When searching for a font that can display a string of text, the follow occurs:
 *
 * - 1. Check the last used font to see if it supports the text
 * - 2. If 1 fails, check all other Fonts in the cache to see if they support the text
 * - 3. If 2 fails, use FontConfig to find a font which supports the text, then cache that font.
 *
 * Suggested future improvements:
 * Stop using FontConfig library, instead use FreeType directly to query and cache font family names and
 * character sets. Then parse the font config configuration file directly to get the system preferred fonts etc.
 */
class FontController : public Platform::FontController
{
public:

  /**
   * Constructor
   */
  FontController();

 /**
   * Destructor
   */
  virtual ~FontController();

  /**
   * Internally caches the font file name for every font family passed in
   * @copydoc Dali::Platform::FontController::GetFontPath()
   */
  virtual std::string GetFontPath( const StyledFontFamily& styledFontFamily );

  /**
   * Internally caches the font list the first time it is called.
   * @copydoc Dali::Platform::FontController::GetFontList()
   */
  virtual FontList GetFontList( FontListMode fontListMode );

  /**
   * @copydoc Dali::Platform::FontController::ValidateFontFamilyName()
   */
  virtual bool ValidateFontFamilyName(const StyledFontFamily& styledFontFamily, bool& isDefaultSystemFont, StyledFontFamily& closestStyledFontFamilyMatch);

  /**
   * @copydoc Dali::Platform::FontController::GetFontFamilyForChars()
   */
  virtual StyledFontFamily GetFontFamilyForChars( const Integration::TextArray& charsRequested);

  /**
   * @copydoc Dali::Platform::FontController::AllGlyphsSupported()
   */
  virtual bool AllGlyphsSupported(const StyledFontFamily& styledFontFamily, const Integration::TextArray& text);

  /**
   * @copydoc Dali::Platform::FontController::SetDefaultFontFamily()
   */
  virtual void SetDefaultFontFamily( const StyledFontFamily& styledFontFamily );

private:

};

} // namespace SlpPlatform

} // namespace Dali

#endif // __DALI_SLP_PLATFORM_FONT_CONTROLLER_H__
