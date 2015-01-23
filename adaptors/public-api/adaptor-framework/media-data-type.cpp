
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
#include <media-data-type.h>

namespace Dali
{

MediaDataType::MediaDataType()
{
}

MediaDataType::MediaDataType( const std::string& plainText )
{
  mData = plainText;
  mDataType = "text/plain";
}

bool MediaDataType::IsPlainText()
{
  return true; //todo
}

bool MediaDataType::IsHTML()
{
  return true; //todo
}

bool MediaDataType::IsImage()
{
  return true; //todo
}

void MediaDataType::setPlainText( std::string& plainText )
{
  // todo pass to implementation and let mDataType be set depending on the underlying system.
  mData = plainText;
  mDataType = "text/plain";
}

void MediaDataType::setHTML( std::string& htmlData )
{
  mData = htmlData;
  mDataType = "text/plain"; //todo
}

void MediaDataType::setImage( std::string& imageData )
{
  mData = imageData;
  mDataType = "text/plain"; //todo
}

const std::string& MediaDataType::getPlainText() const
{
  return mData;
}

} // namespace Dali
