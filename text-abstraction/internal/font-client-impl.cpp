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

// CLASS  HEADER
#include "font-client-impl.h"

// EXTERNAL INCLUDES
#include <vector>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include <fontconfig/fontconfig.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <singleton-service-impl.h>

namespace Dali
{

namespace TextAbstraction
{

namespace Internal
{

struct FontClient::Plugin
{
  struct CacheItem
  {
    CacheItem( FontId id, FT_Face ftFace, const std::string& path, PointSize26Dot6 pointSize, FaceIndex face )
    : mFreeTypeFace( ftFace ),
      mPath( path ),
      mPointSize( pointSize ),
      mFaceIndex( face )
    {
    }

    FT_Face mFreeTypeFace;
    std::string mPath;
    PointSize26Dot6 mPointSize;
    FaceIndex mFaceIndex;
  };

  Plugin( unsigned int horizontalDpi, unsigned int verticalDpi )
  : mFreeTypeLibrary( NULL ),
    mDpiHorizontal( horizontalDpi ),
    mDpiVertical( verticalDpi )
  {
  }

  ~Plugin()
  {
    FT_Done_FreeType( mFreeTypeLibrary );
  }

  void Initialize()
  {
    int error = FT_Init_FreeType( &mFreeTypeLibrary );
    if( FT_Err_Ok != error )
    {
      DALI_LOG_ERROR( "FreeType Init error: %d\n", error );
    }
  }

  void SetDpi( unsigned int horizontalDpi, unsigned int verticalDpi  )
  {
    mDpiVertical = horizontalDpi;
    mDpiVertical = verticalDpi;
  }

  void GetSystemFonts( FontList& systemFonts )
  {
    if( !mSystemFonts.empty() )
    {
      InitSystemFonts();
    }

    systemFonts = mSystemFonts;
  }

  void InitSystemFonts()
  {
    FcFontSet* fontSet = GetFcFontSet();

    if( fontSet )
    {
      mSystemFonts.reserve( fontSet->nfont );

      for( int i = 0u; i < fontSet->nfont; ++i )
      {
        FcPattern* fontPattern = fontSet->fonts[i];

        std::string path;

        // Skip fonts with no path
        if( GetFcString( fontPattern, FC_FILE, path ) )
        {
          mSystemFonts.push_back( FontDescription() );
          FontDescription& fontDescription = mSystemFonts.back();

          fontDescription.path = path;

          GetFcString( fontPattern, FC_FAMILY, fontDescription.family );
          GetFcString( fontPattern, FC_STYLE, fontDescription.style );
        }
      }

      FcFontSetDestroy( fontSet );
    }
  }

  _FcFontSet* GetFcFontSet() const
  {
    // create a new pattern.
    // a pattern holds a set of names, each name refers to a property of the font
    FcPattern* pattern = FcPatternCreate();

    // create an object set used to define which properties are to be returned in the patterns from FcFontList.
    FcObjectSet* objectSet = FcObjectSetCreate();

    // build an object set from a list of property names
    FcObjectSetAdd( objectSet, FC_FILE );
    FcObjectSetAdd( objectSet, FC_FAMILY );
    FcObjectSetAdd( objectSet, FC_STYLE );

    // get a list of fonts
    // creates patterns from those fonts containing only the objects in objectSet and returns the set of unique such patterns
    FcFontSet* fontset = FcFontList( NULL /* the default configuration is checked to be up to date, and used */, pattern, objectSet );

    // clear up the object set
    if( objectSet )
    {
      FcObjectSetDestroy( objectSet );
    }
    // clear up the pattern
    if( pattern )
    {
      FcPatternDestroy( pattern );
    }

    return fontset;
  }

  bool GetFcString( const FcPattern* pattern, const char* n, std::string& string )
  {
    FcChar8* file = NULL;
    const FcResult retVal = FcPatternGetString( pattern, n, 0u, &file );

    if( FcResultMatch == retVal )
    {
      // Have to use reinterpret_cast because FcChar8 is unsigned char*, not a const char*.
      string.assign( reinterpret_cast<const char*>( file ) );

      return true;
    }

    return false;
  }

  FontId GetFontId( const std::string& path, PointSize26Dot6 pointSize, FaceIndex faceIndex )
  {
    FontId id( 0 );

    if( NULL != mFreeTypeLibrary )
    {
      FontId foundId(0);
      if( FindFont( path, pointSize, faceIndex, foundId ) )
      {
        id = foundId;
      }
      else
      {
        id = CreateFont( path, pointSize, faceIndex );
      }
    }

    return id;
  }

  FontId CreateFont( const std::string& path, PointSize26Dot6 pointSize, FaceIndex faceIndex )
  {
    FontId id( 0 );

    // Create & cache new font face
    FT_Face ftFace;
    int error = FT_New_Face( mFreeTypeLibrary,
                             path.c_str(),
                             0,
                             &ftFace );

    if( FT_Err_Ok == error )
    {
      error = FT_Set_Char_Size( ftFace,
                                0,
                                pointSize,
                                mDpiHorizontal,
                                mDpiVertical );

      if( FT_Err_Ok == error )
      {
        id = mFontCache.size() + 1;
        mFontCache.push_back( CacheItem( id, ftFace, path, pointSize, faceIndex ) );
      }
      else
      {
        DALI_LOG_ERROR( "FreeType Set_Char_Size error: %d for pointSize %d\n", pointSize );
      }
    }
    else
    {
      DALI_LOG_ERROR( "FreeType New_Face error: %d for %s\n", error, path.c_str() );
    }

    return id;
  }

