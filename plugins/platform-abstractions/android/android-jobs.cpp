#include <jni.h>
#include <stdint.h>
#include <sys/types.h>
#include <android/native_window_jni.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>
#include <string>
#include <vector>
#include <list>
#include <queue>
#include <map>
#include <algorithm>
#include "android-interface.h"
#include <sys/select.h>
#include <boost/thread.hpp>
#include <dali/dali.h>
#include <sys/system_properties.h>
#include <boost/signals2.hpp>
#include <boost/functional/hash.hpp>
#include <boost/lockfree/queue.hpp>
#include "android-jobs.h"
#include "android-interface.h"
#include "nativeLogging.h"
#include <jni.h>
#include <sys/prctl.h>

namespace Jobs
{
  const uint32_t kMainQueueId       = 0;
  const uint32_t kGlobalQueueId     = 1;
  const int      kDefaultQueueSize  = 1024;
  const uint32_t kNumWorkers        = 2;
  const int      kCondWaitTimout    = 5;

  void Job::DoExec(void* payload)
  {
#ifdef ACORE_DEBUG_ENABLED
    if (mStatus != EJOBSTATUS_CREATED)
    {
      LOGI("%p job already executed!.\n", this);
    }
#endif

    Execute(payload);
#ifdef ACORE_DEBUG_ENABLED
    LOGI("%p job executed, waking up listeners.\n", this);
#endif
    mStatus = EJOBSTATUS_DONE;
    mStateChanged.notify_all();
  }

  void Job::DoCancel()
  {
#ifdef ACORE_DEBUG_ENABLED
    LOGI("%p job cancelled, waking up listeners.\n", this);
#endif
    mStatus = EJOBSTATUS_CANCELLED;
    mStateChanged.notify_all();
  }

  void Job::Wait()
  {
#ifdef ACORE_DEBUG_ENABLED
      LOGI("waiting for job %p to complete or be cancelled %d.\n", this, unsigned(mStatus));
#endif
      boost::unique_lock<boost::mutex> lock(mStateMutex);
      while(mStatus == EJOBSTATUS_CREATED)
      {
        // put thread to sleep
        if (mStateChanged.timed_wait(lock, boost::posix_time::milliseconds(kCondWaitTimout)))
          break;
      }

#ifdef ACORE_DEBUG_ENABLED
    LOGI("job wait complete.\n");
#endif
  }

  struct JobQueue : public Dali::RefObject
  {
    JobQueue(uint32_t context,
             int32_t queueSize) : mContext(context),
                                  mQueue(queueSize),
                                  mSize(0)
    {
    }

    bool Push(JobHandle job)
    {
#ifdef ACORE_DEBUG_ENABLED
      LOGW("Scheduling on %x job %p size %d\n", mContext, job.Get(), (unsigned)mSize);
#endif
      job->Reference();
      ++mSize;
      return mQueue.push(job.Get());
    }

    bool Pop(JobHandle& job)
    {
      Job* tmpJob;
      if (mQueue.pop(tmpJob))
      {
        --mSize;
        job.Reset(tmpJob);
        tmpJob->Unreference();

        return true;
      }

      return false;
    }

    bool Empty()
    {
      return mQueue.empty();
    }

    // Internal ID for the queue
    // 0 : main threads queue (serial)
    // 1 : global queue (parallel job pool)
    // other: name hash (serial execution)
    uint32_t              mContext;

    // Internal queue structure.
    boost::lockfree::queue<Job*>  mQueue;

    // Flag used
    boost::atomic<bool>   mInUse;
    boost::atomic<size_t> mSize;
  };

  struct Implementation
  {
    typedef Dali::IntrusivePtr<JobQueue>  JobQueueHandle;

    JobQueueHandle                        MainQueue[MSGPRI_MAX];
    JobQueueHandle                        SharedQueue;

    boost::mutex                          SerialQueueMutex;
    std::map<uint32_t, JobQueueHandle>    SerialQueues;

