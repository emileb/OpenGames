LOCAL_PATH := $(call my-dir)



include $(CLEAR_VARS)


LOCAL_MODULE    := ioq3

LOCAL_CFLAGS = \
    -Wall \
    -fno-strict-aliasing \
    -Wimplicit \
    -Wstrict-prototypes \
    -pipe \
    -fsigned-char \
    -ftree-vectorize \
    -fsingle-precision-constant \
    -DPANDORA \
    -DARM \
    -DHAVE_GLES \
    -DNO_GZIP \
    -DUSE_INTERNAL_JPEG \
    -DPRODUCT_VERSION=\"1.36\" \
    -Wformat=2 \
    -Wno-format-zero-length \
    -Wformat-security \
    -Wno-format-nonliteral \
    -Wstrict-aliasing=2 \
    -Wmissing-format-attribute \
    -Wdisabled-optimization \
    -MMD \
    -DNDEBUG \
    -O3 \
    -funroll-loops \
    -fomit-frame-pointer \
    -ffast-math \
 


LOCAL_CFLAGS += -DUSE_OPENAL -DUSE_OPENAL_DLOPEN_cm -DUSE_OPENAL -DUSE_VOIP -DFLOATING_POINT -DUSE_ALLOCA
LOCAL_CFLAGS += -DBOTLIB


LOCAL_C_INCLUDES = $(LOCAL_PATH)/code/libspeex/include $(LOCAL_PATH)/code/ $(LOCAL_PATH)/code/jpeg-8c $(LOCAL_PATH)/code/SDL12/include

#also include my SDL for SDL_android_extra.h 
LOCAL_C_INCLUDES += $(TOP_DIR)/SDL/include $(TOP_DIR)/SDL_mixer    $(TOP_DIR)/MobileTouchControls $(TOP_DIR)/


ANDROID_SRC = \
	code/android/android-jni.cpp \
	code/android/in_android.c \
	code/android/ifaddrs.c \
		
		
