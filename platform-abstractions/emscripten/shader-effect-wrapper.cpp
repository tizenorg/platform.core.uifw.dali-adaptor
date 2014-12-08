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
