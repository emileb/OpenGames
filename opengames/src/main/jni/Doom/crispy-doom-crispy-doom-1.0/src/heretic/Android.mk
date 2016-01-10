
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := heretic


LOCAL_CFLAGS :=  -O2 -g -Wall -Wdeclaration-after-statement  -D_GNU_SOURCE=1 -D_REENTRANT
LOCAL_CFLAGS += -DPROGRAM_PREFIX=\"ANDROID\"

LOCAL_LDLIBS += -llog

LOCAL_CFLAGS += -march=armv7-a -mfloat-abi=softfp
LOCAL_LDLIBS += -Wl,--fix-cortex-a8


LOCAL_C_INCLUDES :=  $(TOP_DIR)/SDL/include $(TOP_DIR)/SDL_net/include $(TOP_DIR)/SDL_mixer $(TOP_DIR)/SDL_image  $(TOP_DIR)/libpng $(TOP_DIR)/jpeg8d $(TOP_DIR)/TouchControls  $(TOP_DIR)/TinyXML $(TOP_DIR)/
LOCAL_C_INCLUDES +=  $(LOCAL_PATH)/../ $(LOCAL_PATH)/../../ $(LOCAL_PATH)/../../textscreen

SOURCE_FILES = \
                       am_data.h                     \
am_map.c               am_map.h                      \
ct_chat.c              ct_chat.h                     \
d_main.c                                             \
d_net.c                                              \
                       doomdata.h                    \
                       doomdef.h                     \
                       dstrings.h                    \
f_finale.c                                           \
g_game.c                                             \
info.c                 info.h                        \
in_lude.c                                            \
m_random.c             m_random.h                    \
mn_menu.c                                            \
                       p_action.h                    \
p_ceilng.c                                           \
p_doors.c                                            \
p_enemy.c                                            \
p_floor.c                                            \
p_inter.c                                            \
p_lights.c                                           \
p_local.h                                            \
p_map.c                                              \
p_maputl.c                                           \
p_mobj.c                                             \
p_plats.c                                            \
p_pspr.c                                             \
p_saveg.c                                            \
p_setup.c                                            \
p_sight.c                                            \
p_spec.c               p_spec.h                      \
p_switch.c                                           \
p_telept.c                                           \
p_tick.c                                             \
p_user.c                                             \
r_bsp.c                                              \
r_data.c                                             \
r_draw.c                                             \
                       r_local.h                     \
r_main.c                                             \
r_plane.c                                            \
r_segs.c                                             \
r_things.c                                           \
sb_bar.c                                             \
sounds.c               sounds.h                      \
s_sound.c              s_sound.h                     


FEATURE_DEHACKED_SOURCE_FILES = \
deh_ammo.c                                 \
deh_frame.c                                \
deh_htext.c                                \
deh_htic.c             deh_htic.h          \
deh_sound.c                                \
deh_thing.c                                \
deh_weapon.c


LOCAL_SRC_FILES = $(SOURCE_FILES) $(FEATURE_DEHACKED_SOURCE_FILES)

LOCAL_LDLIBS :=  -ldl  -lz
LOCAL_STATIC_LIBRARIES := SDL_net libjpeg
LOCAL_SHARED_LIBRARIES :=  SDL SDL_mixer SDL_image 

include $(BUILD_STATIC_LIBRARY)






