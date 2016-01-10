
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := setup_choc


LOCAL_CFLAGS :=  -O2 -g -Wall -Wdeclaration-after-statement  -D_GNU_SOURCE=1 -D_REENTRANT
LOCAL_CFLAGS += -DPROGRAM_PREFIX=""

LOCAL_LDLIBS += -llog

LOCAL_C_INCLUDES :=  $(TOP_DIR)/SDL/include $(TOP_DIR)/SDL_net/include $(TOP_DIR)/SDL_mixer $(TOP_DIR)/SDL_image  $(TOP_DIR)/libpng $(TOP_DIR)/jpeg8d $(TOP_DIR)/TouchControls  $(TOP_DIR)/TinyXML $(TOP_DIR)/
LOCAL_C_INCLUDES +=   $(LOCAL_PATH)/../../textscreen $(LOCAL_PATH)/../ $(LOCAL_PATH)/../../  $(LOCAL_PATH)/../android

SOURCE_FILES = \
    compatibility.c   compatibility.h           \
    display.c         display.h                 \
    joystick.c        joystick.h                \
    keyboard.c        keyboard.h                \
    mainmenu.c                                  \
    mode.c            mode.h                    \
    mouse.c           mouse.h                   \
    multiplayer.c     multiplayer.h             \
    sound.c           sound.h                   \
    execute.c         execute.h                 \
    txt_joyaxis.c txt_joybinput.c   txt_joybinput.h           \
    txt_keyinput.c    txt_keyinput.h            \
    txt_mouseinput.c  txt_mouseinput.h



LOCAL_SRC_FILES = $(SOURCE_FILES)

LOCAL_LDLIBS :=  -ldl  -lz
LOCAL_STATIC_LIBRARIES := SDL_net libjpeg
LOCAL_SHARED_LIBRARIES :=  SDL SDL_mixer SDL_image 

include $(BUILD_STATIC_LIBRARY)