CC_SRC = \
code/client/cl_cgame.c \
code/client/cl_cin.c \
code/client/cl_console.c \
code/client/cl_input.c \
code/client/cl_keys.c \
code/client/cl_main.c \
code/client/cl_net_chan.c \
code/client/cl_parse.c \
code/client/cl_scrn.c \
code/client/cl_ui.c \
code/client/cl_avi.c \
code/qcommon/cm_load.c \
code/qcommon/cm_patch.c \
code/qcommon/cm_polylib.c \
code/qcommon/cm_test.c \
code/qcommon/cm_trace.c \
code/qcommon/cmd.c \
code/qcommon/common.c \
code/qcommon/cvar.c \
code/qcommon/files.c \
code/qcommon/md4.c \
code/qcommon/md5.c \
code/qcommon/msg.c \
code/qcommon/net_chan.c \
code/qcommon/net_ip.c \
code/qcommon/huffman.c \
code/client/snd_adpcm.c \
code/client/snd_dma.c \
code/client/snd_mem.c \
code/client/snd_mix.c \
code/client/snd_wavelet.c \
code/client/snd_main.c \
code/client/snd_codec.c \
code/client/snd_codec_wav.c \
code/client/snd_codec_ogg.c \
code/client/snd_codec_opus.c \
code/client/qal.c \
code/client/snd_openal.c \
code/client/cl_curl.c \
code/server/sv_bot.c \
code/server/sv_ccmds.c \
code/server/sv_client.c \
code/server/sv_game.c \
code/server/sv_init.c \
code/server/sv_main.c \
code/server/sv_net_chan.c \
code/server/sv_snapshot.c \
code/server/sv_world.c \
code/qcommon/q_math.c \
code/qcommon/q_shared.c \
code/qcommon/unzip.c \
code/qcommon/ioapi.c \
code/qcommon/puff.c \
code/qcommon/vm.c \
code/qcommon/vm_interpreted.c \
code/qcommon/vm_armv7l.c \
code/botlib/be_aas_bspq3.c \
code/botlib/be_aas_cluster.c \
code/botlib/be_aas_debug.c \
code/botlib/be_aas_entity.c \
code/botlib/be_aas_file.c \
code/botlib/be_aas_main.c \
code/botlib/be_aas_move.c \
code/botlib/be_aas_optimize.c \
code/botlib/be_aas_reach.c \
code/botlib/be_aas_route.c \
code/botlib/be_aas_routealt.c \
code/botlib/be_aas_sample.c \
code/botlib/be_ai_char.c \
code/botlib/be_ai_chat.c \
code/botlib/be_ai_gen.c \
code/botlib/be_ai_goal.c \
code/botlib/be_ai_move.c \
code/botlib/be_ai_weap.c \
code/botlib/be_ai_weight.c \
code/botlib/be_ea.c \
code/botlib/be_interface.c \
code/botlib/l_crc.c \
code/botlib/l_libvar.c \
code/botlib/l_log.c \
code/botlib/l_memory.c \
code/botlib/l_precomp.c \
code/botlib/l_script.c \
code/botlib/l_struct.c \
code/android/sdl_input.c \
code/android/sdl_snd.c \
code/sys/con_log.c \
code/sys/sys_main.c \
code/sys/con_tty.c \
code/libspeex/bits.c \
code/libspeex/buffer.c \
code/libspeex/cb_search.c \
code/libspeex/exc_10_16_table.c \
code/libspeex/exc_10_32_table.c \
code/libspeex/exc_20_32_table.c \
code/libspeex/exc_5_256_table.c \
code/libspeex/exc_5_64_table.c \
code/libspeex/exc_8_128_table.c \
code/libspeex/fftwrap.c \
code/libspeex/filterbank.c \
code/libspeex/filters.c \
code/libspeex/gain_table.c \
code/libspeex/gain_table_lbr.c \
code/libspeex/hexc_10_32_table.c \
code/libspeex/hexc_table.c \
code/libspeex/high_lsp_tables.c \
code/libspeex/jitter.c \
code/libspeex/kiss_fft.c \
code/libspeex/kiss_fftr.c \
code/libspeex/lpc.c \
code/libspeex/lsp.c \
code/libspeex/lsp_tables_nb.c \
code/libspeex/ltp.c \
code/libspeex/mdf.c \
code/libspeex/modes.c \
code/libspeex/modes_wb.c \
code/libspeex/nb_celp.c \
code/libspeex/preprocess.c \
code/libspeex/quant_lsp.c \
code/libspeex/resample.c \
code/libspeex/sb_celp.c \
code/libspeex/smallft.c \
code/libspeex/speex.c \
code/libspeex/speex_callbacks.c \
code/libspeex/speex_header.c \
code/libspeex/stereo.c \
code/libspeex/vbr.c \
code/libspeex/vq.c \
code/libspeex/window.c \
code/zlib/adler32.c \
code/zlib/crc32.c \
code/zlib/inffast.c \
code/zlib/inflate.c \
code/zlib/inftrees.c \
code/zlib/zutil.c \
code/sys/sys_unix.c \
code/renderergl1/tr_animation.c \
code/renderergl1/tr_backend.c \
code/renderergl1/tr_bsp.c \
code/renderergl1/tr_cmds.c \
code/renderergl1/tr_curve.c \
code/renderergl1/tr_flares.c \
code/renderercommon/tr_font.c \
code/renderergl1/tr_image.c \
code/renderercommon/tr_image_png.c \
code/renderercommon/tr_image_jpg.c \
code/renderercommon/tr_image_bmp.c \
code/renderercommon/tr_image_tga.c \
code/renderercommon/tr_image_pcx.c \
code/renderergl1/tr_init.c \
code/renderergl1/tr_light.c \
code/renderergl1/tr_main.c \
code/renderergl1/tr_marks.c \
code/renderergl1/tr_mesh.c \
code/renderergl1/tr_model.c \
code/renderergl1/tr_model_iqm.c \
code/renderercommon/tr_noise.c \
code/renderergl1/tr_scene.c \
code/renderergl1/tr_shade.c \
code/renderergl1/tr_shade_calc.c \
code/renderergl1/tr_shader.c \
code/renderergl1/tr_shadows.c \
code/renderergl1/tr_sky.c \
code/renderergl1/tr_surface.c \
code/renderergl1/tr_world.c \
code/android/sdl_gamma.c \
code/android/sdl_glimp.c \
code/jpeg-8c/jaricom.c \
code/jpeg-8c/jcapimin.c \
code/jpeg-8c/jcapistd.c \
code/jpeg-8c/jcarith.c \
code/jpeg-8c/jccoefct.c \
code/jpeg-8c/jccolor.c \
code/jpeg-8c/jcdctmgr.c \
code/jpeg-8c/jchuff.c \
code/jpeg-8c/jcinit.c \
code/jpeg-8c/jcmainct.c \
code/jpeg-8c/jcmarker.c \
code/jpeg-8c/jcmaster.c \
code/jpeg-8c/jcomapi.c \
code/jpeg-8c/jcparam.c \
code/jpeg-8c/jcprepct.c \
code/jpeg-8c/jcsample.c \
code/jpeg-8c/jctrans.c \
code/jpeg-8c/jdapimin.c \
code/jpeg-8c/jdapistd.c \
code/jpeg-8c/jdarith.c \
code/jpeg-8c/jdatadst.c \
code/jpeg-8c/jdatasrc.c \
code/jpeg-8c/jdcoefct.c \
code/jpeg-8c/jdcolor.c \
code/jpeg-8c/jddctmgr.c \
code/jpeg-8c/jdhuff.c \
code/jpeg-8c/jdinput.c \
code/jpeg-8c/jdmainct.c \
code/jpeg-8c/jdmarker.c \
code/jpeg-8c/jdmaster.c \
code/jpeg-8c/jdmerge.c \
code/jpeg-8c/jdpostct.c \
code/jpeg-8c/jdsample.c \
code/jpeg-8c/jdtrans.c \
code/jpeg-8c/jerror.c \
code/jpeg-8c/jfdctflt.c \
code/jpeg-8c/jfdctfst.c \
code/jpeg-8c/jfdctint.c \
code/jpeg-8c/jidctflt.c \
code/jpeg-8c/jidctfst.c \
code/jpeg-8c/jidctint.c \
code/jpeg-8c/jmemmgr.c \
code/jpeg-8c/jmemnobs.c \
code/jpeg-8c/jquant1.c \
code/jpeg-8c/jquant2.c \
code/jpeg-8c/jutils.c \

 
LOCAL_SRC_FILES = $(ANDROID_SRC) $(CC_SRC)

LOCAL_LDLIBS := -lGLESv1_CM  -lEGL -ldl -llog -lOpenSLES -lz -lm 

ifeq ($(BUILD_SERIAL),1)
LOCAL_CPPFLAGS += -DANTI_HACK 
endif

#Hacky hack hack
LOCAL_LDLIBS +=	-L$(TOP_DIR)/openssl/libs/ -lcrypto

LOCAL_LDLIBS += -fuse-ld=bfd

LOCAL_STATIC_LIBRARIES := s-setup SDL SDL_net libjpeg libpng
LOCAL_SHARED_LIBRARIES := touchcontrols openal SDL SDL_mixer SDL_image

include $(BUILD_SHARED_LIBRARY)






