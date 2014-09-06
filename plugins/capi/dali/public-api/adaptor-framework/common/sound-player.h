#ifndef __DALI_SOUND_PLAYER_H__
#define __DALI_SOUND_PLAYER_H__

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
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/signals/dali-signal-v2.h>

namespace Dali DALI_IMPORT_API
{

namespace Internal DALI_INTERNAL
{
namespace Adaptor
{
class SoundPlayer;
}
}

/**
 * Plays sound effects.
 */
class SoundPlayer : public BaseHandle
{

public:

  typedef SignalV2< void (SoundPlayer&) > SoundPlayFinishedSignalV2;

  //Typedef's
  //typedef boost::signals2::signal_type<void (SoundPlayer&), DummyMutex>::type SoundPlayFinishedSignalType;
  //typedef boost::function<void (SoundPlayer&)> SoundPlayFinishedSignalCallbackType;
  //typedef Signal<SoundPlayFinishedSignalType, SoundPlayFinishedSignalCallbackType> SoundPlayFinishedSignal;

  // Signal Names
  static const char* const SIGNAL_SOUND_PLAY_FINISHED;

public:

  /**
   * Create an uninitialized handle.
   * This can be initialized by calling SoundPlayer::Get().
   */
  SoundPlayer();

  /**
   * Create an initialized handle to the SoundPlayer.
   * @return A handle to a newly allocated Dali resource.
   */
  static SoundPlayer Get();

  /**
   * Destructor
   */
  ~SoundPlayer();

  /**
   * Plays a sound file.
   * @pre The SoundPlayer needs to be initialized.
   * @param[in]  fileName Path to the sound file to play.
   * @return a handle to the currently playing sound file which can be used to stop.
   */
  int PlaySound(const std::string fileName);

  /**
   * Stops the currently playing sound.
   * @pre The SoundPlayer needs to be initialized.
   */
  void Stop(int handle);

  /**
   * This signal will be emitted after a given sound file is completely played.
   * @pre The SoundPlayer needs to be initialized.
   * @return The signal to connect to.
   */
  //SoundPlayFinishedSignal SignalSoundPlayFinished();
  SoundPlayFinishedSignalV2& SoundPlayFinishedSignal();


public: // Not intended for application developers

  /**
   * This constructor is used by SoundPlayer::Get().
   * @param[in] soundPlayer A pointer to the sound player.
   */
  SoundPlayer( Internal::Adaptor::SoundPlayer* soundPlayer );
};

} // namespace Dali

#endif // __DALI_SOUND_PLAYER_H__
