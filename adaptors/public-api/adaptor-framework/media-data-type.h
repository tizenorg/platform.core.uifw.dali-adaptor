#ifndef __DALI_MEDIA_DATA_TYPE_H__
#define __DALI_MEDIA_DATA_TYPE_H__

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

#include <dali/public-api/common/dali-common.h>

namespace Dali
{


/**
 * @brief Class that provides a container for media data types along with identifiable information.
 * The container can be queried to recover the type and then extract the data accordingly.
 *
 * Enables data to be passed around and be easily identified and extracted in the correct manner.
 * "myfilename.img" which is plain text can be distinguished from "myfilename.img" which is a location to a resource.
 *
 *
 */
class DALI_IMPORT_API MediaDataType
{
public:

  enum MIME_TYPES
  {
  "text/plain",
  "text/uri-list",
  "text/csv",
  "text/css",
  "text/html",
  "application/xhtml+xml",
  "image/png",
  "image/jpg, image/jpeg",
  "image/gif",
  "image/svg+xml",
  "application/xml, text/xml",
  "application/javascript",
  "application/json",
  "application/octet-stream"
  };

  /**
   * @brief Constructs a MediaDataType with no data and no type set.
   */
  MediaDataType();

  /**
   * @brief Constructs a MediaDataType of type plain text containing plain text.
   *
   * @param[in] plainText plainText string to be stored in data container.
   */
  MediaDataType( std::string& plainText );

  /**
   * @brief Check if the data type is plain text.
   *
   * @return true if plain text
   */
  bool IsPlainText();

  /**
   * @brief Set the MediaDataType to type text and fill it with a string of text.
   *
   * @param[in] plainText the string to be contained
   */
  void setPlainText( std::string& plainText );

  /**
   * @brief Retreive the plain text from the MediaDataType container
   *
   * @return the extracted string/text
   */
  std::string& getPlainText();

private:

  std::string mData;
  MIME_TYPES mDataType;

};

} // namespace Dali

#endif // __DALI_MEDIA_DATA_TYPE_H__
