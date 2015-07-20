#ifndef __DALI_FILE_LOADER_H__
#define __DALI_FILE_LOADER_H__

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
 */

#include <string>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/base-handle.h>

namespace Dali
{
namespace Internal
{
class FileLoader;
}

class DALI_IMPORT_API FileLoader : public BaseHandle
{
public:
  /**
   * @brief Create a file loader. This will automatically load the binary file.
   *
   * @param[in] filename  Filename of the file to load.
   *
   * NOTE: User is reponsible for freeing the buffer
   */
  static char * ReadFile(const std::string& filename);

  /**
   * @brief Create a file loader. This will automatically load the a file.
   *
   * @param[in] filename  Filename of the file to load.
   * @param[in] fileType  How we want to load the file. 0 - Binary, 1 - Text
   *
   * NOTE: User is reponsible for freeing the buffer
   */
  static char * ReadFile(const std::string& filename, int fileType);

  /**
   * @brief Create a file loader. This will automatically load the a file.
   *
   * @param[in] filename  Filename of the file to load.
   * @param[in] fileSize  Size of the loaded file
   * @param[in] fileType  How we want to load the file. 0 - Binary, 1 - Text
   *
   * NOTE: User is reponsible for freeing the buffer
   */
  static char * ReadFile(const std::string& filename, std::streampos& fileSize , int fileType);

  /**
   * @brief Get the file size of a file
   *
   * @param[in] filename  Filename of the file to load.
   */
  static std::streampos GetFileSize(const std::string& filename);
};

} // Dali
#endif // __DALI_FILE_LOADER_H__
