#ifndef __DALI_FEEDBACK_PLUGIN_H__
#define __DALI_FEEDBACK_PLUGIN_H__

/*
Copyright (c) 2000-2013 Samsung Electronics Co., Ltd All Rights Reserved

This file is part of Dali

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
#include <dali/public-api/common/dali-common.h>

namespace Dali DALI_IMPORT_API
{

/**
 * FeedbackPlugin is an abstract interface, used by Dali-adaptor to access haptic and audio feedback.
 * A concrete implementation must be created for each platform and provided as a dynamic library which
 * will be loaded at run time by the adaptor.
 */
class FeedbackPlugin
{
public:

  typedef void (*SoundStopCallBack)( void* ptr );

  /**
   * Destructor.
   */
  virtual ~FeedbackPlugin()
  {
  }

  /**
   * Plays vibration in predefined patterns.
   * @param[in] filePath Path to the file containing the effect.
   */
  virtual void PlayHaptic( const std::string& filePath ) = 0;

  /**
   * Plays a monotone vibration.
   * @param[in]  duration  The duration of the vibration.
   */
  virtual void PlayHapticMonotone( unsigned int duration ) = 0;

  /**
   * Stops the currently playing vibration effects.
   */
  virtual void StopHaptic() = 0;

  /**
   * Plays a sound file.
   * @param[in] fileName Path to the sound file to play.
   * @param[in] callback A pointer to a method called when the sound playback stops.
   * @param[in] callbackData The parameter passed into callback.
   * @return A handle which can be used to stop the sound playback.
   */
  virtual int PlaySound( const std::string& fileName, SoundStopCallBack callback, void* callbackData ) = 0;

  /**
   * Stops a currently playing sound.
   * @param[in] handle A handle to the currently playing sound.
   */
  virtual void StopSound( int handle ) = 0;

  /**
   * Plays a feedback pattern.
   * @param[in] type The type of feedback.
   * @param[in] pattern The ID of the pattern to play.
   */
  virtual void PlayFeedbackPattern( int type, int pattern ) = 0;

  // Types for plugin factories

  /**
   * Function pointer called in adaptor to create a feedback plugin instance.
   * @param [in] pluginName name of the plugin to load.
   * @return Pointer to the newly created plugin object
   */
  typedef FeedbackPlugin* CreateFeedbackPlugin( void );

}; // class FeedbackPlugin

} // namespace Dali

#endif // __DALI_FEEDBACK_PLUGIN_H__
