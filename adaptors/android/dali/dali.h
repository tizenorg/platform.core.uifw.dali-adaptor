#ifndef __DALI_H__
#define __DALI_H__

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

// dali-core
#include <dali/public-api/dali-core.h>

// Application / UI Framework adaption

// These defines come from running pkg-config --cflags with the correct package config file
// For backwards compatibility, dali.pc defines all three.

#include <dali/public-api/adaptor-framework/application.h>


#include <adaptors/common/adaptor.h>
#include <dali/public-api/adaptor-framework/device-layout.h>
#include <dali/public-api/adaptor-framework/style-monitor.h>
#include <dali/public-api/adaptor-framework/style-change.h>
#include <dali/public-api/adaptor-framework/timer.h>

// Window adaption
#include <dali/public-api/adaptor-framework/android-native-image.h>
#include <dali/public-api/adaptor-framework/surface-image.h>
#include <dali/public-api/adaptor-framework/render-surface.h>
#include <dali/public-api/adaptor-framework/window.h>

// Event and keyboard adaption
#include <public-api/adaptor-framework/accessibility-action-handler.h>
#include <public-api/adaptor-framework/accessibility-gesture-handler.h>
#include <dali/public-api/adaptor-framework/accessibility-manager.h>
#include <dali/public-api/adaptor-framework/clipboard.h>
#include <dali/public-api/adaptor-framework/clipboard-event-notifier.h>
#include <dali/public-api/adaptor-framework/drag-and-drop-detector.h>
#include <dali/public-api/adaptor-framework/virtual-keyboard.h>
#include <dali/public-api/adaptor-framework/singleton-service.h>
#include <dali/public-api/adaptor-framework/key.h>
#include <dali/public-api/adaptor-framework/physical-keyboard.h>

#include <dali/public-api/adaptor-framework/imf-manager.h>
#include <dali/public-api/adaptor-framework/clipboard.h>

// Sensor and feedback adaption
#include <public-api/adaptor-framework/feedback-plugin.h>
#include <dali/public-api/adaptor-framework/haptic-player.h>
#include <dali/public-api/adaptor-framework/orientation.h>
#include <dali/public-api/adaptor-framework/sound-player.h>
#include <dali/public-api/adaptor-framework/tts-player.h>

#include <android-interface.h>


#endif //__DALI_H__
