#include <cstdlib>
#include <jni.h>
#include <android/native_window_jni.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include "android-interface.h"
#include <boost/thread/mutex.hpp>
#include <sys/select.h>
#include <boost/thread.hpp>
#include <dali/dali.h>
#include <sys/system_properties.h>
#include <boost/signals2.hpp>
#include "android-jobs.h"
#include "nativeLogging.h"
#include <sys/prctl.h>
#include <android_native_app_glue.h>

extern void quit_looper();
extern void wake_looper();
extern bool keepRunningMainLoop;
extern bool isLooperRunning;

extern "C"
{
  JNIEXPORT void JNICALL Java_com_sec_dalidemo_DaliNativeActivity_test_1setDisplayConfig(JNIEnv *env, jobject obj, jfloat xdpi, jfloat ydpi, jfloat dpiScale)
  {
    LOGW("XDPI %04.02f YPDI %04.02f DENS %04.02f\n", xdpi, ydpi, dpiScale);
    android_setDpi(xdpi * dpiScale, ydpi * dpiScale);
  }
}

struct AndroidIdler
{
  void* mContext;
  bool (*mFunction)(void*);
  int	  mData;
  bool  mRemoved;

  AndroidIdler(bool (*callback)(void*), void* context) :	mContext(context),
                              mFunction(callback),
                              mData(0),
                              mRemoved(false)
  {
  }

  AndroidIdler(bool (*callback)(void*), void* context, int data) :	  mContext(context),
                                                                      mFunction(callback),
                                                                      mData(data),
                                                                      mRemoved(false)
  {
  }

  void Call()
  {
    if (!mRemoved)
    {
      (*mFunction)(mContext);
    }
  }
};

struct AndroidInputCallback
{
  void* mContext;
  bool  (*mFunction)(void*, AndroidTouchEvent&);

  AndroidInputCallback(bool (*callback)(void*, AndroidTouchEvent&), void* context) :  mContext(context),
                                                                                      mFunction(callback)
  {
  }

  void Call(AndroidTouchEvent& event)
  {
    (*mFunction)(mContext, event);
  };
};

struct AndroidTimer
{
  void* mContext;
  bool  (*mFunction)(void*);
  bool  mRemoved;
  bool  mActive;
  float mTime;
  float mPeriodTime;

  AndroidTimer(float periodTime,
               bool (*callback)(void*),
               void* context)             :  mContext(context),
                                             mFunction(callback),
                                             mRemoved(false),
                                             mActive(true),
                                             mTime(0.0f),
                                             mPeriodTime(periodTime)
  {
  }

  bool Call(float elapsedTime)
  {
    mTime += elapsedTime;

    if (mActive && mTime >= mPeriodTime)
    {
      mActive = (*mFunction)(mContext);
      if (mActive)
      {
        mTime = 0.0f;
      }
    }

    return mRemoved;
  }
};

class messageTimer {
private:
  struct timespec m_time;
  static const int MS_PER_NS = 1000 * 1000;
  static const int S_PER_NS = 1000 * 1000 * 1000;
  static const int S_PER_MS = 1000;
public:
  messageTimer ( ) : m_time( (struct timespec){0,0} ) { }
  void setExpireTime ( int msecTimeout ) {
    clock_gettime(CLOCK_MONOTONIC,&m_time);
    m_time.tv_nsec += msecTimeout * 1000000;
    if ( m_time.tv_nsec >= S_PER_NS ) {
      m_time.tv_nsec -= S_PER_NS;
      m_time.tv_sec++;
    }
  }
  int timeToNextExpiry ( ) {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC,&now);
    int result = (m_time.tv_sec - now.tv_sec) * S_PER_MS;
    result += (m_time.tv_nsec - now.tv_nsec ) / MS_PER_NS;
    return result;
  }
};

namespace
{
  ANativeWindow* nativeWindow = NULL;
  AAssetManager* assetManager = NULL;
  android_core_ops* currentOps = NULL;
  boost::thread* startupthread = 0;
  bool isSurfaceCreated = false;
  bool isDaliSurfaceCreated = false;
  bool isAppActive = false;
  bool isResumeRequested = false;
  std::string currentLoopName;
  std::string cacheDirName;
  void (*init_func)() = NULL;
  Android::ImageDataHandle (*android_LoadImageFunc)(const std::string& path) = NULL;

