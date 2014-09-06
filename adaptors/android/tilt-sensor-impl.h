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
#include <xf86drm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <boost/thread.hpp>

#include <dali/integration-api/core.h>
#include <dali/integration-api/platform-abstraction.h>

// INTERNAL INCLUDES
#include "vsync-notifier.h"
#include <base/interfaces/adaptor-internal-services.h>
#include <base/update-render-synchronization.h>
#include <base/environment-options.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace
{

const unsigned int MICROSECONDS_PER_SECOND( 100000u );
const unsigned int TIME_PER_FRAME_IN_MICROSECONDS( 16667u );

#if defined(DEBUG_ENABLED)
Integration::Log::Filter* gLogFilter = Integration::Log::Filter::New(Debug::Concise, false, "LOG_VSYNC_NOTIFIER");
#endif

} // unnamed namespace

VSyncNotifier::VSyncNotifier( UpdateRenderSynchronization& sync,
                              AdaptorInternalServices& adaptorInterfaces,
                              const EnvironmentOptions& environmentOptions )
: mUpdateRenderSync( sync ),
  mCore( adaptorInterfaces.GetCore() ),
  mPlatformAbstraction( adaptorInterfaces.GetPlatformAbstractionInterface() ),
  mVSyncMonitor( adaptorInterfaces.GetVSyncMonitorInterface() ),
  mThread( NULL ),
  mEnvironmentOptions( environmentOptions )
{
}

VSyncNotifier::~VSyncNotifier()
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "%s\n", __func__ );

  Stop();
}

void VSyncNotifier::Start()
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "%s\n", __func__ );

  if ( !mThread )
  {
    mVSyncMonitor->Initialize();

    mThread = new boost::thread( boost::bind( &VSyncNotifier::Run, this ) );
  }
}

void VSyncNotifier::Stop()
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "%s\n", __func__ );

  if( mThread )
  {
    // wait for the thread to finish
    mThread->join();

    delete mThread;
    mThread = NULL;
  }

  mVSyncMonitor->Terminate();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The following is executed inside the notifier thread !!!
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void VSyncNotifier::Run()
{
  // install a function for logging
  mEnvironmentOptions.InstallLogFunction();

  unsigned int frameNumber( 0u );             // frameCount, updated when the thread is paused
  unsigned int currentSequenceNumber( 0u );   // platform specific vsync sequence number (increments with each vsync)
  unsigned int currentSeconds( 0u );          // timestamp at latest vsync
  unsigned int currentMicroseconds( 0u );     // timestamp at latest vsync
  unsigned int seconds( 0u );
  unsigned int microseconds( 0u );

  bool running( true );
  while( running )
  {
    bool validSync( true );

    // Hardware VSyncs available?
    if( mVSyncMonitor->UseHardware() )
    {
      // Yes..wait for hardware VSync
      validSync = mVSyncMonitor->DoSync( currentSequenceNumber, currentSeconds, currentMicroseconds );
    }
    else
    {
      // No..use software timer
      mPlatformAbstraction.GetTimeMicroseconds( seconds, microseconds );

      unsigned int timeDelta( MICROSECONDS_PER_SECOND * (seconds - currentSeconds) );
      if( microseconds < currentMicroseconds)
      {
        timeDelta += (microseconds + MICROSECONDS_PER_SECOND) - currentMicroseconds;
      }
      else
      {
        timeDelta += microseconds - currentMicroseconds;
      }

      if( timeDelta < TIME_PER_FRAME_IN_MICROSECONDS )
      {
          usleep( TIME_PER_FRAME_IN_MICROSECONDS - timeDelta );
      }
      else
      {
        usleep( TIME_PER_FRAME_IN_MICROSECONDS );
      }
    }

    running = mUpdateRenderSync.VSyncNotifierSyncWithUpdateAndRender( validSync, ++frameNumber, currentSeconds, currentMicroseconds );
  }

  // uninstall a function for logging
  mEnvironmentOptions.UnInstallLogFunction();

}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali


