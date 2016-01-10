
TOP_DIR := $(call my-dir)

LOCAL_PATH := $(call my-dir)


ifeq ($(G),WOLF)
    #include $(TOP_DIR)/ecwolf_libs/Android.mk
    include $(TOP_DIR)/ecwolf-1.3.2-src/Android.mk
    #BUILD_SDL := 1
    include $(TOP_DIR)/SDL/Android.mk
    include $(TOP_DIR)/SDL_mixer/Android.mk
    include $(TOP_DIR)/jpeg8d/Android.mk
    #include $(TOP_DIR)/SDL2_mixer/Android.mk
else ifeq ($(G),LEM)
    BUILD_SDL := 1
    include $(TOP_DIR)/lemmings/Android.mk
else ifeq ($(G),RTCW)
    #RTCW
    include $(TOP_DIR)/rtcw/Android.mk
else ifeq ($(G),GISH)
    #Gish
    include $(TOP_DIR)/GishGLES/Android.mk
    include $(TOP_DIR)/openal/Android.mk
    #include $(TOP_DIR)/OpenAL-Soft/Android.mk
    include $(TOP_DIR)/liboggvorbis/Android.mk
    BUILD_SDL := 1
else ifeq ($(G),JK2)
    #JK2
    include $(TOP_DIR)/openal/Android.mk
    include $(TOP_DIR)/jk2/Android.mk
    include $(TOP_DIR)/jk2/Android_game.mk
else ifeq ($(G),JK3)
    #JK3
    include $(TOP_DIR)/openal/Android.mk
    include $(TOP_DIR)/jk3/Android.mk
    include $(TOP_DIR)/jk3/Android_game.mk

    #JK3 MP
    include $(TOP_DIR)/OpenJK/Android.mk
    include $(TOP_DIR)/SDL2/Android.mk
    include $(TOP_DIR)/jpeg8d/Android.mk

else ifeq ($(G),Q1)
    include $(TOP_DIR)/Quake/WinQuake/Android.mk
    include $(TOP_DIR)/Quake/darkplaces_latest/Android.mk
    include $(TOP_DIR)/Quake/fteqw/Android.mk
    include $(TOP_DIR)/snprintf_portable/Android.mk
    BUILD_GLES2_TC := 1
else ifeq ($(G),Q2)
    #Quake 2
    include $(TOP_DIR)/quake2/Android.mk
    include $(TOP_DIR)/quake2/Android_ctf_dll.mk
    include $(TOP_DIR)/quake2/Android_crbot_dll.mk
    include $(TOP_DIR)/quake2/Android_xsrc_dll.mk
    include $(TOP_DIR)/quake2/Android_rsrc_dll.mk
    include $(TOP_DIR)/quake2/Android_dll.mk
else ifeq ($(G),Q3)
    #Q3
    include $(TOP_DIR)/ioq3/Android.mk
    include $(TOP_DIR)/openal/Android.mk
    BUILD_SDL := 1
else ifeq ($(G),BS)
    #Blake Stone
    include $(TOP_DIR)/bstone/Android.mk
    include $(TOP_DIR)/SDL2_net/Android.mk
    include $(TOP_DIR)/SDL2_mixer/Android.mk
    include $(TOP_DIR)/SDL2_image/Android.mk
    include $(TOP_DIR)/SDL2/Android.mk
    BUILD_GLES2_TC := 1
else ifeq ($(G),DOOM)
    include $(TOP_DIR)/GL/Android.mk
    include $(TOP_DIR)/Doom/prboom/MUSIC/Android.mk
    include $(TOP_DIR)/Doom/prboom/SDL/Android.mk
    include $(TOP_DIR)/Doom/prboom/TEXTSCREEN/Android.mk
    include $(TOP_DIR)/Doom/prboom/PCSOUND/Android.mk
    include $(TOP_DIR)/Doom/prboom/Android.mk

    #include $(TOP_DIR)/Doom/crispy-doom-crispy-doom-1.0/Android.mk
    include $(TOP_DIR)/Doom/chocolate-doom-2.2.0/Android.mk

    include $(TOP_DIR)/Doom/gzdoom-g1.8.6/Android.mk
    include $(TOP_DIR)/Doom/gzdoom_2/Android.mk
    #include $(TOP_DIR)/Doom/gzdoom/Android.mk

    include $(TOP_DIR)/FMOD_studio/Android.mk
    include $(TOP_DIR)/fluidsynth/Android.mk
    include $(TOP_DIR)/openal/Android.mk
    BUILD_SDL := 1
else ifeq ($(G),ABUSE)
    include $(TOP_DIR)/abuse-0.8/src/Android.mk
    BUILD_SDL := 1
endif


include $(TOP_DIR)/MobileTouchControls/Android.mk

ifeq ($(ANTI_HACK),1)
include $(TOP_DIR)/beloko_common/Android.mk
endif

ifeq ($(BUILD_SDL),1)
#Common SDL
include $(TOP_DIR)/SDL/Android.mk
include $(TOP_DIR)/SDL_net/Android.mk
include $(TOP_DIR)/SDL_mixer/Android.mk
include $(TOP_DIR)/SDL_image/Android.mk
include $(TOP_DIR)/jpeg8d/Android.mk
endif


