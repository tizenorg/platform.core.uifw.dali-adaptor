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

#include <iostream>
#include <stdlib.h>
#include <dali-test-suite-utils.h>

#include "internal-test-manager.h"
#include "internal-test.h"

using namespace Dali;
using Dali::Internal::Platform::InternalTestManager;

namespace {

}


void internal_test__startup(void)
{
  // Get it early just so we segv outside of tests if not setup properly:
  Dali::Internal::Platform::InternalTestManager::GetInstance();
}

void internal_test_cleanup(void)
{
}

/** A grab-bag of unrelated tests that are not in any particular test suite. */
int UtcDaliInternalTestsDefaultSuite(void)
{
  DALI_TEST_CHECK( InternalTestManager::TestResultSuccess == InternalTestManager::GetInstance().RunSuite( Dali::Internal::Platform::DALI_INTERNAL_TEST_DEFAULT_SUITE_NAME ) );
  END_TEST;
}

/** Some simple example tests to show the system is working. */
int UtcDaliInternalTestsTestTesting(void)
{
  DALI_TEST_CHECK( InternalTestManager::TestResultSuccess == InternalTestManager::GetInstance().RunSuite( "TestTheTesting" ) );
  END_TEST;
}
