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
#include "internal-test.h"
#include "internal-test-manager.h"

namespace Dali
{
namespace Internal
{
namespace Platform
{

Dali::Integration::Log::Filter* InternalTest::gFilter = Dali::Integration::Log::Filter::New( Debug::NoLogging, false, "LOG_INTERNAL_TESTS");

InternalTest::InternalTest( const char * const testCase, const char* const suiteName ) :
  mTestName( testCase ),
  mSuiteName( suiteName )
{
  InternalTestManager::GetInstance().RegisterTest( *this,  mTestName, mSuiteName );
}

} /* namespace Platform */
} /* namespace Internal */
} /* namespace Dali */

#endif // DEBUG_ENABLED
