/*
Copyright (c) 2000-2013 Samsung Electronics Co., Ltd All Rights Reserved

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
#include <internal/common/sound-player-impl.h>

// EXTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>

// INTERNAL INCLUDES
//#include <internal/common/feedback/feedback-plugin-proxy.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace // unnamed namespace
{

// Type Registration
Dali::BaseHandle Create()
{
  return Dali::SoundPlayer::Get() ;
}

Dali::TypeRegistration typeRegistration( typeid(Dali::SoundPlayer), typeid(Dali::BaseHandle), Create );

Dali::SignalConnectorType signalConnector1( typeRegistration, Dali::SoundPlayer::SIGNAL_SOUND_PLAY_FINISHED, Dali::Internal::Adaptor::SoundPlayer::DoConnectSignal );

} // unnamed namespace

Dali::SoundPlayer SoundPlayer::New()
{
  Dali::SoundPlayer player = Dali::SoundPlayer( new SoundPlayer() );

  // Register the object
  GetImplementation(player).RegisterObject();

  return player;
}

// void SoundPlayer::SetFeedbackPlugin( FeedbackPluginProxy* plugin )
//{
  // mPlugin = plugin;
//}

int SoundPlayer::PlaySound( const std::string fileName )
{
  int handle(0);
  return handle;
}

void SoundPlayer::Stop( int handle )
{
}

SoundPlayer::SoundPlayFinishedSignalV2& SoundPlayer::SoundPlayFinishedSignal()
{
  return mSoundPlayFinishedSignalV2;
}

bool SoundPlayer::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  bool connected( true );
  SoundPlayer* player = dynamic_cast<SoundPlayer*>( object );

  if( player &&
      Dali::SoundPlayer::SIGNAL_SOUND_PLAY_FINISHED == signalName )
  {
    player->SoundPlayFinishedSignal().Connect( tracker, functor );
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

SoundPlayer::SoundPlayer()
{
}

SoundPlayer::~SoundPlayer()
{
}

void SoundPlayer::EmitSoundPlayFinishedSignal()
{
  // Emit SoundPlayFinished signal

  if ( !mSoundPlayFinishedSignalV2.Empty() )
  {
    Dali::SoundPlayer handle( this );
    mSoundPlayFinishedSignalV2.Emit( handle );
  }
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
