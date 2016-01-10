# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.crg/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)



include $(CLEAR_VARS)

# from clear-vars.mk :
#NDK_LOCAL_VARS := \
#  LOCAL_MODULE \
#  LOCAL_SRC_FILES \
#  LOCAL_CFLAGS \
#  LOCAL_LDFLAGS \
#  LOCAL_ARFLAGS \
#  LOCAL_CPP_EXTENSION \
#  LOCAL_STATIC_LIBRARIES \
#  LOCAL_STATIC_WHOLE_LIBRARIES \
#  LOCAL_SHARED_LIBRARIES \
#  LOCAL_MAKEFILE \
#  LOCAL_NO_UNDEFINED_SYMBOLS \

LOCAL_MODULE    := quake2
LOCAL_SRC_FILES := src/android/quake2-jni.cpp



LOCAL_CFLAGS :=  -Dstricmp=strcasecmp -DREF_HARD_LINKED  -DHAVE_NEON=1
LOCAL_ARM_NEON := true

ifeq ($(ANTI_HACK),1)
	LOCAL_CFLAGS += -DANTI_HACK
endif


LOCAL_LDLIBS += -lGLESv1_CM
LOCAL_LDLIBS += -llog

LOCAL_LDLIBS += -fuse-ld=bfd


LOCAL_STATIC_LIBRARIES := beloko_tools  sigc libzip libpng
LOCAL_SHARED_LIBRARIES := touchcontrols 

LOCAL_CFLAGS += -fvisibility=hidden 

LOCAL_C_INCLUDES :=   $(TOP_DIR)/../../Serial/jni  $(TOP_DIR)/MobileTouchControls  $(TOP_DIR) $(LOCAL_PATH)/client $(LOCAL_PATH)/qclib $(LOCAL_PATH)/botlib $(LOCAL_PATH)/d3d  $(LOCAL_PATH)/server  $(LOCAL_PATH)/sw  $(LOCAL_PATH)/libs/freetype2/include $(LOCAL_PATH)/common  $(LOCAL_PATH)/gl


#############################################################################
# CLIENT/SERVER
#############################################################################

QUAKE2_SRC = \
	src/client/cl_cin.c \
	src/client/cl_ents.c \
	src/client/cl_fx.c \
	src/client/cl_input.c \
	src/client/cl_inv.c \
	src/client/cl_main.c \
	src/client/cl_parse.c \
	src/client/cl_pred.c \
	src/client/cl_tent.c \
	src/client/cl_scrn.c \
	src/client/cl_view.c \
	src/client/cl_newfx.c \
	src/client/console.c \
	src/client/keys.c \
	src/client/menu.c \
	src/client/snd_dma.c \
	src/client/snd_mem.c \
	src/client/snd_mix.c \
	src/client/qmenu.c \
	\
	src/qcommon/cmd.c \
	src/qcommon/cmodel.c \
	src/qcommon/common.c \
	src/qcommon/crc.c \
	src/qcommon/cvar.c \
	src/qcommon/files.c \
	src/qcommon/md4.c \
	src/qcommon/net_chan.c \
	\
	src/server/sv_ccmds.c \
	src/server/sv_ents.c \
	src/server/sv_game.c \
	src/server/sv_init.c \
	src/server/sv_main.c \
	src/server/sv_send.c \
	src/server/sv_user.c \
	src/server/sv_world.c \
	\
	src/linux/q_shlinux.c \
	src/linux/vid_menu.c \
	src/android/vid_so.c \
	src/android/sys_android.c \
	src/linux/glob.c \
	src/linux/net_udp.c \
	\
	src/qcommon/pmove.c \
	\
	src/android/cd_droid.c
	
#	src/null/snddma_null.c 
#	src/game/m_flash.c \
#	src/game/q_shared.c \

#############################################################################
# GAME
#############################################################################

GAME_SRC = \
	src/game/g_ai.c \
	src/game/p_client.c \
	src/game/g_chase.c \
	src/game/g_cmds.c \
	src/game/g_svcmds.c \
	src/game/g_combat.c \
	src/game/g_func.c \
	src/game/g_items.c \
	src/game/g_main.c \
	src/game/g_misc.c \
	src/game/g_monster.c \
	src/game/g_phys.c \
	src/game/g_spawn.c \
	src/game/g_target.c \
	src/game/g_trigger.c \
	src/game/g_turret.c \
	src/game/g_utils.c \
	src/game/g_weapon.c \
	src/game/m_actor.c \
	src/game/m_berserk.c \
	src/game/m_boss2.c \
	src/game/m_boss3.c \
	src/game/m_boss31.c \
	src/game/m_boss32.c \
	src/game/m_brain.c \
	src/game/m_chick.c \
	src/game/m_flipper.c \
	src/game/m_float.c \
	src/game/m_flyer.c \
	src/game/m_gladiator.c \
	src/game/m_gunner.c \
	src/game/m_hover.c \
	src/game/m_infantry.c \
	src/game/m_insane.c \
	src/game/m_medic.c \
	src/game/m_move.c \
	src/game/m_mutant.c \
	src/game/m_parasite.c \
	src/game/m_soldier.c \
	src/game/m_supertank.c \
	src/game/m_tank.c \
	src/game/p_hud.c \
	src/game/p_trail.c \
	src/game/p_view.c \
	src/game/p_weapon.c \
	src/game/q_shared.c \
	src/game/m_flash.c \
	src/game/g_save.c \

	#src/game/g_save.c \
	#src/game/savegame/savegame.c \
#############################################################################
# REF_GL
#############################################################################

REF_GL_SRC = \
	src/ref_gl/gl_draw.c \
	src/ref_gl/gl_image.c \
	src/ref_gl/gl_light.c \
	src/ref_gl/gl_mesh.c \
	src/ref_gl/gl_model.c \
	src/ref_gl/gl_rmain.c \
	src/ref_gl/gl_rmisc.c \
	src/ref_gl/gl_rsurf.c \
	src/ref_gl/gl_warp.c \
	\
	src/android/rw_in_android.c \
	src/android/glimp_android.c \
	src/android/qgl_android.c \
	src/android/nanoWrap.c 

#	src/android/qgl_android.c \
#	src/linux/q_shlinux.c \
#	src/linux/glob.c \
# 	src/linux/qgl_linux.c \
#	src/game/q_shared.c \


	
#LOCAL_SRC_FILES += $(GAME_SRC) 
LOCAL_SRC_FILES += src/game/q_shared.c 
LOCAL_SRC_FILES += src/game/m_flash.c 

LOCAL_SRC_FILES += $(QUAKE2_SRC) 

LOCAL_SRC_FILES += $(REF_GL_SRC) 

LOCAL_SRC_FILES += src/android/snd_android.c


include $(BUILD_SHARED_LIBRARY)








