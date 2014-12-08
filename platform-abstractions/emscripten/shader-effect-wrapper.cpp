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

#include "handle-wrapper.h"

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include "shader-effect-wrapper.h"
#include "emscripten-utils.h"

namespace Dali
{
namespace Internal
{
namespace Emscripten
{

//
// ShaderEffectWrapper
//
ShaderEffectWrapper::ShaderEffectWrapper(Dali::ShaderEffect effect)
  : HandleWrapper(effect), mShaderEffect(effect)
{
}

ShaderEffectWrapper::ShaderEffectWrapper()
{
}

ShaderEffectWrapper::ShaderEffectWrapper( const std::string& vertexShader,
                                          const std::string& fragmentShader,
                                          int geometryType,
                                          int geometryHints)
{
  mShaderEffect = Dali::ShaderEffect::New(vertexShader, fragmentShader,
                                          static_cast<Dali::GeometryType>(geometryType),
                                          static_cast<Dali::ShaderEffect::GeometryHints>(geometryHints));
  mHandle = mShaderEffect;
}


ShaderEffectWrapper::~ShaderEffectWrapper()
{

}

void ShaderEffectWrapper::SetEffectImage(const std::string& data)
{
  DALI_ASSERT_ALWAYS(mShaderEffect);
  if(mShaderEffect)
  {
    mShaderEffect.SetEffectImage( GetImage( data ) );
  }
}


void ShaderEffectWrapper::SetUniform( const std::string& name,
                                      Dali::Property::Value& propertyValue )
{
  switch(propertyValue.GetType())
  {
    case Dali::Property::FLOAT:
    {
      mShaderEffect.SetUniform( name, propertyValue.Get<float>() );
      break;
    };
    case Dali::Property::VECTOR2:
    {
      mShaderEffect.SetUniform( name, propertyValue.Get<Dali::Vector2>() );
      break;
    };
    case Dali::Property::VECTOR3:
    {
      mShaderEffect.SetUniform( name, propertyValue.Get<Dali::Vector3>() );
      break;
    };
    case Dali::Property::VECTOR4:
    {
      mShaderEffect.SetUniform( name, propertyValue.Get<Dali::Vector4>() );
      break;
    };
    case Dali::Property::MATRIX:
    {
      mShaderEffect.SetUniform( name, propertyValue.Get<Dali::Matrix>() );
      break;
    };
    case Dali::Property::MATRIX3:
    {
      mShaderEffect.SetUniform( name, propertyValue.Get<Dali::Matrix3>() );
      break;
    };
  }
}


}; // namespace Emscripten
}; // namespace Internal
}; // namespace Dali
