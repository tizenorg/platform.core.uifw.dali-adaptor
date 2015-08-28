#ifndef __DALI_INTERNAL_TEXT_ABSTRACTION_FONT_CLIENT_PLUGIN_IMPL_H__
#define __DALI_INTERNAL_TEXT_ABSTRACTION_FONT_CLIENT_PLUGIN_IMPL_H__

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

// INTERNAL INCLUDES
#include <dali/devel-api/text-abstraction/font-metrics.h>
#include <dali/devel-api/text-abstraction/glyph-info.h>
#include <dali/internal/text-abstraction/font-client-impl.h>

// EXTERNAL INCLUDES
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

// forward declarations of font config types.
struct _FcFontSet;
struct _FcPattern;

namespace Dali
{

namespace TextAbstraction
{

namespace Internal
{

/**
 *@brief Type used for indices addressing the vector with front descriptions of validated pairs 'font family name, font style'.
 */
typedef uint32_t FontDescriptionId;

/**
 * @brief FontClient implementation.
 */
struct FontClient::Plugin
{
  /**
   * @brief Caches an index to the vector of font descriptions for a given 'font family name, font style'.
   */
  struct FontDescriptionCacheItem
  {
    FontDescriptionCacheItem( const FontDescription& fontDescription,
                              FontDescriptionId index );

    FontDescription fontDescription; ///< The font description.
    FontDescriptionId index;         ///< Index to the vector of font descriptions.
  };

  /**
   * @brief Caches the font id of the pair font point size and the index to the vector of font descriptions of validated fonts.
   */
  struct FontIdCacheItem
  {
    FontIdCacheItem( FontDescriptionId validatedFontId,
                     PointSize26Dot6 pointSize,
                     FontId fontId );

    FontDescriptionId validatedFontId; ///< Index to the vector with font descriptions.
    PointSize26Dot6   pointSize;       ///< The font point size.
    FontId            fontId;          ///< The font id.
  };

  /**
   * @brief Caches the FreeType face and font metrics of the triplet 'path to the font file name, font point size and face index'.
   */
  struct CacheItem
  {
    CacheItem( FT_Face ftFace,
               const FontPath& path,
               PointSize26Dot6 pointSize,
               FaceIndex face,
               const FontMetrics& metrics );

    CacheItem( FT_Face ftFace,
               const FontPath& path,
               PointSize26Dot6 pointSize,
               FaceIndex face,
               const FontMetrics& metrics,
               float fixedWidth,
               float fixedHeight );

    FT_Face mFreeTypeFace;       ///< The FreeType face.
    FontPath mPath;              ///< The path to the font file name.
    PointSize26Dot6 mPointSize;  ///< The font point size.
    FaceIndex mFaceIndex;        ///< The face index.
    FontMetrics mMetrics;        ///< The font metrics.
    FT_Short mFixedWidthPixels;  ///< The height in pixels (fixed size bitmaps only)
    FT_Short mFixedHeightPixels; ///< The height in pixels (fixed size bitmaps only)
    bool mIsFixedSizeBitmap;     ///< Whether the font has fixed size bitmaps.
  };

  struct EllipsisItem
  {
    PointSize26Dot6 size;
    GlyphInfo glyph;
  };

  /**
   * Constructor.
   *
   * Initializes the FreeType library.
   * Initializes the dpi values.
   *
   * @param[in] horizontalDpi The horizontal dpi.
   * @param[in] verticalDpi The vertical dpi.
   */
  Plugin( unsigned int horizontalDpi, unsigned int verticalDpi );

  /**
   * Default destructor.
   *
   * Frees any allocated resource.
   */
  ~Plugin();

  /**
   * @copydoc Dali::FontClient::SetDpi()
   */
  void SetDpi( unsigned int horizontalDpi, unsigned int verticalDpi );

  /**
   * @copydoc Dali::FontClient::SetDefaultFont()
   */
  void SetDefaultFont( const FontDescription& fontDescription );

  /**
   * @copydoc Dali::FontClient::GetDefaultFonts()
   */
  void GetDefaultFonts( FontList& defaultFonts );

  /**
   * @copydoc Dali::FontClient::GetSystemFonts()
   */
  void GetSystemFonts( FontList& systemFonts );

