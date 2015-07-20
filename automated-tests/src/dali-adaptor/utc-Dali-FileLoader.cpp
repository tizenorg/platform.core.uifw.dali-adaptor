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

#include <dali/dali.h>
#include <dali-test-suite-utils.h>
#include <dali/devel-api/adaptor-framework/file-loader.h>



using namespace Dali;


void utc_dali_file_loader_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_file_loader_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliReadFileNew1(void)
{
  char * buffer = NULL;

  //negative case
  buffer = FileLoader::ReadFile( TEST_RESOURCE_DIR "/not_exist.txt", FileLoader::TEXT );

  DALI_TEST_CHECK( buffer == NULL );

  //positive case
  buffer = FileLoader::ReadFile( TEST_RESOURCE_DIR "/test.txt", FileLoader::TEXT );

  DALI_TEST_CHECK( buffer != NULL );

  if( buffer )
  {
    delete buffer;
  }

  END_TEST;
}

int UtcDaliReadFileNew2(void)
{
  char * buffer = NULL;
  std::streampos fileSize = 0;

  //negative case
  buffer = FileLoader::ReadFile( TEST_RESOURCE_DIR "/not_exist.txt", fileSize, FileLoader::TEXT );

  DALI_TEST_CHECK( buffer == NULL );

  DALI_TEST_CHECK( fileSize == 0 );

  //positive case
  buffer = FileLoader::ReadFile( TEST_RESOURCE_DIR "/test.txt", fileSize, FileLoader::TEXT );

  DALI_TEST_CHECK( buffer != NULL );

  DALI_TEST_CHECK( fileSize != 0 );

  if( buffer )
  {
    delete buffer;
  }

  END_TEST;
}

int UtcDaliReadFileNew3(void)
{
  std::streampos fileSize = 0;

  //negative case
  fileSize = FileLoader::GetFileSize( TEST_RESOURCE_DIR "/not_exist.txt" );

  DALI_TEST_CHECK( fileSize == 0 );

  //positive case
  fileSize = FileLoader::GetFileSize( TEST_RESOURCE_DIR "/test.txt" );

  DALI_TEST_CHECK( fileSize != 0 );

  END_TEST;
}
