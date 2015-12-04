#ifndef ADAPTORS_ANDROID_PROFILER_H
#define ADAPTORS_ANDROID_PROFILER_H

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
 */

#ifdef ENABLE_PROFILE
class AndroidProfiler
{
public:
  AndroidProfiler(const char* name) : mName(name)
  {
    start = androidcore_time();
    __android_log_print(ANDROID_LOG_DEBUG, "dali-test:", "EXC %s execution starts\n", mName.c_str());
  }

  ~AndroidProfiler()
  {
    acTime_t elapsed = abs(androidcore_time() - start);
    __android_log_print(ANDROID_LOG_DEBUG, "dali-test:", "EXC %s executed %lu cycles\n", mName.c_str(), elapsed);
  }

protected:
  std::string mName;
  acTime_t start;
};
#endif

#ifdef ENABLE_PROFILE
#define PROFILE_ME(x) AndroidProfiler prf__COUNTER__p(x)
#else
#define PROFILE_ME(x)
#endif


#endif // ADAPTORS_ANDROID_PROFILER_H
