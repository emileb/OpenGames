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

LOCAL_MODULE := q2gamealt

LOCAL_CFLAGS :=  -DANDROID -DANDROID_NDK -DGLQUAKE -Wno-write-strings 
LOCAL_CPPFLAGS :=  $(LOCAL_CFLAGS)

LOCAL_C_INCLUDES :=  $(TOP_DIR)/TouchControls  $(TOP_DIR)/TinyXML $(TOP_DIR) $(LOCAL_PATH)/client $(LOCAL_PATH)/qclib $(LOCAL_PATH)/botlib $(LOCAL_PATH)/d3d  $(LOCAL_PATH)/server  $(LOCAL_PATH)/sw  $(LOCAL_PATH)/libs/freetype2/include $(LOCAL_PATH)/common  $(LOCAL_PATH)/gl

# Quake  2 source
LOCAL_SRC_FILES = \
	src/common/shared/flash.c \
	src/common/shared/rand.c \
	src/common/shared/shared.c \
	src/game/g_ai.c \
	src/game/g_chase.c \
	src/game/g_cmds.c \
	src/game/g_combat.c \
	src/game/g_func.c \
	src/game/g_items.c \
	src/game/g_main.c \
	src/game/g_misc.c \
	src/game/g_monster.c \
	src/game/g_phys.c \
	src/game/g_spawn.c \
	src/game/g_svcmds.c \
	src/game/g_target.c \
	src/game/g_trigger.c \
	src/game/g_turret.c \
	src/game/g_utils.c \
	src/game/g_weapon.c \
	src/game/monster/berserker/berserker.c \
	src/game/monster/boss2/boss2.c \
	src/game/monster/boss3/boss3.c \
	src/game/monster/boss3/boss31.c \
	src/game/monster/boss3/boss32.c \
	src/game/monster/brain/brain.c \
	src/game/monster/chick/chick.c \
	src/game/monster/flipper/flipper.c \
	src/game/monster/float/float.c \
	src/game/monster/flyer/flyer.c \
	src/game/monster/gladiator/gladiator.c \
	src/game/monster/gunner/gunner.c \
	src/game/monster/hover/hover.c \
	src/game/monster/infantry/infantry.c \
	src/game/monster/insane/insane.c \
	src/game/monster/medic/medic.c \
	src/game/monster/misc/move.c \
	src/game/monster/mutant/mutant.c \
	src/game/monster/parasite/parasite.c \
	src/game/monster/soldier/soldier.c \
	src/game/monster/supertank/supertank.c \
	src/game/monster/tank/tank.c \
	src/game/player/client.c \
	src/game/player/hud.c \
	src/game/player/trail.c \
	src/game/player/view.c \
	src/game/player/weapon.c \
	src/game/savegame/savegame.c

#	game/q_shared.c \

LOCAL_LDLIBS := -lGLESv1_CM -ldl -llog -lOpenSLES -lz
include $(BUILD_SHARED_LIBRARY)



include 
