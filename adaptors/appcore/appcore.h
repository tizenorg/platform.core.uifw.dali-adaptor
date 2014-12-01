#ifndef __DALI_INTERNAL_ADAPTOR_TIZEN_COMMON_APPCORE_H__
#define __DALI_INTERNAL_ADAPTOR_TIZEN_COMMON_APPCORE_H__

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

#include <aul.h>
#include <Ecore.h>
#include <appcore-efl.h>

#include "appcore-ops.h"
#include "appcore-rotation.h"
#include "vconf-events.h"

namespace Dali
{
namespace Internal
{
namespace Adaptor
{

typedef int (RotateCallback) (enum appcore_rm, void *);

/**
 * Class to emulate Appcore package, and remove dependencies on Elementary package
 */
class Appcore : public AppcoreRotation::Observer, public VconfEvents::Observer
{
private:
  class State
  {
  public:
    enum States
    {
      INIT,
      RUNNING,
      PAUSED
    };
    /**
     * Constructor
     * @param[in] appcore A ref to appcore (so state change can invoke methods)
     */
    State(Appcore& appcore);

    /**
     * Equality operator
     * @param[in] state The state to test equality
     * @return true if the current state matches
     */
    bool operator==(States state);

    /**
     * Assigment operator
     * Sets the current state to the given value and performs all transition actions
     * @param[in] state The new state
     */
    void operator=(States state);

  private:
    States mState;      ///< The current state
    Appcore& mAppcore;  ///< A ref to appcore
  };

public:
  /**
   * Constructor for appcore package replacement
   */
  Appcore(appcore_ops* ops);

  /**
   * Constructor for application package replacement
   */
  Appcore(app_event_callback_s* ops, void* userData);

  /**
   * Destructor
   */
  ~Appcore();

  /**
   * Execute the main loop
   * @param[in] name The application name
   * @param[in] argc The number of command line arguments
   * @param[in] argv The argument vector
   */
  void Run(const char* name, int argc, char** argv);

  /**
   * Exit the main loop
   */
  void Exit();

  /**
   * Add a callback to be triggered whenever the event occurs
   * @param[in] event The event to test
   * @param[in] cb The callback
   * @param[in] data User data to pass to the callback on invocation
   */
  void AddEventCallback(enum appcore_event event, int (*cb) (void *), void *data);

  /**
   * Add a rotate callback to be called whenever the device rotates to a new orientation
   * Only one callback is supported.
   * @param[in] rotateCallback The callback
   * @param[in] data User data to pass to the callback on invocation
   */
  void AddRotationCallback(RotateCallback* rotateCallback, void* data);

  /**
   * Remove the rotation callback.
   */
  void RemoveRotationCallback();

  /**
   * Set whether appcore performs resource reclamation
   * @param[in] enable True to perform resource reclamation on app pause
   */
  void SetSystemResourceReclamation(bool enable);

  /**
   * Get the application name. The returned string is allocated and should be
   * freed after use.
   * @return A pointer to an allocated string containing the name of the application
   */
  static const char* GetAppName();

private:

  /**
   * Initialize ecore
   *
   */
  void Initialize(const char* name, int argc, char** argv);

  /**
   * Shutdown ecore
   */
  void Shutdown();

  static Eina_Bool ExitCallback(void* data, int eventType, void* event);

  /**
   * Callback for AUL events for application start, resume, termination
   */
  static int AulHandlerCallback(aul_type type, bundle* bundleData, void* data);

  /**
   * AUL event handler
   * @param[in] type Event - one of start, resume, terminate
   * @param[in] bundle Application bundle info
   */
  int AulHandler(aul_type type, bundle* bundle);

  /**
   * Get directory name from package name of this process
   * @param[inout] dirname Buffer to hold directory name
   * @param[in] bufferLength Length of dirname buffer
   */
  int GetDirName(char *dirname, int bufferLength);

  /**
   * @copydoc Dali::Internal::Adaptor::AppcoreRotation::Observer::RotationChanged()
   */
  virtual void RotationChanged(enum appcore_rm rotation);

  /**
   * @copydoc Dali::Internal::Adaptor::VconfEvents::Observer::OnLowMemory()
   */
  virtual void OnLowMemory();

  /**
   * Called when resource reclaim timer is triggered to release memory.
   */
  static Eina_Bool MemoryFlushCallback(void* data);

  /**
   * Add window callbacks for show, hide, visibility, WM Rotation
   */
  void AddWindowCallbacks();

  /**
   * Remove window callbacks
   */
  void RemoveWindowCallbacks();

  /**
   * Called when window shown.
   * @param[in] data Client data (this object ptr)
   * @param[in] type Type
   * @param[in] event Window event
   * @return RENEW or PASS_ON
   */
  static Eina_Bool ShowCallback(void *data, int type, void *event);

  /**
   * Called when window hidden.
   * @param[in] data Client data (this object ptr)
   * @param[in] type Type
   * @param[in] event Window event
   * @return RENEW or PASS_ON
   */
  static Eina_Bool HideCallback(void *data, int type, void *event);

  /**
   * Called when window visibility changed.
   * @param[in] data Client data (this object ptr)
   * @param[in] type Type
   * @param[in] event Window event
   * @return RENEW or PASS_ON
   */
  static Eina_Bool VisibilityCallback(void *data, int type, void *event);

  /**
   * Called when window shown.
   * @param[in] type Type
   * @param[in] event Window event
   * @return RENEW or PASS_ON
   */
  Eina_Bool OnShow(int type, void *event);

  /**
   * Called when window hidden.
   * @param[in] type Type
   * @param[in] event Window event
   * @return RENEW or PASS_ON
   */
  Eina_Bool OnHide(int type, void *event);

  /**
   * Called when window visibility changed.
   * @param[in] type Type
   * @param[in] event Window event
   * @return RENEW or PASS_ON
   */
  Eina_Bool OnVisibility(int type, void *event);

private:
  Operations       mOperations;      ///< Client provided operation callbacks
  bool             mInitialised;     ///< True after appcore has been initialized
  RotateCallback*  mRotateCallback;  ///< Client provided callback to handle rotation
  void*            mRotateData;      ///< Client data to pass to the rotation callback
  AppcoreRotation* mRotationSensor;  ///< Rotation sensor client
  State            mState;           ///< Active state
  VconfEvents*     mVconfEventHandler;

  Ecore_X_Atom         mAtomParent;  ///< Parent border window

  Ecore_Event_Handler *mShowHandler;
  Ecore_Event_Handler *mHideHandler;
  Ecore_Event_Handler *mVisibilityHandler;
  Ecore_Event_Handler *mExitHandler;

  bool                 mResourceReclamation;
  Ecore_Timer*         mReclaimTimer;   ///< Ecore Timer for memory flushing
};

} // Adaptor
} // Internal
} // Dali

#endif //__DALI_INTERNAL_ADAPTOR_TIZEN_COMMON_APPCORE_H__
