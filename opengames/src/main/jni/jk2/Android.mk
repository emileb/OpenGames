
LOCAL_PATH := $(call my-dir)


include $(CLEAR_VARS)


LOCAL_MODULE    := jk2


LOCAL_CPPFLAGS := -O2  -Wno-write-strings -Wno-conversion-null -Wno-pointer-arith -Wno-multichar -Wno-int-to-pointer-cast -Wno-enum-compare -Wno-cpp -fexceptions  -ffast-math -fpermissive -DLINUX  -D_JK2EXE -D_M_IX86

LOCAL_CFLAGS = -DHAVE_GLES -DARM -DAUTOAIM -DFINAL_BUILD
LOCAL_LDLIBS += -lGLESv1_CM -lEGL


ifeq ($(ANTI_HACK),1)
	LOCAL_CFLAGS += -DANTI_HACK
endif

LOCAL_LDLIBS += -llog

#Needed so lib can be loaded (_exit error)
LOCAL_LDLIBS += -fuse-ld=bfd


LOCAL_STATIC_LIBRARIES := beloko_tools sigc libpng  #libmy_z
LOCAL_SHARED_LIBRARIES := touchcontrols openal 


LOCAL_C_INCLUDES :=  $(TOP_DIR)/MobileTouchControls   $(TOP_DIR)/zlib/src $(TOP_DIR) $(LOCAL_PATH)/client $(LOCAL_PATH)/qclib $(LOCAL_PATH)/botlib $(LOCAL_PATH)/d3d  $(LOCAL_PATH)/server  $(LOCAL_PATH)/sw  $(LOCAL_PATH)/libs/freetype2/include $(LOCAL_PATH)/common  $(LOCAL_PATH)/gl


#############################################################################
# CLIENT/SERVER
#############################################################################

