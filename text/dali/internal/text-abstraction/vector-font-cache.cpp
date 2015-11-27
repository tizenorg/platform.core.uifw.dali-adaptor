/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include <dali/internal/text-abstraction/vector-font-cache.h>

// EXTERNAL INCLUDES
#include <vector>

// INTERNAL INCLUDES
#include "../glyphy/glyphy.h"
#include "../glyphy/glyphy-freetype.h"

using namespace std;

namespace
{

const unsigned int INITIAL_GLYPH_CAPACITY = 50;

} // unnamed namespace

namespace Dali
{

namespace TextAbstraction
{

namespace Internal
{

typedef vector<VectorBlob> BlobArray;

struct VectorGlyph
{
  glyphy_extents_t extents;
  double           advance;
  unsigned int     nominalWidth;
  unsigned int     nominalHeight;
  unsigned int     atlasX;
  unsigned int     atlasY;
  GlyphInfo        glyphInfo;
  BlobArray        blobData;
};

typedef vector<VectorGlyph*> GlyphCache;

struct VectorFont
{
  VectorFont( FT_Face face )
  : mFace( face )
    mGlyphCache()
  {
    mGlyphCache.reserve( INITIAL_GLYPH_CAPACITY );
  }

  FT_Face    mFace;
  GlyphCache mGlyphCache;
};

struct VectorFontCache::Impl
{
  Impl()
  : mIdLookup(),
    mVectorFonts(),
    mAccumulator( NULL )
  {
    mAccumulator = glyphy_arc_accumulator_create();
  }

  Impl()
  : mIdLookup(),
    mVectorFonts(),
    mAccumulator( NULL )
  {
    glyphy_arc_accumulator_destroy( mAccumulator );
  }

  vector<string> mIdLookup;

  vector<VectorFont*> mVectorFonts;

  glyphy_arc_accumulator_t* mAccumulator;
};

VectorFontCache::VectorFontCache()
: mImpl( NULL )
{
  mImpl = new Impl();
}

VectorFontCache::~VectorFontCache()
{
  delete mImpl;
}

FontId GetFontId( const std::string& url )
{
  FontId id( 0 );

  if( mImpl )
  {
    if( ! FindFont( path, id ) )
    {
      id = CreateFont( ? );
    }
  }
}

void VectorFontCache::GetGlyphMetrics( FontId vectorFontId, GlyphInfo& glyphInfo )
{
  if( mImpl )
  {
    if( fontId > 0 &&
        fontId-1 < mVectorFonts.size() )
    {
      VectorFont* font = mVectorFonts[ fontId-1 ];
      GlyphCache& cache = font->mGlyphCache;

      unsigned int i( 0 );
      if( ! FindGlyph( cache, glyphInfo.index, i )
      {
        VectorGlyph* newGlyph = CreateVectorGlyph( font->mFace, glyphInfo.index );
        cache.push_back( newGlyph );
        i = cache.size() - 1;
      }

      glyphInfo = cache[i].glyphInfo;
    }
  }
}

void VectorFontCache::GetVectorBlob( FontId vectorFontId, GlyphIndex glyphIndex, VectorBlob*& blob, unsigned int& blobLength, unsigned int& nominalWidth, unsigned int& nominalHeight )
{
  if( mImpl )
  {
    if( fontId > 0 &&
        fontId-1 < mVectorFonts.size() )
    {
      GlyphCache& cache = mVectorFonts[ fontId-1 ].mGlyphCache;

      // TODO
    }
  }
}

bool VectorFontCache::FindFont( const FontPath& url, FontId& fontId ) const
{
  fontId = 0u;

  const vector<string>& idLookup = mImpl->mIdLookup;

  for( unsigned int i=0; i<idLookup.size(); ++i, ++fontId )
  {
    if( url == idLookup[i] )
    {
      ++fontId;
      return true;
    }
  }

  return false;
}

FontId VectorFontCache::CreateFont( const FontPath& url )
{
  FontId id( 0 );

  // Create & cache new font face
  FT_Face ftFace;
  int error = FT_New_Face( mFreeTypeLibrary,
                           url.c_str(),
                           0,
                           &ftFace );

  if( FT_Err_Ok == error )
  {
    mImpl->mIdLookup.push_back( url );
    id = mImpl->mIdLookup.size();

    VectorFont* newFont = new VectorFont( ftFace );
    mImpl->mVectorFonts.push_back( newFont );

    DALI_ASSERT_DEBUG( mImpl->mIdLookup.size() == mImpl->mVectorFonts.size() );
  }

  return id;
}

} // namespace Internal

} // namespace TextAbstraction

} // namespace Dali
