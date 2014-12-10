LOCAL_PATH:= $(call my-dir)

# ------------------------------------------------------------------
# Static library for Cocos
# ------------------------------------------------------------------

include $(CLEAR_VARS)

C_FILE_PATH := /dependencies/source/libjpeg/src

# LOCAL_MODULE := cocos_jpeg_static
# APP_MODULES:=cocos_jpeg_static
APP_MODULES:= jpeg
# LOCAL_MODULE_FILENAME := libjpeg
LOCAL_MODULE := jpeg

LOCAL_SRC_FILES := \
	$(C_FILE_PATH)/jcapimin.c $(C_FILE_PATH)/jcapistd.c $(C_FILE_PATH)/jccoefct.c \
	$(C_FILE_PATH)/jccolor.c $(C_FILE_PATH)/jcdctmgr.c $(C_FILE_PATH)/jchuff.c \
	$(C_FILE_PATH)/jcinit.c $(C_FILE_PATH)/jcmainct.c \
	$(C_FILE_PATH)/jcmarker.c $(C_FILE_PATH)/jcmaster.c $(C_FILE_PATH)/jcomapi.c \
	$(C_FILE_PATH)/jcparam.c $(C_FILE_PATH)/jcprepct.c $(C_FILE_PATH)/jcsample.c \
	$(C_FILE_PATH)/jctrans.c $(C_FILE_PATH)/jdapimin.c $(C_FILE_PATH)/jdapistd.c \
	$(C_FILE_PATH)/jdatadst.c $(C_FILE_PATH)/jdatasrc.c $(C_FILE_PATH)/jdcoefct.c \
	$(C_FILE_PATH)/jdcolor.c $(C_FILE_PATH)/jddctmgr.c $(C_FILE_PATH)/jdhuff.c \
	$(C_FILE_PATH)/jdinput.c $(C_FILE_PATH)/jdmainct.c $(C_FILE_PATH)/jdmarker.c \
	$(C_FILE_PATH)/jdmaster.c $(C_FILE_PATH)/jdmerge.c \
	$(C_FILE_PATH)/jdpostct.c $(C_FILE_PATH)/jdsample.c \
	$(C_FILE_PATH)/jdtrans.c $(C_FILE_PATH)/jerror.c $(C_FILE_PATH)/jfdctflt.c $(C_FILE_PATH)/jfdctfst.c \
	$(C_FILE_PATH)/jfdctint.c $(C_FILE_PATH)/jidctflt.c $(C_FILE_PATH)/jidctfst.c \
	$(C_FILE_PATH)/jidctint.c $(C_FILE_PATH)/jquant1.c \
	$(C_FILE_PATH)/jquant2.c $(C_FILE_PATH)/jutils.c $(C_FILE_PATH)/jmemmgr.c \
	$(C_FILE_PATH)/jcarith.c $(C_FILE_PATH)/jdarith.c $(C_FILE_PATH)/jaricom.c

# Use the no backing store memory manager provided by
# libjpeg. See install.txt
LOCAL_SRC_FILES += \
	$(C_FILE_PATH)/jmemnobs.c


LOCAL_CFLAGS    := -w 
LOCAL_CFLAGS += -DASSIMP_BUILD_NO_OWN_ZLIB
## LOCAL_CFLAGS += -DASSIMP_BUILD_NO_OWN_ZLIB
## LOCAL_CFLAGS += -I$(NDK_ROOT)/sources/boost/include $(bullet_includes)
## LOCAL_CFLAGS += -O0 -g

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/libjpeg/src

LOCAL_LDLIBS += -l$(LOCAL_PATH)/libexif/lib/libexifa.so

LOCAL_CPPFLAGS += -fexceptions
LOCAL_CPPFLAGS += -frtti 

include $(BUILD_STATIC_LIBRARY)
