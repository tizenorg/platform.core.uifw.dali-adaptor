#pragma once
#include <android/log.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <dali/dali.h>
#include <boost/function.hpp>

struct ANativeWindow;
struct AAssetManager;
struct AndroidIdler;
struct AndroidFdHandler;
struct AndroidInputCallback;

enum touchActionTypes
{
    ACTION_CANCEL       = 0x00000003,
    ACTION_DOWN         = 0x00000000,
    ACTION_MOVE         = 0x00000002,
    ACTION_UP           = 0x00000001,
    ACTION_OUTSIDE      = 0x00000004,
    ACTION_POINTER_DOWN = 0x00000005,
    ACTION_POINTER_UP   = 0x00000006,
    ACTION_HOVER_MOVE   = 0x00000007,
    ACTION_SCROLL       = 0x00000008,
    ACTION_HOVER_ENTER  = 0x00000009,
    ACTION_HOVER_EXIT   = 0x0000000a,
    ACTION_LAST         = (ACTION_HOVER_EXIT+1),
};

struct android_core_ops
{
    void *data;
        /**< Callback data */
    int (*create) (void *);
               /**< Called before main loop \n
            If this returns non-zero, Appcore does not run the main loop. */
    int (*terminate) (void *);
              /**< Called after main loop */
    int (*pause) (void *);
              /**< Called when every window goes back */
    int (*resume) (void *);
               /**< Called when any window comes on top */
    int (*reset) (void*, void *);
                /**< Called at the first idler
                  and every relaunching */
    int (*surfaceCreated) (void *);
                /**< Called whenever surface created */
    int (*surfaceDestroyed) (void *);
                /**< Called whenever surface destroyed */

    int (*notificationNeeded) (void *);
                /**< Called whenever surface destroyed */

    void *reserved[6];
           /**< Reserved */
};

struct AndroidTouchEvent
{
  AndroidTouchEvent(ANativeWindow* _window,
                    int id,
                    Dali::TouchPoint::State state,
                    float screenX, float screenY,
                    long _timestamp) :              point(id, state, screenX, screenY),
                                                    window(_window),
                                                    timestamp(_timestamp)
  {
  }

  AndroidTouchEvent ( ) : point(0, Dali::TouchPoint::Last, 0, 0),
      window(0),
      timestamp(0)
  {
  }

  Dali::TouchPoint point;
  ANativeWindow*   window;
  long             timestamp;
};

long androidcore_time();

struct AndroidTimer;

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
    long elapsed = abs(androidcore_time() - start);
    __android_log_print(ANDROID_LOG_DEBUG, "dali-test:", "EXC %s executed %ld cycles\n", mName.c_str(), elapsed);
  }

protected:
  std::string mName;
  long start;
};
#endif

#ifdef ENABLE_PROFILE
#define PROFILE_ME(x) AndroidProfiler prf__COUNTER__p(x)
#else
#define PROFILE_ME(x)
#endif

void androidcore_main(const char* name, int* argc, char*** argv, android_core_ops* ops);
void androidcore_OnLoad();
void androidcore_create();
void androidcore_idletest();
void androidcore_run_idlers();
void androidcore_quit();
void androidcore_pause();
void androidcore_resume();
void androidcore_wakeup();
void androidcore_surfaceCreated(ANativeWindow* window);
void androidcore_surfaceDestroyed(ANativeWindow* window);
void androidcore_init(void (*callback)());
ANativeWindow* androidcore_GetPrimaryWindow();
void androidcore_SetPrimaryWindow(ANativeWindow* window);
void androidcore_SetApplication(Dali::Application);
AAssetManager* androidcore_GetAssetManager();
void androidcore_SetAssetManager(AAssetManager*);
void androidcore_SetCacheDir(const std::string& path);
std::string androidcore_GetCacheDir();
bool androidcore_LoadAsset(const std::string& name, Dali::Vector<uint8_t>& buffer);
void androidcore_SetLoadImageFunc(Android::ImageDataHandle (*func)(const std::string& path));
Android::ImageDataHandle androidcore_LoadBitmap(const std::string& path);

AndroidTimer* androidcore_add_timer( float precision, bool (*func)(void*), void* context );
void androidcore_remove_timer(AndroidTimer*);

AndroidInputCallback* androidcore_register_input_handler(void* context, bool (*func)(void*, AndroidTouchEvent&) );
void androidcore_unregister_input_handler(AndroidInputCallback* callback);

void android_idler_del(AndroidIdler*);
AndroidIdler* android_idler_add(bool (*func)(void*), void*);

Dali::Vector2 android_getDpi();
void android_setDpi(float xdpi, float ydpi);
void android_setDpiScale(float scale);

void android_startThread();

typedef void (*cbfnType)(void* fn,int tmo,int lim,int prio);
void registerMessageQueueCallback( cbfnType fn, void *data, int priority );

void androidcore_queue_touch_event ( int pointerID, int type, float x, float y, long int ntimestamp );

bool androidcore_is_on_emulator ( );
bool androidcore_read_fake_pinch ( float *cx, float *cy );

void android_setPanMinimumMotion( float pixels );

// Interface to support calling of demo apps
// =========================================
typedef void (*appfn_t)(void);
void androidcore_registerDemoApp(std::string name, appfn_t entryPoint);
appfn_t androidcore_getDemoApp(std::string name);

class registerTestApp {
public:
  registerTestApp(std::string name, appfn_t entryPoint ) {
    androidcore_registerDemoApp(name,entryPoint);
  }
};
