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
#include "internal-test.h"
#ifdef DALI_INTERNAL_TESTS_ENABLED
#include "internal-test-manager.h"

namespace Dali
{
namespace Internal
{
namespace Platform
{
namespace
{

void printSuites(Dali::Integration::Log::Filter * filter, const SuiteCollection& suites )
{
  DALI_LOG_INFO( filter, Debug::Verbose, "<Suites>\n" );
  for( SuiteCollection::const_iterator i = suites.begin(), end = suites.end(); i != end; ++i )
  {
    std::string name = i->first;
    const Suite& suite = i->second;
    DALI_LOG_INFO( filter, Debug::Verbose, "\tSuite: %s. Tests:\n", name.c_str() );
    for( Suite::const_iterator j = suite.begin(), end2 = suite.end(); j != end2; ++j )
    {
      DALI_LOG_INFO( filter, Debug::Verbose, "\t\t[%s]\n", j->first.c_str() );
    }
  }
  DALI_LOG_INFO( filter, Debug::Verbose, "</Suites>\n" );
}

} // unnamed namespace

InternalTestManager::InternalTestManager()
{

}

InternalTestManager::~InternalTestManager()
{
}

InternalTestManager& InternalTestManager::GetInstance()
{
  static InternalTestManager* testManager = 0;
  // Doesn't need to be thread safe as called on default thread of process
  // before main() is run by constructors of static InternalTest objects
  // being initialised.
  if( !testManager )
  {
    testManager = new InternalTestManager();
  }
  return *testManager;
}

void InternalTestManager::RegisterTest( InternalTest& test, std::string testName, std::string suiteName )
{
  DALI_LOG_INFO( InternalTest::gFilter, Debug::General, "InternalTestManager::RegisterTest: %s, %s.\n", testName.c_str(), suiteName.c_str() );
  Suite& suite = mSuites[suiteName];
  DALI_ASSERT_DEBUG( suite.count( testName ) == 0 );
  suite[testName] = &test;
  printSuites( InternalTest::gFilter, mSuites );
}

InternalTestManager::TestResult InternalTestManager::RunTest( std::string testName, std::string suiteName )
{
  DALI_LOG_INFO( InternalTest::gFilter, Debug::General, "InternalTestManager::RunTest: %s, %s.\n", testName.c_str(), suiteName.c_str() );
  printSuites( InternalTest::gFilter, mSuites );
  DALI_ASSERT_DEBUG( mSuites.count( suiteName ) == 1 );
  Suite& suite = mSuites[suiteName];
  DALI_ASSERT_DEBUG( suite.count( testName ) == 1 );
  InternalTest * const test = suite[testName];

  InternalTest::TestResult testResult = test->Run();
  return testResult == InternalTest::TestResultSuccess ? TestResultSuccess : TestResultFailure;
}

InternalTestManager::TestResult InternalTestManager::RunSuite( std::string suiteName )
{
  DALI_LOG_INFO( InternalTest::gFilter, Debug::General, "InternalTestManager::RunSuite(%s), mSuites.size(): %u.\n", suiteName.c_str(), unsigned(mSuites.size()) );
  printSuites( InternalTest::gFilter,  mSuites );
  DALI_ASSERT_DEBUG( mSuites.count( suiteName ) == 1 );
  Suite& suite = mSuites[suiteName];

  TestResult testResult = TestResultSuccess;

  for( Suite::const_iterator i = suite.begin(), end = suite.end(); i != end; ++i )
  {
    testResult = UpdateTestResult( testResult, (i->second->Run() == InternalTest::TestResultSuccess ? TestResultSuccess : TestResultFailure) );
  }
  return testResult;
}

InternalTestManager::TestResult InternalTestManager::RunAll()
{
  DALI_LOG_INFO( InternalTest::gFilter, Debug::General, "InternalTestManager::RunAll().\n" );
  printSuites( InternalTest::gFilter,  mSuites );

  TestResult testResult = TestResultSuccess;

  for( SuiteCollection::const_iterator i = mSuites.begin(), end = mSuites.end(); i != end; ++i )
  {
    testResult = UpdateTestResult( testResult, RunSuite( i->first ) );
  }
  return testResult;
}

} /* namespace Platform */
} /* namespace Internal */
} /* namespace Dali */

#endif // DALI_INTERNAL_TESTS_ENABLED

/**
 * @name ExampleInternalTestUsage
 * A couple of examples showing how to use the embedded internal testing
 * from user code.
 * Similar tests can be placed in any cpp file within the library codebase and
 * should automatically register themselves with the testing framework.
 *
 * @note This code is all compiled-out if DALI_INTERNAL_TESTS_ENABLED is not
 * defined (as it isn't in release builds), apart from add() and mul() which
 * should themselves be eliminated as unused inline functions local to this
 * compilation unit.
 * @{
 */

namespace Dali
{
namespace Internal
{
namespace Platform
{
namespace
{

/** Function hidden in this compilation unit that we would like to test even
 *  though our testing framework can't see it. */
inline int add( int a, int b = 0 )
{
  return a + b;
}

/** Another hidden function it would be nice to test. */
inline int mul( int a, int b = 1 )
{
  return a * b;
}
}

// Clean usage relies on compiler to remove all code inside a branch guarded by a const false and then eliminate an empty class which is unreacheable from outside the compilation unit:
DALI_BEGIN_INTERNAL_TEST( Add, TestTheTesting )

  DALI_INTERNAL_TEST_CHECK( add( 1, 1 ) == 2 );
  DALI_INTERNAL_TEST_CHECK( add( 99, 1 ) == 100 );
  DALI_INTERNAL_TEST_CHECK( add( -1, 1 ) == 0 );

  DALI_INTERNAL_TEST_CHECK( add( -1 ) == -1 );
  DALI_INTERNAL_TEST_CHECK( add( 0 ) == 0 );
  DALI_INTERNAL_TEST_CHECK( add( 1 ) == 1 );

DALI_END_INTERNAL_TEST( Add )

// Belt and braces to be 100% sure no code is generated in release builds:
#if (DALI_INTERNAL_TESTS_ON == true)
DALI_BEGIN_INTERNAL_TEST( Mul, TestTheTesting )

    DALI_INTERNAL_TEST_CHECK( mul( 1, 1 ) == 1 );
    DALI_INTERNAL_TEST_CHECK( mul( 99, 2 ) == 198 );
    DALI_INTERNAL_TEST_CHECK( mul( -100, -100 ) == 10000 );

    DALI_INTERNAL_TEST_CHECK( mul( -1 ) == -1 );
    DALI_INTERNAL_TEST_CHECK( mul( 0 ) == 0 );
    DALI_INTERNAL_TEST_CHECK( mul( 1 ) == 1 );

DALI_END_INTERNAL_TEST( Mul )
#endif // (DALI_INTERNAL_TESTS_ON == true)

// Use the default suite to have the test run in a bucket of unrelated tests:
DALI_BEGIN_INTERNAL_TEST_NO_SUITE( MoreMath )

   const int mulled = mul( mul ( 2, 3 ), mul(5, 7) );
   const int simpleMulled = 2 * 3 * 5 * 7;
   DALI_INTERNAL_TEST_CHECK( mulled == simpleMulled );

   const int added = add( add( 65535, 16777215 ), add( 131071, 262143 ) );
   const int simpleAddded = 65535 + 16777215 + 131071 + 262143;
   DALI_INTERNAL_TEST_CHECK( added == simpleAddded );

DALI_END_INTERNAL_TEST( MoreMath )

/// @}

} /* namespace Platform */
} /* namespace Internal */
} /* namespace Dali */
