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

// CLASS HEADER
#include "file-loader.h"


#include <iostream>
#include <fstream>

namespace Dali
{

char * FileLoader::ReadFile(const std::string& filename, FileLoader::FileType fileType)
{
  std::streampos size;
  char * memblock = NULL;

  std::ifstream * file;
  if( fileType == BINARY )
  {
    file = new std::ifstream (filename.c_str(), std::ios::in|std::ios::binary|std::ios::ate);
  }
  else if( fileType == TEXT )
  {
    file = new std::ifstream (filename.c_str(), std::ios::in|std::ios::ate);
  }
  else
  {
    return 0;
  }

  if( file->is_open() )
  {
    size = file->tellg();
    memblock = new char [size];
    file->seekg (0, std::ios::beg);
    file->read (memblock, size);
    file->close();

    delete file;
  }

  return memblock;
}

char * FileLoader::ReadFile(const std::string& filename, std::streampos& fileSize , FileLoader::FileType fileType)
{
  char * memblock = NULL;

  std::ifstream * file;
  if( fileType == BINARY )
  {
    file = new std::ifstream (filename.c_str(), std::ios::in|std::ios::binary|std::ios::ate);
  }
  else if( fileType == TEXT )
  {
    file = new std::ifstream (filename.c_str(), std::ios::in|std::ios::ate);
  }
  else
  {
    return 0;
  }

  if( file->is_open() )
  {
    fileSize = file->tellg();
    memblock = new char [fileSize];
    file->seekg (0, std::ios::beg);
    file->read (memblock, fileSize);
    file->close();

    delete file;
  }

  return memblock;
}


std::streampos FileLoader::GetFileSize(const std::string& filename)
{
  std::streampos size = 0;

  std::ifstream file (filename.c_str(), std::ios::in|std::ios::binary|std::ios::ate);
  if( file.is_open() )
  {
    size = file.tellg();
    file.close();
  }
  return size;
}


}