  std::list<AndroidIdler*>          platformIdlers;
  std::list<AndroidIdler*>          fdHandlers;
  std::list<AndroidTimer*>          timers;

  // 0 is normal, 1 is priority
  std::list<AndroidInputCallback*>  inputcallbacks;

  boost::mutex mutex;
  boost::atomic<bool>         shutdownRequested;
  boost::condition_variable   mainLoopConditionVariable;
  boost::mutex                mainLoopMutex;
  long current_time = 0;
  Dali::Vector2 androidDpi;
  float androidDpiScale = 1.0f;

  class FrameworkNotificationJob : public Jobs::Job
  {
  public:
    FrameworkNotificationJob(const char* title,
                             int (*function_ptr) (void *), void* context, bool running) : mFunctionPtr(function_ptr),
#ifdef ACORE_DEBUG_ENABLED
                                                                            mTitle(title),
#endif
                                                                            mContext(context),
                                                                            mSetRunning(running)
    {
    }

    void Execute(void *p)
    {
#ifdef ACORE_DEBUG_ENABLED
      LOGI("%s %p %p", mTitle.c_str(), mFunctionPtr, mContext);
#endif
      (*mFunctionPtr)(mContext);
    }

  protected:
    int (*mFunctionPtr) (void *);
    void* mContext;
#ifdef ACORE_DEBUG_ENABLED
    std::string mTitle;
#endif

    bool mSetRunning;
  };

  // Nearest integer millisecond count corresponding to
  // a 60Hz refresh rate is 16mS.  Set to half of this
  // to ensure most up to date information is usually available
  const int frameRateMilliseconds = 8;

  const int priorityMQTimeout = 0;
  const int priorityMQLimit   = 1000;
  const int priorityMQPriority= 1;
  // TODO: determine average/worse case processing time for this number of messages
  // The aim is to fetch as many as possible without interfering with motion events.
  const int normalMQLimit     = 100;
  const int normalMQPriority  = 0;

}

class IdleJob : public Jobs::Job
{
public:
  void Execute(void *p)
  {
    androidcore_run_idlers();
  }
};

long androidcore_time()
{
  struct timespec now;
  int err = clock_gettime(CLOCK_MONOTONIC, &now);
  assert(err == 0);
  return (now.tv_sec * 1000) + (now.tv_nsec /  1000000);
}

AndroidInputCallback* androidcore_register_input_handler(void* context, bool (*callback)(void*, AndroidTouchEvent&))
{
  boost::mutex::scoped_lock(mutex);

  AndroidInputCallback* newCallback = new AndroidInputCallback(callback, context);
  inputcallbacks.push_back(newCallback);

  return newCallback;
}

void androidcore_unregister_input_handler(AndroidInputCallback* callback)
{
  boost::mutex::scoped_lock(mutex);

  std::list<AndroidInputCallback*>::iterator it = std::find(inputcallbacks.begin(), inputcallbacks.end(), callback);
  if (it != inputcallbacks.end())
  {
    AndroidInputCallback* cb = (*it);
    it = inputcallbacks.erase(it);
    delete cb;
  }
}

AndroidTimer* androidcore_add_timer( float precision, bool (*func)(void*), void* context )
{
  boost::mutex::scoped_lock lock(mutex);
  AndroidTimer* newTimer = new AndroidTimer(precision, func, context);
  timers.push_back(newTimer);
  return newTimer;
}

void androidcore_remove_timer(AndroidTimer* timer)
{
  boost::mutex::scoped_lock(mutex);
  std::list<AndroidTimer*>::iterator it = std::find(timers.begin(), timers.end(), timer);
  if (it != timers.end())
  {
    timer->mRemoved = true;
  }
}

void thread_startup()
{
  nice(-16);
  prctl(PR_SET_NAME, "dali_app_thread");

#ifdef ACORE_DEBUG_ENABLED
  LOGI("App thread starting up.\n");
#endif
  (*init_func)();
}

