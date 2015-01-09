#ifndef __DALI_PLATFORM_TEXT_ABSTRACTION_PLUGIN_PROXY_H__
#define __DALI_PLATFORM_TEXT_ABSTRACTION_PLUGIN_PROXY_H__

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


#include "text-abstraction-interface.h"

#include <text-processing/shaping-interface.h>
#include <text-processing/segmentation-interface.h>
#include <text-processing/reordering-interface.h>
#include <control/clipboard-interface.h>
#include <control/imf-manager-interface.h>
#include <control/virtual-keyboard-interface.h>
#include <glyph/glyph-creation-interface.h>
#include <glyph/glyph-metrics-interface.h>
#include <font/font-validation-interface.h>

namespace Dali
{

namespace TextAbstraction
{

//class AbstractionPlugin;

/**
 *  Proxy is used to load the plugin.
 *  The plugin will only be loaded when the developer calls an API on an interface
 *  like  ShowClipboard()
 *
 */
class PluginProxy :
    public TextAbstractionInterface,
    public GlyphCreationInterface,
    public ReorderingInterface,
    public ShapingInterface,
    public SegmentationInterface,
    public GlyphMetricsInterface,
    public FontValidationInterface
{

public:
  /*
   * constructor
   */
  PluginProxy();

  /**
   * non-virtual destructor, not intended as a base class
   */
   ~PluginProxy();

public:       // TextAbstractionInterface


   virtual ShapingInterface& GetShapingInterface();

   virtual SegmentationInterface& GetSegmentationInterface();

   virtual ReorderingInterface& GetReorderingInterface();

   virtual GlyphMetricsInterface& GetGlyphMetricsInterface();

   virtual GlyphCreationInterface& GetGlyphCreationInterface();

   virtual FontValidationInterface& GetFontValidationInterface();

/*
   potentially use a different mechanism for these 3 interfaces
   virtual VirtualKeyboardInterface& GetVirtualKeyboardInterface();
   virtual ClipboardInterface& GetClipboardInterface();
   virtual ImfManagerInterface& GetImfManagerInterface();
*/


public: // ShapingInterface
   /**
    * @copydoc ShapingInterface::Shape()
    */
   virtual uint32_t Shape( const uint32_t* const text,
                         uint32_t startCharacter,
                         uint32_t characterCount,
                         uint32_t fontId,
                         uint32_t scriptId
                         ) ;

   /**
    * @copydoc ShapingInterface::GetGlyphs()
    */
   virtual void GetGlyphs( GlyphInfo* glyphInfo, uint32_t*  glyphToCharacterMap );


public: // SegmentationInterface

   /**
    * @copydoc SegmentationInterface::GetLineBreakPositions()
    */
   virtual void GetLineBreakPositions(  const uint32_t* const text,
                                        uint32_t startCharacter,
                                        uint32_t characterCount,
                                        uint32_t* positions);


   /**
    * @copydoc SegmentationInterface::GetWordBreakPositions()
    */
   virtual void GetWordBreakPositions(  const uint32_t* const text,
                                        uint32_t startCharacter,
                                        uint32_t characterCount,
                                        uint32_t* positions);

public: // ReorderingInterface

   /**
    * @copydoc ReorderingInterface::CreateBidirectionalInfo()
    */
   virtual uint32_t CreateBidirectionalInfo( const uint32_t* const text,
                                             uint32_t startCharacter,
                                             uint32_t characterCount );

   /**
    * @copydoc ReorderingInterface::Reorder()
    */
   virtual void Reorder( uint32_t bidiInfoIndex,
                 uint32_t startCharacter,
                 uint32_t numberOfCharacters,
                 uint32_t* visualToLogicalMap );


public: // GlyphMetricsInterface

   /**
    * @copydoc GlyphMetricsInterface::GetMetrics()
    */
   virtual void GetMetrics( uint32_t fontId,
                            GlyphInfo* glyphInfoArray,
                            uint32_t numberGlyphs);

public: // GlyphCreationInterface

   /**
    * @copydoc GlyphCreationInterface::CreateBitmapGlyphs()
    */
   virtual void CreateBitmapGlyphs( GlyphBitmap* bitmapArray,
                                     GlyphInfo* glyphInfo,
                                     uint32_t numberGlyphs);

public: // FontValidationInterface

   /**
    * @copydoc FontValidationInterface::ValidateFont()
    */
   virtual uint32_t ValidateFont( const std::string& fontFamilyName,
                          const std::string& fontStyle );

   /**
    * @copydoc FontValidationInterface::Validate()
    */
   virtual void Validate( const uint32_t* const text,
                        uint32_t characterCount,
                        uint32_t* fontIdsToValidate);

private:

    void Initialize();

    void* mPlugin;

    // Undefined copy constructor.
    PluginProxy( const PluginProxy& );

  // Undefined assignment operator.
    PluginProxy& operator=( const PluginProxy& );
};

} // namespace TextAbstraction

} // namespace Dali

#endif // __DALI_PLATFORM_TEXT_ABSTRACTION_PLUGIN_PROXY_H__
