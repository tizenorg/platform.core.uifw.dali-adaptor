LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE:= gif

C_FILE_PATH := /dependencies/source/libgif
LOCAL_EXPORT_C_INCLUDES := $(PLATFORM_PREFIX)/source/libgif


LOCAL_SRC_FILES := \
	$(C_FILE_PATH)/dgif_lib.c \
	$(C_FILE_PATH)/gifalloc.c \
	$(C_FILE_PATH)/gif_err.c

LOCAL_CFLAGS    := -w 
LOCAL_CFLAGS += -I$(PLATFORM_PREFIX)/source/libgif
LOCAL_CFLAGS += -DASSIMP_BUILD_NO_OWN_ZLIB
LOCAL_CFLAGS += -Wno-format -DHAVE_CONFIG_H



LOCAL_CPPFLAGS += -frtti 
include $(BUILD_STATIC_LIBRARY)