void androidcore_reset()
{
  boost::mutex::scoped_lock(mutex);
  if ( 0 != platformIdlers.size() )
  {
    std::list<AndroidIdler*>::iterator it = platformIdlers.begin();
    while (it != platformIdlers.end())
    {
      AndroidIdler* idler = *it;
      delete idler;
      ++it;
    }
  }
  platformIdlers.clear();
  if ( 0 != fdHandlers.size() )
  {
    std::list<AndroidIdler*>::iterator it = fdHandlers.begin();
    while (it != fdHandlers.end())
    {
      AndroidIdler* idler = *it;
      delete idler;
      ++it;
    }
  }
  fdHandlers.clear();
  if ( 0 != timers.size() )
  {
    std::list<AndroidTimer*>::iterator it = timers.begin();
    while (it != timers.end())
    {
      AndroidTimer* idler = *it;
      delete idler;
      ++it;
    }
  }
  timers.clear();

  // Make sure to clear/reset the static variables to detect misuses
  // and prepare in case the application is restarted
  nativeWindow = 0;
  assetManager = 0;
  android_LoadImageFunc = NULL;
  isSurfaceCreated = false;
  isDaliSurfaceCreated = false;
  isAppActive = false;
  isResumeRequested = false;
  shutdownRequested = false;

  currentOps = 0;
  currentLoopName.clear();
  init_func = 0;

  current_time = 0;
}

void androidcore_init(void (*callback)())
{
  ACORE_PROLOG;

  shutdownRequested = false;
  init_func = callback;
}

void androidcore_SetPrimaryWindow(ANativeWindow* window)
{
  ACORE_PROLOG;

  nativeWindow = window;
}

ANativeWindow* androidcore_GetPrimaryWindow()
{
  return nativeWindow;
}

AAssetManager* androidcore_GetAssetManager()
{
  return assetManager;
}

void androidcore_SetAssetManager(AAssetManager* assetManagerPtr)
{
  assetManager = assetManagerPtr;
}

void androidcore_SetCacheDir(const std::string& path)
{
  cacheDirName = path;
}

std::string androidcore_GetCacheDir()
{
  return cacheDirName;
}

void androidcore_SetLoadImageFunc(Android::ImageDataHandle (*func)(const std::string& path))
{
  android_LoadImageFunc = func;
}

Android::ImageDataHandle androidcore_LoadBitmap(const std::string& path)
{
  Android::ImageDataHandle imgData;
  if (android_LoadImageFunc)
  {
    imgData = android_LoadImageFunc(path);
  }
  return imgData;
}

void androidcore_run_idlers()
{
  boost::mutex::scoped_lock(mutex);
  for(std::list<AndroidIdler*>::iterator it = platformIdlers.begin(); it != platformIdlers.end(); ++it)
  {
    AndroidIdler* idler = (*it);
    idler->Call();
  }

  std::list<AndroidIdler*>::iterator it = platformIdlers.begin();
  while (it != platformIdlers.end())
  {
    if ((*it)->mRemoved)
    {
      AndroidIdler* idler = *it;
      it = platformIdlers.erase(it);
      delete idler;
    }
    else
    {
      ++it;
    }
  }
}

void androidcore_terminate()
{
  ACORE_PROLOG;

  if (currentOps && currentOps->terminate)
  {
    (*currentOps->terminate)(currentOps->data);
  }
}

void run_timers()
{
  long time = androidcore_time();
  // Dali timer expect milliseconds.
  if (current_time == 0)
  {
    current_time = time;
  }
  float elapsedTime = (time - current_time) * 0.001f;
  current_time = time;
  boost::mutex::scoped_lock lock(mutex);
  std::list<AndroidTimer*>::iterator it = timers.begin();
  while(it != timers.end())
  {
    AndroidTimer* timer = *it;
    if ( timer->mRemoved )
    {
      ++it;
    }
    else if (timer->Call(elapsedTime))
    {
      it = timers.erase(it);
    }
    else
    {
      ++it;
    }
  }
}

void androidcore_idle()
{
  ACORE_PROLOG;

  Jobs::JavaEnv environment;

  while(!shutdownRequested)
  {
    run_timers();
   
    // Drain main queue on the main thread until there is no more job to process.
    while (false == Jobs::mainloop(environment))
    {      
    }

    if (Jobs::mainloop_empty())
    {
      androidcore_run_idlers();
      boost::unique_lock<boost::mutex> lock(mainLoopMutex);
      if (!shutdownRequested && Jobs::mainloop_empty())
      {
#ifdef ACORE_DEBUG_ENABLED
        LOGD("App thread going to sleep.\n");
#endif
        // mainLoopConditionVariable.wait(lock);
        wake_looper();
      }
#ifdef ACORE_DEBUG_ENABLED
      LOGD("App thread active.\n");
#endif
    }

#ifdef ACORE_DEBUG_ENABLED
      LOGD("App thread leaving event loop.\n");
#endif
  }
}

