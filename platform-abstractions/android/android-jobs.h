#pragma once
#include <android/log.h>
#include <stdint.h>
#include <boost/atomic.hpp>
#include <dali/public-api/object/ref-object.h>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>
#include <jni.h>

namespace Jobs
{
  class Job;
  typedef Dali::IntrusivePtr<Job> JobHandle;

  void Create();
  void Shutdown();

  enum eMessagePriority
  {
    MSGPRI_NORMAL,
    MSGPRI_HIGH,
    MSGPRI_NOTIFICATION,
    MSGPRI_MAX
  };


  class Job : public Dali::RefObject
  {
  public:

    enum eJobStatus
    {
      EJOBSTATUS_CREATED,
      EJOBSTATUS_DONE,
      EJOBSTATUS_CANCELLED,

      EJOBSTATUS_COUNT
    };

    uint32_t GetStatus() const
    {
      return mStatus;
    }

    void Queue(uint32_t queueId)
    {
      mQueue = queueId;
    }

    Job() : mJniEnv(0),
            mQueue(0),
            mStatus(EJOBSTATUS_CREATED),
            mCancelRequested(false)
    {
    }

    virtual ~Job()
    {
    }

    void Cancel()
    {
      mCancelRequested = true;
    }

    bool IsProcessed() const
    {
      const bool v = (mStatus == EJOBSTATUS_DONE) || (mStatus == EJOBSTATUS_CANCELLED);
      return v;
    }

    bool IsCancelRequested() const
    {
      return mCancelRequested;
    }

    void DoExec(JNIEnv* env, void* payload);
    void DoCancel();

    virtual void Cleanup()
    {
    }

    void Wait();

  protected:
    virtual void Execute(void* payload) = 0;

    JNIEnv*                 mJniEnv;
    std::string             mJVMCallStack;
  private:

    uint32_t                  mQueue;
    boost::atomic<uint32_t>   mStatus;
    boost::atomic<bool>       mCancelRequested;

    boost::condition_variable mStateChanged;
    boost::mutex              mStateMutex;
  };

  class JavaEnv
  {
  public:
    JavaEnv();
    ~JavaEnv();

    operator JNIEnv*()
    {
      return mEnv;
    }

    JNIEnv* operator->() const
    {
      return mEnv;
    }

  protected:
    bool    mDetached;
    JNIEnv* mEnv;

    void DetachEnv();
  };

  bool    mainloop(JNIEnv* environment);
  bool    mainloop_empty();

  void     add_job(uint32_t queueId, JobHandle job);
  void     add_job(uint32_t queueId, JobHandle job, eMessagePriority priority);
  uint32_t get_main_queue();
  uint32_t get_global_queue();
  uint32_t create_queue(uint32_t queueId, int queueSize);
};


