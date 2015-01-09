#ifndef __DALI_TEXT_ABSTRACTION_FONT_VALIDATION_INTERFACE_H__
#define __DALI_TEXT_ABSTRACTION_FONT_VALIDATION_INTERFACE_H__

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
#include "font-run.h"

// EXTERNAL INCLUDES
#include <string>

namespace Dali
{

namespace TextAbstraction
{

class FontValidationInterface
{

public:

  /**
   * Validates a font.
   *
   * @param[in] fontFamilyName The font's family name.
   * @param[in] fontStyle The font's style.
   *
   * @return A font id.
   */
  virtual FontId ValidateFont( const std::string& fontFamilyName,
                               const std::string& fontStyle ) = 0;

  /**
   * Retrieves the font file path of a validated font.
   *
   * @param[in] fontId The validated font id.
   *
   * @return The font file path.
   */
  virtual const std::string& GetValidatedFontFilePath( FontId fontId ) const = 0;

  /**
   * Validates all characters are supported by the given fonts.
   *
   * It sets a default font to those characters without font set.
   *
   * Call GetValidatedFontRuns() to get the validated font runs.
   *
   * @param[in] text The text to be validated.
   * @param[in] characterIndex Index to the first character.
   * @param[in] numberOfCharacters The number of characters to be validated.
   * @param[in] fontIdsToValidate Buffer containing the font Id and the number of consecutive characters with the same font.
   * @param[in] numberOfFontRuns The number of font runs in the buffer.
   *
   * @retunr The number of validated font runs.
   */
  virtual Length Validate( const Character* const text,
                           Index characterIndex,
                           Length numberOfCharacters,
                           const FontRun* const fontIdsToValidate,
                           Length numberOfFontRuns ) = 0;

  /**
   * Retrieves the validated font runs.
   *
   * @param[out] fontIdsToValidate The validated font runs.
   */
  virtual void GetValidatedFontRuns( FontRun* fontIdsToValidate ) = 0;

protected:

  FontValidationInterface() {};

  virtual ~FontValidationInterface() {};

private:

  // Undefined copy constructor.
  FontValidationInterface( const FontValidationInterface& );

  // Undefined assignment operator.
  FontValidationInterface& operator=( const FontValidationInterface& );
};

} // namespace TextAbstraction

} // namespace Dali

#endif // __DALI_TEXT_ABSTRACTION_FONT_VALIDATION_INTERFACE_H__
