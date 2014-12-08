/*
Copyright (c) 2000-2013 Samsung Electronics Co., Ltd All Rights Reserved

This file is part of Dali

PROPRIETARY/CONFIDENTIAL

This software is the confidential and proprietary information of
SAMSUNG ELECTRONICS ("Confidential Information"). You shall not
disclose such Confidential Information and shall use it only in
accordance with the terms of the license agreement you entered
into with SAMSUNG ELECTRONICS.

SAMSUNG make no representations or warranties about the suitability
of the software, either express or implied, including but not limited
to the implied warranties of merchantability, fitness for a particular
purpose, or non-infringement. SAMSUNG shall not be liable for any
damages suffered by licensee as a result of using, modifying or
distributing this software or its derivatives.
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

//
// MeshDataWrapper
//
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
