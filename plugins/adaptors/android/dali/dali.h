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

#include <android/public-api/adaptor-framework/application.h>


#include <dali/public-api/adaptor-framework/common/adaptor.h>
#include <dali/public-api/adaptor-framework/common/device-layout.h>
#include <dali/public-api/adaptor-framework/common/style-monitor.h>
#include <dali/public-api/adaptor-framework/common/style-change.h>
#include <dali/public-api/adaptor-framework/common/timer.h>

// Window adaption
#include <dali/public-api/adaptor-framework/common/android-native-image.h>
#include <dali/public-api/adaptor-framework/common/surface-image.h>
#include <dali/public-api/adaptor-framework/common/render-surface.h>
#include <dali/public-api/adaptor-framework/common/window.h>

// Event and keyboard adaption
#include <public-api/adaptor-framework/common/accessibility-action-handler.h>
#include <public-api/adaptor-framework/common/accessibility-gesture-handler.h>
#include <dali/public-api/adaptor-framework/common/accessibility-manager.h>
#include <dali/public-api/adaptor-framework/common/clipboard.h>
#include <dali/public-api/adaptor-framework/common/clipboard-event-notifier.h>
#include <dali/public-api/adaptor-framework/common/drag-and-drop-detector.h>
#include <dali/public-api/adaptor-framework/common/virtual-keyboard.h>
#include <dali/public-api/adaptor-framework/common/key.h>

// Sensor and feedback adaption
#include <public-api/adaptor-framework/common/feedback-plugin.h>
#include <dali/public-api/adaptor-framework/common/haptic-player.h>
#include <dali/public-api/adaptor-framework/common/orientation.h>
#include <dali/public-api/adaptor-framework/common/sound-player.h>
#include <dali/public-api/adaptor-framework/common/tts-player.h>


#endif //__DALI_H__
