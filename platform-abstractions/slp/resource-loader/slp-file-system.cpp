/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

#include "slp-file-system.h"
#include <fstream>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <algorithm>
#include <stdint.h>
#include <sys/stat.h>

size_t Dali::SlpPlatform::FileSystem::GetFilesize(const std::string& filename)
{
  std::fstream inputStream(filename.c_str(), std::ios::in | std::ios::binary);
  if (inputStream.is_open())
  {
    inputStream.seekg (0, inputStream.end);
    return inputStream.tellg();
  }

  return false;
}

bool Dali::SlpPlatform::FileSystem::Exists(const std::string& filename)
{
  std::fstream inputStream(filename.c_str(), std::ios::in | std::ios::binary);
  return inputStream.is_open();
}

namespace Dali
{
  namespace SlpPlatform
  {
    class StandardFile : public Dali::Integration::File
    {
      FILE*       mFile;
      int64_t     mFileSize;
      std::string mName;

    public:

      StandardFile(const std::string& name) : mName(name)
      {
        mFile = fopen(name.c_str(), "rb");
        if (mFile)
        {
          struct stat sb;
          if (stat(name.c_str(), &sb) != -1)
          {
            mFileSize = sb.st_size;
          }
          else
          {
            mFileSize = -1;
            fclose(mFile);
            mFile = NULL;
          }
        }        

        printf("Opened file :%s size %ld\n", mName.c_str(), mFileSize);
      }

      ~StandardFile()
      {
        if (mFile)
        {
          fclose(mFile);
        }
      }

      const std::string& GetName() const
      {
        return mName;
      }

      bool IsOpen() const
      {
        return mFile != NULL;
      }

      int Seek(int64_t offset, int whence)
      {
        printf("Seeking file :%s to %ld from", mName.c_str(), offset);
        if (whence == SEEK_SET)
        {
          printf(" beginning.\n");
        }
        else if (whence == SEEK_END)
        {
          printf(" end.\n");
        }
        else 
        {
          printf(" current position which is %ld.\n", ftell(mFile));
        }
        return fseek(mFile, offset, whence);
      }

      int Read(void* buffer, int count, int size)
      {
        printf("Reading file :%s %ld bytes.\n", mName.c_str(), int64_t(count * size));
        return fread(buffer, count, size, mFile);
      }

      int64_t Available() const
      {
        return mFileSize - ftell(mFile);
      }

      int64_t Size() const
      {
        return mFileSize;
      }
    };

    class MemoryBackedFile : public Dali::Integration::File
    {
    protected:
      int64_t             mFilePointer;
      int64_t             mDataSize;

      virtual void CopyTo(void* buffer, int bytes_to_copy)
      {
      }

    public:
      MemoryBackedFile() : mFilePointer(0), mDataSize(0)
      {
      }

      bool IsOpen() const
      {
        return mDataSize != 0;
      }

      int Seek(int64_t offset, int whence)
      {
        int64_t newOffset;
        switch(whence)
        {
          case SEEK_SET:
          {
            newOffset = offset;
            break;
          }

          case SEEK_CUR:
          {
            if (offset < 0)
            {
              offset = std::min(-mFilePointer, offset);
            }
            newOffset = mFilePointer + offset;
            break;
          }

          case SEEK_END:
          {
            if (offset < 0)
            {
              offset = std::min(-mDataSize, offset);
            }
            newOffset = mDataSize + offset;
            break;
          }

          default:
          {
            assert(0 && "invalid seek mode.");
          }
        }

        newOffset = std::min(newOffset, mDataSize);
        newOffset = std::max(newOffset, int64_t(0));
        mFilePointer = newOffset;
        return mFilePointer;    
      }

      int Read(void* buffer, int count, int size)
      {
        int bytes_to_copy = count * size;
        const int bytes_left = mDataSize - mFilePointer;
        bytes_to_copy = std::min(bytes_left, bytes_to_copy);
        CopyTo(buffer, bytes_to_copy);
        mFilePointer += bytes_to_copy;
        return bytes_to_copy;
      }

      int64_t Available() const
      {
        return mDataSize - mFilePointer;
      }

      int64_t Size() const
      {
        return mDataSize;
      }      
    };

    class BufferedFile : public MemoryBackedFile
    {
    protected:
      Dali::Vector<char> mData;
      std::string        mName;

      virtual void CopyTo(void* buffer, int bytes_to_copy)
      {
        memcpy(buffer, &mData[mFilePointer], bytes_to_copy);              
      }

    public:
      const std::string& GetName() const
      {
        return mName;
      }

      BufferedFile(const std::string& filename) : mName(filename)
      {
        Integration::FileDataHelper file = new StandardFile(filename);
        if (file)
        {
          const size_t size = file->Size();
          if (size)
          {
            mData.Resize(size);
            const size_t bytesReceived = file->Read(mData.Begin(), 1, size);
            if (bytesReceived == size)
            {
              mDataSize = size;
            }
          }
        } 
      }

    };

    class MemoryFile : public MemoryBackedFile
    {
    protected:
      const uint8_t* mData;
      std::string    mName;

      virtual void CopyTo(void* buffer, int bytes_to_copy)
      {
        memcpy(buffer, &mData[mFilePointer], bytes_to_copy);
      }
    public:

      const std::string& GetName() const
      {
        return mName;
      }

      MemoryFile(const uint8_t* data, const size_t size, const char* name = NULL) : mData(data)
      {        
        mDataSize = size;
        if (name)
        {
          mName = name;
        }
      }

    };
  }
}

Dali::Integration::File* Dali::SlpPlatform::FileSystem::OpenFile(const std::string& name)
{
  if (GetFilesize(name))
  {
    // return new Dali::SlpPlatform::StandardFile(name);
    return new Dali::SlpPlatform::BufferedFile(name);
  }

  return NULL;
}

Dali::Integration::File* Dali::SlpPlatform::FileSystem::MapToMemory(const uint8_t* data, size_t size)
{
  return new Dali::SlpPlatform::MemoryFile(data, size);  
}



