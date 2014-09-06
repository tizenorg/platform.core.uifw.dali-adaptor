/*
Copyright (c) 2000-2012 Samsung Electronics Co., Ltd All Rights Reserved

This file is part of Dali Adaptor

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

// FUNCTION HEADER
#include "android-logging.h"
#include "nativeLogging.h"
#include "android-jobs.h"
#include <android/log.h>

// EXTERNAL INCLUDES

namespace Dali
{

namespace SlpPlatform
{

void LogMessage(Dali::Integration::Log::DebugPriority level, std::string& message)
{
  const char* DALI_TAG = "DALI";
  switch(level)
  {
    case Dali::Integration::Log::DebugInfo:
      __android_log_print(ANDROID_LOG_DEBUG, DALI_TAG, "%s", message.c_str());
      break;
    case Dali::Integration::Log::DebugWarning:
      __android_log_print(ANDROID_LOG_DEBUG, DALI_TAG, "%s", message.c_str());
      break;
    case Dali::Integration::Log::DebugError:
      __android_log_print(ANDROID_LOG_DEBUG, DALI_TAG, "%s", message.c_str());
      break;
    default:
      __android_log_print(ANDROID_LOG_DEBUG, DALI_TAG, "%s", message.c_str());
      break;
  }
}

} // namespace SlpPlatform

} // namespace Dali

#if 0
void dumpJavaStack()
{
  Jobs::JavaEnv env;
  jclass cls = env->FindClass("com/sec/daliviews/utils/Utils");
  if (cls)
  {
    jmethodID method = env->GetStaticMethodID(cls, "dumpStack", "()V");
    env->CallStaticVoidMethod(cls, method);
  }
}
#endif

//#include <utils/CallStack.h> // TODO: MH, header not found
void dumpNativeStack()
{
//  android::CallStack stack;
//  stack.update(1,30);
//  stack.dump("xxx");
}

