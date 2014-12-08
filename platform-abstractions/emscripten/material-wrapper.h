#ifndef __DALI_MATERIAL_WRAPPER_H__
#define __DALI_MATERIAL_WRAPPER_H__

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