  /**
   * @copydoc Dali::FontClient::GetDescription()
   */
  void GetDescription( FontId id, FontDescription& fontDescription ) const;

  /**
   * @copydoc Dali::FontClient::GetPointSize()
   */
  PointSize26Dot6 GetPointSize( FontId id );

  /**
   * @copydoc Dali::FontClient::FindDefaultFont()
   */
  FontId FindDefaultFont( Character charcode, PointSize26Dot6 pointSize, bool preferColor );

  /**
   * @see Dali::FontClient::GetFontId( const FontPath& path, PointSize26Dot6 pointSize, FaceIndex faceIndex )
   *
   * @param[in] cacheDescription Whether to cache the font description.
   */
  FontId GetFontId( const FontPath& path, PointSize26Dot6 pointSize, FaceIndex faceIndex, bool cacheDescription = true );

  /**
   * @copydoc Dali::FontClient::GetFontId( const FontDescription& fontDescription, PointSize26Dot6 pointSize, FaceIndex faceIndex )
   */
  FontId GetFontId( const FontDescription& fontDescription,
                    PointSize26Dot6 pointSize,
                    FaceIndex faceIndex );

  /**
   * @copydoc Dali::FontClient::IsScalable( const FontPath& path )
   */
  bool IsScalable( const FontPath& path );

  /**
   * @copydoc Dali::FontClient::IsScalable( const FontDescription& fontDescription )
   */
  bool IsScalable( const FontDescription& fontDescription );

  /**
   * @copydoc Dali::FontClient::GetFixedSizes()
   */
  void GetFixedSizes( const FontPath& path, Dali::Vector< PointSize26Dot6>& sizes );

  /**
   * @copydoc Dali::FontClient::GetFixedSizes()
   */
  void GetFixedSizes( const FontDescription& fontDescription,
                      Dali::Vector< PointSize26Dot6 >& sizes );

  /**
   * @copydoc Dali::FontClient::GetFontMetrics()
   */
  void GetFontMetrics( FontId fontId, FontMetrics& metrics, int maxFixedSize );

  /**
   * @copydoc Dali::FontClient::GetGlyphIndex()
   */
  GlyphIndex GetGlyphIndex( FontId fontId, Character charcode );

  /**
   * @copydoc Dali::FontClient::GetGlyphMetrics()
   */
  bool GetGlyphMetrics( GlyphInfo* array, uint32_t size, bool horizontal, int maxFixedSize );

  /**
   * @copydoc Dali::FontClient::CreateBitmap()
   */
  BufferImage CreateBitmap( FontId fontId, GlyphIndex glyphIndex );

  /**
   * @copydoc Dali::FontClient::GetEllipsisGlyph()
   */
  const GlyphInfo& GetEllipsisGlyph( PointSize26Dot6 pointSize );

private:

  /**
   * Caches the fonts present in the platform.
   *
   * Calls GetFcFontSet() to retrieve the fonts.
   */
  void InitSystemFonts();

  /**
   * @brief Creates a font family pattern used to match fonts.
   *
   * @param[in] fontFamily The font family name.
   * @param[in] fontStyle The font style.
   *
   * @return The pattern.
   */
  _FcPattern* CreateFontFamilyPattern( const FontDescription& fontDescription );

  /**
   * Retrieves the fonts present in the platform.
   *
   * @return A font fonfig data structure with the platform's fonts.
   */
  _FcFontSet* GetFcFontSet() const;

  /**
   * Retrieves a font config object's value from a pattern.
   *
   * @param[in] pattern The font config pattern.
   * @param[in] n The object.
   * @param[out] string The object's value.
   *
   * @return @e true if the operation is successful.
   */
  bool GetFcString( const _FcPattern* const pattern, const char* const n, std::string& string );

  /**
   * Retrieves a font config object's value from a pattern.
   *
   * @param[in] pattern The font config pattern.
   * @param[in] n The object.
   * @param[out] intVal The object's value.
   *
   * @return @e true if the operation is successful.
   */
  bool GetFcInt( const _FcPattern* const pattern, const char* const n, int& intVal );

  /**
   * Retrieves a font config object's value from a pattern.
   *
   * @param[in] pattern The font config pattern.
   * @param[in] n The object.
   * @param[out] intBool The object's value.
   *
   * @return @e true if the operation is successful.
   */
  bool GetFcBool( const _FcPattern* const pattern, const char* const n, bool& boolVal );

