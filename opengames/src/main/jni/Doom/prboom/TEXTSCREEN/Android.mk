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


LOCAL_MODULE    := textscreen


LOCAL_CFLAGS :=   -O2  -DANDROID -DUSE_GLES  -DHAVE_CONFIG_H -g -Wall -Wextra -Wno-missing-field-initializers -Wwrite-strings  -Wbad-function-cast  -Wcast-qual -Wdeclaration-after-statement -Wpointer-arith -ffast-math -Wno-unused -Wno-switch -Wno-sign-compare -Wno-pointer-sign  -fomit-frame-pointer -D_GNU_SOURCE=1 -D_REENTRANT
#-Wcast-align

LOCAL_LDLIBS += -lGLESv1_CM -lEGL

LOCAL_LDLIBS += -llog

LOCAL_CFLAGS += -march=armv7-a -mfloat-abi=softfp
LOCAL_LDLIBS += -Wl,--fix-cortex-a8


LOCAL_C_INCLUDES :=  $(TOP_DIR)/SDL/include  $(TOP_DIR)/prboom/  $(TOP_DIR)/



LOCAL_SRC_FILES =                              \
	textscreen.h                                      \
	txt_checkbox.c           txt_checkbox.h           \
	txt_desktop.c            txt_desktop.h            \
	txt_dropdown.c           txt_dropdown.h           \
	txt_gui.c                txt_gui.h                \
	txt_inputbox.c           txt_inputbox.h           \
	txt_io.c                 txt_io.h                 \
	                         txt_main.h               \
	txt_button.c             txt_button.h             \
	txt_label.c              txt_label.h              \
	txt_radiobutton.c        txt_radiobutton.h        \
        txt_scrollpane.c         txt_scrollpane.h         \
	txt_separator.c          txt_separator.h          \
	txt_spinctrl.c           txt_spinctrl.h           \
	              txt_sdl.h                \
      txt_sdl.c                           txt_smallfont.h          \
	txt_strut.c              txt_strut.h              \
	txt_table.c              txt_table.h              \
	txt_widget.c             txt_widget.h             \
	txt_window.c             txt_window.h             \
	txt_window_action.c      txt_window_action.h      \
	                         txt_font.h               \
	                         doomkeys.h



LOCAL_LDLIBS :=  -ldl -llog

include $(BUILD_STATIC_LIBRARY)