void androidcore_main(const char* name, int* argc, char*** argv, android_core_ops* ops)
{
  // Make sure glue isn't stripped.
  app_dummy();

  currentOps = ops;
  currentLoopName = name;
  Jobs::JavaEnv environment;

#ifdef ACORE_DEBUG_ENABLED
  LOGI("Waiting for window.\n");
#endif
  while (NULL == nativeWindow)
  {
    // if there's no window but shutdown has been requested,
    // teminate jobs and return
    if (shutdownRequested)
    {
#ifdef ACORE_DEBUG_ENABLED
  LOGI("Startup aborted.\n");
#endif
     Jobs::Shutdown();
     return;
    }
  }
#ifdef ACORE_DEBUG_ENABLED
  LOGI("Got window, resuming startup on thread %d/%x.\n", pthread_self(), pthread_self());
#endif

  androidcore_create();
  androidcore_idle();
  androidcore_terminate();
  Jobs::Shutdown();
}

void androidcore_create()
{
  ACORE_PROLOG;

  if (currentOps->create)
  {
    (*currentOps->create)(currentOps->data);
  }
}

void androidcore_quit()
{
  ACORE_PROLOG;

  if (!shutdownRequested)
  {
    quit_looper();

    shutdownRequested = true;
    androidcore_wakeup();
    startupthread->join();
    delete startupthread;
    startupthread = NULL;

    androidcore_reset();    // Resets all statics back to initial state
  }

  LOGW("Core left\n");
}

void androidcore_pause()
{
  ACORE_PROLOG;

  // Dont queue a job if we are not active - the job system might have been terminated.
  if (isAppActive)
  {
    Jobs::JobHandle pauseJob = new FrameworkNotificationJob("core_pause", currentOps->pause, currentOps->data, false);
    Jobs::add_job(Jobs::get_main_queue(), pauseJob);
    pauseJob->Wait();
  }

  isAppActive = false;
  isResumeRequested = false;
}

void androidcore_trigger_resume()
{
#ifdef ACORE_DEBUG_ENABLED
  LOGD("App active - creating surface for native window %p\n", nativeWindow);
#endif
  // On 4.4 this is to ensure we dont create the surface if we haven't lost it.
  if (false == isDaliSurfaceCreated)
  {
    Jobs::JobHandle surfaceCreatedJob = new FrameworkNotificationJob("core_surfcreated", currentOps->surfaceCreated, currentOps->data, true);
    Jobs::add_job(Jobs::get_main_queue(), surfaceCreatedJob);
    isDaliSurfaceCreated = true;
  }

  Jobs::JobHandle resumeJob = new FrameworkNotificationJob("core_resume", currentOps->resume, currentOps->data, true);
  Jobs::add_job(Jobs::get_main_queue(), resumeJob);
  resumeJob->Wait();
  isAppActive = true;
}

void androidcore_resume()
{
  ACORE_PROLOG;

  if (!isAppActive)
  {
    // Did we receive a surface created message at this point?
    if (isSurfaceCreated)
    {
      // If yes, trigger the surface creation, and then we are safe to resume.
      androidcore_trigger_resume();
    }
  }

  // if we did not have a surface when this callback was triggered,
  // flag this for later.
  isResumeRequested = !isSurfaceCreated;
}

void androidcore_surfaceCreated(ANativeWindow* window)
{
  ACORE_PROLOG;
  isSurfaceCreated = true;
  assert(window);
  androidcore_SetPrimaryWindow(window);

  // We dont have a dali-surface yet, but resume was already requested.
  // If we are not active at this point, but received the resume message, activate the app.
  if (!isDaliSurfaceCreated && isResumeRequested)
  {
    androidcore_trigger_resume();
    isResumeRequested = false;
  }
}

