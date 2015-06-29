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
#include "update-render-synchronization.h"

using Dali::Internal::Adaptor::UpdateRenderSynchronization;

namespace // for local variables to avoid name clashes
{
volatile int gGlobalValue = 0;
volatile bool gWorkerThreadWait = true;
volatile enum ThreadState { INIT, RUN, LOCKING, TERMINATE } gWorkerThreadState = INIT;
}
void* WorkerThread1( void* ptr )
{
  gWorkerThreadState = RUN;
  {
    gWorkerThreadState = LOCKING;
    gGlobalValue = -1;
    while( gWorkerThreadWait ) // wait till we can exit
    {
      usleep( 1 ); // 1 microsecond
    }
  }
  gWorkerThreadState = TERMINATE;
  return NULL;
}

int UtcUpdateRenderSynchronization(void)
{
  tet_infoline("Testing UpdateRenderSynchronization - scenario 1: update, render, pause");

  pthread_t thread1;
  // initialize values
  gGlobalValue = 0;
  gWorkerThreadWait = true;
  DALI_TEST_EQUALS( INIT, gWorkerThreadState, TEST_LOCATION );
  DALI_TEST_EQUALS( 0, gGlobalValue, TEST_LOCATION );

  pthread_create( &thread1, NULL, &WorkerThread1, NULL );
  // wait till the thread is in run state
  while( RUN != gWorkerThreadState )
  {
    usleep( 1 ); // 1 microsecond
  }
  // now the thread is running and mutex is still locked by this thread so value is not changed
  DALI_TEST_EQUALS( 0, gGlobalValue, TEST_LOCATION );

  // now child thread is allowed to change the value
  // wait till the thread is in locking state
  while( LOCKING != gWorkerThreadState )
  {
    usleep( 1 ); // 1 microsecond
  }
  // value is changed
  DALI_TEST_EQUALS( -1, gGlobalValue, TEST_LOCATION );
  // let worker finish
  gWorkerThreadWait = false;
  // wait till the thread is terminated state
  while( TERMINATE != gWorkerThreadState )
  {
    usleep( 1 ); // 1 microsecond
  }

  void* exitValue;
  pthread_join( thread1, &exitValue );

  END_TEST;
}

int UtcUpdateRenderSynchronizationNonCopyable(void)
{
  // we want to make sure that UpdateRenderSynchronization is not copyable (its copy constructor is not defined)
  // this test will stop compiling if UpdateRenderSynchronization has compiler generated copy constructor
  DALI_COMPILE_TIME_ASSERT( !__has_trivial_copy( UpdateRenderSynchronization ) );

  DALI_TEST_CHECK( true );
  END_TEST;
}


