LOCAL_PATH := $(call my-dir)


include $(CLEAR_VARS)


LOCAL_MODULE    := doommusic


LOCAL_CFLAGS :=   -O2  -DANDROID -DUSE_GLES  -DHAVE_CONFIG_H -g -Wall -Wextra -Wno-missing-field-initializers -Wwrite-strings  -Wbad-function-cast  -Wcast-qual -Wdeclaration-after-statement -Wpointer-arith -ffast-math -Wno-unused -Wno-switch -Wno-sign-compare -Wno-pointer-sign  -fomit-frame-pointer -D_GNU_SOURCE=1 -D_REENTRANT

LOCAL_LDLIBS += -llog

LOCAL_C_INCLUDES :=   $(LOCAL_PATH)/include/fluidsynth/  $(LOCAL_PATH)/../

LOCAL_SRC_FILES =  \
 dbopl.c \
 dbopl.h \
 dumbplayer.c \
 dumbplayer.h \
 flplayer.c \
 flplayer.h \
 madplayer.c \
 madplayer.h \
 midifile.c \
 midifile.h \
 opl.c \
 opl.h \
 oplplayer.c \
 oplplayer.h \
 opl_queue.c \
 opl_queue.h \
 portmidiplayer.c \
 portmidiplayer.h \
 vorbisplayer.c \
 vorbisplayer.h \
 musicplayer.h



LOCAL_LDLIBS :=  -ldl -llog
LOCAL_STATIC_LIBRARIES := fluidsynth-static
include $(BUILD_STATIC_LIBRARY)