void androidcore_surfaceDestroyed(ANativeWindow* window)
{
  ACORE_PROLOG;

  if (isDaliSurfaceCreated)
  {
    Jobs::JobHandle surfaceDestroyedJob = new FrameworkNotificationJob("core_surfdestroyed", currentOps->surfaceDestroyed, currentOps->data, false);
    Jobs::add_job(Jobs::get_main_queue(), surfaceDestroyedJob);
    surfaceDestroyedJob->Wait();

    isDaliSurfaceCreated = false;
    androidcore_SetPrimaryWindow(NULL);
  }

  isSurfaceCreated = false;
}

bool androidcore_LoadAsset(const std::string& name, Dali::Vector<uint8_t>& buffer)
{
  LOGI("%s loading %s\n", __PRETTY_FUNCTION__, name.c_str());
  AAsset* asset = AAssetManager_open(assetManager, name.c_str(), AASSET_MODE_BUFFER);
  if (asset)
  {
    const int totalSize = AAsset_getLength(asset);
    if (totalSize > 0)
    {
      int bytesReadTotal = 0;
      buffer.Resize(totalSize);
      while(bytesReadTotal < totalSize)
      {
        int bytesRead = AAsset_read(asset, &buffer[bytesReadTotal], totalSize);
        if (bytesRead > 0)
        {
          bytesReadTotal += bytesRead;
        }
      }
    }

    AAsset_close(asset);

    return true;
  }

  return false;
}

void android_idler_del(AndroidIdler* idler)
{
  boost::mutex::scoped_lock(mutex);

  std::list<AndroidIdler*>::iterator it = std::find(platformIdlers.begin(), platformIdlers.end(), idler);
  if (it != platformIdlers.end())
  {
    idler->mRemoved = true;
  }
  androidcore_wakeup();
}

AndroidIdler* android_idler_add(bool (*callback)(void*), void* context)
{
  boost::mutex::scoped_lock(mutex);

  AndroidIdler* idler = new AndroidIdler(callback, context);
  platformIdlers.push_back(idler);
  androidcore_wakeup();
  wake_looper();
  return idler;
}

#ifdef ACORE_DEBUG_ENABLED
void monitor_idle_performance()
{
  static const int samples = 1000;
  static bool first = true;
  static int numIdleRuns = 0;
  static long prevTime = 0;
  if ( first ) {
    prevTime = androidcore_time();
    first = false;
  }
  long thisTime = androidcore_time();
  numIdleRuns++;
  if (thisTime - prevTime >= (frameRateMilliseconds*samples))
  {
    LOGV("%s had %d idle calls (expected %d)\n", __PRETTY_FUNCTION__, numIdleRuns, samples);
    numIdleRuns = 0;
    prevTime = thisTime;
  }
}
#endif

void androidcore_wakeup()
{
  ACORE_PROLOG;

  boost::unique_lock<boost::mutex> lock(mainLoopMutex);
  mainLoopConditionVariable.notify_one();
}

Dali::Vector2 android_getDpi()
{
  return androidDpi;
}

void android_setDpi(float xdpi, float ydpi)
{
  androidDpi.x = xdpi;
  androidDpi.y = ydpi;
}

void android_setDpiScale(float scale)
{
  androidDpiScale = scale;
}

float android_getDpiScale()
{
  return androidDpiScale;
}

void android_startThread()
{
  if (!startupthread)
  {
    startupthread = new boost::thread(thread_startup);
  }
}

volatile int stopper = 0, counter = 0;

static void ndk_gdb_trap ( void ) {
  char name[PROP_NAME_MAX];
  if ( __system_property_get("dali.ndk_gdb_stop",name) > 0 && name[0] == '1' ) {
    // 1. At the command line, do something like
    // adb shell "setprop dali.ndk_gdb_stop 1"
    // 2. Run the test from eclipse (or wherever)
    // 3. At the black screen, it will be stopped on this loop.
    // 4. Run "ndk-gdb", setup other breakpoints, then
    // 5. "set variable stopper=1" and "continue"
    while ( !stopper ) counter++;
  }
}

void androidcore_OnLoad(void)
{
  // This just exists now to permit early breakpoints when debugging
  // from the command line.
  ndk_gdb_trap();

  android_startThread();
}