    boost::lockfree::queue<JobQueue*>     SerialWorkflowRequest;
    boost::thread_group                   workerThreads;
    boost::lockfree::queue<Job*>          DeleteQueue;

    boost::condition_variable             TaskConditionVariable;
    boost::condition_variable             TaskConditionVariableSerial;
    boost::mutex                          TaskMutex;
    boost::mutex                          SerialTaskMutex;
    boost::atomic<bool>                   mShutdownRequested;


    Implementation(int queueSize) :   SharedQueue(new JobQueue(get_global_queue(), queueSize)),
                                      SerialWorkflowRequest(queueSize),
                                      DeleteQueue(queueSize),
                                      mShutdownRequested(false)
    {
      for(int i = 0; i < MSGPRI_MAX; ++i)
      {
        MainQueue[i] = new JobQueue(get_main_queue(), queueSize);
      }
    }

    void add_job(uint32_t queueId, JobHandle job, eMessagePriority priority)
    {
      if (queueId == get_main_queue())
      {
        MainQueue[priority]->Push(job);
        androidcore_wakeup();
      }
      else if (queueId == get_global_queue())
      {
        boost::mutex::scoped_lock lock(TaskMutex);
        SharedQueue->Push(job);
        TaskConditionVariable.notify_one();
      }
      else
      {
        boost::mutex::scoped_lock lock(SerialQueueMutex);
        // this is a custom serial queue.
        JobQueueHandle jobQueue = SerialQueues[queueId];
        if (jobQueue)
        {
          SerialWorkflowRequest.push(jobQueue.Get());
          TaskConditionVariableSerial.notify_one();
        }
        else
        {
          LOGE("serial queue %x does not exist.\n", queueId);
        }
      }
    }

    void terminate()
    {
      mShutdownRequested = true;
      TaskConditionVariable.notify_all();
      TaskConditionVariableSerial.notify_all();

      workerThreads.join_all();
      JobQueueHandle jobQueue;
      while(!SerialWorkflowRequest.empty())
      {
        SerialWorkflowRequest.pop(jobQueue);
      }
    }

    uint32_t create_queue(uint32_t queueId, int queueSize)
    {
      boost::mutex::scoped_lock lock(SerialQueueMutex);
      JobQueueHandle jobQueue = new JobQueue(queueId, queueSize);
      SerialQueues[queueId] = jobQueue;
      jobQueue->mInUse = false;

      return queueId;
    }

    void clear_delete_queue()
    {
      JobHandle job;
      while (DeleteQueue.pop(job))
      {
        job->Cleanup();
        job->Unreference();
      }
    }

    uint32_t create_queue(const std::string& name, int queueSize)
    {
      boost::hash<std::string> StringHash;
      uint32_t queueId = StringHash(name);

      return create_queue(queueId, queueSize);
    }


    bool run_mainloop()
    {
      JobHandle job;
      while (MainQueue[MSGPRI_HIGH]->Pop(job))
      {
        job->DoExec(NULL);
      }
      if (MainQueue[MSGPRI_NORMAL]->Pop(job))
      {
        job->DoExec(NULL);
      }
      else if (mainloop_empty())
      {
        while (MainQueue[MSGPRI_NOTIFICATION]->Pop(job))
        {
          job->DoExec(NULL);
        }
      }

      clear_delete_queue();
      return mainloop_empty();
    }

    bool mainloop_empty()
    {
      for(int i = 0; i < MSGPRI_NOTIFICATION; ++i)
      {
        if (false == MainQueue[i]->Empty())
        {
          return false;
        }
      }

      return true;
    }

    static void cbWorkerFunction(Implementation* impl, uint32_t context)
    {
      impl->WorkerFunction(context);
    }

    static void cbWorkerFunctionSerial(Implementation* impl, uint32_t context)
    {
      impl->WorkerFunction_Serial(context);
    }

