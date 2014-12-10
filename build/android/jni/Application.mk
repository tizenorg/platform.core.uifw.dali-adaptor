# Updated to build armeabi-v7a binary (hardware FPU)
APP_ABI := armeabi-v7a
NDK_TOOLCHAIN_VERSION=4.6
APP_PLATFORM := android-19
APP_STL := gnustl_static
APP_MODULES := daliapp

PLATFORM_PREFIX := $(call my-dir)/dependencies
APP_CPPFLAGS := -I$(PLATFORM_PREFIX)/include
APP_CPPFLAGS += -I$(PLATFORM_PREFIX)/include/freetype2
APP_CPPFLAGS += -I$(PLATFORM_PREFIX)/include/libpng15
APP_CPPFLAGS += -I$(PLATFORM_PREFIX)/include/libxml2
APP_CPPFLAGS += -I$(PLATFORM_PREFIX)/include/boost
ifeq ($(BUILD_RELEASE),1)
APP_OPTIM := release
else
APP_OPTIM := debug
endif
