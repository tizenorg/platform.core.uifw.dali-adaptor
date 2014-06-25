#ifndef __DALI_H__
#define __DALI_H__

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

#include <dali/public-api/dali-core.h>

// Application / UI Framework adaption

// These defines come from running pkg-config --cflags with the correct package config file

#if defined(DALI_LIVEBOX_PLUGIN)
#include <public-api/livebox-plugin.h>
#endif

#if defined(DALI_EVAS_PLUGIN)
#include <public-api/evas-plugin.h>
#endif

#if defined(DALI_NATIVE_BUFFER_PLUGIN)
#include <public-api/native-buffer-plugin.h>
#endif

#if defined(DALI_APPLICATION)
#include <public-api/application.h>
#endif

#include <public-api/accessibility-manager.h>
#include <public-api/adaptor.h>
#include <public-api/clipboard.h>
#include <public-api/clipboard-event-notifier.h>
#include <public-api/device-layout.h>
#include <public-api/drag-and-drop-detector.h>
#include <public-api/haptic-player.h>
#include <public-api/imf-manager.h>
#include <public-api/key.h>
#include <public-api/orientation.h>
#include <public-api/pixmap-image.h>
#include <public-api/render-surface.h>
#include <public-api/sound-player.h>
#include <public-api/style-change.h>
#include <public-api/style-monitor.h>
#include <public-api/timer.h>
#include <public-api/tts-player.h>
#include <public-api/virtual-keyboard.h>
#include <public-api/window.h>

#include <public-api/accessibility-action-handler.h>
#include <public-api/accessibility-gesture-handler.h>
#include <public-api/event-feeder.h>
#include <public-api/feedback-plugin.h>
#include <public-api/physical-keyboard.h>
#include <public-api/tilt-sensor.h>

#endif //__DALI_H__
