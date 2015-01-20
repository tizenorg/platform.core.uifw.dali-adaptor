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
#include "shaping-impl.h"

// INTERNAL INCLUDES
#include <singleton-service-impl.h>
#include <dali/public-api/text-abstraction/font-client.h>
#include <dali/public-api/text-abstraction/glyph-info.h>
#include <dali/integration-api/debug.h>

// EXTERNAL INCLUDES
#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ft.h>

#include <ft2build.h>

namespace Dali
{

namespace TextAbstraction
{

namespace Internal
{

const unsigned int HIGH_QUALITY_PIXEL_SIZE = 200u;  // Pixel size sent to FreeType2 FT_Set_Char_Size() for high quality glyphs.
const char*        DEFAULT_LANGUAGE = "en";
const unsigned int DEFAULT_LANGUAGE_LENGTH = 2u;
const float        TO_PIXELS = 64.f;

struct Shaping::Plugin
{
  Plugin()
  : mFreeTypeLibrary( NULL ),
    mIndices(),
    mAdvance(),
    mCharacterMap()
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

  hb_direction_t ScriptToHarfBuzzDirection( Script scriptId )
  {
    hb_direction_t hbDirection = HB_DIRECTION_LTR;

    switch( scriptId )
    {
      case ARABIC:
      {
        hbDirection = HB_DIRECTION_RTL;
        break;
      }
      default:
      {
        hbDirection = HB_DIRECTION_LTR;
        break;
      }
    }

    return hbDirection;
  }

  hb_script_t ScriptToHarfBuzz( Script scriptId )
  {
    hb_script_t hbScript = HB_SCRIPT_COMMON;

    switch( scriptId )
    {
      case LATIN:
      {
        hbScript = HB_SCRIPT_LATIN;
        break;
      }
      case ARABIC:
      {
        hbScript = HB_SCRIPT_ARABIC;
        break;
      }
      case DEVANAGARI:
      {
        hbScript = HB_SCRIPT_DEVANAGARI;
        break;
      }
      case BENGALI:
      {
        hbScript = HB_SCRIPT_BENGALI;
        break;
      }
      case GURMUKHI:
      {
        hbScript = HB_SCRIPT_GURMUKHI;
        break;
      }
      case GUJARATI:
      {
        hbScript = HB_SCRIPT_GUJARATI;
        break;
      }
      case ORIYA:
      {
        hbScript = HB_SCRIPT_ORIYA;
        break;
      }
      case TAMIL:
      {
        hbScript = HB_SCRIPT_TAMIL;
        break;
      }
      case TELUGU:
      {
        hbScript = HB_SCRIPT_TELUGU;
        break;
      }
      case KANNADA:
      {
        hbScript = HB_SCRIPT_KANNADA;
        break;
      }
      case MALAYALAM:
      {
        hbScript = HB_SCRIPT_MALAYALAM;
        break;
      }
      case SINHALA:
      {
        hbScript = HB_SCRIPT_SINHALA;
        break;
      }
      case CJK:
      {
        hbScript = HB_SCRIPT_HAN;
        break;
      }
      case HANGUL:
      {
        hbScript = HB_SCRIPT_HANGUL;
        break;
      }
      case KHMER:
      {
        hbScript = HB_SCRIPT_KHMER;
        break;
      }
      case LAO:
      {
        hbScript = HB_SCRIPT_LAO;
        break;
      }
      case THAI:
      {
        hbScript = HB_SCRIPT_THAI;
        break;
      }
      case BURMESE:
      {
        hbScript = HB_SCRIPT_MYANMAR;
        break;
      }
      case UNKNOWN:
      {
        hbScript = HB_SCRIPT_UNKNOWN;
        break;
      }
    }

    return hbScript;
  }

