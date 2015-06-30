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

#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <dali-test-suite-utils.h>
#include "conditional-wait.h"

using Dali::Internal::Adaptor::ConditionalWait;

namespace // for local variables to avoid name clashes
{
volatile int gGlobalValue = 0;
volatile bool gWorkerThreadWait = true;
enum ThreadState { INIT, RUN, TERMINATE } volatile gWorkerThreadState = INIT;
ConditionalWait* volatile gConditionalWait; // volatile pointer to a ConditionalWait object
}

void* WorkerThreadNotify( void* ptr )
{
  gGlobalValue = -1;
  while( gWorkerThreadWait ) // wait till we can exit
  {
    gWorkerThreadState = RUN;
    usleep( 1 ); // 1 microseconds
  }
  usleep( 1000 ); // 1000 microseconds to give other thread time to do its thing
  gConditionalWait->Notify();
  gWorkerThreadState = TERMINATE;
  return NULL;
}

int UtcConditionalWait1P(void)
{
  tet_infoline("Testing ConditionalWait - scenario 1:  wait - notify from separate thread");

  pthread_t thread1;
  // initialize values
  gConditionalWait = new ConditionalWait();
  gWorkerThreadWait = true;
  DALI_TEST_EQUALS( INIT, gWorkerThreadState, TEST_LOCATION );
  DALI_TEST_EQUALS( 0, gGlobalValue, TEST_LOCATION );

  pthread_create( &thread1, NULL, &WorkerThreadNotify, NULL );
  // wait till the thread is in run state
  while( RUN != gWorkerThreadState )
  {
    usleep( 1 ); // 1 microsecond
  }
  // let worker continue and finish
  gWorkerThreadWait = false;
  gConditionalWait->Wait();

  // wait till the thread is terminated state
  while( TERMINATE != gWorkerThreadState )
  {
    usleep( 1 ); // 1 microsecond
  }

  void* exitValue;
  pthread_join( thread1, &exitValue );

  delete gConditionalWait;
  END_TEST;
}

int UtcConditionalWait2P(void)
{
  tet_infoline("Testing ConditionalWait - scenario 2:  notify without wait");

  ConditionalWait wait;
  DALI_TEST_EQUALS( false, wait.IsWaiting(), TEST_LOCATION );
  wait.Notify();
  DALI_TEST_EQUALS( false, wait.IsWaiting(), TEST_LOCATION );

  END_TEST;
}

unsigned int gNotifyCount = 0;
void* WorkerThreadNotifyN( void* ptr )
{
  while( gNotifyCount )
  {
    gConditionalWait->Notify();
    usleep( 10 ); // 10 microseconds between each notify
  }
  return NULL;
}

int UtcConditionalWait3P(void)
{
  tet_infoline("Testing ConditionalWait - scenario 1:  wait - notify N times");

  // initialize values
  gConditionalWait = new ConditionalWait();
  gNotifyCount = 100;

  pthread_t thread1;
  gWorkerThreadWait = true;
  pthread_create( &thread1, NULL, &WorkerThreadNotifyN, NULL );

  while( gNotifyCount )
  {
    gConditionalWait->Wait();
    --gNotifyCount;
    DALI_TEST_EQUALS( false, gConditionalWait->IsWaiting(), TEST_LOCATION );
    usleep( 10 ); // 10 microseconds between each notify
  }

  void* exitValue;
  pthread_join( thread1, &exitValue );

  delete gConditionalWait;
  END_TEST;
}

int UtcUpdateRenderSynchronizationNonCopyable(void)
{
  // we want to make sure that UpdateRenderSynchronization is not copyable (its copy constructor is not defined)
  // this test will stop compiling if UpdateRenderSynchronization has compiler generated copy constructor
  DALI_COMPILE_TIME_ASSERT( !__has_trivial_copy( ConditionalWait ) );

  DALI_TEST_CHECK( true );
  END_TEST;
}


