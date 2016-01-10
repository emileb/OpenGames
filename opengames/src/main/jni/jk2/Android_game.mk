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


LOCAL_MODULE    := jk2game


LOCAL_CPPFLAGS :=    -O2 -Wno-write-strings -Wno-conversion-null -Wno-pointer-arith -Wno-multichar -Wno-int-to-pointer-cast -Wno-enum-compare -Wno-cpp -fexceptions  -fPIC -ffast-math -DLINUX 
LOCAL_CFLAGS =  -DARM  -DAUTOAIM -DFINAL_BUILD  -fvisibility=hidden

LOCAL_LDLIBS += -lGLESv1_CM

LOCAL_LDLIBS += -llog
LOCAL_STATIC_LIBRARIES := sigc  libpng
LOCAL_SHARED_LIBRARIES := touchcontrols openal


LOCAL_C_INCLUDES :=  $(LOCAL_PATH)/client $(LOCAL_PATH)/qclib $(LOCAL_PATH)/botlib $(LOCAL_PATH)/d3d  $(LOCAL_PATH)/server  $(LOCAL_PATH)/sw  $(LOCAL_PATH)/libs/freetype2/include $(LOCAL_PATH)/common  $(LOCAL_PATH)/gl


#############################################################################
# CLIENT/SERVER
#############################################################################

JK2_SRC = \
	game/g_nav.cpp \
	game/g_inventory.cpp \
	game/g_itemLoad.cpp \
	game/g_items.cpp \
	game/g_local.cpp \
	game/g_mem.cpp \
	game/g_misc.cpp \
	game/g_misc_model.cpp \
	game/g_missile.cpp \
	game/g_mover.cpp \
	game/g_navigator.cpp \
	game/g_navnew.cpp \
	game/g_object.cpp \
	game/g_objectives.cpp \
	game/g_ref.cpp \
	game/g_roff.cpp \
	game/g_savegame.cpp \
	game/g_session.cpp \
	game/bg_pangles.cpp \
	game/bg_panimate.cpp \
	game/bg_pmove.cpp \
	game/bg_slidemove.cpp \
	game/g_ICARUS.cpp \
	game/g_active.cpp \
	game/g_breakable.cpp \
	game/g_camera.cpp \
	game/g_client.cpp \
	game/g_cmds.cpp \
	game/g_combat.cpp \
	game/g_functions.cpp \
	game/g_fx.cpp \
	game/g_headers.cpp \
	game/../icarus/BlockStream.cpp \
	game/../icarus/Instance.cpp \
	game/../icarus/Sequence.cpp \
	game/../icarus/Sequencer.cpp \
	game/../icarus/TaskManager.cpp \
	game/../qcommon/tri_coll_test.cpp \
	game/../ui/gameinfo.cpp \
	game/g_spawn.cpp \
	game/g_svcmds.cpp \
	game/g_target.cpp \
	game/g_trigger.cpp \
	game/g_turret.cpp \
	game/g_usable.cpp \
	game/g_utils.cpp \
	game/g_weapon.cpp \
	game/g_weaponLoad.cpp \
	game/genericparser2.cpp \
	game/bg_misc.cpp \
	game/q_math.cpp \
	game/q_shared.cpp \
	game/wp_saber.cpp \
	game/../cgame/cg_consolecmds.cpp \
	game/../cgame/cg_credits.cpp \
	game/../cgame/cg_draw.cpp \
	game/../cgame/cg_drawtools.cpp \
	game/../cgame/cg_effects.cpp \
	game/../cgame/cg_ents.cpp \
	game/../cgame/cg_event.cpp \
	game/../cgame/cg_headers.cpp \
	game/../cgame/cg_info.cpp \
	game/../cgame/cg_lights.cpp \
	game/../cgame/cg_localents.cpp \
	game/../cgame/cg_main.cpp \
	game/../cgame/cg_marks.cpp \
	game/../cgame/cg_players.cpp \
	game/../cgame/cg_playerstate.cpp \
	game/../cgame/cg_predict.cpp \
	game/../cgame/cg_scoreboard.cpp \
	game/../cgame/cg_servercmds.cpp \
	game/../cgame/cg_snapshot.cpp \
	game/../cgame/cg_syscalls.cpp \
	game/../cgame/FxParsing.cpp \
	game/../cgame/FX_ATSTMain.cpp \
	game/../cgame/FX_Blaster.cpp \
	game/../cgame/FX_Bowcaster.cpp \
	game/../cgame/FX_BryarPistol.cpp \
	game/../cgame/FX_DEMP2.cpp \
	game/../cgame/FX_Disruptor.cpp \
	game/../cgame/FX_Emplaced.cpp \
	game/../cgame/FX_Flechette.cpp \
	game/../cgame/FX_HeavyRepeater.cpp \
	game/../cgame/FX_RocketLauncher.cpp \
	game/../cgame/cg_text.cpp \
	game/../cgame/FxPrimitives.cpp \
	game/../cgame/FxScheduler.cpp \
	game/../cgame/FxSystem.cpp \
	game/../cgame/FxTemplate.cpp \
	game/../cgame/FxUtil.cpp \
	game/../cgame/cg_camera.cpp \
	game/NPC_sounds.cpp \
	game/AI_Sniper.cpp \
	game/AI_Stormtrooper.cpp \
	game/AI_Utils.cpp \
	game/G_Timer.cpp \
	game/NPC.cpp \
	game/NPC_behavior.cpp \
	game/NPC_combat.cpp \
	game/NPC_goal.cpp \
	game/NPC_misc.cpp \
	game/NPC_move.cpp \
	game/NPC_reactions.cpp \
	game/NPC_senses.cpp \
	game/AI_Sentry.cpp \
	game/NPC_spawn.cpp \
	game/NPC_stats.cpp \
	game/NPC_utils.cpp \
	game/Q3_Interface.cpp \
	game/Q3_Registers.cpp \
	game/AI_GalakMech.cpp \
	game/../cgame/cg_view.cpp \
	game/../cgame/cg_weapons.cpp \
	game/AI_Atst.cpp \
	game/AI_Default.cpp \
	game/AI_Droid.cpp \
	game/AI_Grenadier.cpp \
	game/AI_Howler.cpp \
	game/AI_ImperialProbe.cpp \
	game/AI_Interrogator.cpp \
	game/AI_Jedi.cpp \
	game/AI_Mark1.cpp \
	game/AI_Mark2.cpp \
	game/AI_MineMonster.cpp \
	game/AI_Remote.cpp \
	game/AI_Seeker.cpp \
	game/g_main.cpp



LOCAL_SRC_FILES += $(JK2_SRC) 


include $(BUILD_SHARED_LIBRARY)