  bool CreateMetrics( FontId fontId, GlyphMetrics* array, uint32_t size, bool horizontal )
  {
    bool success( true );

    if( fontId > 0 &&
        fontId-1 < mFontCache.size() )
    {
      FT_Face ftFace = mFontCache[fontId-1].mFreeTypeFace;

      for( unsigned int i=0; i<size; ++i )
      {
        int error = FT_Load_Glyph( ftFace, array[i].index, FT_LOAD_DEFAULT );

        if( FT_Err_Ok == error )
        {
          array[i].width  = ftFace->glyph->metrics.width;
          array[i].height = ftFace->glyph->metrics.height;
          if( horizontal )
          {
            array[i].xBearing = ftFace->glyph->metrics.horiBearingX;
            array[i].yBearing = ftFace->glyph->metrics.horiBearingY;
            array[i].advance  = ftFace->glyph->metrics.horiAdvance;
          }
          else
          {
            array[i].xBearing = ftFace->glyph->metrics.vertBearingX;
            array[i].yBearing = ftFace->glyph->metrics.vertBearingY;
            array[i].advance  = ftFace->glyph->metrics.vertAdvance;
          }
        }
        else
        {
          success = false;
        }
      }
    }

    return success;
  }

  BitmapImage CreateBitmap( FontId fontId, GlyphIndex glyphIndex )
  {
    BitmapImage bitmap;

    if( fontId > 0 &&
        fontId-1 < mFontCache.size() )
    {
      FT_Face ftFace = mFontCache[fontId-1].mFreeTypeFace;

      FT_Error error = FT_Load_Glyph( ftFace, glyphIndex, FT_LOAD_DEFAULT );
      if( FT_Err_Ok != error )
      {
        FT_Glyph glyph;
        error = FT_Get_Glyph( ftFace->glyph, &glyph );

        // Convert to bitmap if necessary
        if ( FT_Err_Ok != error &&
             glyph->format != FT_GLYPH_FORMAT_BITMAP)
        {
          error = FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, 0, 1);
        }

        if( FT_Err_Ok != error )
        {
          // Access the underlying bitmap data
          FT_BitmapGlyph bitmapGlyph = (FT_BitmapGlyph)glyph;
          ConvertBitmap( bitmap, bitmapGlyph->bitmap );
        }

        // Created FT_Glyph object must be released with FT_Done_Glyph
        FT_Done_Glyph( glyph );
      }
    }

    return bitmap;
  }

  void ConvertBitmap( BitmapImage& destBitmap, FT_Bitmap srcBitmap )
  {
    if( srcBitmap.width*srcBitmap.rows > 0 )
    {
      // TODO
      std::cout << "pixel_mode: " << srcBitmap.pixel_mode << std::endl;

      //destBitmap = BitmapImage::New(  );
      //bitmap.buffer
    }
  }

private:

  bool FindFont( const std::string& path, PointSize26Dot6 pointSize, FaceIndex faceIndex, FontId& found ) const
  {
    for( unsigned int i=0; i<mFontCache.size(); ++i )
    {
      if( mFontCache[i].mPointSize == pointSize &&
          mFontCache[i].mPath == path &&
          mFontCache[i].mFaceIndex == faceIndex )
      {
        found = i + 1;
        return true;
      }
    }

    return false;
  }

  FT_Library mFreeTypeLibrary;

  FontList mSystemFonts;

  std::vector<CacheItem> mFontCache;

  unsigned int mDpiHorizontal;
  unsigned int mDpiVertical;
};

FontClient::FontClient()
: mPlugin( NULL )
{
}

FontClient::~FontClient()
{
  delete mPlugin;
}

Dali::TextAbstraction::FontClient FontClient::Get()
{
  Dali::TextAbstraction::FontClient fontClientHandle;

  Dali::SingletonService service( SingletonService::Get() );
  if ( service )
  {
    // Check whether the singleton is already created
    Dali::BaseHandle handle = service.GetSingleton( typeid( Dali::TextAbstraction::FontClient ) );
    if(handle)
    {
      // If so, downcast the handle
      FontClient* impl = dynamic_cast< Dali::TextAbstraction::Internal::FontClient* >( handle.GetObjectPtr() );
      fontClientHandle = Dali::TextAbstraction::FontClient( impl );
    }
    else // create and register the object
    {
      fontClientHandle = Dali::TextAbstraction::FontClient( new FontClient );
      service.Register( typeid( fontClientHandle ), fontClientHandle );
    }
  }

  return fontClientHandle;
}

void FontClient::SetDpi( unsigned int horizontalDpi, unsigned int verticalDpi  )
{
  mDpiVertical = horizontalDpi;
  mDpiVertical = verticalDpi;

  // Allow DPI to be set without loading plugin
  if( mPlugin )
  {
    mPlugin->SetDpi( horizontalDpi, verticalDpi  );
  }
}

void FontClient::GetSystemFonts( FontList& systemFonts )
{
  CreatePlugin();

  mPlugin->GetSystemFonts( systemFonts );
}

FontId FontClient::GetFontId( const FontPath& path, PointSize26Dot6 pointSize, FaceIndex faceIndex )
{
  CreatePlugin();

  return mPlugin->GetFontId( path, pointSize, faceIndex );
}

bool FontClient::CreateMetrics( FontId fontId, GlyphMetrics* array, uint32_t size, bool horizontal )
{
  CreatePlugin();

  return mPlugin->CreateMetrics( fontId, array, size, horizontal );
}

BitmapImage FontClient::CreateBitmap( FontId fontId, GlyphIndex glyphIndex )
{
  CreatePlugin();

  return mPlugin->CreateBitmap( fontId, glyphIndex );
}

void FontClient::CreatePlugin()
{
  if( !mPlugin )
  {
    mPlugin = new Plugin( mDpiHorizontal, mDpiVertical );
    mPlugin->Initialize();
  }
}

} // namespace Internal

} // namespace FontClient

} // namespace Dali
