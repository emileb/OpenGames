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

LOCAL_MODULE := q2gamecc

#LOCAL_CFLAGS :=  -Dstricmp=strcasecmp -DREF_HARD_LINKED 
LOCAL_CPPFLAGS :=  -D_linux_ -fexceptions

LOCAL_C_INCLUDES :=   $(LOCAL_PATH)/src/cc_game/include $(LOCAL_PATH)/src/cc_game/shared

LOCAL_SRC_FILES := src/cc_game/shared/MathLib.cpp

LOCAL_SRC_FILES := src/cc_game/source/Game/Game.cpp


include $(BUILD_SHARED_LIBRARY)



include 
