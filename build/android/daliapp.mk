# DALI APPLICATION SHARED LIBRARY
#

LOCAL_PATH:= $(call my-dir)
MY_PATH := $(LOCAL_PATH)
include $(CLEAR_VARS)
include $(LOCAL_PATH)/defines.mk
#include $(LOCAL_PATH)/jniheader.mk

LOCAL_MODULE := daliapp
LOCAL_CFLAGS += -Wall

cppfiles :=
cppfiles += $(shell find $(LOCAL_PATH)/dali-adaptor/adaptors/base -name '*.cpp' -or -name '*.c' )
cppfiles += $(shell find $(LOCAL_PATH)/dali-adaptor/adaptors/common -name '*.cpp' -or -name '*.c' )
cppfiles += $(shell find $(LOCAL_PATH)/dali-adaptor/adaptors/android -name '*.cpp' -or -name '*.c')
cppfiles += $(shell find $(LOCAL_PATH)/dali-adaptor/adaptors/public-api -name '*.cpp' -or -name '*.c')
cppfiles += $(shell find $(LOCAL_PATH)/dali-adaptor/platform-abstractions/android -name '*.cpp' -or -name '*.c' )
cppfiles += $(shell find $(LOCAL_PATH)/dali/dali/public-api -name '*.cpp' -print )
cppfiles += $(shell find $(LOCAL_PATH)/dali/dali/integration-api -name '*.cpp' )
cppfiles += $(LOCAL_PATH)/dali-demo/main.cpp
cppfiles += $(shell find $(LOCAL_PATH)/nativeUtils -name '*.cpp' )
cppfiles += $(shell find $(LOCAL_PATH)/dali/dali/internal -path '*/dynamics' -prune -o -name '*.cpp' -print )
cppfiles += $(shell find $(LOCAL_PATH)/dali-generated -name '*.cpp' )
#cppfiles += $(shell find $(LOCAL_PATH)/assimp -name '*.c' -or -name '*.cpp')
#cppfiles += $(shell find $(LOCAL_PATH)/bullet -name '*.cpp' )
cppfiles += $(shell find $(LOCAL_PATH)/dali-toolkit/base/dali-toolkit -path '*/text-view' -prune -o -name '*.cpp' -print )
cppfiles += $(shell find $(LOCAL_PATH)/dali-toolkit/optional/dali-toolkit -name '*.cpp' -print )
#cppfiles += $(shell find $(LOCAL_PATH)/resource -name '*.cpp' )

MY_LOCAL_SRC_FILES := $(cppfiles)
LOCAL_SRC_FILES := $(subst $(LOCAL_PATH)/, , $(MY_LOCAL_SRC_FILES))

DALI_PATH:=$(LOCAL_PATH)
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
LOCAL_CFLAGS += -I$(LOCAL_PATH)/dali-adaptor/platform-abstractions/android
LOCAL_CFLAGS += -I$(LOCAL_PATH)/dali-adaptor/adaptors/android
LOCAL_CFLAGS += -I$(LOCAL_PATH)/jniGlue/headers
#LOCAL_CFLAGS += -I$(LOCAL_PATH)/resource/headers
LOCAL_CFLAGS += -I$(LOCAL_PATH)/nativeUtils/include

LOCAL_STATIC_LIBRARIES := gif jpeg png xml2 boost_thread boost_system boost_atomic freetype
LOCAL_STATIC_LIBRARIES += android_native_app_glue
LOCAL_SHARED_LIBRARIES += libvsync libgraphicbuffer
LOCAL_LDLIBS += -llog -lGLESv2 -lEGL -lz -landroid -ljnigraphics
LOCAL_LDFLAGS += -L$(TARGET_OUT) -Wl,--version-script=$(LOCAL_PATH)/nativeDali.export

ifeq ($(BUILD_NATIVE_TRACE), 1)
LOCAL_LDLIBS += -lcutils
LOCAL_LDFLAGS += -L$(call host-path,$(TARGET_C_INCLUDES)/../lib/rs)
endif

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := gif
LOCAL_SRC_FILES := dependencies/homescreen-lib/libgif.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libvsync
LOCAL_SRC_FILES := dependencies/homescreen-lib/libvsync.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libgraphicbuffer
LOCAL_SRC_FILES := dependencies/homescreen-lib/libgraphicbufferwrapper.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := jpeg
LOCAL_SRC_FILES := dependencies/homescreen-lib/libjpeg.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := xml2
LOCAL_SRC_FILES := dependencies/homescreen-lib/libxml2.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := png
LOCAL_SRC_FILES := dependencies/homescreen-lib/libpng15.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := freetype
LOCAL_SRC_FILES := dependencies/homescreen-lib/libfreetype.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := boost_thread
LOCAL_SRC_FILES := dependencies/homescreen-lib/libboost_thread-gcc-mt-1_53.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := boost_system
LOCAL_SRC_FILES := dependencies/homescreen-lib/libboost_system-gcc-mt-1_53.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := boost_atomic
LOCAL_SRC_FILES := dependencies/homescreen-lib/libboost_atomic-gcc-mt-1_53.a
include $(PREBUILT_STATIC_LIBRARY)


clean-daliapp:
	$(eval CLEAN_OBJECTS := $(__ndk_modules.daliapp.SRC_FILES))
	$(eval CLEAN_OUTPUT := $(__ndk_modules.daliapp.BUILT_MODULE))
	$(eval OBJ_DIR := $(__ndk_modules.daliapp.OBJS_DIR))
	$(foreach _ext,$(all_source_extensions),\
               $(eval CLEAN_OBJECTS := $$(CLEAN_OBJECTS:%$(_ext)=%.o))\
	)
	$(eval CLEAN_OBJECTS := $(filter %.o,$(CLEAN_OBJECTS)))
	$(eval CLEAN_OBJECTS := $(subst ../,__/,$(CLEAN_OBJECTS)))
	$(eval CLEAN_OBJECTS := $(foreach _obj,$(CLEAN_OBJECTS),$(OBJ_DIR)/$(_obj)))
	rm -f $(CLEAN_OBJECTS)
	rm $(CLEAN_OUTPUT)

