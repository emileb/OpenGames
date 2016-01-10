# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := q2gamexatrix

LOCAL_CFLAGS :=  -Dstricmp=strcasecmp -DREF_HARD_LINKED 
LOCAL_CPPFLAGS :=  $(LOCAL_CFLAGS)

LOCAL_C_INCLUDES :=  $(TOP_DIR)/TouchControls  $(TOP_DIR)/TinyXML $(TOP_DIR) $(LOCAL_PATH)/client $(LOCAL_PATH)/qclib $(LOCAL_PATH)/botlib $(LOCAL_PATH)/d3d  $(LOCAL_PATH)/server  $(LOCAL_PATH)/sw  $(LOCAL_PATH)/libs/freetype2/include $(LOCAL_PATH)/common  $(LOCAL_PATH)/gl


# Quake  2 source
LOCAL_SRC_FILES = \
	src/xsrc/g_ai.c \
	src/xsrc/p_client.c \
	src/xsrc/g_chase.c \
	src/xsrc/g_cmds.c \
	src/xsrc/g_svcmds.c \
	src/xsrc/g_combat.c \
	src/xsrc/g_func.c \
	src/xsrc/g_items.c \
	src/xsrc/g_main.c \
	src/xsrc/g_misc.c \
	src/xsrc/g_monster.c \
	src/xsrc/g_phys.c \
	src/xsrc/g_spawn.c \
	src/xsrc/g_target.c \
	src/xsrc/g_trigger.c \
	src/xsrc/g_turret.c \
	src/xsrc/g_utils.c \
	src/xsrc/g_weapon.c \
	src/xsrc/m_actor.c \
	src/xsrc/m_berserk.c \
	src/xsrc/m_boss2.c \
	src/xsrc/m_boss3.c \
	src/xsrc/m_boss31.c \
	src/xsrc/m_boss32.c \
	src/xsrc/m_boss5.c \
	src/xsrc/m_brain.c \
	src/xsrc/m_chick.c \
	src/xsrc/m_flipper.c \
	src/xsrc/m_float.c \
	src/xsrc/m_flyer.c \
	src/xsrc/m_gladiator.c \
	src/xsrc/m_gladb.c \
	src/xsrc/m_gekk.c \
	src/xsrc/m_gunner.c \
	src/xsrc/m_hover.c \
	src/xsrc/m_infantry.c \
	src/xsrc/m_insane.c \
	src/xsrc/m_medic.c \
	src/xsrc/m_move.c \
	src/xsrc/m_mutant.c \
	src/xsrc/m_parasite.c \
	src/xsrc/m_soldier.c \
	src/xsrc/m_supertank.c \
	src/xsrc/m_tank.c \
	src/xsrc/m_fixbot.c \
	src/xsrc/p_hud.c \
	src/xsrc/p_trail.c \
	src/xsrc/p_view.c \
	src/xsrc/p_weapon.c \
	src/xsrc/q_shared.c \
	src/xsrc/m_flash.c \
	src/xsrc/g_save.c \

	#src/xsrc/g_save.c \
	#/src/xsrc/savegame/savegame.c \
LOCAL_LDLIBS := -lGLESv1_CM -ldl -llog -lOpenSLES -lz
include $(BUILD_SHARED_LIBRARY)



include 
