#ifndef _DALI_INTERNAL_PLATFORM_INTERNAL_TEST_H_
#define _DALI_INTERNAL_PLATFORM_INTERNAL_TEST_H_
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

 */

#define DALI_TO_STRING( A ) #A
#define DALI_EXPAND_TO_STRING( A ) DALI_TO_STRING( A )
#define DALI_SPLICE( A, B ) A ## B
#define DALI_SPLICE3( A, B, C ) A ## B ## C

#ifdef DEBUG_ENABLED ///< Internal Tests are only built in debug configurations.

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>

// EXTERNAL INCLUDES
#include <string>

namespace Dali
{
namespace Integration
{
namespace Log
{
class Filter;
}
}
namespace Internal
{
namespace Platform
{

#define DALI_INTERNAL_TEST_DEFAULT_SUITE NoSuite
const char * const DALI_INTERNAL_TEST_DEFAULT_SUITE_NAME = DALI_EXPAND_TO_STRING( DALI_INTERNAL_TEST_DEFAULT_SUITE );
#define DALI_INTERNAL_TESTS_ON true

/***
 * @brief Base class for tests of functions and classes which cannot be accessed
 * from outside their compilation unit.
 *
 * Intended to allow the testing of code which is not visible outside its
 * compilation unit such as small utility functions in unnamed namespaces.
 *
 * Can be useful as a low-friction way to embed a small unit test immediately
 * next to the code that it exercises, so they can be maintained side by side,
 * even if the target code is callable from outside.
 *
 * Can also reduce the effort for writing a test for a small function that might
 * otherwise go untested.
 *
 * Use the DALI_BEGIN_INTERNAL_TEST and DALI_END_INTERNAL_TEST macros instead
 * of deriving from this directly. Most simple, isolated tests should use the
 * variant DALI_BEGIN_INTERNAL_TEST_NO_SUITE. This ensures the test will be
 * executed without writing any further boilerplate.
 *
 * If you use a new test suite string, you must also add a test case or cases to
 * the testing framework to execute the suite. See existing examples here:
 *     automated-tests/src/dali-adaptor-internal/utc-Dali-InternalTests.cpp
 * If you use an existing suite name, that is already executed from the automated
 * tests, your new test case will be run automatically with no further effort.
 * If you use the macro DALI_BEGIN_INTERNAL_TEST_NO_SUITE to define a test
 * without a suite, that will also be executed automatically with no further
 * boilerplate code to write.
 *
 * Usage:
 * Simply place a block of code like the one below outside function or class
 * scope in any namespace of any .cpp file within the library. This example
 * tests a hypothetical user-written <code>mul(int, int)</code> function.
 *
 * <code>
 * DALI_BEGIN_INTERNAL_TEST_NO_SUITE( MyTestName )
 *
 * const int mulled = mul( mul ( 2, 3 ), mul(5, 7) );
 * const int simpleMulled = 2 * 3 * 5 * 7;
 * DALI_INTERNAL_TEST_CHECK( mulled == simpleMulled );
 *
 * DALI_END_INTERNAL_TEST( MyTestName )
 * </code>
 */
class InternalTest
{
public:
  enum TestResult
  {
    TestResultSuccess = 0,
    TestResultFailure = 1
  };

  InternalTest( const char * const testCase, const char* const suiteName = DALI_INTERNAL_TEST_DEFAULT_SUITE_NAME );
  virtual ~InternalTest() {}

  /**
   * @brief Run the test case and return the result.
   * */
  virtual TestResult Run() = 0;

  std::string GetSuite() { return mSuiteName; }
  std::string GetTest() { return mTestName; }

  static Dali::Integration::Log::Filter *gFilter;

private:
  std::string mTestName;
  std::string mSuiteName;
};

} /* namespace Platform */
} /* namespace Internal */
} /* namespace Dali */

/** Use in a test to indicate failure. Failure is sticky. */
#define DALI_INTERNAL_TEST_FAILED testResult = InternalTest::TestResultFailure;

/** Use in a test to indicate success. Success is not sticky and does not
 *  override a previous failure of the same test. */
#define DALI_INTERNAL_TEST_PASSED testResult = testResult == InternalTest::TestResultFailure ? InternalTest::TestResultFailure : InternalTest::TestResultSuccess;

/** Use in a test case body to update success / fail for the test. */
#define DALI_INTERNAL_TEST_CHECK( BOOLEAN ) \
  if ( BOOLEAN )\
  {\
    DALI_INTERNAL_TEST_PASSED;\
  }\
  else\
  {\
    DALI_INTERNAL_TEST_FAILED;\
  }

#define DALI_BEGIN_INTERNAL_TEST( TEST_NAME, SUITE_NAME )\
  namespace {\
  class DALI_SPLICE( InternalTest, TEST_NAME ) : public InternalTest\
  {\
    public:\
    DALI_SPLICE( InternalTest, TEST_NAME )() : InternalTest( DALI_TO_STRING( TEST_NAME ), DALI_TO_STRING( SUITE_NAME ) ) {}\
    TestResult Run()\
    {\
      TestResult testResult = TestResultSuccess; ///< Tests should update this using the supplied macros
#define DALI_END_INTERNAL_TEST( TEST_NAME )\
      DALI_LOG_INFO( gFilter, Debug::General, "Completed test "DALI_EXPAND_TO_STRING( TEST_NAME )". Result = %u.\n", unsigned(testResult) );\
      return testResult;\
    }\
  };\
  DALI_SPLICE( InternalTest, TEST_NAME ) DALI_SPLICE3( InternalTest, TEST_NAME, Instance );\
  }

#else // DEBUG_ENABLED

#define DALI_INTERNAL_TESTS_ON false

// Turn off internal tests in release builds:
#define DALI_BEGIN_INTERNAL_TEST( TEST_NAME, SUITE_NAME )\
  namespace {\
  inline void DALI_SPLICE( CompileOut, TEST_NAME ) ()\
  {\
    if( (DALI_INTERNAL_TESTS_ON) )\
    {

#define DALI_END_INTERNAL_TEST( TEST_NAME )\
    }\
  }\
  }

#define DALI_INTERNAL_TEST_FAILED do { } while(0);
#define DALI_INTERNAL_TEST_PASSED do { } while(0);
#define DALI_INTERNAL_TEST_CHECK( BOOLEAN ) do { bool temp1 = (BOOLEAN); bool temp2 = temp1; temp1 = temp2; } while(0); ///< temp assignments are to suppressed unused variable warnings.

#endif // DEBUG_ENABLED

#define DALI_BEGIN_INTERNAL_TEST_NO_SUITE( TEST_NAME ) DALI_BEGIN_INTERNAL_TEST( TEST_NAME, DALI_INTERNAL_TEST_DEFAULT_SUITE )

#endif // _DALI_INTERNAL_PLATFORM_INTERNAL_TEST_H_
