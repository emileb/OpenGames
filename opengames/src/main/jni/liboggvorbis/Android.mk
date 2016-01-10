LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := libvorbis
LOCAL_SRC_FILES := lib/libvorbis.so
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE    := libogg
LOCAL_SRC_FILES := lib/libogg.so
include $(PREBUILT_SHARED_LIBRARY)