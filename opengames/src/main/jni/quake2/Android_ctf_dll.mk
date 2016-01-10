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

LOCAL_MODULE := q2gamectf

LOCAL_CFLAGS :=  -Dstricmp=strcasecmp -DREF_HARD_LINKED 
LOCAL_CPPFLAGS :=  $(LOCAL_CFLAGS)

LOCAL_C_INCLUDES :=  $(TOP_DIR)/TouchControls  $(TOP_DIR)/TinyXML $(TOP_DIR) $(LOCAL_PATH)/client $(LOCAL_PATH)/qclib $(LOCAL_PATH)/botlib $(LOCAL_PATH)/d3d  $(LOCAL_PATH)/server  $(LOCAL_PATH)/sw  $(LOCAL_PATH)/libs/freetype2/include $(LOCAL_PATH)/common  $(LOCAL_PATH)/gl


# Quake  2 source
LOCAL_SRC_FILES = \
	src/ctf/g_ai.c \
	src/ctf/g_chase.c \
	src/ctf/g_cmds.c \
	src/ctf/g_ctf.c \
	src/ctf/g_svcmds.c \
	src/ctf/g_combat.c \
	src/ctf/g_func.c \
	src/ctf/g_items.c \
	src/ctf/g_main.c \
	src/ctf/g_monster.c \
	src/ctf/g_misc.c \
	src/ctf/g_phys.c \
	src/ctf/g_spawn.c \
	src/ctf/g_target.c \
	src/ctf/g_trigger.c \
	src/ctf/g_utils.c \
	src/ctf/g_weapon.c \
	src/ctf/m_move.c \
	src/ctf/p_client.c \
	src/ctf/p_hud.c \
	src/ctf/p_trail.c \
	src/ctf/p_menu.c \
	src/ctf/p_view.c \
	src/ctf/p_weapon.c \
	src/ctf/q_shared.c \
	src/ctf/g_save.c \

	#src/ctf/g_save.c \
	#/src/ctf/savegame/savegame.c \
LOCAL_LDLIBS := -lGLESv1_CM -ldl -llog -lOpenSLES -lz
include $(BUILD_SHARED_LIBRARY)



include 
