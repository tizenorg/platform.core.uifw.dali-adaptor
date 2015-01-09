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


#include "text-processing/text-shaping-interface.h"
#include "text-processing/text-segmentation-interface.h"
#include "text-processing/text-reordering-interface.h"

#include "control/clipboard-interface.h"
#include "control/imf-manager-interface.h"
#include "control/virtual-keyboard-interface.h"

#include "glyphs/glyph-creation-interface.h"
#include "glyphs/glyph-metrics-interface.h"



namespace Dali
{

namespace TextAbstraction
{


/**
 *  Text abstraction interface provides interfaces to:
 *
 *  - text processing ( segmentation, re-ordering, shaping)
 *  - glyph creation and metrics
 *  - font validation and matching ( does this font support text xyz)
 *  - keyboard, ime and clipboard functionality
 */
class TextAbstractionInterface
{

public:     // Text processing

  /**
   * @return TextReorderingInterface
   */
  virtual TextReorderingInterface* GetTextReorderingInterface() = 0;

  /**
   * @return TextShapingInterface
   */
  virtual TextShapingInterface* GetTextShapingInterface() = 0;

  /**
   * @return TextShapingInterface
   */
  virtual TextSegmentationInterface* GetTextSegmentationInterface() = 0;


public:     // Glyph creation and metrics

  /**
   * @return GlyphCreationInterface
   */
  virtual GlyphCreationInterface* GetGlyphCreationInterface() = 0;

  /**
   * @return GlyphMetricsInterface
   */
  virtual GlyphMetricsInterface* GetGlyphMetricsInterface() = 0;


public:   // control

  /**
   * @return VirtualKeyboardInterface
   */
  virtual VirtualKeyboardInterface* GetVirtualKeyboardInterface() = 0;

  /**
   * @return VirtualKeyboardInterface
   */
  virtual ClipboardInterface* GetClipboardInterface() = 0;

  /**
   * @return VirtualKeyboardInterface
   */
  virtual ImfManagerInterface* GetImfManagerInterface() = 0;


private:

  // Undefined copy constructor.
  TextAbstractionInterface( const TextAbstractionInterface& );

  // Undefined assignment operator.
  TextAbstractionInterface& operator=( const TextAbstractionInterface& );
};

} // namespace Platform

} // namespace Dali

#endif // __DALI_PLATFORM_TEXT_ABSTRACTION_H__
