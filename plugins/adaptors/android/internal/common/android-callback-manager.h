#ifndef __DALI_ANDROID_CALLBACK_MANAGER_H__
#define __DALI_ANDROID_CALLBACK_MANAGER_H__

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

// INTERNAL INCLUDES
#include "callback-manager.h"

// EXTERNAL INCLUDES
#include <boost/thread.hpp>
#include <list>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

struct CallbackData;

/**
 * Ecore interface to install call backs in the applications main loop.
 */
class AndroidCallbackManager : public CallbackManager
{

public:

     /**
     * constructor
     */
    AndroidCallbackManager();

    /**
     * destructor
     */
    ~AndroidCallbackManager()
    {
    }

    /**
     * @copydoc CallbackManager::AddCallback()
     */
    virtual bool AddCallback(Callback callback, Priority priority);

    /**
     * @copydoc CallbackManager::AddEventCallback()
     */
    virtual bool AddEventCallback(Callback callback, int type, EventControl control);

    /**
     * @copydoc CallbackManager::Start()
     */
    virtual void Start();

    /**
     * @copydoc CallbackManager::Stop()
     */
    virtual void Stop();

    /**
     * Remove all call backs
     * Always called from the main thread
     */
    void RemoveAllCallbacksFromMainThread();

private:

    /**
     * Deletes any expired callbacks in the callback container
     */
    void RefreshContainer();

    /**
     * Removes a single call back from the container
     * Always called from main thread
     * @param callbackData callback data
     */
    void RemoveCallbackFromContainer(CallbackData *callbackData);

    /**
     * Remove a standard call back from ecore
     * Always called from main thread
     * @param callbackData callback data
     */
    void RemoveStandardCallback(CallbackData *callbackData);

    /**
     * Remove an event handler from ecore
     * Always called from main thread
     * @param callbackData callback data
     */
    void RemoveEventCallback(CallbackData *callbackData);



    typedef std::list<CallbackData *>  CallbackList;

    bool                           mRunning;            ///< flag is set to true if when running
    CallbackList                   mCallbackContainer;  ///< container of live callbacks
    boost::mutex                   mMutex;              ///< protect access to shared data
};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_ECORE_CALLBACK_MANAGER_H__
