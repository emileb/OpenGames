
LOCAL_PATH := $(call my-dir)


include $(CLEAR_VARS)


LOCAL_MODULE    := prboomsdl


LOCAL_CFLAGS :=   -O2  -DANDROID -DUSE_GLES  -DHAVE_CONFIG_H -g -Wall -Wextra -Wno-missing-field-initializers -Wwrite-strings  -Wbad-function-cast  -Wcast-qual -Wdeclaration-after-statement -Wpointer-arith -ffast-math -Wno-unused -Wno-switch -Wno-sign-compare -Wno-pointer-sign  -fomit-frame-pointer -D_GNU_SOURCE=1 -D_REENTRANT


LOCAL_LDLIBS += -llog

LOCAL_C_INCLUDES :=   $(TOP_DIR)  $(LOCAL_PATH)/../ $(TOP_DIR)/SDL/include $(TOP_DIR)/SDL_mixer  $(TOP_DIR)/SDL_net/include



LOCAL_SRC_FILES =                              \
	 i_main.c       i_network.c     i_video.c       i_sound.c \
 i_system.c     i_joy.c         i_sshot.c


LOCAL_STATIC_LIBRARIES := SDL_net 

LOCAL_LDLIBS :=  -ldl -llog

include $(BUILD_STATIC_LIBRARY)

