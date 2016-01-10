LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := ecwolf
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/libecwolf.so
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE    := SDL
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/libSDL.so
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE    := SDL_mixer
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/libSDL_mixer.so
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE    := touchcontrols_blutz
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/libtouchcontrols.so
include $(PREBUILT_SHARED_LIBRARY)
