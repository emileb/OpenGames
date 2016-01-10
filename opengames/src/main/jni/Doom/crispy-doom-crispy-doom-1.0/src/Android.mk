
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := choc-doom


LOCAL_CFLAGS :=  -O2 -g -Wall -Wdeclaration-after-statement  -D_GNU_SOURCE=1 -D_REENTRANT

LOCAL_LDLIBS += -lGLESv1_CM -lEGL

LOCAL_LDLIBS += -llog -lz

LOCAL_C_INCLUDES :=  $(TOP_DIR)/SDL/include $(TOP_DIR)/SDL_net/include $(TOP_DIR)/SDL_mixer   $(TOP_DIR)/MobileTouchControls  $(TOP_DIR)/
LOCAL_C_INCLUDES +=   $(LOCAL_PATH)/../textscreen $(LOCAL_PATH)/../pcsound $(LOCAL_PATH)/../opl

 
 ANDROID_FILES = \
       android/android-jni.cpp \
        android/in_android.cpp \
          
COMMON_SOURCE_FILES = \
i_main.c                                   \
i_system.c           i_system.h            \
m_argv.c             m_argv.h              \
m_misc.c             m_misc.h

GAME_SOURCE_FILES = \
d_event.c            d_event.h             \
                     doomkeys.h            \
                     doomfeatures.h        \
                     doomtype.h            \
d_iwad.c             d_iwad.h              \
d_loop.c             d_loop.h              \
d_mode.c             d_mode.h              \
                     d_ticcmd.h            \
deh_str.c            deh_str.h             \
i_cdmus.c            i_cdmus.h             \
i_endoom.c           i_endoom.h            \
i_joystick.c         i_joystick.h          \
i_scale.c            i_scale.h             \
                     i_swap.h              \
i_sound.c            i_sound.h             \
i_timer.c            i_timer.h             \
i_video.c            i_video.h             \
i_videohr.c          i_videohr.h           \
m_bbox.c             m_bbox.h              \
m_cheat.c            m_cheat.h             \
m_config.c           m_config.h            \
m_controls.c         m_controls.h          \
m_fixed.c            m_fixed.h             \
sha1.c               sha1.h                \
memio.c              memio.h               \
tables.c             tables.h              \
v_video.c            v_video.h             \
                     v_patch.h             \
w_checksum.c         w_checksum.h          \
w_main.c             w_main.h              \
w_wad.c              w_wad.h               \
w_file.c             w_file.h              \
w_file_stdc.c                              \
w_file_posix.c                             \
w_file_win32.c                             \
z_zone.c             z_zone.h


# source files needed for FEATURE_DEHACKED
FEATURE_DEHACKED_SOURCE_FILES = \
deh_defs.h                                 \
deh_io.c             deh_io.h              \
deh_main.c           deh_main.h            \
deh_mapping.c        deh_mapping.h         \
deh_text.c


# source files needed for FEATURE_MULTIPLAYER
FEATURE_MULTIPLAYER_SOURCE_FILES = \
aes_prng.c           aes_prng.h            \
net_client.c         net_client.h          \
net_common.c         net_common.h          \
net_dedicated.c      net_dedicated.h       \
net_defs.h                                 \
net_gui.c            net_gui.h             \
net_io.c             net_io.h              \
net_loop.c           net_loop.h            \
net_packet.c         net_packet.h          \
net_query.c          net_query.h           \
net_sdl.c            net_sdl.h             \
net_server.c         net_server.h          \
net_structrw.c       net_structrw.h


# source files needed for FEATURE_WAD_MERGE
FEATURE_WAD_MERGE_SOURCE_FILES = \
w_merge.c            w_merge.h


# source files needed for FEATURE_SOUND
FEATURE_SOUND_SOURCE_FILES = \
gusconf.c            gusconf.h             \
i_pcsound.c                                \
i_sdlsound.c                               \
i_sdlmusic.c                               \
i_oplmusic.c                               \
midifile.c           midifile.h            \
mus2mid.c            mus2mid.h


