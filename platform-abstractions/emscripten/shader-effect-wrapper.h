#ifndef __DALI_SHADER_EFFECT_WRAPPER_H__
#define __DALI_SHADER_EFFECT_WRAPPER_H__

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
