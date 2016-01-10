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

LOCAL_C_INCLUDES := $(LOCAL_PATH)  $(TOP_DIR) $(TOP_DIR)/beloko_common $(TOP_DIR)/MobileTouchControls
LOCAL_MODULE := libquake

LOCAL_DISABLE_FORMAT_STRING_CHECKS := true

LOCAL_CFLAGS := -Werror -DANDROID_NDK  -Wno-aggressive-loop-optimizations

ifeq ($(ANTI_HACK),1)
	LOCAL_CFLAGS += -DANTI_HACK
endif

LOCAL_SRC_FILES:= \
  cd_droid.cpp \
  cl_demo.cpp \
  cl_input.cpp \
  cl_main.cpp \
  cl_parse.cpp \
  cl_tent.cpp \
  chase.cpp \
  cmd.cpp \
  common.cpp \
  console.cpp \
  crc.cpp \
  cvar.cpp \
  gl_draw.cpp \
  gl_mesh.cpp \
  gl_model.cpp \
  gl_refrag.cpp \
  gl_rlight.cpp \
  gl_rmain.cpp \
  gl_rmisc.cpp \
  gl_rsurf.cpp \
  gl_screen.cpp \
  gl_vidportable.cpp \
  gl_warp.cpp \
  host.cpp \
  host_cmd.cpp \
  keys.cpp \
  main.cpp \
  PortableMain.cpp \
  AndroidMain.cpp \
  mathlib.cpp \
  menu.cpp \
  net_bsd.cpp \
  net_dgrm.cpp \
  net_loop.cpp \
  net_main.cpp \
  net_vcr.cpp \
  net_udp.cpp \
  nonintel.cpp \
  pr_cmds.cpp \
  pr_edict.cpp \
  pr_exec.cpp \
  r_part.cpp \
  sbar.cpp \
  snd_dma.cpp \
  snd_mem.cpp \
  snd_mix.cpp \
  snd_opensl.cpp \
  sv_main.cpp \
  sv_phys.cpp \
  sv_move.cpp \
  sv_user.cpp \
  sys_portable.cpp \
  view.cpp \
  wad.cpp \
  world.cpp \
  zone.cpp

LOCAL_LDLIBS := -lGLESv1_CM -ldl -llog -lOpenSLES

LOCAL_SHARED_LIBRARIES := touchcontrols
LOCAL_STATIC_LIBRARIES := beloko_tools

include $(BUILD_SHARED_LIBRARY)



include 
