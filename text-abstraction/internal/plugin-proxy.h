#ifndef __DALI_TEXT_ABSTRACTION_PLUGIN_PROXY_H__
#define __DALI_TEXT_ABSTRACTION_PLUGIN_PROXY_H__

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
#include <text-abstraction/public-api/text-abstraction-interface.h>
#include <text-abstraction/internal/control/clipboard-interface.h>
#include <text-abstraction/internal/control/imf-manager-interface.h>
#include <text-abstraction/internal/control/virtual-keyboard-interface.h>
#include <text-abstraction/internal/font/font-validation-interface.h>
#include <text-abstraction/internal/glyph/glyph-creation-interface.h>
#include <text-abstraction/internal/glyph/glyph-metrics-interface.h>
#include <text-abstraction/internal/text-processing/reordering-interface.h>
#include <text-abstraction/internal/text-processing/segmentation-interface.h>
#include <text-abstraction/internal/text-processing/shaping-interface.h>

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
  virtual Length Shape( const Character* const text,
                        Length numberOfCharacters,
                        FontId fontId,
                        Script scriptId );

  /**
   * @copydoc ShapingInterface::GetGlyphs()
   */
  virtual void GetGlyphs( GlyphInfo* glyphInfo,
                          Index* glyphToCharacterMap );

public: // SegmentationInterface

  /**
   * @copydoc SegmentationInterface::GetLineBreakPositions()
   */
  virtual void GetLineBreakPositions( const Character* const text,
                                      Length numberOfCharacters,
                                      LineBreakInfo* breakInfo );

  /**
   * @copydoc SegmentationInterface::GetWordBreakPositions()
   */
  virtual void GetWordBreakPositions( const Character* const text,
                                      Length numberOfCharacters,
                                      WordBreakInfo* breakInfo );

public: // ReorderingInterface

  /**
   * @copydoc ReorderingInterface::CreateBidirectionalInfo()
   */
  virtual Index CreateBidirectionalInfo( const Character* const text,
                                         Length numberOfCharacters );

  /**
   * @copydoc ReorderingInterface::DestroyBidirectionalInfo()
   */
  virtual void DestroyBidirectionalInfo( Index bidiInfoIndex );

  /**
   * @copydoc ReorderingInterface::Reorder()
   */
  virtual void Reorder( Index bidiInfoIndex,
                        Index firstCharacterIndex,
                        Length numberOfCharacters,
                        Index* visualToLogicalMap );

public: // GlyphMetricsInterface

  /**
   * @copydoc GlyphMetricsInterface::GetMetrics()
   */
  virtual void GetMetrics( FontId fontId,
                           GlyphInfo* glyphInfo,
                           Length numberOfGlyphs );

public: // GlyphCreationInterface

  /**
   * @copydoc GlyphCreationInterface::CreateBitmapGlyphs()
   */
  virtual void CreateBitmapGlyphs( GlyphBitmap* bitmapArray,
                                   GlyphInfo* glyphInfo,
                                   Length numberOfGlyphs );

public: // FontValidationInterface

  /**
   * @copydoc FontValidationInterface::ValidateFont()
   */
  virtual FontId ValidateFont( const std::string& fontFamilyName,
                               const std::string& fontStyle );

  /**
   * @copydoc FontValidationInterface::GetValidatedFontFilePath()
   */
  virtual const std::string& GetValidatedFontFilePath( FontId fontId ) const;

  /**
   * @copydoc FontValidationInterface::Validate()
   */
  virtual Length Validate( const Character* const text,
                           Index characterIndex,
                           Length numberOfCharacters,
                           const FontRun* const fontIdsToValidate,
                           Length numberOfFontRuns );

  /**
   * @copydoc FontValidationInterface::GetValidatedFontRuns()
   */
  virtual void GetValidatedFontRuns( FontRun* fontIdsToValidate );

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

#endif // __DALI_TEXT_ABSTRACTION_PLUGIN_PROXY_H__
