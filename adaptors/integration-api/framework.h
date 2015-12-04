#ifndef __DALI_INTEGRATION_FRAMEWORK_H__
#define __DALI_INTEGRATION_FRAMEWORK_H__

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

// EXTERNAL INCLUDES
#include <string>
#include <dali/public-api/math/rect.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/signals/callback.h>
#include <dali/public-api/events/touch-point.h>
#include <jni.h>

class ANativeWindow;
class AAssetManager;

namespace Dali
{

namespace Integration
{

/**
 * The Framework class is used to interface to the application
 * lifecycle events, and optionally to manage the main-loop.
 *
 * In Tizen, this class can be directly instantiated internally,
 * however in Android, this class needs to be instantiated separately
 * in the JNI implementation.
 */
class Framework
{
public:

  /**
   * Observer class for the framework.
   */
  class Observer
  {
  public:

    /**
     * Invoked when the application is to be initialised.
     */
    virtual void OnInit() {}

    /**
     * Invoked when the application is to be terminated.
     */
    virtual void OnTerminate() {}

    /**
     * Invoked when the application is to be paused.
     */
    virtual void OnPause() {}

    /**
     * Invoked when the application is to be resumed.
     */
    virtual void OnResume() {}

    /**
     * Invoked when surface is created.
     */
    virtual void OnSurfaceCreated() {}

    /**
     * Invoked when surface is destroyed.
     */
    virtual void OnSurfaceDestroyed() {}

    /**
     * Invoked when the application is to be reset.
     */
    virtual void OnReset() {}

    /**
     * Invoked when the language of the device is changed.
     */
    virtual void OnLanguageChanged() {}

    /**
     * Invoked when the app requests a notification
     */
     virtual void OnNotificationRequested() {}
  };

  /**
   * Idler observer callback
   */
  class Idler
  {
  public:
    /**
     * Run any associated idle processing
     */
    virtual void Run() = 0;
  };

public:

  struct TouchEvent
  {
    TouchEvent( ANativeWindow* _window, int id,
                Dali::TouchPoint::State state, float screenX, float screenY,
                unsigned long _timestamp )
    : point(id, state, screenX, screenY),
      window(_window),
      timestamp(_timestamp)
    {
    }

    TouchEvent()
    : point(0, Dali::TouchPoint::Last, 0, 0),
      window(0),
      timestamp(0)
    {
    }

    Dali::TouchPoint  point;
    ANativeWindow*    window;
    unsigned long     timestamp;
  };

public:

  /**
   * Constructor
   * @param[in]  observer  The observer of the Framework.
   * @param[in]  argc      A pointer to the number of arguments.
   * @param[in]  argv      A pointer the the argument list.
   */
  Framework( Observer& observer, int* argc, char ***argv );

  /**
   * Constructor. Creates an unitialized object. Must call Initialize on this
   * object before it can be used.
   */
  Framework();

  /**
   * Destructor
   */
  ~Framework();

  /**
   * Second stage initialization. Used when a framework needs to be separately
   * attached to the application.
   * @param[in]  observer  The observer of the Framework.
   * @param[in]  argc      A pointer to the number of arguments.
   * @param[in]  argv      A pointer the the argument list.
   */
  void Initialize( Framework::Observer& observer, int *argc, char ***argv );

public: // Application client methods

  /**
   * Starts the adaptor when there is no internal mainloop
   */
  void Start();

  /**
   * Runs the main loop of framework. This function doesn't exit
   * until the application quits or is terminated.
   */
  void Run();

  /**
   * Quits the main loop
   */
  void Quit();

  /**
   * Checks whether the main loop of the framework is running.
   * @return true, if the main loop is running, false otherwise.
   */
  bool IsMainLoopRunning();

  /**
   * If the main loop aborts unexpectedly, then the connected callback function is called.
   * @param[in]  callBack  The function to call.
   * @note Only one callback can be registered.  The last callback to be set will be called on abort.
   */
  void AddAbortCallback( CallbackBase* callBack );

public: // Hooks from native application
  void SetWindow( ANativeWindow* nativeWindow );
  void SetJavaVM( JavaVM* jvm );
  void Create();
  void SurfaceCreated();
  void SurfaceDestroyed();
  void ResizeSurface( int width, int height );
  void SetWindowParameters( float xdpi, float ydpi, int width, int height );
  const Vector2& GetDpi();
  Rect<int> GetWidthHeight();
  void SetAssetManager( AAssetManager* assetManager );
  static AAssetManager* GetAssetManager();
  void SetCacheDir( const std::string& path );
  std::string GetCacheDir();
  void RunIdlers();
  void Pause();
  void Resume();
  void Stop();

public: // Platform Adaption ( called from dali adaptor layer )
  struct Timer;
  typedef bool (*TimerCallback)(void*);
  Timer* AddTimer( float seconds, TimerCallback callback, void* context );
  void RemoveTimer( Timer* timer );

  struct InputCallback;
  struct TouchEvent;

  /**
   * Set an idle handler, e.g. for supporting callbacks in the main loop
   */
  void SetIdleHandler ( Framework::Idler* idler );

  /**
   * Get the native window pointer
   * @return The native window pointer
   */
  ANativeWindow* GetWindow();

  /**
   * Get the Java VM this framework was created on.
   * This method should only be used sparingly, and the caller must
   * call AttachCurrentThread() and DetachCurrentThread appropriately.
   */
  JavaVM* GetJavaVM();

private:
  void TriggerResume();

  // Undefined
  Framework(const Framework&);
  Framework& operator=(Framework&);

private:
  Observer*          mObserver;
  bool               mInitialised;
  bool               mRunning;
  int*               mArgc;
  char***            mArgv;

private: // impl members

  struct Impl;
  Impl*              mImpl;

};

} // namespace Integration

} // namespace Dali

#endif // __DALI_INTEGRATION_FRAMEWORK_H__
