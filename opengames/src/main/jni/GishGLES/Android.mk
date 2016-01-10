
LOCAL_PATH := $(call my-dir)


include $(CLEAR_VARS)


LOCAL_MODULE    := gish


LOCAL_CFLAGS := -O2  -DUSE_GLES -DPANDORA -DUSE_EGL_SDL
LOCAL_CPPFLAGS := 


LOCAL_LDLIBS +=  -lGLESv1_CM -lEGL -llog -lz
LOCAL_STATIC_LIBRARIES := sigc s-setup libzip libpng  lz 
LOCAL_SHARED_LIBRARIES := touchcontrols openal SDL SDL_mixer libvorbis libogg


LOCAL_C_INCLUDES :=  $(TOP_DIR) $(TOP_DIR)/MobileTouchControls/libpng  $(TOP_DIR)/liboggvorbis/include  $(TOP_DIR)/MobileTouchControls $(TOP_DIR)/openal/include  $(TOP_DIR)/SDL/include $(TOP_DIR)/SDL_net/include $(TOP_DIR)/SDL_mixer



LOCAL_SRC_FILES += \
	android/android-jni.cpp \
	android/in_android.cpp \
		audio/audio.c \
       game/ai.c \
       game/animation.c \
       game/block.c \
       game/boss.c \
       game/config.c \
       game/credits.c \
       game/custom.c \
       game/damage.c \
       game/debug.c \
       game/editor.c \
       game/gameaudio.c \
       game/game.c \
       game/gamemenu.c \
       game/gameobject.c \
       game/gametexture.c \
       game/high.c \
       game/level.c \
       game/lighting.c \
       game/logic.c \
       game/mainmenu.c \
       game/mappack.c \
       game/music.c \
       game/objedit.c \
       game/objfunc.c \
       game/options.c \
       game/physics.c \
       game/player.c \
       game/prerender.c \
       game/random.c \
       game/record.c \
       game/render.c \
       game/replay.c \
       game/ropeedit.c \
       game/setup.c \
       game/socket.c \
       game/sprite.c \
       game/vsmode.c \
       input/joystick.c \
       input/keyboard.c \
       input/mouse.c \
       main.c \
       math/intersec.c \
       math/vector.c \
       menu/menu.c \
       parser/parser.c \
       physics/bond.c \
       physics/object.c \
       physics/particle.c \
       sdl/endian.c \
       sdl/event.c \
       sdl/file.c \
       sdl/video.c \
       video/glfunc.c \
       video/opengl.c \
       video/text.c \
       video/texture.c \
      egl/androidport.c


include $(BUILD_SHARED_LIBRARY)








