# DALI PORT STATIC LIBRARY

LOCAL_PATH := $(call my-dir)
MY_PATH := $(LOCAL_PATH)
include $(CLEAR_VARS)
include $(LOCAL_PATH)/defines.mk

LOCAL_MODULE := daliport
# Daliport still needs work to be able to compile with -Wall
LOCAL_CFLAGS += -w

cppfiles :=
cppfiles += $(shell find $(LOCAL_PATH)/dali-adaptor/adaptors/base -name '*.cpp' -or -name '*.c' )
cppfiles += $(shell find $(LOCAL_PATH)/dali-adaptor/adaptors/common -name '*.cpp' -or -name '*.c' )
cppfiles += $(shell find $(LOCAL_PATH)/dali-adaptor/adaptors/android -name '*.cpp' -or -name '*.c')
cppfiles += $(shell find $(LOCAL_PATH)/dali-adaptor/adaptors/public-api -name '*.cpp' -or -name '*.c')
cppfiles += $(shell find $(LOCAL_PATH)/dali-adaptor/platform-abstractions/android -name '*.cpp' -or -name '*.c' )
cppfiles += $(shell find $(LOCAL_PATH)/dali/dali/public-api -name '*.cpp' -print ) 
cppfiles += $(shell find $(LOCAL_PATH)/dali/dali/integration-api -name '*.cpp' )
cppfiles += $(shell find $(LOCAL_PATH)/dali/dali/internal -path '*/dynamics' -prune -o -name '*.cpp' -print )
cppfiles += $(shell find $(LOCAL_PATH)/dali-generated -name '*.cpp' )
#cppfiles += $(shell find $(LOCAL_PATH)/assimp -name '*.c' -or -name '*.cpp')
#cppfiles += $(shell find $(LOCAL_PATH)/bullet -name '*.cpp' )
cppfiles += $(shell find $(LOCAL_PATH)/dali-toolkit/base/dali-toolkit -path '*/text-view' -prune -o -name '*.cpp' -print )
cppfiles += $(shell find $(LOCAL_PATH)/dali-toolkit/optional/dali-toolkit -name '*.cpp' -print )

MY_LOCAL_SRC_FILES := $(cppfiles)
LOCAL_SRC_FILES := $(subst $(LOCAL_PATH)/, , $(MY_LOCAL_SRC_FILES))

LOCAL_STATIC_LIBRARIES := cpufeatures

LOCAL_CFLAGS += -I$(LOCAL_PATH)/assimp
LOCAL_CFLAGS += -I$(LOCAL_PATH)/bullet
LOCAL_CFLAGS += -I$(LOCAL_PATH)/dali
LOCAL_CFLAGS += -I$(LOCAL_PATH)/dali-adaptor
LOCAL_CFLAGS += -I$(LOCAL_PATH)/dali-adaptor/adaptors
LOCAL_CFLAGS += -I$(LOCAL_PATH)/dali-adaptor/adaptors/common
LOCAL_CFLAGS += -I$(LOCAL_PATH)/dali-adaptor/adaptors/android
LOCAL_CFLAGS += -I$(LOCAL_PATH)/dali-adaptor/adaptors/android/dali
LOCAL_CFLAGS += -I$(LOCAL_PATH)/dali-adaptor/adaptors/public-api
LOCAL_CFLAGS += -I$(LOCAL_PATH)/dali-adaptor/adaptors/public-api/adaptor-framework
LOCAL_CFLAGS += -I$(LOCAL_PATH)/dali-adaptor/adaptors/slp
LOCAL_CFLAGS += -I$(LOCAL_PATH)/dali-adaptor/platform-abstractions/android
LOCAL_CFLAGS += -I$(LOCAL_PATH)/dali-adaptor/platform-abstractions/slp
LOCAL_CFLAGS += -I$(LOCAL_PATH)/dali/dali
LOCAL_CFLAGS += -I$(LOCAL_PATH)/dali-generated
LOCAL_CFLAGS += -I$(LOCAL_PATH)/dali-toolkit/base
LOCAL_CFLAGS += -I$(LOCAL_PATH)/dali-toolkit/optional
LOCAL_CFLAGS += -I$(LOCAL_PATH)/libexif
LOCAL_CFLAGS += -I$(LOCAL_PATH)/libjpeg/src
LOCAL_CFLAGS += -I$(NDK_ROOT)/sources/boost/include
LOCAL_CFLAGS += -I$(PLATFORM_PREFIX)/source/libexif
LOCAL_CFLAGS += -I$(PLATFORM_PREFIX)/source/libgif
LOCAL_CFLAGS += -I$(PLATFORM_PREFIX)/source/libjpeg/src

include $(BUILD_STATIC_LIBRARY)


