#ifndef __DALI_SLP_PLATFORM_FONT_CONTROLLER_H__
#define __DALI_SLP_PLATFORM_FONT_CONTROLLER_H__

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

// INTERNAL INCLUDES
#include "../../interfaces/font-controller.h"
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/common/map-wrapper.h>

// EXTERNAL INCLUDES
#include <boost/thread.hpp>


// forward declarations of font config types
struct _FcConfig;
struct _FcPattern;
struct _FcCharSet;
struct _FcFontSet;

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
  virtual const std::string& GetFontPath( const StyledFontFamily& styledFontFamily );

  /**
   * Internally caches the font list the first time it is called.
   * @copydoc Dali::Platform::FontController::GetFontList()
   */
  virtual void GetFontList( FontListMode fontListMode, FontList& fontList );

  /**
   * @copydoc Dali::Platform::FontController::ValidateFontFamilyName()
   */
  virtual bool ValidateFontFamilyName( const StyledFontFamily& styledFontFamily,
                                       bool& isDefaultSystemFontFamily,
                                       bool& isDefaultSystemFontStyle,
                                       StyledFontFamily& closestStyledFontFamilyMatch );

  /**
   * @copydoc Dali::Platform::FontController::GetFontFamilyForChars()
   */
  virtual const StyledFontFamily& GetFontFamilyForChars( const TextArray& charsRequested);

  /**
   * @copydoc Dali::Platform::FontController::AllGlyphsSupported()
   */
  virtual bool AllGlyphsSupported( const StyledFontFamily& styledFontFamily, const TextArray& text );

  /**
   * @copydoc Dali::Platform::FontController::SetDefaultFontFamily()
   */
  virtual void SetDefaultFontFamily( const StyledFontFamily& styledFontFamily );

private:

  /**
   * Gets a cached font list
   * @param[in] fontListMode the font list mode
   * @return font list
   */
  void GetCachedFontList( FontListMode fontListMode, FontList& fontList ) const;

  /**
   * returns the font path given a font family.
   * @param[in] styledFontFamily The name of the font's family and the font's style.
   * @return The font's file name path.
   */
  const std::string& GetCachedFontPath( const StyledFontFamily& styledFontFamily ) const;

  /**
   * returns the cached character set for a font family.
   * @param[in] styledFontFamily The name of the font's family and the font's style.
   * @return pointer to font configs character set object.
   *
   */
  _FcCharSet* GetCachedFontCharacterSet( const StyledFontFamily& styledFontFamily ) const;

  /**
   * Add a font path to the font path cache.
   * @param[in] styledFontFamily The name of the font's family and the font's style.
   * @param[in] fontPath font path.
   * @param[in] characterSet font config character set object (not const because it's ref count is increased).
   */
  void AddCachedFont( const StyledFontFamily& styledFontFamily, const std::string& fontPath, _FcCharSet *characterSet );

  /**
   * Store information held in a fontconfig pattern, into the font cache.
   * Stores font name, filename and character set.
   * @param[in] pattern pointer to a font config pattern.
   * @param[in] inputStyledFontFamily font family name with its style used to perform the pattern match.
   * @param[out] closestStyledFontFamilyMatch matched family name and style.
   */
  void CacheFontInfo( _FcPattern* pattern, const StyledFontFamily& inputStyledFontFamily, StyledFontFamily& closestStyledFontFamilyMatch );

  /**
   * Create a pattern for font lookups.
   * @param[in] styledFontFamily The name of the font's family and the font's style.
   */
  _FcPattern* CreateFontFamilyPattern( const StyledFontFamily& styledFontFamily );

  /**
   * Checks whether a character is a control character.
   */
  bool IsAControlCharacter( uint32_t character ) const;

  /**
   * Clear the font family cache.
   * Should only be called by the destructor.
   */
  void ClearFontFamilyCache();

  /**
   * Adds a font to the cached font list.
   * Checks for duplicates.
   * @param fileName the font full filename with path
   * @param styledFontFamily The name of the font's family and the font's style.
   */
  void AddToFontList( const std::string& fileName, const StyledFontFamily& styledFontFamily );

  /**
   * Returns a FontConfig font set, which has a list of fonts.
   * @return pointer to _FcFontSet struct.
   */
  _FcFontSet* GetFontSet() const;

  /**
   * Create a font config character set object, using the text string.
   * @param charsRequested array of characters.
   * @return a pointer to _FcCharSet object on success, NULL on failure.
   */
  _FcCharSet* CreateCharacterSet( const TextArray& charsRequested );

  /**
   * Add a font that has not been found on the system but a match has been found.
   * @param[in] missingStyledFontFamily the missing font and its style to add.
   * @param[in] closestStyledFontFamilyMatch it's nearest font and its style match.
   */
  void AddMatchedFont( const StyledFontFamily& missingStyledFontFamily, StyledFontFamily& closestStyledFontFamilyMatch );

  /**
   * Check the cache to see if a font and its style has been added to the missing fonts list.
   * @param styledFontFamily the font and its style to check.
   * @return the closest styled font that the missing styled font has been matched with by font-config.
   */
   const StyledFontFamily& GetMatchedFont( const StyledFontFamily& styledFontFamily ) const;

   /**
    * Create a preferred list of fonts to use for when GetFontFamilyForChars() is called.
    */
  void CreatePreferedFontList();

  /**
   * Deletes all preferred fonts.
   */
  void ClearPreferredFontList();

  /**
   * Font cache item.
   */
  struct FontCacheItem
  {
    std::string FontFileName;     ///< font file name
    _FcCharSet* FcCharSet;        ///< font config character set, used to test if a character is supported
  };

  typedef std::map<StyledFontFamily, FontCacheItem> FontFamilyLookup;     ///<  lookup for font names and font cache itmes

  typedef std::map<StyledFontFamily, StyledFontFamily> MatchedFontLookup; ///< lookup for fonts that don't exist, and their nearest match return by FontConfig

  boost::mutex      mFontConfigMutex;       ///< FontConfig needs serializing because it isn't thread safe
  boost::mutex      mFontFamilyCacheMutex;  ///< to protect the FontFamilyCache data
  boost::mutex      mFontListMutex;         ///< to prevent more than one thread creating the font list data

  StyledFontFamily  mDefaultStyledFont;     ///< default font

  FontList          mFontSystemList;        ///< cached list of system fonts
  FontList          mFontApplicationList;   ///< cached list of application fonts

  FontFamilyLookup  mFontFamilyCache;       ///< cache of font names and corresponding font cache items
  MatchedFontLookup mMatchedFontsFound;     ///< lookup for fonts that haven't been found on the sytem, and the nearest matching font.
  Vector<StyledFontFamily*> mPreferredFonts;          ///< Ordered list of preferred fonts.
  Vector<bool>              mPreferredFontsValidated; ///< Stores which of the prefered fonts have been validated.

};

} // namespace SlpPlatform

} // namespace Dali

#endif // __DALI_SLP_PLATFORM_FONT_CONTROLLER_H__
