#ifndef __DALI_PLATFORM_TEXT_ABSTRACTION_H__
#define __DALI_PLATFORM_TEXT_ABSTRACTION_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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


#include "text-processing/shaping-interface.h"
#include "text-processing/segmentation-interface.h"
#include "text-processing/reordering-interface.h"
#include "control/clipboard-interface.h"
#include "control/imf-manager-interface.h"
#include "control/virtual-keyboard-interface.h"
#include "glyph/glyph-creation-interface.h"
#include "glyph/glyph-metrics-interface.h"
#include "font/font-validation-interface.h"


namespace Dali
{

namespace TextAbstraction
{


/**
 *  Proxy is used to load the plugin, only if an API call is used.
 *  It will only be loaded when the developer calls an API on an interface
 *  like  ShowClipboard()
 *
 */
class TextPluginProxy :
    public ReorderingInterface,
    public ShapingInterface,
    public SegmentationInterface,
    public GlyphCreationInterface,
    public GlyphMetricsInterface,
    public FontValidationInterface,
    public VirtualKeyboardInterface,
    public ClipboardInterface,
    public ImfManagerInterface
{

public:     // TextReorderingInterface

    virtual unsigned int Shape( const uint32_t* const text,
                          uint32_t startCharacter,
                          uint32_t characterCount,
                          uint32_t fontId,
                          uint32_t scriptId
                          ) = 0 ;

    /**
     * Gets the shaped text data
     * @param[out] glyphInfo Vector with indices to the glyph within the font, glyph's metrics and advance.
     * @param[out] glyphToCharacterMap The glyph to character conversion map.
     */
    virtual void GetGlyphs( GlyphInfo* glyphInfo, uint32_t*  glyphToCharacterMap );


public:   //  TextSegmentationInterface& GetSegmentationInterface();



private:

    TextPlugin* mPlugin;

  // Undefined copy constructor.
    TextPluginProxy( const TextPluginProxy& );

  // Undefined assignment operator.
    TextPluginProxy& operator=( const TextPluginProxy& );
};

} // namespace TextAbstraction

} // namespace Dali

#endif // __DALI_PLATFORM_TEXT_ABSTRACTION_H__
