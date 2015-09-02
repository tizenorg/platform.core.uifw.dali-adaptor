#ifdef WAYLAND_PERFORMANCE_DEBUG

#ifndef __DALI_WAYLAND_PERFORMANCE_DEBUG_H__
#define __DALI_WAYLAND_PERFORMANCE_DEBUG_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <dlfcn.h>
#include <stdio.h>

// INTERNAL INCLUDES
#include <base/interfaces/performance-interface.h>

/**
 * @brief set performance interface to allow markers to be logged
 */
void SetPerformanceInterface( Dali::Internal::Adaptor::PerformanceInterface* performanceInterface );

/**
 * @brief intercept function for wayland client, to log interaction between
 *
 * To enable, make sure adaptor.so is the first library in LD_PRELOAD environment variable
 * export LD_PRELOAD=/usr/lib/libdali-adaptor.so
 * @param[in] display wayland display pointer
 * @return number of bytes sent
 */
DALI_IMPORT_API int wl_display_flush(struct wl_display *display);

/**
 * @brief intercept function for wayland client,
 * dispatch performs a flush then a blocking read
 * To enable, make sure adaptor.so is the first library in LD_PRELOAD environment variable
 * export LD_PRELOAD=/usr/lib/libdali-adaptor.so
 *
 * @param[in] display wayland display pointer
 * @return number of dispatch events
 */
DALI_IMPORT_API int wl_display_dispatch(struct wl_display *display);


/**
 * @brief intercept function for wayland client
 * dispatch performs a flush on the queue then a blocking read
 *
 * To enable, make sure adaptor.so is the first library in LD_PRELOAD environment variable
 * export LD_PRELOAD=/usr/lib/libdali-adaptor.so
 *
 * @param[in] display wayland display pointer
 * @param[in] queue event queue
 * @return number of dispatched events
 */
DALI_IMPORT_API int wl_display_dispatch_queue(struct wl_display *display,  struct wl_event_queue *queue);

#endif // __DALI_WAYLAND_PERFORMANCE_DEBUG_H__
#endif // WAYLAND_PERFORMANCE_DEBUG