  /**
   * @brief Creates a font.
   *
   * @param[in] path The path to the font file name.
   * @param[in] pointSize The font point size.
   * @param[in] faceIndex A face index.
   * @param[in] cacheDescription Whether to cache the font description.
   *
   * @return The font id.
   */
  FontId CreateFont( const FontPath& path, PointSize26Dot6 pointSize, FaceIndex faceIndex, bool cacheDescription );

  /**
   * @brief Creates a fixed size font
   *
   * @param[in] path The path to the font file name.
   * @param[in] pointSize The font point size( must be an available size ).
   * @param[in] faceIndex A face index.
   * @param[in] cacheDescription Whether to cache the font description.
   *
   * @return The font id.
   */
  FontId CreateFixedSizeFont( const FontPath& path, PointSize26Dot6 pointSize, FaceIndex faceIndex, bool cacheDescription );

  /**
   *
   * @param[in] destBitmap
   * @param[in] srcBitmap
   */
  void ConvertBitmap( BufferImage& destBitmap, FT_Bitmap srcBitmap );

  /**
   * @brief Finds in the cache if there is a triplet with the path to the font file name, the font point size and the face index.
   * If there is one , if writes the font id in the param @p fontId.
   *
   * @param[in] path Path to the font file name.
   * @param[in] pointSize The font point size.
   * @param[in] faceIndex The face index.
   * @param[out] fontId The font id.
   *
   * @return @e true if there triplet is found.
   */
  bool FindFont( const FontPath& path, PointSize26Dot6 pointSize, FaceIndex faceIndex, FontId& fontId ) const;

  /**
   * @brief Finds in the cahce a cluster 'font family, font style, font width' or 'font family, font weight, font width, font slant'
   * If there is one, it writes the index to the vector with font descriptions in the param @p  validatedFontId.
   *
   * @param[in] fontDescription The font to validate.
   * @param[out] validatedFontId The index to the vector with font descriptions.
   *
   * @return @e true if the pair is found.
   */
  bool FindValidatedFont( const FontDescription& fontDescription,
                          FontDescriptionId& validatedFontId );

  /**
   * @brief Finds in the cache a pair 'validated font id and font point size'.
   * If there is one it writes the font id in the param @p fontId.
   *
   * @param[in] validatedFontId Index to the vector with font descriptions.
   * @param[in] pointSize The font point size.
   * @param[out] fontId The font id.
   *
   * @return @e true if the pair is found.
   */
  bool FindFont( FontDescriptionId validatedFontId,
                 PointSize26Dot6 pointSize,
                 FontId& fontId );

  /**
   * @brief Validate a font family and style
   *
   * @note If the font style is not empty it will be used instead the weight and slant.
   *
   * @param[in] fontDescription The font to validate.
   * @param[out] validatedFontId Result of validation
   */
  void ValidateFont( const FontDescription& fontDescription,
                     FontDescriptionId& validatedFontId );

  FT_Library mFreeTypeLibrary; ///< A handle to a FreeType library instance.

  unsigned int mDpiHorizontal; ///< Horizontal dpi.
  unsigned int mDpiVertical;   ///< Vertical dpi.

  FontList mSystemFonts;       ///< Cached system fonts.
  FontList mDefaultFonts;      ///< Cached default fonts.

  std::vector<CacheItem>                mFontCache;            ///< Caches the FreeType face and font metrics of the triplet 'path to the font file name, font point size and face index'.
  std::vector<FontDescriptionCacheItem> mValidatedFontCache;   ///< Caches indices to the vector of font descriptions for a given 'font family name, font style'.
  FontList                              mFontDescriptionCache; ///< Caches font descriptions for the validated font family name and font style pairs.
  std::vector<FontIdCacheItem>          mFontIdCache;          ///< Caches font ids for the pairs of font point size and the index to the vector with font descriptions of the validated fonts.

  Vector<EllipsisItem> mEllipsisCache;      ///< Caches ellipsis glyphs for a particular point size.
};

} // namespace Internal

} // namespace TextAbstraction

} // namespace Dali

#endif // __DALI_INTERNAL_TEXT_ABSTRACTION_FONT_CLIENT_PLUGIN_IMPL_H__
