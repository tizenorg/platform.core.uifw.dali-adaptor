LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := daliport 
LOCAL_SRC_FILES := $(LOCALPATH)/dependencies/homescreen-lib/libdaliport.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := gif 
LOCAL_SRC_FILES := $(LOCALPATH)/dependencies/homescreen-lib/libgif.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := jpeg 
LOCAL_SRC_FILES := $(LOCALPATH)/dependencies/homescreen-lib/libjpeg.a
include $(PREBUILT_STATIC_LIBRARY)
