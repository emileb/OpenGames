LOCAL_CHOC_PATH := $(call my-dir)


include $(LOCAL_CHOC_PATH)/opl/Android.mk

include $(LOCAL_CHOC_PATH)/pcsound/Android.mk

include $(LOCAL_CHOC_PATH)/textscreen/Android.mk


include $(LOCAL_CHOC_PATH)//src/doom/Android.mk
include $(LOCAL_CHOC_PATH)/src/hexen/Android.mk
include $(LOCAL_CHOC_PATH)/src/strife/Android.mk
include $(LOCAL_CHOC_PATH)/src/setup/Android.mk
include $(LOCAL_CHOC_PATH)/src/heretic/Android.mk

include $(LOCAL_CHOC_PATH)/src/Android.mk



