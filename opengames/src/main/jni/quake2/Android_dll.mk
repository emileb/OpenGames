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

LOCAL_MODULE := q2game

LOCAL_CFLAGS :=  -Dstricmp=strcasecmp -DREF_HARD_LINKED 
LOCAL_CPPFLAGS :=  $(LOCAL_CFLAGS)

LOCAL_C_INCLUDES :=  $(TOP_DIR)/TouchControls  $(TOP_DIR)/TinyXML $(TOP_DIR) $(LOCAL_PATH)/client $(LOCAL_PATH)/qclib $(LOCAL_PATH)/botlib $(LOCAL_PATH)/d3d  $(LOCAL_PATH)/server  $(LOCAL_PATH)/sw  $(LOCAL_PATH)/libs/freetype2/include $(LOCAL_PATH)/common  $(LOCAL_PATH)/gl


# Quake  2 source
LOCAL_SRC_FILES = \
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
	#/src/game/savegame/savegame.c \
LOCAL_LDLIBS := -lGLESv1_CM -ldl -llog -lOpenSLES -lz
include $(BUILD_SHARED_LIBRARY)



include 
