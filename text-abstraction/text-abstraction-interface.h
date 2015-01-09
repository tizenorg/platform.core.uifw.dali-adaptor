#ifndef __DALI_TEXT_ABSTRACTION_INTERFACE_H__
#define __DALI_TEXT_ABSTRACTION_INTERFACE_H__

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

namespace Dali
{

namespace TextAbstraction
{

// forward declarations
class GlyphCreationInterface;
class ReorderingInterface;
class ShapingInterface;
class SegmentationInterface;
class GlyphMetricsInterface;
class FontValidationInterface;
class VirtualKeyboardInterface;
class ClipboardInterface;
class ImfManagerInterface;

/**
 *   provides TextAbstractionInterfaces to:
 *
 *  - text processing ( segmentation, re-ordering, shaping)
 *  - glyph creation and metrics
 *  - font validation and matching ( does this font support text xyz)
 *  - keyboard, ime and clipboard functionality
 *
 *   NOTE: ONLY GetGlyphCreationInterface enabled
 */
class TextAbstractionInterface
{
  static TextAbstractionInterface* Get();

public:     // Text processing

  /**
   * @return TextShapingTextAbstractionInterface
   */
  virtual ShapingInterface& GetShapingInterface() = 0;

  /**
   * @return SegmentationTextAbstractionInterface
   */
  virtual SegmentationInterface& GetSegmentationInterface() = 0;

  /**
   * @return TextReorderingTextAbstractionInterface
   */
  virtual ReorderingInterface& GetReorderingInterface() = 0;


public:     // Glyph creation and metrics

  /**
   * @return GlyphMetricsTextAbstractionInterface
   */
  virtual GlyphMetricsInterface& GetGlyphMetricsInterface() = 0;


  /**
   * @return GlyphCreationTextAbstractionInterface
   */
  virtual GlyphCreationInterface& GetGlyphCreationInterface() = 0;


public:  // Font

  /**
   * @return FontValidationTextAbstractionInterface
   */
  virtual FontValidationInterface& GetFontValidationInterface() = 0;

public:   // control, disabled for now

  /**
   * @return VirtualKeyboardTextAbstractionInterface
   */
  //virtual VirtualKeyboardInterface& GetVirtualKeyboardInterface() = 0;

  /**
   * @return ClipboardTextAbstractionInterface
   */
  //virtual ClipboardInterface& GetClipboardInterface() = 0;

  /**
   * @return ImfManagerTextAbstractionInterface
   */
  //virtual ImfManagerInterface& GetImfManagerInterface() = 0;


protected:

  TextAbstractionInterface() {};

  virtual ~TextAbstractionInterface() {};

private:

  // Undefined copy constructor.
  TextAbstractionInterface( const TextAbstractionInterface& );

  // Undefined assignment operator.
  TextAbstractionInterface& operator=( const TextAbstractionInterface& );
};

} // namespace TextAbstractionInterface

} // namespace Dali

#endif // __DALI_TEXT_ABSTRACTION_INTERFACE_H__
