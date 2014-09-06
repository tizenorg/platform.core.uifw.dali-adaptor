#ifndef __DALI_INTERNAL_SOUND_PLAYER_H__
#define __DALI_INTERNAL_SOUND_PLAYER_H__

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

// EXTERNAL INCLUDES
#include <string>
#include <dali/integration-api/debug.h>
#include <dali/public-api/object/base-object.h>

// INTERNAL INCLUDES
#include <dali/public-api/adaptor-framework/common/sound-player.h>


namespace Dali
{

namespace Internal
{

namespace Adaptor
{

// class FeedbackPluginProxy;

/**
 * Plays haptic effects.
 */
class SoundPlayer : public Dali::BaseObject
{

public:

  typedef Dali::SoundPlayer::SoundPlayFinishedSignalV2 SoundPlayFinishedSignalV2;

  /**
   * Create a SoundPlayer.
   * This should only be called once by the Adaptor class.
   * @return A newly created SoundPlayer.
   */
  static Dali::SoundPlayer New();

  /**
   * Connect or disconnect from the feedback plugin.
   * @param[in] plugin Used to dynamically load feedback libraries.
   */
  // void SetFeedbackPlugin( FeedbackPluginProxy* plugin );

  /**
   * @copydoc Dali::SoundPlayer::PlaySound()
   */
  int PlaySound(const std::string fileName);

  /**
   * @copydoc Dali::SoundPlayer::Stop()
   */
  void Stop(int handle);

  /**
   * @copydoc Dali::SoundPlayer::SoundPlayFinishedSignal()
   */
  SoundPlayFinishedSignalV2& SoundPlayFinishedSignal();

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
   * Private Constructor; see also soundPlayer::New()
   * @param[in]  soundPlayer  The public sound player class
   */
  SoundPlayer();

  /**
   * Destructor
   */
  virtual ~SoundPlayer();

  /**
   * Emits the SoundPlayFinished signal.
   */
  void EmitSoundPlayFinishedSignal();

  // Undefined
  SoundPlayer(const SoundPlayer&);

  // Undefined
  SoundPlayer& operator=(SoundPlayer&);

private:

  // FeedbackPluginProxy* mPlugin;

  SoundPlayFinishedSignalV2 mSoundPlayFinishedSignalV2;

#if defined(DEBUG_ENABLED)
public:
  static Debug::Filter* gLogFilter;
#endif
};

} // namespace Adaptor

} // namespace Internal


// Helpers for public-api forwarding methods

inline Internal::Adaptor::SoundPlayer& GetImplementation(Dali::SoundPlayer& player)
{
  DALI_ASSERT_ALWAYS( player && "SoundPlayer handle is empty" );

  BaseObject& handle = player.GetBaseObject();

  return static_cast<Internal::Adaptor::SoundPlayer&>(handle);
}

inline const Internal::Adaptor::SoundPlayer& GetImplementation(const Dali::SoundPlayer& player)
{
  DALI_ASSERT_ALWAYS( player && "SoundPlayer handle is empty" );

  const BaseObject& handle = player.GetBaseObject();

  return static_cast<const Internal::Adaptor::SoundPlayer&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_SOUND_PLAYER_H__