JK2_SRC = \
	qcommon/common.cpp \
	qcommon/cmd.cpp \
	qcommon/cm_trace.cpp \
	qcommon/cm_test.cpp \
	qcommon/cm_polylib.cpp \
	qcommon/cm_patch.cpp \
	qcommon/cvar.cpp \
	qcommon/cm_load.cpp \
	mp3code/wavep.c \
	mp3code/upsf.c \
	mp3code/uph.c \
	mp3code/towave.c \
	mp3code/msis.c \
	qcommon/unzip.cpp \
	qcommon/strip.cpp \
	qcommon/net_chan.cpp \
	qcommon/msg.cpp \
	qcommon/md4.cpp \
	qcommon/hstring.cpp \
	qcommon/files.cpp \
	jpeg-6/jutils.cpp \
	jpeg-6/jmemnobs.cpp \
	mp3code/cdct.c \
	mp3code/cwinb.c \
	mp3code/mhead.c \
	mp3code/mdct.c \
	mp3code/l3init.c \
	mp3code/l3dq.c \
	mp3code/hwin.c \
	mp3code/cwinm.c \
	renderer/matcomp.c \
	mp3code/cwin.c \
	mp3code/cupl3.c \
	mp3code/cupini.c \
	mp3code/cupL1.c \
	mp3code/cup.c \
	mp3code/csbtb.c \
	mp3code/csbtL3.c \
	mp3code/csbt.c \
	server/sv_main.cpp \
	ui/ui_debug.cpp \
	ui/ui_connect.cpp \
	ui/ui_atoms.cpp \
	server/sv_world.cpp \
	server/sv_snapshot.cpp \
	server/sv_savegame.cpp \
	ui/ui_main.cpp \
	server/sv_init.cpp \
	server/sv_game.cpp \
	server/sv_client.cpp \
	server/sv_ccmds.cpp \
	server/exe_headers.cpp \
	renderer/tr_worldeffects.cpp \
	renderer/tr_world.cpp \
	ui/ui_syscalls.cpp \
	ui/ui_shared.cpp \
	renderer/etc1.cpp \
	renderer/tr_draw.cpp \
	renderer/tr_light.cpp \
	renderer/tr_jpeg_interface.cpp \
	renderer/tr_init.cpp \
	renderer/tr_image.cpp \
	renderer/tr_ghoul2.cpp \
	renderer/tr_font.cpp \
	renderer/tr_main.cpp \
	renderer/tr_curve.cpp \
	renderer/tr_cmds.cpp \
	renderer/tr_bsp.cpp \
	renderer/tr_backend.cpp \
	renderer/tr_animation.cpp \
	renderer/tr_shade.cpp \
	renderer/tr_surfacesprites.cpp \
	renderer/tr_surface.cpp \
	renderer/tr_stl.cpp \
	renderer/tr_sky.cpp \
	renderer/tr_shadows.cpp \
	renderer/tr_shader.cpp \
	renderer/tr_shade_calc.cpp \
	jpeg-6/jmemmgr.cpp \
	renderer/tr_scene.cpp \
	renderer/tr_quicksprite.cpp \
	renderer/tr_noise.cpp \
	renderer/tr_model.cpp \
	renderer/tr_mesh.cpp \
	renderer/tr_marks.cpp \
	encryption/sockets.cpp \
	encryption/cpp_interface.cpp \
	encryption/buffer.cpp \
	client/cl_cin.cpp \
	client/cl_ui.cpp \
	client/cl_scrn.cpp \
	client/cl_parse.cpp \
	client/cl_mp3.cpp \
	client/cl_main.cpp \
	client/cl_keys.cpp \
	client/cl_input.cpp \
	client/cl_console.cpp \
	client/cl_cgame.cpp \
	client/snd_dma.cpp \
	client/vmachine.cpp \
	client/snd_music.cpp \
	client/snd_mix.cpp \
	client/snd_mem.cpp \
	client/snd_ambient.cpp \
	jpeg-6/jcomapi.cpp \
	jpeg-6/jdatadst.cpp \
	jpeg-6/jdapistd.cpp \
	jpeg-6/jdapimin.cpp \
	jpeg-6/jctrans.cpp \
	jpeg-6/jcsample.cpp \
	jpeg-6/jcprepct.cpp \
	jpeg-6/jcphuff.cpp \
	jpeg-6/jcparam.cpp \
	jpeg-6/jdatasrc.cpp \
	jpeg-6/jcmaster.cpp \
	jpeg-6/jcmarker.cpp \
	jpeg-6/jcmainct.cpp \
	jpeg-6/jcinit.cpp \
	jpeg-6/jchuff.cpp \
	jpeg-6/jcdctmgr.cpp \
	jpeg-6/jccolor.cpp \
	jpeg-6/jdmarker.cpp \
	jpeg-6/jidctflt.cpp \
	jpeg-6/jfdctflt.cpp \
	jpeg-6/jerror.cpp \
	jpeg-6/jdtrans.cpp \
	jpeg-6/jdsample.cpp \
	jpeg-6/jdpostct.cpp \
	jpeg-6/jdmaster.cpp \
	jpeg-6/jccoefct.cpp \
	jpeg-6/jdmainct.cpp \
	jpeg-6/jdinput.cpp \
	jpeg-6/jdhuff.cpp \
	jpeg-6/jddctmgr.cpp \
	jpeg-6/jdcolor.cpp \
	jpeg-6/jdcoefct.cpp \
	game/q_shared.cpp \
	game/q_math.cpp \
	game/genericparser2.cpp \
	jpeg-6/jcapimin.cpp \
	ghoul2/G2_surfaces.cpp \
	ghoul2/G2_misc.cpp \
	ghoul2/G2_bones.cpp \
	ghoul2/G2_bolts.cpp \
	ghoul2/G2_API.cpp\
	android/android-jni.cpp \
	android/android_console.cpp \
	android/android_snddma.cpp \
	android/android_main.cpp \
	android/android_glimp.cpp \
	android/in_android.cpp \
	android/gles_qgl.cpp \


LOCAL_SRC_FILES += $(JK2_SRC) 


include $(BUILD_SHARED_LIBRARY)








