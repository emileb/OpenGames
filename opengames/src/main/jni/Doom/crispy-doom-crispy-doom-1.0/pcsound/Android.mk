
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := pcsound_choc


LOCAL_CFLAGS :=  -O2 -g -Wall -Wdeclaration-after-statement  -D_GNU_SOURCE=1 -D_REENTRANT

LOCAL_LDLIBS += -lGLESv1_CM -lEGL

LOCAL_LDLIBS += -llog

LOCAL_CFLAGS += -march=armv7-a -mfloat-abi=softfp
LOCAL_LDLIBS += -Wl,--fix-cortex-a8


LOCAL_C_INCLUDES :=  $(TOP_DIR)/SDL/include $(TOP_DIR)/SDL_net/include $(TOP_DIR)/SDL_mixer $(TOP_DIR)/SDL_image  $(TOP_DIR)/libpng $(TOP_DIR)/jpeg8d $(TOP_DIR)/TouchControls  $(TOP_DIR)/TinyXML $(TOP_DIR)/
LOCAL_C_INCLUDES +=  $(LOCAL_PATH)/../src

libpcsound_a_SOURCES = \
        pcsound.c           pcsound.h             \
        pcsound_bsd.c                             \
        pcsound_sdl.c                             \
        pcsound_linux.c                           \
        pcsound_win32.c                           \
                            pcsound_internal.h


LOCAL_SRC_FILES = $(libpcsound_a_SOURCES) 

LOCAL_LDLIBS := -lGLESv1_CM -ldl -llog  -lz
LOCAL_STATIC_LIBRARIES := SDL_net libjpeg
LOCAL_SHARED_LIBRARIES :=  SDL 

include $(BUILD_STATIC_LIBRARY)






