#ifndef __DALI_PROPERTY_VALUE_WRAPPER_H__
#define __DALI_PROPERTY_VALUE_WRAPPER_H__

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
#include <string>
#include <dali/public-api/dali-core.h>
#include "emscripten/val.h"

// INTERNAL INCLUDES

namespace Dali
{
namespace Internal
{
namespace Emscripten
{

std::string JavaScriptNameToPropertyName(const std::string& camelCase);
std::string PropertyNameToJavaScriptName(const std::string& hyphenatedName);

void RecursiveSetProperty(Dali::Property::Value& propertyValue, const emscripten::val& fromVal);

emscripten::val JavascriptValue( const Dali::Property::Value& v );

Dali::Property::Value PropertyMapGet( Dali::Property::Map& self, const std::string& key );

/* // */
/* // Creating property Value Objects */
/* //   javascript can't select on type so we have renamed constructors */
/* // */
/* // Emscripten can convert some basic types and ones we've declared as value_array(s) */
/* // (These are given member access offsets/functions and are for simple structs etc) */
/* // */
/* // The composite types need converting. */
/* // */
/* Dali::Property::Value PropertyValueBoolean(bool v); */

/* Dali::Property::Value PropertyValueFloat(float v); */

/* Dali::Property::Value PropertyValueInteger(int v); */

/* Dali::Property::Value PropertyValueVector2( const Dali::Vector2& v ); */

/* Dali::Property::Value PropertyValueVector3( const Dali::Vector3& v ); */

/* Dali::Property::Value PropertyValueVector4( const Dali::Vector4& v ); */

/* Dali::Property::Value PropertyValueMatrix( const Dali::Matrix& v ); */

/* Dali::Property::Value PropertyValueMatrix3( const Dali::Matrix3& v ); */

/* Dali::Property::Value PropertyValueEuler( const Dali::Vector3& v ); */

/* Dali::Property::Value PropertyValueAxisAngle( const Dali::Vector4& v ); */

/* Dali::Property::Value PropertyValueString( const std::string& v ); */

/* Dali::Property::Value PropertyValueContainer( const emscripten::val& v ); */


}; // namespace Emscripten
}; // namespace Internal
}; // namespace Dali

#endif // header
