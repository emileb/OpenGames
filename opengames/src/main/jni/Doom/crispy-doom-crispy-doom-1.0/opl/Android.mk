
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := opl_choc


LOCAL_CFLAGS :=  -O2 -g -Wall -Wdeclaration-after-statement  -D_GNU_SOURCE=1 -D_REENTRANT

LOCAL_LDLIBS += -lGLESv1_CM -lEGL

LOCAL_LDLIBS += -llog

LOCAL_CFLAGS += -march=armv7-a -mfloat-abi=softfp
LOCAL_LDLIBS += -Wl,--fix-cortex-a8


LOCAL_C_INCLUDES :=  $(TOP_DIR)/SDL/include $(TOP_DIR)/SDL_net/include $(TOP_DIR)/SDL_mixer $(TOP_DIR)/SDL_image  $(TOP_DIR)/libpng $(TOP_DIR)/jpeg8d $(TOP_DIR)/TouchControls  $(TOP_DIR)/TinyXML $(TOP_DIR)/
LOCAL_C_INCLUDES +=  $(LOCAL_PATH)/../src

libopl_a_SOURCES = \
                            opl_internal.h        \
        opl.c               opl.h                 \
        opl_linux.c                               \
        opl_obsd.c                                \
        opl_queue.c         opl_queue.h           \
        opl_sdl.c                                 \
        opl_timer.c         opl_timer.h           \
        opl_win32.c                               \
        ioperm_sys.c        ioperm_sys.h          \
        dbopl.c             dbopl.h



LOCAL_SRC_FILES = $(libopl_a_SOURCES) 

LOCAL_LDLIBS := -lGLESv1_CM -ldl -llog  -lz
LOCAL_STATIC_LIBRARIES := SDL_net libjpeg
LOCAL_SHARED_LIBRARIES :=  SDL 

include $(BUILD_STATIC_LIBRARY)






