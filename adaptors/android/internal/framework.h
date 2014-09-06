#ifndef __DALI_INTERNAL_FRAMEWORK_H__
#define __DALI_INTERNAL_FRAMEWORK_H__

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

// EXTERNAL INCLUDES
#include <string>
#include <boost/function.hpp>


namespace Dali
{

namespace Internal
{

namespace Adaptor
{

/**
 * The Framework class is used to register callbacks with the SLP platform so that
 * we know when any of the application lifecycle events occur.  This includes events
 * like when our application is to be initialised, terminated, paused, resumed etc.
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

public:

  /**
   * Constructor
   * @param[in]  observer  The observer of the Framework.
   * @param[in]  argc      A pointer to the number of arguments.
   * @param[in]  argv      A pointer the the argument list.
   */
  Framework(Observer& observer, int* argc, char ***argv, const std::string& name);

  /**
   * Destructor
   */
  ~Framework();

public:

  /**
   * Runs the main loop of framework
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
  void AddAbortCallback(boost::function<void(void)> callBack);

  /**
   * Gets bundle name which was passed in app_reset callback.
   */
  std::string GetBundleName() const;

private:

  // Undefined
  Framework(const Framework&);
  Framework& operator=(Framework&);

private:

  /**
   * Called by the SLP framework when an application lifecycle event occurs.
   * @param[in]  type  The type of event occurred.
   */
  int SlpAppStatusHandler(int type);

  /**
   * Called app_reset callback was called with bundle.
   */
  void SetBundleName(const std::string& name);

  /**
   * Called if the application is aborted.
   */
  void AbortCallback();

private:

  Observer&          mObserver;
  bool               mInitialised;
  bool               mRunning;
  int*               mArgc;
  char***            mArgv;
  std::string        mName;
  std::string        mBundleName;

private: // impl members

  struct Impl;
  Impl*              mImpl;

};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_FRAMEWORK_H__
