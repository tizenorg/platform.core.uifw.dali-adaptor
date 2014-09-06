#ifndef __DALI_HAPTIC_PLAYER_H__
#define __DALI_HAPTIC_PLAYER_H__

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
#include <dali/public-api/object/base-handle.h>

namespace Dali DALI_IMPORT_API
{

namespace Internal DALI_INTERNAL
{
namespace Adaptor
{
class HapticPlayer;
}
}

/**
 * Plays haptic effects.
 */
class HapticPlayer : public BaseHandle
{
public:

  /**
   * Create an uninitialized handle.
   * This can be initialized by calling HapticPlayer::Get().
   */
  HapticPlayer();

  /**
   * Create an initialized handle to the HapticPlayer.
   * @return A handle to a newly allocated Dali resource.
   */
  static HapticPlayer Get();

  /**
   * Destructor
   */
  ~HapticPlayer();

  /**
   * Checks if the controller is initialized and available to play.
   * @deprecated This method isn't compatible with lazy loading of feedback libraries.
   * @return true if initialized false otherwise.
   */
  bool IsEnabled();

  /**
   * Plays a monotone vibration.
   * @param[in]  duration  The duration of the vibration.
   */
  void PlayMonotone(unsigned int duration);

  /**
   * Plays vibration in predefined patterns.
   * @param[in] filePath Path to the file containing the effect.
   */
  void PlayFile(const std::string filePath);

  /**
   * Stops the currently playing vibration effects.
   */
  void Stop();

public: // Not intended for application developers

  /**
   * This constructor is used by HapticPlayer::Get().
   * @param[in] hapticPlayer A pointer to the haptic player.
   */
  HapticPlayer( Internal::Adaptor::HapticPlayer* hapticPlayer );
};

} // namespace Dali

#endif // __DALI_HAPTIC_PLAYER_H__
