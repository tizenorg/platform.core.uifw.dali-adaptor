#ifndef NATIVELOGGING_H_
#define NATIVELOGGING_H_

#include <string>
#include <android/log.h>
#include <boost/lexical_cast.hpp>

#ifdef BUILD_NATIVE_TRACE
#define HAVE_ANDROID_OS 1
#define ANDROID_SMP 1
#define ATRACE_TAG ATRACE_TAG_APP
#include "trace.h"
#else
#include "trace.h"
#endif

#ifndef LOG_ENABLED

#ifdef DEBUG_ENABLED
  #define LOG_ENABLED 1
#else
  #define LOG_ENABLED 0
#endif

#endif // LOG_ENABLED

inline std::string filename_only ( std::string path )
{
  std::string res = path.substr(path.find_last_of('/')+1);
  res = res.substr(0,res.find_last_of('.'));
  return res;
}

#include <pthread.h>

#define TAG     "NativeDali"

#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__))
#define LOGF(...) ((void)__android_log_print(ANDROID_LOG_FATAL, TAG, __VA_ARGS__))

#if (LOG_ENABLED == 1)

#define LOGV(...) ((void)__android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__))
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__))
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__))

#define ACTOR_LOGI(fmt) ((void)__android_log_print(ANDROID_LOG_INFO, TAG, (std::string("%s[%d]: ") + std::string(fmt)).c_str(), GetActor().GetName().c_str(), GetActor().GetId()))
#define ACTOR_LOGIARGS(fmt, ...) ((void)__android_log_print(ANDROID_LOG_INFO, TAG, (std::string("%s[%d]: ") + std::string(fmt)).c_str(), GetActor().GetName().c_str(), GetActor().GetId(), __VA_ARGS__))
#define LAYOUT_LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, LAYOUT_TAG, __VA_ARGS__))

#else // LOG_ENABLED

#define LOGV(...)
#define LOGD(...)
#define LOGI(...)

#define ACTOR_LOGI(fmt)
#define ACTOR_LOGIARGS(fmt, ...)
#define LAYOUT_LOGI(...)


#endif // LOG_ENABLED

#ifdef ACORE_DEBUG_ENABLED
class cPrologHelper
{
public:
  cPrologHelper(const char* str) : mText(str)
  {
    LOGW("%s entering thread 0x%x\n", mText.c_str(), pthread_self());
  }

  ~cPrologHelper()
  {
    LOGW("%s exiting\n", mText.c_str());
  }

protected:
  std::string mText;
};

#define ACORE_PROLOG cPrologHelper test(__PRETTY_FUNCTION__)
#else
#define ACORE_PROLOG
#endif

void dumpJavaStack();
void dumpNativeStack();
#define LOG(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, "minru", __VA_ARGS__))

#endif /* NATIVELOGGING_H_ */
