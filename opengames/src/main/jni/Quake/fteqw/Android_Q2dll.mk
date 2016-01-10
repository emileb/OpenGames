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

LOCAL_CFLAGS :=  -DANDROID -DANDROID_NDK -DGLQUAKE -Wno-write-strings -O0
LOCAL_CPPFLAGS :=  $(LOCAL_CFLAGS)

LOCAL_C_INCLUDES :=  $(TOP_DIR)/TouchControls  $(TOP_DIR)/TinyXML $(TOP_DIR) $(LOCAL_PATH)/client $(LOCAL_PATH)/qclib $(LOCAL_PATH)/botlib $(LOCAL_PATH)/d3d  $(LOCAL_PATH)/server  $(LOCAL_PATH)/sw  $(LOCAL_PATH)/libs/freetype2/include $(LOCAL_PATH)/common  $(LOCAL_PATH)/gl

# Quake  2 source
LOCAL_SRC_FILES = \
	game/g_ai.c \
	game/p_client.c \
	game/g_chase.c \
	game/g_cmds.c \
	game/g_svcmds.c \
	game/g_combat.c \
	game/g_func.c \
	game/g_items.c \
	game/g_main.c \
	game/g_misc.c \
	game/g_monster.c \
	game/g_phys.c \
	game/g_save.c \
	game/g_spawn.c \
	game/g_target.c \
	game/g_trigger.c \
	game/g_turret.c \
	game/g_utils.c \
	game/g_weapon.c \
	game/m_actor.c \
	game/m_berserk.c \
	game/m_boss2.c \
	game/m_boss3.c \
	game/m_boss31.c \
	game/m_boss32.c \
	game/m_brain.c \
	game/m_chick.c \
	game/m_flipper.c \
	game/m_float.c \
	game/m_flyer.c \
	game/m_gladiator.c \
	game/m_gunner.c \
	game/m_hover.c \
	game/m_infantry.c \
	game/m_insane.c \
	game/m_medic.c \
	game/m_move.c \
	game/m_mutant.c \
	game/m_parasite.c \
	game/m_soldier.c \
	game/m_supertank.c \
	game/m_tank.c \
	game/p_hud.c \
	game/p_trail.c \
	game/p_view.c \
	game/p_weapon.c \
	game/q_shared.c \
	game/m_flash.c

#	game/q_shared.c \

LOCAL_LDLIBS := -lGLESv1_CM -ldl -llog -lOpenSLES -lz
include $(BUILD_SHARED_LIBRARY)



include 
