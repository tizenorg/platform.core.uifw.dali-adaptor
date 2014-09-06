#ifndef __DALI_INTERNAL_HAPTIC_PLAYER_H__
#define __DALI_INTERNAL_HAPTIC_PLAYER_H__

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
#include <dali/public-api/object/base-object.h>

// INTERNAL INCLUDES
#include <dali/public-api/adaptor-framework/common/haptic-player.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

class FeedbackPluginProxy;

/**
 * Plays haptic effects.
 */
class HapticPlayer : public Dali::BaseObject
{

public:

  /**
   * Create a HapticPlayer.
   * This should only be called once by the Adaptor class.
   * @return A newly created HapticPlayer.
   */
  static Dali::HapticPlayer New();

  /**
   * Connect or disconnect from the feedback plugin.
   * @param[in] plugin Used to dynamically load feedback libraries.
   */
  // void SetFeedbackPlugin( FeedbackPluginProxy* plugin );

  /**
   * @copydoc Dali::HapticPlayer::IsEnabled()
   */
  bool IsEnabled();

  /**
   * @copydoc Dali::HapticPlayer::PlayMonotone()
   */
  void PlayMonotone(unsigned int duration);

  /**
   * @copydoc Dali::HapticPlayer::PlayFile()
   */
  void PlayFile( const std::string& filePath );

  /**
   * @copydoc Dali::HapticPlayer::Stop()
   */
  void Stop();

private:

  /**
   * Private Constructor; see also HapticPlayer::New()
   */
  HapticPlayer();

  /**
   * Virtual Destructor
   */
  virtual ~HapticPlayer();

  // Undefined
  HapticPlayer(const HapticPlayer&);

  // Undefined
  HapticPlayer& operator=(HapticPlayer&);

private:

  // FeedbackPluginProxy* mPlugin;
};

} // namespace Adaptor

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::Adaptor::HapticPlayer& GetImplementation(Dali::HapticPlayer& player)
{
  DALI_ASSERT_ALWAYS( player && "HapticPlayer handle is empty" );

  BaseObject& handle = player.GetBaseObject();

  return static_cast<Internal::Adaptor::HapticPlayer&>(handle);
}

inline const Internal::Adaptor::HapticPlayer& GetImplementation(const Dali::HapticPlayer& player)
{
  DALI_ASSERT_ALWAYS( player && "HapticPlayer handle is empty" );

  const BaseObject& handle = player.GetBaseObject();

  return static_cast<const Internal::Adaptor::HapticPlayer&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_HAPTIC_PLAYER_H__
