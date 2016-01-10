LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := openal
LOCAL_SRC_FILES := lib/libopenal.so

include $(PREBUILT_SHARED_LIBRARY)
