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

// CLASS HEADER
#include "tilt-sensor-impl.h"

// EXTERNAL INCLUDES
#include <cmath>

#include <dali/public-api/object/type-registry.h>

// INTERNAL INCLUDES


#ifdef __arm__
// #define SENSOR_ENABLED
#endif

#ifdef SENSOR_ENABLED
#include <sensor.h>
#endif

namespace // unnamed namespace
{

const int NUMBER_OF_SAMPLES = 10;

const float MAX_ACCELEROMETER_XY_VALUE = 9.8f;

// Type Registration
Dali::BaseHandle Create()
{
  return Dali::Internal::Adaptor::TiltSensor::New();
}

Dali::TypeRegistration typeRegistration( typeid(Dali::TiltSensor), typeid(Dali::BaseHandle), Create );

Dali::SignalConnectorType signalConnector1( typeRegistration, Dali::TiltSensor::SIGNAL_TILTED, Dali::Internal::Adaptor::TiltSensor::DoConnectSignal );

} // unnamed namespace

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

Dali::TiltSensor TiltSensor::New()
{
  Dali::TiltSensor sensor = Dali::TiltSensor(new TiltSensor());

  // Register the object
  GetImplementation(sensor).RegisterObject();

  return sensor;
}

TiltSensor::~TiltSensor()
{
  Disable();
}

bool TiltSensor::Enable()
{
  // Make sure sensor API is responding
  bool success = Update();

  if ( success )
  {
    if ( !mTimer )
    {
      mTimer = Dali::Timer::New( 1000.0f / mFrequencyHertz );
      mTimer.TickSignal().Connect( mTimerSlot, &TiltSensor::Update );
    }

    if ( mTimer &&
         !mTimer.IsRunning() )
    {
      mTimer.Start();
    }
  }

  return success;
}

void TiltSensor::Disable()
{
  if ( mTimer )
  {
    mTimer.Stop();
    mTimer.Reset();
  }
}

bool TiltSensor::IsEnabled() const
{
  return ( mTimer && mTimer.IsRunning() );
}

float TiltSensor::GetRoll() const
{
  return mRoll;
}

float TiltSensor::GetPitch() const
{
  return mPitch;
}

Quaternion TiltSensor::GetRotation() const
{
  return mRotation;
}

TiltSensor::TiltedSignalV2& TiltSensor::TiltedSignal()
{
  return mTiltedSignalV2;
}

void TiltSensor::SetUpdateFrequency( float frequencyHertz )
{
  DALI_ASSERT_ALWAYS( frequencyHertz > 0.0f && "Frequency must have a positive value" );

  if ( fabsf(mFrequencyHertz - frequencyHertz) >= GetRangedEpsilon(mFrequencyHertz, frequencyHertz) )
  {
    mFrequencyHertz = frequencyHertz;

    if ( mTimer )
    {
      mTimer.SetInterval( 1000.0f / mFrequencyHertz );
    }
  }
}

float TiltSensor::GetUpdateFrequency() const
{
  return mFrequencyHertz;
}

void TiltSensor::SetRotationThreshold(Radian rotationThreshold)
{
  mRotationThreshold = rotationThreshold;
}

Radian TiltSensor::GetRotationThreshold() const
{
  return mRotationThreshold;
}

bool TiltSensor::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  bool connected( true );
  TiltSensor* sensor = dynamic_cast<TiltSensor*>( object );

  if( sensor &&
      Dali::TiltSensor::SIGNAL_TILTED == signalName )
  {
    sensor->TiltedSignal().Connect( tracker, functor );
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

TiltSensor::TiltSensor()
: mFrequencyHertz( Dali::TiltSensor::DEFAULT_UPDATE_FREQUENCY ),
  mTimerSlot( this ),
  mSensorFrameworkHandle( -1 ),
  mRoll( 0.0f ),
  mPitch( 0.0f ),
  mRotation( 0.0f, Vector3::YAXIS ),
  mRotationThreshold( 0.0f )
{
  mRollValues.resize( NUMBER_OF_SAMPLES, 0.0f );
  mPitchValues.resize( NUMBER_OF_SAMPLES, 0.0f );
}

bool TiltSensor::Update()
{
  float newRoll = 0.0f;
  float newPitch = 0.0f;
  Quaternion newRotation;
#ifdef SENSOR_ENABLED

  // Read accelerometer data

  mSensorFrameworkHandle = sf_connect( ACCELEROMETER_SENSOR );
  if ( mSensorFrameworkHandle < 0 )
  {
    DALI_LOG_ERROR( "Failed to connect to sensor framework" );
    return false;
  }

  if ( sf_start(mSensorFrameworkHandle, 0) < 0 )
  {
    DALI_LOG_ERROR( "Failed to start sensor" );
    sf_disconnect(mSensorFrameworkHandle);
    return false;
  }

  sensor_data_t* base_data_values = (sensor_data_t*)malloc(sizeof(sensor_data_t));

  int dataErr = sf_get_data(mSensorFrameworkHandle, ACCELEROMETER_BASE_DATA_SET, base_data_values);
  if ( dataErr < 0 )
  {
    DALI_LOG_ERROR( "Failed to retrieve sensor data" );
    free(base_data_values);
    sf_stop(mSensorFrameworkHandle);
    sf_disconnect(mSensorFrameworkHandle);
    return false;
  }

  sf_stop(mSensorFrameworkHandle);
  sf_disconnect(mSensorFrameworkHandle);

  mRollValues.push_back( base_data_values->values[0] );
  mRollValues.pop_front();

  mPitchValues.push_back( base_data_values->values[1] );
  mPitchValues.pop_front();

  free(base_data_values);
  base_data_values = NULL;

  float averageRoll( 0.0f );
  for ( std::deque<float>::const_iterator iter = mRollValues.begin(); mRollValues.end() != iter; ++iter )
  {
    averageRoll += *iter;
  }
  averageRoll /= mRollValues.size();

  float averagePitch( 0.0f );
  for ( std::deque<float>::const_iterator iter = mPitchValues.begin(); mPitchValues.end() != iter; ++iter )
  {
    averagePitch += *iter;
  }
  averagePitch /= mPitchValues.size();

  newRoll  = Clamp( float(averageRoll  / MAX_ACCELEROMETER_XY_VALUE), -1.0f/*min*/, 1.0f/*max*/ );
  newPitch = Clamp( float(averagePitch / MAX_ACCELEROMETER_XY_VALUE), -1.0f/*min*/, 1.0f/*max*/ );

  newRotation = Quaternion( newRoll  * Math::PI * -0.5f, Vector3::YAXIS ) *
              Quaternion( newPitch * Math::PI * -0.5f, Vector3::XAXIS );
#endif // SENSOR_ENABLED

  Radian angle(Quaternion::AngleBetween(newRotation, mRotation));
  // If the change in value is more than the threshold then emit tilted signal.
  if( angle > mRotationThreshold )
  {
    mRoll = newRoll;
    mPitch = newPitch;
    mRotation = newRotation;

    if ( !mTiltedSignalV2.Empty() )
    {
      Dali::TiltSensor handle( this );
      mTiltedSignalV2.Emit( handle );
    }
  }

  return true;
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
