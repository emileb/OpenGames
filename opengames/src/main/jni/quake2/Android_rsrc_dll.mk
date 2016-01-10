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

LOCAL_MODULE := q2gamerogue

LOCAL_CFLAGS :=  -Dstricmp=strcasecmp -DREF_HARD_LINKED 
LOCAL_CPPFLAGS :=  $(LOCAL_CFLAGS)

LOCAL_C_INCLUDES :=  $(TOP_DIR)/TouchControls  $(TOP_DIR)/TinyXML $(TOP_DIR) $(LOCAL_PATH)/client $(LOCAL_PATH)/qclib $(LOCAL_PATH)/botlib $(LOCAL_PATH)/d3d  $(LOCAL_PATH)/server  $(LOCAL_PATH)/sw  $(LOCAL_PATH)/libs/freetype2/include $(LOCAL_PATH)/common  $(LOCAL_PATH)/gl


# Quake  2 source
LOCAL_SRC_FILES = \
	src/rsrc/dm_ball.c \
	src/rsrc/dm_tag.c \
	src/rsrc/g_ai.c \
	src/rsrc/g_chase.c \
	src/rsrc/g_cmds.c \
	src/rsrc/g_svcmds.c \
	src/rsrc/g_combat.c \
	src/rsrc/g_func.c \
	src/rsrc/g_items.c \
	src/rsrc/g_main.c \
	src/rsrc/g_misc.c \
	src/rsrc/g_monster.c \
	src/rsrc/g_newai.c \
	src/rsrc/g_newdm.c \
	src/rsrc/g_newfnc.c \
	src/rsrc/g_newtarg.c \
	src/rsrc/g_newtrig.c \
	src/rsrc/g_newweap.c \
	src/rsrc/g_phys.c \
	src/rsrc/g_spawn.c \
	src/rsrc/g_sphere.c \
	src/rsrc/g_target.c \
	src/rsrc/g_trigger.c \
	src/rsrc/g_turret.c \
	src/rsrc/g_utils.c \
	src/rsrc/g_weapon.c \
	src/rsrc/m_actor.c \
	src/rsrc/m_berserk.c \
	src/rsrc/m_boss2.c \
	src/rsrc/m_boss3.c \
	src/rsrc/m_boss31.c \
	src/rsrc/m_boss32.c \
	src/rsrc/m_brain.c \
	src/rsrc/m_chick.c \
	src/rsrc/m_carrier.c \
	src/rsrc/m_flipper.c \
	src/rsrc/m_float.c \
	src/rsrc/m_flyer.c \
	src/rsrc/m_gladiator.c \
	src/rsrc/m_gunner.c \
	src/rsrc/m_hover.c \
	src/rsrc/m_infantry.c \
	src/rsrc/m_insane.c \
	src/rsrc/m_medic.c \
	src/rsrc/m_move.c \
	src/rsrc/m_mutant.c \
	src/rsrc/m_parasite.c \
	src/rsrc/m_soldier.c \
	src/rsrc/m_stalker.c \
	src/rsrc/m_supertank.c \
	src/rsrc/m_tank.c \
	src/rsrc/m_turret.c \
	src/rsrc/m_widow.c \
	src/rsrc/m_widow2.c \
	src/rsrc/p_client.c \
	src/rsrc/p_hud.c \
	src/rsrc/p_trail.c \
	src/rsrc/p_view.c \
	src/rsrc/p_weapon.c \
	src/rsrc/q_shared.c \
	src/rsrc/m_flash.c \
	src/rsrc/g_save.c \

	#src/rsrc/g_save.c \
	#/src/rsrc/savegame/savegame.c \
LOCAL_LDLIBS := -lGLESv1_CM -ldl -llog -lOpenSLES -lz
include $(BUILD_SHARED_LIBRARY)



include 
