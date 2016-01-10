
LOCAL_PATH := $(call my-dir)


include $(CLEAR_VARS)


LOCAL_MODULE    := rd-gles_arm


LOCAL_CFLAGS :=  $(JK3_BASE_CFLAGS) -DHAVE_GLES
LOCAL_CPPFLAGS := $(JK3_BASE_CPPFLAGS) 

LOCAL_LDLIBS := $(JK3_BASE_LDLIBS)


LOCAL_LDLIBS +=  -lGLESv1_CM -lEGL -llog -lz
LOCAL_STATIC_LIBRARIES := s-setup libpng libjpeg lz 
LOCAL_SHARED_LIBRARIES := SDL2 jk3mp #jk3mp is onky needed to see the androidSwapped, massive hack

LOCAL_C_INCLUDES :=  $(JK3_BASE_C_INCLUDES) $(TOP_DIR)/libpng $(TOP_DIR)/jpeg8d $(LOCAL_PATH)/$(MPDir)/rd-gles $(TOP_DIR)

JK3_SRC = \
	${MPDir}/rd-gles/G2_API.cpp \
	${MPDir}/rd-gles/G2_bolts.cpp \
	${MPDir}/rd-gles/G2_bones.cpp \
	${MPDir}/rd-gles/G2_misc.cpp \
	${MPDir}/rd-gles/G2_surfaces.cpp \
	${MPDir}/rd-gles/tr_backend.cpp \
	${MPDir}/rd-gles/tr_bsp.cpp \
	${MPDir}/rd-gles/tr_cmds.cpp \
	${MPDir}/rd-gles/tr_curve.cpp \
	${MPDir}/rd-gles/tr_decals.cpp \
	${MPDir}/rd-gles/tr_ghoul2.cpp \
	${MPDir}/rd-gles/tr_image.cpp \
	${MPDir}/rd-gles/tr_init.cpp \
	${MPDir}/rd-gles/tr_light.cpp \
	${MPDir}/rd-gles/tr_main.cpp \
	${MPDir}/rd-gles/tr_marks.cpp \
	${MPDir}/rd-gles/tr_mesh.cpp \
	${MPDir}/rd-gles/tr_model.cpp \
	${MPDir}/rd-gles/tr_quicksprite.cpp \
	${MPDir}/rd-gles/tr_scene.cpp \
	${MPDir}/rd-gles/tr_shade.cpp \
	${MPDir}/rd-gles/tr_shade_calc.cpp \
	${MPDir}/rd-gles/tr_shader.cpp \
	${MPDir}/rd-gles/tr_shadows.cpp \
	${MPDir}/rd-gles/tr_skin.cpp \
	${MPDir}/rd-gles/tr_sky.cpp \
	${MPDir}/rd-gles/tr_subs.cpp \
	${MPDir}/rd-gles/tr_surface.cpp \
	${MPDir}/rd-gles/tr_surfacesprites.cpp \
	${MPDir}/rd-gles/tr_world.cpp \
	${MPDir}/rd-gles/tr_WorldEffects.cpp \
	${MPDir}/ghoul2/G2_gore.cpp \
	${MPDir}/rd-common/tr_font.cpp \
	${MPDir}/rd-common/tr_image_load.cpp \
	${MPDir}/rd-common/tr_image_jpg.cpp \
	${MPDir}/rd-common/tr_image_tga.cpp \
	${MPDir}/rd-common/tr_image_png.cpp \
	${MPDir}/rd-common/tr_noise.cpp \
	${MPDir}/qcommon/GenericParser2.cpp \
	${MPDir}/qcommon/matcomp.cpp \
	${MPDir}/qcommon/q_math.cpp \
	${MPDir}/qcommon/q_shared.cpp \
	${MPDir}/android/android_glimp.cpp \
	
LOCAL_SRC_FILES += $(JK3_SRC) 


include $(BUILD_SHARED_LIBRARY)








