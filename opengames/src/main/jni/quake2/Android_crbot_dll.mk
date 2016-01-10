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

LOCAL_MODULE := q2gamecrbot

LOCAL_CFLAGS :=  -Dstricmp=strcasecmp -DREF_HARD_LINKED 
LOCAL_CPPFLAGS :=  $(LOCAL_CFLAGS)
LOCAL_C_INCLUDES :=  $(TOP_DIR)/TouchControls  $(TOP_DIR)/TinyXML $(TOP_DIR) $(LOCAL_PATH)/client $(LOCAL_PATH)/qclib $(LOCAL_PATH)/botlib $(LOCAL_PATH)/d3d  $(LOCAL_PATH)/server  $(LOCAL_PATH)/sw  $(LOCAL_PATH)/libs/freetype2/include $(LOCAL_PATH)/common  $(LOCAL_PATH)/gl


# Quake  2 source
LOCAL_SRC_FILES = \
	src/crbot/g_ai.c \
	src/crbot/g_chase.c \
	src/crbot/g_cmds.c \
	src/crbot/g_ctf.c \
	src/crbot/g_svcmds.c \
	src/crbot/g_combat.c \
	src/crbot/g_func.c \
	src/crbot/g_items.c \
	src/crbot/g_main.c \
	src/crbot/g_monster.c \
	src/crbot/g_misc.c \
	src/crbot/g_phys.c \
	src/crbot/g_spawn.c \
	src/crbot/g_target.c \
	src/crbot/g_trigger.c \
	src/crbot/g_utils.c \
	src/crbot/g_weapon.c \
	src/crbot/m_move.c \
	src/crbot/p_client.c \
	src/crbot/p_hud.c \
	src/crbot/p_trail.c \
	src/crbot/p_menu.c \
	src/crbot/p_view.c \
	src/crbot/p_weapon.c \
	src/crbot/q_shared.c \
	src/crbot/g_save.c \
	src/crbot/cr_main.c \
	src/crbot/cr_menu.c \

	
include $(BUILD_SHARED_LIBRARY)



include 
