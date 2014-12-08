#ifndef __DALI_SHADER_EFFECT_WRAPPER_H__
#define __DALI_SHADER_EFFECT_WRAPPER_H__

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

// INTERNAL INCLUDES
#include "handle-wrapper.h"

namespace Dali
{
namespace Internal
{
namespace Emscripten
{

//
// ShaderEffectWrapper
//
class ShaderEffectWrapper : public HandleWrapper
{
public:
  ShaderEffectWrapper();
  explicit ShaderEffectWrapper(Dali::ShaderEffect effect);
  ShaderEffectWrapper( const std::string& vertexShader,
                       const std::string& fragmentShader,
                       int geometryType,    // int for consistency with geometryHints
                       int geometryHints);  // int because emscripten/embind cant represent additive enums


  ~ShaderEffectWrapper();

  void SetEffectImage(const std::string& data);

  Dali::ShaderEffect GetShaderEffect() { return mShaderEffect ; }

  void SetUniform( const std::string& name,
                   Dali::Property::Value& propertyValue );


protected:
  Dali::ShaderEffect mShaderEffect;
};

}; // namespace Emscripten
}; // namespace Internal
}; // namespace Dali

#endif // header
