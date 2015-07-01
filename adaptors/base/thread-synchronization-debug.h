#ifndef __DALI_INTERNAL_THREAD_SYNCHRONIZATION_DEBUG_H__
#define __DALI_INTERNAL_THREAD_SYNCHRONIZATION_DEBUG_H__

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
#include <cstdio>
#include <iostream>
#include <dali/integration-api/debug.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace
{
#define ENABLE_LOG_IN_COLOR 1
#define ENABLE_VSYNC_COUNTER_LOGGING 1
#define ENABLE_UPDATE_COUNTER_LOGGING 1
#define ENABLE_VSYNC_THREAD_LOGGING 1
#define ENABLE_UPDATE_THREAD_LOGGING 1
#define ENABLE_RENDER_THREAD_LOGGING 1
#define ENABLE_EVENT_LOGGING

#ifdef ENABLE_LOG_IN_COLOR
#define COLOR_RED            "\033[31m"
#define COLOR_YELLOW         "\033[33m"
#define COLOR_BLUE           "\033[34m"
#define COLOR_LIGHT_RED      "\033[91m"
#define COLOR_LIGHT_YELLOW   "\033[93m"
#define COLOR_LIGHT_BLUE     "\033[94m"
#define COLOR_WHITE          "\033[97m"
#define COLOR_CLEAR          "\033[0m"
#else
#define COLOR_RED
#define COLOR_YELLOW
#define COLOR_BLUE
#define COLOR_LIGHT_RED
#define COLOR_LIGHT_YELLOW
#define COLOR_LIGHT_BLUE
#define COLOR_WHITE
#define COLOR_CLEAR
#endif

#define LOG_THREAD_SYNC(color,...)      std::cout << color; printf(__VA_ARGS__); std::cout << COLOR_CLEAR << std::endl;

#ifdef ENABLE_VSYNC_COUNTER_LOGGING
#define LOG_VSYNC_COUNTER_VSYNC(...)    LOG_THREAD_SYNC(COLOR_LIGHT_RED,__VA_ARGS__)
#define LOG_VSYNC_COUNTER_UPDATE(...)   LOG_THREAD_SYNC(COLOR_LIGHT_YELLOW,__VA_ARGS__)
#else
#define LOG_VSYNC_COUNTER_VSYNC(...)
#define LOG_VSYNC_COUNTER_UPDATE(...)
#endif

#ifdef ENABLE_UPDATE_COUNTER_LOGGING
#define LOG_UPDATE_COUNTER_UPDATE(...)  LOG_THREAD_SYNC(COLOR_YELLOW,__VA_ARGS__)
#define LOG_UPDATE_COUNTER_RENDER(...)  LOG_THREAD_SYNC(COLOR_LIGHT_BLUE,__VA_ARGS__)
#else
#define LOG_UPDATE_COUNTER_UPDATE(...)
#define LOG_UPDATE_COUNTER_RENDER(...)
#endif

#ifdef ENABLE_VSYNC_THREAD_LOGGING
#define LOG_VSYNC(...)             LOG_THREAD_SYNC(COLOR_RED,__VA_ARGS__)
#else
#define LOG_VSYNC(...)
#endif

#ifdef ENABLE_UPDATE_THREAD_LOGGING
#define LOG_UPDATE(...)            LOG_THREAD_SYNC(COLOR_YELLOW,__VA_ARGS__)
#else
#define LOG_UPDATE(...)
#endif

#ifdef ENABLE_RENDER_THREAD_LOGGING
#define LOG_RENDER(...)            LOG_THREAD_SYNC(COLOR_BLUE,__VA_ARGS__)
#else
#define LOG_RENDER(...)
#endif

#ifdef ENABLE_EVENT_LOGGING
#define LOG_EVENT(...)             LOG_THREAD_SYNC(COLOR_WHITE,__VA_ARGS__)
#else
#define LOG_EVENT(...)
#endif
} // unnamed namespace

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_THREAD_SYNCHRONIZATION_DEBUG_H__
