#ifndef _DALI_INTERNAL_PLATFORM_INTERNAL_TEST_MANAGER_H_
#define DALI_INTERNAL_PLATFORM_INTERNAL_TEST_MANAGER_H_
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
#ifdef DEBUG_ENABLED ///< Internal Tests are only built in debug configurations.
// INTERNAL INCLUDES

// EXTERNAL INCLUDES
#include <string>
//#include <dali/common/map-wrapper.h>
#include <dali/public-api/common/map-wrapper.h>

namespace Dali
{
namespace Internal
{
namespace Platform
{

class InternalTest;

/** @brief Map from test name to test instance. */
typedef std::map< std::string, InternalTest* > Suite;

/** @brief Map from suite name to a suite's collection of tests. */
typedef std::map< std::string, Suite > SuiteCollection;

class InternalTestManager
{
public:
  enum TestResult
  {
    TestResultSuccess = 0,
    TestResultFailure = 1
  };
private:

  InternalTestManager();
  ~InternalTestManager();

public:
  void RegisterTest( InternalTest& test, std::string testName , std::string suiteName );
  TestResult RunTest( std::string test, std::string suite );
  TestResult RunSuite( std::string suite );
  TestResult RunAll();

  static InternalTestManager& GetInstance();

private:
  // data:
  SuiteCollection mSuites;
};

inline InternalTestManager::TestResult UpdateTestResult( InternalTestManager::TestResult oldTestResult, InternalTestManager::TestResult newTestResult )
{
  return oldTestResult == InternalTestManager::TestResultFailure ? InternalTestManager::TestResultFailure : newTestResult;
}

} /* namespace Platform */
} /* namespace Internal */
} /* namespace Dali */

#endif // DEBUG_ENABLED
#endif /* DALI_INTERNAL_PLATFORM_INTERNAL_TEST_MANAGER_H_ */
