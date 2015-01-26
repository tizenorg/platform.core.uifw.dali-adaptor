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

// CLASS HEADER
#include "font-client.h"

// INTERNAL INCLUDES
#include <internal/font-client-impl.h>

namespace Dali
{

namespace TextAbstraction
{

FontClient::FontClient()
{
}

FontClient::~FontClient()
{
}

FontClient::FontClient( Internal::FontClient* internal )
: BaseHandle( internal )
{
}

FontClient FontClient::Get()
{
  return Internal::FontClient::Get();
}

void FontClient::SetDpi( unsigned int horizontalDpi, unsigned int verticalDpi  )
{
  GetImplementation(*this).SetDpi( horizontalDpi, verticalDpi );
}

FontId FontClient::GetFontId( const std::string& path, PointSize26Dot6 pointSize, FaceIndex faceIndex )
{
  return GetImplementation(*this).GetFontId( path, pointSize, faceIndex );
}

bool FontClient::CreateMetrics( FontId fontId, GlyphMetrics* array, uint32_t size, bool horizontal )
{
  return GetImplementation(*this).CreateMetrics( fontId, array, size, horizontal );
}

} // namespace TextAbstraction

} // namespace Dali
