# Updated to build armeabi-v7a binary (hardware FPU)
ifeq ($(BUILD_64BIT),1)
APP_ABI := arm64-v8a
NDK_TOOLCHAIN_VERSION=4.9
APP_PLATFORM := android-21
else
APP_ABI := armeabi-v7a
NDK_TOOLCHAIN_VERSION=4.9
APP_PLATFORM := android-21
endif
APP_STL := gnustl_static

APP_MODULES := $(MODULE_NAME)
APP_BUILD_SCRIPT := $(MODULE_PATH)/Android.mk

PLATFORM_PREFIX := $(PROJECT_PATH)/dependencies/$(APP_ABI)-$(APP_PLATFORM)-$(NDK_TOOLCHAIN_VERSION)
APP_CPPFLAGS := -I$(PLATFORM_PREFIX)/include
APP_CPPFLAGS += -I$(PLATFORM_PREFIX)/include/freetype2
APP_CPPFLAGS += -I$(PLATFORM_PREFIX)/include/libpng16
APP_CPPFLAGS += -I$(PLATFORM_PREFIX)/include/libxml2

ifeq ($(BUILD_64BIT),1)
APP_CPPFLAGS += -DIS_64BIT_BUILD
endif

ifeq ($(BUILD_RELEASE),1)
APP_OPTIM := release
else
APP_OPTIM := debug
endif
