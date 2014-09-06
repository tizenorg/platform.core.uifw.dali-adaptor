#ifndef __FEEDBACK_PLUGIN_H__
#define __FEEDBACK_PLUGIN_H__

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
#include <haptic.h>

// INTERNAL INCLUDES
#include <adaptors/tizen/public-api/adaptor-framework/common/feedback-plugin.h>

namespace Dali
{

namespace Plugin
{

/**
 * Plays feedback effects for Dali-Toolkit UI Controls.
 */
class DaliFeedback : public Dali::FeedbackPlugin
{

public: // Construction & Destruction

  /**
   * Constructor
   */
  DaliFeedback();

  /**
   * Destructor
   */
  virtual ~DaliFeedback();

public: // FeedbackPlugin overrides

  /**
   * @copydoc Dali::Integration::FeedbackPlugin::PlayHaptic()
   */
  void PlayHaptic( const std::string& filePath );

  /**
   * @copydoc Dali::FeedbackPlugin::PlayHapticMonotone()
   */
  void PlayHapticMonotone( unsigned int duration );

  /**
   * @copydoc Dali::FeedbackPlugin::StopHaptic()
   */
  void StopHaptic();

  /**
   * @copydoc Dali::FeedbackPlugin::PlaySound()
   */
  int PlaySound( const std::string& fileName, Dali::FeedbackPlugin::SoundStopCallBack callback, void* callbackData );

  /**
   * @copydoc Dali::FeedbackPlugin::StopSound()
   */
  void StopSound( int handle );

  /**
   * @copydoc Dali::FeedbackPlugin::PlayFeedbackPattern()
   */
  void PlayFeedbackPattern( int type, int pattern );

private:

  haptic_device_h mDeviceHandle;

  bool mHapticInitialized;
};

}  // namespace Plugin

}  // namespace Dali

#endif // __FEEDBACK_PLUGIN_H__