void androidcore_queue_touch_event ( int pointerID, int type, float x, float y, long int ntimestamp ) {
  bool validMessage = false;
  AndroidTouchEvent event;

  switch ( type )
  {
    case ACTION_DOWN:
    case ACTION_POINTER_DOWN:
    case ACTION_HOVER_ENTER:
    {
      validMessage = true;
      event = AndroidTouchEvent(nativeWindow, pointerID, Dali::TouchPoint::Down,x,y, ntimestamp);
      break;
    }
    case ACTION_UP:
    case ACTION_POINTER_UP:
    case ACTION_HOVER_EXIT:
    {
      validMessage = true;
      event = AndroidTouchEvent(nativeWindow, pointerID, Dali::TouchPoint::Up,x,y, ntimestamp);
      break;
    }
    case ACTION_MOVE:
    case ACTION_HOVER_MOVE:
    {
      validMessage = true;
      event = AndroidTouchEvent(nativeWindow, pointerID, Dali::TouchPoint::Motion,x,y, ntimestamp);
      break;
    }
    case ACTION_CANCEL:
    {
      validMessage = true;
      event = AndroidTouchEvent(nativeWindow, pointerID, Dali::TouchPoint::Interrupted,x,y, ntimestamp);
      break;
    }
    default:
    {
      static bool reported[ACTION_LAST] = { false };
      if ( !reported[type] ) {
        reported[type] = true;
        LOGE("Unhandled message type=%x",type);
      }
      break;
    }
  }
  if ( validMessage ) {
    boost::mutex::scoped_lock(mutex);
    for(std::list<AndroidInputCallback*>::iterator it = inputcallbacks.begin(); it != inputcallbacks.end(); ++it)
    {
      (*it)->Call(event);
    }
  }


  mainLoopConditionVariable.notify_one();
}

// A utility function to detect whether the current target device is the emulator.
// Some kinds of testing cannot (easily) be performed on the emulator, and this allows
// other code to take this into account.
bool androidcore_is_on_emulator ( ) {
  static bool first = true;
  static bool onEmulator = false;
  if ( first ) {
    first = false;
    char name[PROP_NAME_MAX];
    // This property only exists on the emulator
    int len = __system_property_get("ro.kernel.qemu",name);
    onEmulator = len > 0;
  }
  return onEmulator;
}

// Read the system property which allows fake 2nd touch point.
// This is only called when running on the emulator.
bool androidcore_read_fake_pinch ( float *cx, float *cy ) {
  bool sendSecondPointer = false;
  char name[PROP_NAME_MAX];
  // The parameters are the dx,dy from the current pointer position
  // where a new 'centre' of motion will be established.
  // adb shell "setprop dali.fake-pinch 100,0"    to enable it (places 2nd finger at 200,0 relative to first finger)
  // adb shell "setprop dali.fake-pinch \"\""     to disable it
  int len = __system_property_get("dali.fake-pinch",name);
  if ( len > 0 && sscanf(name,"%f,%f",cx,cy) == 2 ) {
    sendSecondPointer = true;
  } else {
    *cx = *cy = 0;                // clear any offsets
    sendSecondPointer = false;    // flag is undefined, or unparsable
  }
  return sendSecondPointer;
}

// this variable is defined in pan-gesture-detector-base.cpp
namespace Dali
{
  namespace Internal
  {
    namespace Adaptor
    {
      extern float MINIMUM_MOTION_BEFORE_PAN_SQUARED;
    }
  }
}

void android_setPanMinimumMotion( float pixels )
{
  Dali::Internal::Adaptor::MINIMUM_MOTION_BEFORE_PAN_SQUARED = pixels * pixels;
}

// Interface to support calling of demo apps
// =========================================
typedef std::map<std::string,appfn_t> funcMap;
funcMap &getMap ( ) {
  // Fix static order initialisation issues by creating the object
  // on first reference.
  static funcMap *res = new funcMap;
  return *res;
}
void androidcore_registerDemoApp(std::string name, appfn_t entryPoint) {
  funcMap::const_iterator i;
  if ( ( i = getMap().find(name) ) != getMap().end() ) {
    LOGE(" %s - duplicate name %s", __PRETTY_FUNCTION__, name.c_str());
  } else {
    getMap()[name] = entryPoint;
  }
}
static void dummy ( void ) {}
appfn_t androidcore_getDemoApp(std::string name) {
  funcMap::const_iterator i;
  if ( ( i = getMap().find(name) ) == getMap().end() ) {
    LOGE(" %s - missing name %s", __PRETTY_FUNCTION__, name.c_str());
    return &dummy;
  } else {
    return getMap()[name];
  }
}
