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
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <singleton-service-impl.h>

namespace Dali
{

namespace TextAbstraction
{

namespace Internal
{

struct FontClient::Impl
{
  struct CacheItem
  {
    CacheItem( FontId id, FT_Face ftFace, const std::string& path, FaceIndex face )
    : mId( id ),
      mFreeTypeFace( ftFace ),
      mPath( path ),
      mFaceIndex( face )
    {
    }

    FontId mId;
    FT_Face mFreeTypeFace;
    std::string mPath;
    FaceIndex mFaceIndex;
  };

  Impl()
  : mFreeTypeLibrary( NULL ),
    mNextId( 1 ) // Zero is not a valid ID
  {
  }

  ~Impl()
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

  FontId GetFontId( const std::string& path, FaceIndex faceIndex )
  {
    FontId id( 0 );

    if( NULL != mFreeTypeLibrary )
    {
      FontId foundId(0);
      if( FindFont( path, faceIndex, foundId ) )
      {
        id = foundId;
      }
      else
      {
        // Create & cache new font face
        FT_Face ftFace;
        int error = FT_New_Face( mFreeTypeLibrary,
                                 path.c_str(),
                                 0,
                                 &ftFace );

        if( FT_Err_Ok == error )
        {
          id = NewId();
          mFontCache.push_back( CacheItem( id, ftFace, path, faceIndex ) );
        }
        else
        {
          DALI_LOG_ERROR( "FreeType New_Face error: %d for %s\n", error, path.c_str() );
        }
      }
    }

    return id;
  }

  bool CreateMetrics( FontId fontId, GlyphMetrics& metrics )
  {
    return false;
  }

  bool CreateMetrics( FontId fontId, GlyphMetrics* array, uint32_t size )
  {
    return false;
  }

private:

  bool FindFont( const std::string& path, FaceIndex faceIndex, FontId& found ) const
  {
    for( unsigned int i=0; i<mFontCache.size(); ++i )
    {
      if( mFontCache[i].mPath == path &&
          mFontCache[i].mFaceIndex == faceIndex )
      {
        found = mFontCache[i].mId;
        return true;
      }
    }

    return false;
  }

  FontId NewId()
  {
    return mNextId++;
  }

  FT_Library mFreeTypeLibrary;

  std::vector<CacheItem> mFontCache;
  FontId mNextId;
};

FontClient::FontClient()
: mImpl( NULL )
{
}

FontClient::~FontClient()
{
  delete mImpl;
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

FontId FontClient::GetFontId( const std::string& path, FaceIndex faceIndex )
{
  CreateImpl();

  return mImpl->GetFontId( path, faceIndex );
}

bool FontClient::CreateMetrics( FontId fontId, GlyphMetrics& metrics )
{
  CreateImpl();

  return mImpl->CreateMetrics( fontId, metrics );
}

bool FontClient::CreateMetrics( FontId fontId, GlyphMetrics* array, uint32_t size )
{
  CreateImpl();

  return mImpl->CreateMetrics( fontId, array, size );
}

void FontClient::CreateImpl()
{
  if( !mImpl )
  {
    mImpl = new Impl();
    mImpl->Initialize();
  }
}

} // namespace Internal

} // namespace FontClient

} // namespace Dali