// EXTERNAL INCLUDES
#include <deque>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/adaptor-framework/common/timer.h>

// INTERNAL INCLUDES
#include <public-api/adaptor-framework/common/tilt-sensor.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

/**
 * TiltSensor provides pitch & roll values when the device is tilted.
 */
class TiltSensor : public Dali::BaseObject
{
public:

  typedef Dali::TiltSensor::TiltedSignalV2 TiltedSignalV2;

  /**
   * Create a TiltSensor.
   * This should only be called once by the Adaptor class.
   * @return A newly allocated tilt-sensor.
   */
  static Dali::TiltSensor New();

  /**
   * @copydoc Dali::TiltSensor::Get()
   */
  static Dali::TiltSensor Get();

  /**
   * @copydoc Dali::TiltSensor::Enable()
   */
  bool Enable();

  /**
   * @copydoc Dali::TiltSensor::Disable()
   */
  void Disable();

  /**
   * @copydoc Dali::TiltSensor::IsEnabled()
   */
  bool IsEnabled() const;

  /**
   * @copydoc Dali::TiltSensor::GetRoll()
   */
  float GetRoll() const;

  /**
   * @copydoc Dali::TiltSensor::GetPitch()
   */
  float GetPitch() const;

  /**
   * @copydoc Dali::TiltSensor::GetRotation()
   */
  Quaternion GetRotation() const;

  /**
   * @copydoc Dali::TiltSensor::TiltedSignal()
   */
  TiltedSignalV2& TiltedSignal();

  /**
   * @copydoc Dali::TiltSensor::SetUpdateFrequency()
   */
  void SetUpdateFrequency( float frequencyHertz );

  /**
   * @copydoc Dali::TiltSensor::GetUpdateFrequency()
   */
  float GetUpdateFrequency() const;

  /**
   * @copydoc Dali::TiltSensor::SetRotationThreshold()
   */
  void SetRotationThreshold(Radian rotationThreshold);

  /**
   * @copydoc Dali::TiltSensor::GetRotationThreshold()
   */
  Radian GetRotationThreshold() const;

  /**
   * Connects a callback function with the object's signals.
   * @param[in] object The object providing the signal.
   * @param[in] tracker Used to disconnect the signal.
   * @param[in] signalName The signal to connect to.
   * @param[in] functor A newly allocated FunctorDelegate.
   * @return True if the signal was connected.
   * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the caller is responsible for deleting the unused functor.
   */
  static bool DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor );

private:

  /**
   * Private constructor; see also TiltSensor::New()
   */
  TiltSensor();

  /**
   * Destructor
   */
  virtual ~TiltSensor();

  /**
   * Timer callback to update the tilt values
   */
  bool Update();

  // Undefined
  TiltSensor(const TiltSensor&);

  // Undefined
  TiltSensor& operator=(TiltSensor&);

private:

  float mFrequencyHertz;
  Dali::Timer mTimer;
  SlotDelegate< TiltSensor > mTimerSlot;

  int mSensorFrameworkHandle;

  float mRoll;
  float mPitch;
  Quaternion mRotation;

  Radian mRotationThreshold;

  std::deque<float> mRollValues;
  std::deque<float> mPitchValues;

  TiltedSignalV2 mTiltedSignalV2;
};

} // namespace Adaptor

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::Adaptor::TiltSensor& GetImplementation(Dali::TiltSensor& sensor)
{
  DALI_ASSERT_ALWAYS( sensor && "TiltSensor handle is empty" );

  BaseObject& handle = sensor.GetBaseObject();

  return static_cast<Internal::Adaptor::TiltSensor&>(handle);
}

inline const Internal::Adaptor::TiltSensor& GetImplementation(const Dali::TiltSensor& sensor)
{
  DALI_ASSERT_ALWAYS( sensor && "TiltSensor handle is empty" );

  const BaseObject& handle = sensor.GetBaseObject();

  return static_cast<const Internal::Adaptor::TiltSensor&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_ADAPTOR_TILT_SENSOR_H__
