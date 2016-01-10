LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := darkplaces

LOCAL_DISABLE_FORMAT_STRING_CHECKS := true

LOCAL_CFLAGS := -DUSE_GLES2 -D__ANDROID__ 
LOCAL_CPPFLAGS := -fpermissive

LOCAL_C_INCLUDES :=  $(TOP_DIR)/snprintf_portable $(TOP_DIR)/beloko_common $(TOP_DIR)/MobileTouchControls  $(LOCAL_PATH)/client $(LOCAL_PATH)/qclib $(LOCAL_PATH)/botlib $(LOCAL_PATH)/d3d  $(LOCAL_PATH)/server  $(LOCAL_PATH)/sw  $(LOCAL_PATH)/libs/freetype2/include $(LOCAL_PATH)/common  $(LOCAL_PATH)/gl

LOCAL_CFLAGS += -march=armv7-a -mfloat-abi=softfp
LOCAL_LDLIBS += -Wl,--fix-cortex-a8



LOCAL_SRC_FILES:= \
	android/android-jni.cpp \
	android/android_interface.cpp \
	android/in_android.c \
	snd_opensl.c \
	snd_main.c \
	snd_mem.c \
	snd_mix.c \
	snd_ogg.c \
	snd_wav.c \
	snd_modplug.c \
	cd_droid.c \
	sys_android.c \
	vid_android.c \
	thread_pthread.c \
	builddate.c \
	bih.c \
	cap_avi.c \
	cap_ogg.c \
	cd_shared.c \
	crypto.c \
	cl_collision.c \
	cl_demo.c \
	cl_dyntexture.c \
	cl_input.c \
	cl_main.c \
	cl_parse.c \
	cl_particles.c \
	cl_screen.c \
	cl_video.c \
	clvm_cmds.c \
	cmd.c \
	collision.c \
	common.c \
	console.c \
	csprogs.c \
	curves.c \
	cvar.c \
	dpsoftrast.c \
	dpvsimpledecode.c \
	filematch.c \
	fractalnoise.c \
	fs.c \
	ft2.c \
	utf8lib.c \
	gl_backend.c \
	gl_draw.c \
	gl_rmain.c \
	gl_rsurf.c \
	gl_textures.c \
	hmac.c \
	host.c \
	host_cmd.c \
	image.c \
	image_png.c \
	jpeg.c \
	keys.c \
	lhnet.c \
	libcurl.c \
	mathlib.c \
	matrixlib.c \
	mdfour.c \
	menu.c \
	meshqueue.c \
	mod_skeletal_animatevertices_sse.c \
	mod_skeletal_animatevertices_generic.c \
	model_alias.c \
	model_brush.c \
	model_shared.c \
	model_sprite.c \
	mvm_cmds.c \
	netconn.c \
	palette.c \
	polygon.c \
	portals.c \
	protocol.c \
	prvm_cmds.c \
	prvm_edict.c \
	prvm_exec.c \
	r_explosion.c \
	r_lerpanim.c \
	r_lightning.c \
	r_modules.c \
	r_shadow.c \
	r_sky.c \
	r_sprites.c \
	sbar.c \
	sv_demo.c \
	sv_main.c \
	sv_move.c \
	sv_phys.c \
	sv_user.c \
	svbsp.c \
	svvm_cmds.c \
	sys_shared.c \
	vid_shared.c \
	view.c \
	wad.c \
	world.c \
	zone.c

	

LOCAL_LDLIBS := -lGLESv2 -ldl -llog -lOpenSLES -lz
LOCAL_STATIC_LIBRARIES := sigc libzip libpng  snprintf_portable
LOCAL_SHARED_LIBRARIES := touchcontrols_gles2

include $(BUILD_SHARED_LIBRARY)



include 