    void WorkerFunction(uint32_t context)
    {
      nice(10);
      prctl(PR_SET_NAME, "global_job_thread_processor");

      while (!mShutdownRequested)
      {
        {
          boost::unique_lock<boost::mutex> lock(TaskMutex);
          while (!mShutdownRequested && SharedQueue->Empty())
          {
            TaskConditionVariable.wait(lock);
          }
        }

        JobHandle job;
        while (SharedQueue->Pop(job))
        {
          if (false == job->IsCancelRequested())
          {
            job->DoExec((void*)context);
          }
          else
          {
            job->DoCancel();
          }

          job->Reference();
          DeleteQueue.push(job.Get());
        }
      }
    }

    void WorkerFunction_Serial(uint32_t context)
    {
      nice(10);
      prctl(PR_SET_NAME, "serial_job_processor");

      while (!mShutdownRequested)
      {
        {
          boost::unique_lock<boost::mutex> lock(SerialTaskMutex);
          while (!mShutdownRequested && SerialWorkflowRequest.empty())
          {
            TaskConditionVariableSerial.wait(lock);
          }
        }

        JobQueueHandle serialQueue;
        while (SerialWorkflowRequest.pop(serialQueue))
        {
          if (!serialQueue->mInUse)
          {
            if (!serialQueue->mInUse.exchange(true))
            {
              JobHandle job;
              while (serialQueue->Pop(job))
              {
                if (false == job->IsCancelRequested())
                {
                  job->DoExec((void*)context);
                }
                else
                {
                  job->DoCancel();
                }

                job->Reference();
                DeleteQueue.push(job.Get());
              }

              serialQueue->mInUse = false;
            }
          }
        }
      }
    }

    void start_threads(uint32_t numWorkers)
    {
      for(uint32_t i = 0; i < numWorkers ; i++)
      {
        workerThreads.add_thread(new boost::thread(cbWorkerFunction, this, i+1));
      }

      // workerThreads.add_thread(new boost::thread(cbWorkerFunctionSerial, this, 0xBADC0DE));
      // workerThreads.add_thread(new boost::thread(cbWorkerFunctionSerial, this, 0xDEADBEEF));
    }

    static Implementation* mInstance;
  };

  Implementation* Implementation::mInstance = NULL;

  uint32_t get_main_queue()
  {
    return kMainQueueId;
  }

  uint32_t get_global_queue()
  {
    return kGlobalQueueId;
  }

  // Assign job for a specific queue
  void add_job(uint32_t queueId, JobHandle job, eMessagePriority priority)
  {
    if (!Implementation::mInstance)
    {
      dumpNativeStack();
    }
    assert(Implementation::mInstance);
    Implementation::mInstance->add_job(queueId, job, priority);
  }

  // Assign job for a specific queue
  void add_job(uint32_t queueId, JobHandle job)
  {
    assert(Implementation::mInstance);
    Implementation::mInstance->add_job(queueId, job, MSGPRI_NORMAL);
  }

  uint32_t create_queue(uint32_t queueId, int queueSize)
  {
    return Implementation::mInstance->create_queue(queueId, queueSize);
  }

  bool mainloop()
  {
    return Implementation::mInstance->run_mainloop();
  }

  bool mainloop_empty()
  {
    return Implementation::mInstance->mainloop_empty();
  }

  void Create()
  {
    LOGI("Jobs::Create()");
    if (!Implementation::mInstance)
    {
      LOGI("Initializing job system for %u worker threads with a queue size of %d\n", kNumWorkers, kDefaultQueueSize);
      Implementation::mInstance = new Implementation(kDefaultQueueSize);
      Implementation::mInstance->start_threads(kNumWorkers);
    }
  }

  void Shutdown()
  {
    LOGI("Jobs::Shutdown()");
    Implementation::mInstance->terminate();
    delete Implementation::mInstance;
    Implementation::mInstance = NULL;
    LOGI("Job system terminated.\n");
  }
}

