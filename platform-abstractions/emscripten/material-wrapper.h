#ifndef __DALI_MATERIAL_WRAPPER_H__
#define __DALI_MATERIAL_WRAPPER_H__

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

// EXTERNAL INCLUDES
#include <dali/public-api/dali-core.h>
#include "emscripten/emscripten.h"
#include "emscripten/bind.h"
#include <string>

// INTERNAL INCLUDES
#include "handle-wrapper.h"

namespace Dali
{
namespace Internal
{
namespace Emscripten
{

//
// MaterialWrapper
//
class MaterialWrapper
{
public:
  explicit MaterialWrapper(const std::string name);

  void SetOpacity(const float opacity);
  void SetShininess(const float shininess);
  void SetAmbientColor(const Dali::Vector4& color);
  void SetDiffuseColor(const Dali::Vector4& color);
  void SetEmissiveColor(const Dali::Vector4& color);
  void SetSpecularColor(const Dali::Vector4& color);

  void SetDiffuseTextureRGBAData(unsigned int width, unsigned int height, const std::string& data);
  void SetDiffuseTextureRGBData(unsigned int width, unsigned int height, const std::string& data);

  void SetOpacityTextureRGBAData(unsigned int width, unsigned int height, const std::string& data);
  void SetOpacityTextureRGBData(unsigned int width, unsigned int height, const std::string& data);

  Dali::Material GetMaterial() const { return mMaterial; };
private:
  Dali::Material mMaterial;
};


}; // namespace Emscripten
}; // namespace Internal
}; // namespace Dali

#endif // header