SETUP_FILES = \
deh_str.c            deh_str.h             \
d_mode.c             d_mode.h              \
d_iwad.c             d_iwad.h              \
i_timer.c            i_timer.h             \
m_config.c           m_config.h            \
m_controls.c         m_controls.h          \
net_io.c             net_io.h              \
net_packet.c         net_packet.h          \
net_sdl.c            net_sdl.h             \
net_query.c          net_query.h           \
net_structrw.c       net_structrw.h        \
z_native.c           z_zone.h

LOCAL_SRC_FILES = $(ANDROID_FILES) $(COMMON_SOURCE_FILES) $(GAME_SOURCE_FILES) \
	$(FEATURE_MULTIPLAYER_SOURCE_FILES) $(FEATURE_WAD_MERGE_SOURCE_FILES) $(FEATURE_SOUND_SOURCE_FILES) \
 	#$(FEATURE_DEHACKED_SOURCE_FILES)
 
ifeq ($(G),DOOM)
LOCAL_STATIC_LIBRARIES += doom
LOCAL_SRC_FILES += $(FEATURE_DEHACKED_SOURCE_FILES)
LOCAL_CFLAGS += -DCHOC_DOOM

else ifeq ($(G),CHOC_HEX)
LOCAL_STATIC_LIBRARIES += hexen
LOCAL_CFLAGS += -DCHOC_HEXEN
ifeq ($(BUILD_SERIAL),1)
LOCAL_CFLAGS += -DANTI_HACK 
endif
else ifeq ($(G),CHOC_HERETIC)
LOCAL_STATIC_LIBRARIES += heretic
LOCAL_SRC_FILES += $(FEATURE_DEHACKED_SOURCE_FILES)
LOCAL_CFLAGS += -DCHOC_HERETIC
else ifeq ($(G),CHOC_STRIFE)
LOCAL_STATIC_LIBRARIES += strife
LOCAL_SRC_FILES += $(FEATURE_DEHACKED_SOURCE_FILES)
LOCAL_CFLAGS += -DCHOC_STRIFE
endif 
 
#LOCAL_SRC_FILES = $(ANDROID_FILES) $(COMMON_SOURCE_FILES) $(SETUP_FILES) 

LOCAL_LDLIBS += -fuse-ld=bfd

LOCAL_STATIC_LIBRARIES += s-setup lz textscreen_choc opl_choc pcsound_choc  SDL_net libjpeg  
LOCAL_SHARED_LIBRARIES := touchcontrols SDL SDL_mixer SDL_image

include $(BUILD_SHARED_LIBRARY)


#Always build setup!
include $(CLEAR_VARS)

LOCAL_MODULE    := choc-setup

LOCAL_CFLAGS :=  -O2 -g -Wall -Wdeclaration-after-statement  -D_GNU_SOURCE=1 -D_REENTRANT

LOCAL_LDLIBS += -lGLESv1_CM -lEGL

LOCAL_LDLIBS += -llog

LOCAL_CFLAGS += -DCHOC_SETUP

LOCAL_C_INCLUDES :=  $(TOP_DIR)/SDL/include $(TOP_DIR)/SDL_net/include $(TOP_DIR)/SDL_mixer $(TOP_DIR)/MobileTouchControls $(TOP_DIR)/
LOCAL_C_INCLUDES +=   $(LOCAL_PATH)/../textscreen $(LOCAL_PATH)/../pcsound $(LOCAL_PATH)/../opl
 
LOCAL_SRC_FILES = $(ANDROID_FILES) $(COMMON_SOURCE_FILES) $(SETUP_FILES)

	
LOCAL_LDLIBS += -fuse-ld=bfd
	
LOCAL_STATIC_LIBRARIES :=  s-setup z setup_choc textscreen_choc opl_choc pcsound_choc  SDL_net libjpeg
LOCAL_SHARED_LIBRARIES := touchcontrols  SDL SDL_mixer SDL_image
include $(BUILD_SHARED_LIBRARY)



