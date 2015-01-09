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
  virtual uint32_t ValidateFont( const std::string& fontFamilyName,
                         const std::string& fontStyle ) = 0;

  /**
   * Validates all characters are supported by the given fonts.
   *
   * @param[in] text The text to be validated.
   * @param[in] charactersFont Vector containing the font Id and the number of consecutive characters with the same font.
   * @param[in] validatedCharactersFont Vector containing the font Id and the number of consecutive characters with the validated font.
   */
  virtual void Validate( const uint32_t* const text,
                       uint32_t characterCount,
                       uint32_t* fontIdsToValidate) = 0;

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

#endif // __DALI_TEXT_ABSTRACTION_GLYPH_METRICS_INTERFACE_H__
