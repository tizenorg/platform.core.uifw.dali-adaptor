/*
  Copyright (c) 2000-2012 Samsung Electronics Co., Ltd All Rights Reserved

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

#include "shared-file.h"

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/mman.h>

#include <cstring>

namespace Dali
{
namespace Internal
{
namespace Adaptor
{

SharedFile* SharedFile::New(const char* filename, int size, bool isSystem)
{
  SharedFile *sf = NULL;

  sf = new SharedFile;

  bool opened = sf->OpenFile( filename, size, isSystem );
  if( !opened )
  {
    delete sf;
    sf = NULL;
  }
  return sf;
}

SharedFile::SharedFile()
: mFileDescriptor(-1),
  mSize(0),
  mAddress(NULL),
  mFilename()
{
}

SharedFile::~SharedFile()
{
  Close();
}

void SharedFile::Close()
{
  if( mAddress != NULL )
  {
    munmap( mAddress, mSize );
    mAddress = NULL;
  }

  if( mFileDescriptor >= 0 )
  {
    close( mFileDescriptor );
    mFileDescriptor = -1;
  }
}

unsigned char* SharedFile::GetAddress()
{
  return static_cast<unsigned char *>( mAddress );
}

bool SharedFile::OpenFile(const char* filename, int size, bool isSystem)
{
  bool opened = false;
//  mode_t mode;

//  mode = S_IRUSR | S_IWUSR;
//  if( isSystem )
//  {
//    mode |= S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
//  }

//  mFileDescriptor = shm_open( filename, O_RDWR, mode );

//  if( mFileDescriptor >= 0 )
//  {
//    mFilename = filename;

//    mSize = size;
//    mAddress = mmap( NULL, mSize, PROT_READ | PROT_WRITE, MAP_SHARED, mFileDescriptor, 0 );

//    if( mAddress != MAP_FAILED )
//    {
//      opened = true;
//    }
//  }
  return opened;
}

} // Adaptor
} // Internal
} // Dali
