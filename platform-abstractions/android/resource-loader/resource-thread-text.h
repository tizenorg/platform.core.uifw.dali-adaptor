#ifndef __DALI_SLP_PLATFORM_RESOURCE_THREAD_TEXT_H__
#define __DALI_SLP_PLATFORM_RESOURCE_THREAD_TEXT_H__

/*
Copyright (c) 2000-2012 Samsung Electronics Co., Ltd All Rights Reserved

This file is part of Dali Adaptor

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

#include <dali/integration-api/resource-cache.h>


#include "resource-thread-base.h"

struct FT_LibraryRec_; // forward declaration of freetype library handle

namespace Dali
{

namespace Integration
{
class GlyphSet;
class TextResourceType;
}
namespace Platform
{
class DataCache;
}

namespace SlpPlatform
{
/**
 * Resource loader for glyph distance fields.
 * Uses FreeType and a distance field generator to create distance fields.
 * The distance fields are then cached to file for future use.
 */
class ResourceThreadText : public ResourceThreadBase
{
public:
  /**
   * Constructor
   * @param[in] resourceLoader A reference to the ResourceLoader
   * @param[in] quality        The quality at which to render the glyph bitmaps
   */
  ResourceThreadText(ResourceLoader& resourceLoader, const Integration::TextResourceType::TextQuality quality);

  /**
   * Destructor
   */
  virtual ~ResourceThreadText();

private:
  /**
   * @copydoc ResourceThreadBase::Load
   */
  virtual void Load(const Integration::ResourceRequest& request);

  /**
   *@copydoc ResourceThreadBase::Save
   */
  virtual void Save(const Integration::ResourceRequest& request);

  /**
   * Load characters from the data cache.
   * @param[in] dataCache reference to a data cache
   * @param[in] glyphset reference to a glyph set
   * @param[in] requestedCharacters the characters to load from the cache
   */
  void LoadCharactersFromCache( Platform::DataCache& dataCache,
                                Integration::GlyphSet& glyphSet,
                                const Integration::TextResourceType::CharacterList& requestedCharacters);

  /**
   * Save characters to the data cache.
   * @param[in] dataCache reference to a data cache
   * @param[in] glyphset reference to a glyph set
   */
  void SaveCharacters( Platform::DataCache& dataCache,
                       const Integration::GlyphSet& glyphSet);

  /**
   * Loading characters using Freetype and write to data cache
   * @param[in] dataCache reference to a data cache
   * @param[in] glyphset reference to a glyph set
   * @param[in] textRequest the text request
   * @param[in] path font filename
   * @param[in] characterCodes the characters to load
    */
  void LoadCharacters( Platform::DataCache& dataCache,
                       Integration::GlyphSet& glyphSet,
                       const Integration::TextResourceType& textRequest,
                       const std::string& path,
                       const Integration::TextResourceType::CharacterList& characterCodes );

  /**
   * Initialize the freetype library.
   * Called if a glyph is required that isn't cached.
   */
  void LoadFreeType();

  /**
   * Creates a data cache
   * @param[in] fileName font name
   * @return pointer to a data cache
   */
  Platform::DataCache* CreateDataCache( const std::string& fileName );

  /**
   * Deletes all data cache objects
   */
  void DeleteDataCaches();

  /**
   * Returns a data cache for a font and creates one if it doesn't exist
   * @param[in] fileName font name
   * @return a data cache for a given font filename
   */
  Platform::DataCache* GetDataCache( const std::string& fileName );

private:

  typedef std::pair< Platform::DataCache*, std::size_t /* hash value*/ >  TDataCachePair;
  std::vector< TDataCachePair >mDataCaches;               ///< Vector of data caches, hash value
  Integration::TextResourceType::TextQuality mQuality;    ///< The quality at which to render the glyph bitmaps
  FT_LibraryRec_* mFreeTypeHandle;                        ///< Handle to the FreeType2 library

}; // class ResourceThreadText

} // namespace SlpPlatform

} // namespace Dali

#endif // __DALI_SLP_PLATFORM_RESOURCE_THREAD_TEXT_H__
