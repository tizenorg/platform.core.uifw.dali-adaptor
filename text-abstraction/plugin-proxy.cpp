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
uint32_t PluginProxy::Shape( const uint32_t* const text,
                      uint32_t startCharacter,
                      uint32_t characterCount,
                      uint32_t fontId,
                      uint32_t scriptId
                      )
{
  Initialize();

  if( !mPlugin )
  {
    return 0;
  }
  return 0;
}

void PluginProxy::GetGlyphs( GlyphInfo* glyphInfo, uint32_t*  glyphToCharacterMap )
{
  Initialize();

  if( !mPlugin )
  {
    return;
  }
}

void PluginProxy::GetLineBreakPositions(  const uint32_t* const text,
                                     uint32_t startCharacter,
                                     uint32_t characterCount,
                                     uint32_t* positions)
{
  Initialize();

  if( !mPlugin )
  {
    return;
  }
}
void PluginProxy::GetWordBreakPositions(  const uint32_t* const text,
                                     uint32_t startCharacter,
                                     uint32_t characterCount,
                                     uint32_t* positions)
{
  Initialize();

  if( !mPlugin )
  {
    return;
  }
}

uint32_t PluginProxy::CreateBidirectionalInfo( const uint32_t* const text,
                                            uint32_t startCharacter,
                                            uint32_t characterCount )
{
  Initialize();

  if( mPlugin )
  {
    return 0;
  }
  return 0;
}
void PluginProxy::Reorder( uint32_t bidiInfoIndex,
                uint32_t startCharacter,
                uint32_t numberOfCharacters,
                uint32_t* visualToLogicalMap )
{
  Initialize();

  if( !mPlugin )
  {
    return;
  }
}


void PluginProxy::GetMetrics( uint32_t fontId,
                         GlyphInfo* glyphInfoArray,
                         uint32_t numberGlyphs)
{
  Initialize();

  if( !mPlugin )
  {
    return;
  }
}

void PluginProxy::CreateBitmapGlyphs( GlyphBitmap* bitmapArray,
                                  GlyphInfo* glyphInfo,
                                  uint32_t numberGlyphs)
{
  Initialize();

  if( !mPlugin )
  {
    return;
  }


}



uint32_t PluginProxy::ValidateFont( const std::string& fontFamilyName,
                        const std::string& fontStyle )
{
  Initialize();

  if( !mPlugin )
  {
    return 0;
  }
  return 0;
}

 /**
  * @copydoc FontValidationInterface::Validate()
  */
 void PluginProxy::Validate( const uint32_t* const text,
                      uint32_t characterCount,
                      uint32_t* fontIdsToValidate)
{
  Initialize();

  if( !mPlugin )
  {
    return;
  }
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