  Length Shape( const Character* const text,
                Length numberOfCharacters,
                FontId fontId,
                Script script )
  {
    // Clear previoursly shaped texts.
    mIndices.Clear();
    mAdvance.Clear();
    mCharacterMap.Clear();

    TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();

    // Get the font's path file name from the font Id.
    FontDescription fontDescription;
    fontClient.GetDescription( fontId, fontDescription );

    // Create a FreeType font's face.
    FT_Face face;
    FT_Error retVal = FT_New_Face( mFreeTypeLibrary, fontDescription.path.c_str(), 0u, &face );
    if( FT_Err_Ok != retVal )
    {
      DALI_LOG_ERROR( "Failed to open face: %s\n", fontDescription.path.c_str() );
      return 0u;
    }

    FT_Set_Pixel_Sizes( face, HIGH_QUALITY_PIXEL_SIZE, HIGH_QUALITY_PIXEL_SIZE );

    /* Get our harfbuzz font struct */
    hb_font_t* harfBuzzFont;
    harfBuzzFont = hb_ft_font_create( face, NULL );

    /* Create a buffer for harfbuzz to use */
    hb_buffer_t* harfBuzzBuffer = hb_buffer_create();

    hb_buffer_set_direction( harfBuzzBuffer,
                             ScriptToHarfBuzzDirection( script ) ); /* or LTR */

    hb_buffer_set_script( harfBuzzBuffer,
                          ScriptToHarfBuzz( script ) ); /* see hb-unicode.h */

    hb_buffer_set_language( harfBuzzBuffer,
                            hb_language_from_string( DEFAULT_LANGUAGE,
                                                     DEFAULT_LANGUAGE_LENGTH ) );

    /* Layout the text */
    hb_buffer_add_utf32( harfBuzzBuffer, text, numberOfCharacters, 0u, numberOfCharacters );

    hb_shape( harfBuzzFont, harfBuzzBuffer, NULL, 0u );

    /* Get glyph data */
    unsigned int glyphCount;
    hb_glyph_info_t* glyphInfo = hb_buffer_get_glyph_infos( harfBuzzBuffer, &glyphCount );
    hb_glyph_position_t *glyphPositions = hb_buffer_get_glyph_positions( harfBuzzBuffer, &glyphCount );

    for( Length i = 0u; i < glyphCount; ++i )
    {
      mIndices.PushBack( glyphInfo[i].codepoint );
      mAdvance.PushBack( glyphPositions[i].x_advance / TO_PIXELS );
      mCharacterMap.PushBack( glyphInfo[i].cluster );
    }

    /* Cleanup */
    hb_buffer_destroy( harfBuzzBuffer );
    hb_font_destroy( harfBuzzFont );
    FT_Done_Face( face );

    return mIndices.Count();
  }

  void GetGlyphs( GlyphInfo* glyphInfo,
                  CharacterIndex* glyphToCharacterMap )
  {
    Vector<CharacterIndex>::ConstIterator indicesIt = mIndices.Begin();
    Vector<float>::ConstIterator advanceIt = mAdvance.Begin();
    Vector<CharacterIndex>::ConstIterator characterMapIt = mCharacterMap.Begin();

    for( Length index = 0u, size = mIndices.Count(); index < size; ++index )
    {
      GlyphInfo& glyph = *( glyphInfo + index );
      CharacterIndex& glyphToCharacter = *( glyphToCharacterMap + index );

      glyph.index = *( indicesIt + index );
      glyph.advance = *( advanceIt + index );

      glyphToCharacter = *( characterMapIt + index );
    }
  }

  FT_Library             mFreeTypeLibrary;

  Vector<CharacterIndex> mIndices;
  Vector<float>          mAdvance;
  Vector<CharacterIndex> mCharacterMap;
};

Shaping::Shaping()
: mPlugin( NULL )
{
}

Shaping::~Shaping()
{
  delete mPlugin;
}

TextAbstraction::Shaping Shaping::Get()
{
  TextAbstraction::Shaping shapingHandle;

  SingletonService service( SingletonService::Get() );
  if( service )
  {
    // Check whether the singleton is already created
    Dali::BaseHandle handle = service.GetSingleton( typeid( TextAbstraction::Shaping ) );
    if( handle )
    {
      // If so, downcast the handle
      Shaping* impl = dynamic_cast< Internal::Shaping* >( handle.GetObjectPtr() );
      shapingHandle = TextAbstraction::Shaping( impl );
    }
    else // create and register the object
    {
      shapingHandle = TextAbstraction::Shaping( new Shaping );
      service.Register( typeid( shapingHandle ), shapingHandle );
    }
  }

  return shapingHandle;
}

Length Shaping::Shape( const Character* const text,
                       Length numberOfCharacters,
                       FontId fontId,
                       Script script )
{
  CreatePlugin();

  return mPlugin->Shape( text,
                         numberOfCharacters,
                         fontId,
                         script );
}

void Shaping::GetGlyphs( GlyphInfo* glyphInfo,
                         CharacterIndex* glyphToCharacterMap )
{
  CreatePlugin();

  mPlugin->GetGlyphs( glyphInfo,
                      glyphToCharacterMap );
}

void Shaping::CreatePlugin()
{
  if( !mPlugin )
  {
    mPlugin = new Plugin();
    mPlugin->Initialize();
  }
}

} // namespace Internal

} // namespace TextAbstraction

} // namespace Dali
