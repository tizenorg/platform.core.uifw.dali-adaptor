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

// CLASS HEADER
#include "plugin-proxy.h"

namespace Dali
{

namespace TextAbstraction
{

const std::string VOID_STRING; // To be removed!

ShapingInterface& PluginProxy::GetShapingInterface()
{
  return *this;
}
SegmentationInterface& PluginProxy::GetSegmentationInterface()
{
  return *this;
}
ReorderingInterface& PluginProxy::GetReorderingInterface()
{
  return *this;
}
GlyphMetricsInterface& PluginProxy::GetGlyphMetricsInterface()
{
  return *this;
}
GlyphCreationInterface& PluginProxy::GetGlyphCreationInterface()
{
  return *this;
}
FontValidationInterface& PluginProxy::GetFontValidationInterface()
{
  return *this;
}
/*
VirtualKeyboardInterface& PluginProxy::GetVirtualKeyboardInterface()
{
  return *this;
}
ClipboardInterface& PluginProxy::GetClipboardInterface()
{
  return *this;
}
ImfManagerInterface& PluginProxy::GetImfManagerInterface()
{
  return *this;
}
*/


/***************************
 *
 * Plugin Intercepter functions
 *
 **************************/
Length PluginProxy::Shape( const Character* const text,
                           Length numberOfCharacters,
                           FontId fontId,
                           Script scriptId )
{
  Initialize();

  if( !mPlugin )
  {
    return 0;
  }
  return 0;
}

void PluginProxy::GetGlyphs( GlyphInfo* glyphInfo,
                             Index* glyphToCharacterMap )
{
  Initialize();

  if( !mPlugin )
  {
    return;
  }
}

void PluginProxy::GetLineBreakPositions( const Character* const text,
                                         Length numberOfCharacters,
                                         LineBreakInfo* breakInfo )
{
  Initialize();

  if( !mPlugin )
  {
    return;
  }
}
void PluginProxy::GetWordBreakPositions( const uint32_t* const text,
                                         Length numberOfCharacters,
                                         WordBreakInfo* breakInfo )
{
  Initialize();

  if( !mPlugin )
  {
    return;
  }
}

Index PluginProxy::CreateBidirectionalInfo( const Character* const text,
                                            Length numberOfCharacters )
{
  Initialize();

  if( mPlugin )
  {
    return 0u;
  }
  return 0u;
}

Index PluginProxy::DestroyBidirectionalInfo( Index bidiInfoIndex )
{
  Initialize();

  if( mPlugin )
  {
    return 0u;
  }

  return 0u;
}

void PluginProxy::Reorder( Index bidiInfoIndex,
                           Index firstCharacterIndex,
                           Length numberOfCharacters,
                           Index* visualToLogicalMap )
{
  Initialize();

  if( !mPlugin )
  {
    return;
  }
}


void PluginProxy::GetMetrics( FontId fontId,
                              GlyphInfo* glyphInfo,
                              Length numberOfGlyphs )
{
  Initialize();

  if( !mPlugin )
  {
    return;
  }
}

void PluginProxy::CreateBitmapGlyphs( GlyphBitmap* bitmapArray,
                                      GlyphInfo* glyphInfo,
                                      Length numberOfGlyphs )
{
  Initialize();

  if( !mPlugin )
  {
    return;
  }
}



FontId PluginProxy::ValidateFont( const std::string& fontFamilyName,
                                  const std::string& fontStyle )
{
  Initialize();

  if( !mPlugin )
  {
    return 0u;
  }
  return 0u;
}

const std::string& PluginProxy::GetValidatedFontFilePath( FontId fontId ) const
{
  return VOID_STRING;
}

Length PluginProxy::Validate( const Character* const text,
                              Index characterIndex,
                              Length numberOfCharacters,
                              const FontRun* const fontIdsToValidate,
                              Length numberOfFontRuns )
{
  Initialize();

  if( !mPlugin )
  {
    return 0u;
  }

  return 0u;
}

void PluginProxy::GetValidatedFontRuns( FontRun* fontIdsToValidate )
{
}

PluginProxy::PluginProxy()
:mPlugin( NULL )
{

}

PluginProxy::~PluginProxy()
{
  //delete mPlugin;
}

void PluginProxy::Initialize()
{
  if( mPlugin )
  {
    return;
  }
}

} // namespace TextAbstraction

} // namespace Dali
