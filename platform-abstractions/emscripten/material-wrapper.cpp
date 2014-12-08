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

#include "material-wrapper.h"

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include "emscripten-utils.h"

namespace Dali
{
namespace Internal
{
namespace Emscripten
{

MaterialWrapper::MaterialWrapper(const std::string name)
{
  mMaterial = Dali::Material::New(name);
}

void MaterialWrapper::SetOpacity(const float opacity)
{
  mMaterial.SetOpacity(opacity);
}

void MaterialWrapper::SetShininess(const float shininess)
{
  mMaterial.SetShininess(shininess);
}

void MaterialWrapper::SetAmbientColor(const Dali::Vector4& color)
{
  mMaterial.SetAmbientColor(color);
}

void MaterialWrapper::SetDiffuseColor(const Dali::Vector4& color)
{
  mMaterial.SetDiffuseColor(color);
}

void MaterialWrapper::SetEmissiveColor(const Dali::Vector4& color)
{
  mMaterial.SetEmissiveColor(color);
}

void MaterialWrapper::SetSpecularColor(const Dali::Vector4& color)
{
  mMaterial.SetSpecularColor(color);
}


void MaterialWrapper::SetDiffuseTextureRGBAData(unsigned int width, unsigned int height, const std::string& data)
{
  mMaterial.SetDiffuseTexture( CreateImageRGBA(width, height, data) );
}

void MaterialWrapper::SetDiffuseTextureRGBData(unsigned int width, unsigned int height, const std::string& data)
{
  mMaterial.SetDiffuseTexture( CreateImageRGB(width, height, data) );
}

void MaterialWrapper::SetOpacityTextureRGBAData(unsigned int width, unsigned int height, const std::string& data)
{
  mMaterial.SetOpacityTexture( CreateImageRGBA(width, height, data) );
}

void MaterialWrapper::SetOpacityTextureRGBData(unsigned int width, unsigned int height, const std::string& data)
{
  mMaterial.SetOpacityTexture( CreateImageRGB(width, height, data) );
}


}; // namespace Emscripten
}; // namespace Internal
}; // namespace Dali
