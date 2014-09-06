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

// CLASS HEADER
#include "trigger-event.h"

// EXTERNAL INCLUDES
#include <sys/eventfd.h>
#include <boost/bind.hpp>

#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES

#include <android-jobs.h>
#include "nativeLogging.h"

namespace
{
  class TriggerJob : public Jobs::Job
  {
  public:
    TriggerJob(boost::function<void()> functor) : mFunctor(functor)
    {
    };

    void Execute(void* p)
    {
#ifdef ACORE_DEBUG_ENABLED_V2
      LOGI("Executing trigger event %p from context %x", this, p);
#endif
      mFunctor();
    }

  protected:
    boost::function<void()> mFunctor; ///< Function object to call
  };
}

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

TriggerEvent::TriggerEvent( boost::function<void()> functor )
: mFunctor(functor)
{
}

TriggerEvent::~TriggerEvent()
{
}

void TriggerEvent::Trigger()
{
  Jobs::JobHandle newTriggerJob = new TriggerJob(mFunctor);
  Jobs::add_job(Jobs::get_main_queue(), newTriggerJob, Jobs::MSGPRI_NOTIFICATION);
}

void TriggerEvent::Triggered()
{
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
