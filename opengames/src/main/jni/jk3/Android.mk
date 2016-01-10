
LOCAL_PATH := $(call my-dir)


include $(CLEAR_VARS)


LOCAL_MODULE    := jk3


LOCAL_CPPFLAGS :=   -O1 -Wno-write-strings -Wno-conversion-null -Wno-pointer-arith -Wno-multichar -Wno-int-to-pointer-cast -Wno-enum-compare -Wno-cpp -fexceptions -fno-operator-names -DLINUX -DNDEBUG -Wstrict-overflow=1 -ffast-math -ftree-vectorize -marm  -fcx-fortran-rules  -fstrict-aliasing -Wdouble-promotion -Ofast  -fsigned-char -fsingle-precision-constant -fpermissive  -D_M_IX86  -D_JK2EXE 
LOCAL_CFLAGS = -DLINUX -DPANDORA -DHAVE_GLES -DARM -DAUTOAIM -DCROUCH -DFINAL_BUILD -fvisibility=hidden

LOCAL_DISABLE_FORMAT_STRING_CHECKS := true

LOCAL_LDLIBS += -lGLESv1_CM -lEGL


LOCAL_LDLIBS += -llog -lz

#Needed so lib can be loaded (_exit error)
LOCAL_LDLIBS += -fuse-ld=bfd


LOCAL_STATIC_LIBRARIES :=  beloko_tools sigc s-setup libzip libpng  lz
LOCAL_SHARED_LIBRARIES := touchcontrols openal 


LOCAL_C_INCLUDES :=   $(TOP_DIR)/MobileTouchControls   $(TOP_DIR) $(LOCAL_PATH)/client $(LOCAL_PATH)/qclib $(LOCAL_PATH)/botlib $(LOCAL_PATH)/d3d  $(LOCAL_PATH)/server  $(LOCAL_PATH)/sw  $(LOCAL_PATH)/libs/freetype2/include $(LOCAL_PATH)/common  $(LOCAL_PATH)/gl

ifeq ($(ANTI_HACK),1)
	LOCAL_CFLAGS += -DANTI_HACK
endif

#############################################################################
# CLIENT/SERVER
#############################################################################

#/../../../Serial/jni/s-setup/b64/cdecode.c \
	/../../../Serial/jni/s-setup/b64/cencode.c \
	/../../../Serial/jni/s-setup/s-setup.cpp \
	
JK2_SRC = \
	code/android/android-jni.cpp \
	code/android/android_console.cpp \
	code/android/android_snddma.cpp \
	code/android/android_main.cpp \
	code/android/android_glimp.cpp \
	code/android/in_android.cpp \
	code/android/gles_qgl.cpp \
	code/RMG/RM_Area.cpp \
	code/RMG/RM_Instance.cpp \
	code/RMG/RM_InstanceFile.cpp \
	code/RMG/RM_Instance_BSP.cpp \
	code/RMG/RM_Instance_Group.cpp \
	code/RMG/RM_Instance_Random.cpp \
	code/RMG/RM_Instance_Void.cpp \
	code/RMG/RM_Manager.cpp \
	code/RMG/RM_Mission.cpp \
	code/RMG/RM_Objective.cpp \
	code/RMG/RM_Path.cpp \
	code/RMG/RM_Terrain.cpp \
	code/client/cl_cgame.cpp \
	code/client/cl_cin.cpp \
	code/client/cl_console.cpp \
	code/client/cl_input.cpp \
	code/client/cl_keys.cpp \
	code/client/cl_main.cpp \
	code/client/cl_mp3.cpp \
	code/client/cl_parse.cpp \
	code/client/cl_scrn.cpp \
	code/client/cl_ui.cpp \
	code/client/snd_ambient.cpp \
	code/client/snd_dma.cpp \
	code/client/snd_mem.cpp \
	code/client/snd_mix.cpp \
	code/client/snd_music.cpp \
	code/client/vmachine.cpp \
	code/ff/cl_ff.cpp \
	code/ff/ff.cpp \
	code/ff/ff_ChannelSet.cpp \
	code/ff/ff_ConfigParser.cpp \
	code/ff/ff_HandleTable.cpp \
	code/ff/ff_MultiCompound.cpp \
	code/ff/ff_MultiEffect.cpp \
	code/ff/ff_MultiSet.cpp \
	code/ff/ff_ffset.cpp \
	code/ff/ff_snd.cpp \
	code/ff/ff_system.cpp \
	code/ff/ff_utils.cpp \
	code/game/genericparser2.cpp \
	code/game/q_math.cpp \
	code/game/q_shared.cpp \
	code/ghoul2/G2_API.cpp \
	code/ghoul2/G2_bolts.cpp \
	code/ghoul2/G2_bones.cpp \
	code/ghoul2/G2_misc.cpp \
	code/ghoul2/G2_surfaces.cpp \
	code/jpeg-6/jcapimin.cpp \
	code/jpeg-6/jccoefct.cpp \
	code/jpeg-6/jccolor.cpp \
	code/jpeg-6/jcdctmgr.cpp \
	code/jpeg-6/jchuff.cpp \
	code/jpeg-6/jcinit.cpp \
	code/jpeg-6/jcmainct.cpp \
	code/jpeg-6/jcmarker.cpp \
	code/jpeg-6/jcmaster.cpp \
	code/jpeg-6/jcomapi.cpp \
	code/jpeg-6/jcparam.cpp \
	code/jpeg-6/jcphuff.cpp \
	code/jpeg-6/jcprepct.cpp \
	code/jpeg-6/jcsample.cpp \
	code/jpeg-6/jctrans.cpp \
	code/jpeg-6/jdapimin.cpp \
	code/jpeg-6/jdapistd.cpp \
	code/jpeg-6/jdatadst.cpp \
	code/jpeg-6/jdatasrc.cpp \
	code/jpeg-6/jdcoefct.cpp \
	code/jpeg-6/jdcolor.cpp \
	code/jpeg-6/jddctmgr.cpp \
	code/jpeg-6/jdhuff.cpp \
	code/jpeg-6/jdinput.cpp \
	code/jpeg-6/jdmainct.cpp \
	code/jpeg-6/jdmarker.cpp \
	code/jpeg-6/jdmaster.cpp \
	code/jpeg-6/jdpostct.cpp \
	code/jpeg-6/jdsample.cpp \
	code/jpeg-6/jdtrans.cpp \
	code/jpeg-6/jerror.cpp \
	code/jpeg-6/jfdctflt.cpp \
	code/jpeg-6/jidctflt.cpp \
	code/jpeg-6/jmemmgr.cpp \
	code/jpeg-6/jmemnobs.cpp \
	code/jpeg-6/jutils.cpp \
	code/mp3code/cdct.c \
	code/mp3code/csbt.c \
	code/mp3code/csbtb.c \
	code/mp3code/csbtl3.c \
	code/mp3code/cup.c \
	code/mp3code/cupini.c \
	code/mp3code/cupl1.c \
	code/mp3code/cupl3.c \
	code/mp3code/cwin.c \
	code/mp3code/cwinb.c \
	code/mp3code/cwinm.c \
	code/mp3code/hwin.c \
	code/mp3code/l3dq.c \
	code/mp3code/l3init.c \
	code/mp3code/mdct.c \
	code/mp3code/mhead.c \
	code/mp3code/msis.c \
	code/mp3code/towave.c \
	code/mp3code/uph.c \
	code/mp3code/upsf.c \
	code/mp3code/wavep.c \
	code/png/png.cpp \
	code/qcommon/cm_draw.cpp \
	code/qcommon/cm_load.cpp \
	code/qcommon/cm_patch.cpp \
	code/qcommon/cm_polylib.cpp \
	code/qcommon/cm_randomterrain.cpp \
	code/qcommon/cm_shader.cpp \
	code/qcommon/cm_terrain.cpp \
	code/qcommon/cm_terrainmap.cpp \
	code/qcommon/cm_test.cpp \
	code/qcommon/cm_trace.cpp \
	code/qcommon/cmd.cpp \
	code/qcommon/common.cpp \
	code/qcommon/cvar.cpp \
	code/qcommon/files_common.cpp \
	code/qcommon/files_pc.cpp \
	code/qcommon/md4.cpp \
	code/qcommon/msg.cpp \
	code/qcommon/net_chan.cpp \
	code/qcommon/stringed_ingame.cpp \
	code/qcommon/stringed_interface.cpp \
	code/qcommon/unzip.cpp \
	code/qcommon/z_memman_pc.cpp \
	code/renderer/etc1.cpp \
	code/renderer/matcomp.c \
	code/renderer/tr_WorldEffects.cpp \
	code/renderer/tr_arioche.cpp \
	code/renderer/tr_backend.cpp \
	code/renderer/tr_bsp.cpp \
	code/renderer/tr_cmds.cpp \
	code/renderer/tr_curve.cpp \
	code/renderer/tr_draw.cpp \
	code/renderer/tr_font.cpp \
	code/renderer/tr_ghoul2.cpp \
	code/renderer/tr_image.cpp \
	code/renderer/tr_init.cpp \
	code/renderer/tr_jpeg_interface.cpp \
	code/renderer/tr_light.cpp \
	code/renderer/tr_main.cpp \
	code/renderer/tr_marks.cpp \
	code/renderer/tr_mesh.cpp \
	code/renderer/tr_model.cpp \
	code/renderer/tr_noise.cpp \
	code/renderer/tr_quicksprite.cpp \
	code/renderer/tr_scene.cpp \
	code/renderer/tr_shade.cpp \
	code/renderer/tr_shade_calc.cpp \
	code/renderer/tr_shader.cpp \
	code/renderer/tr_shadows.cpp \
	code/renderer/tr_sky.cpp \
	code/renderer/tr_stl.cpp \
	code/renderer/tr_surface.cpp \
	code/renderer/tr_surfacesprites.cpp \
	code/renderer/tr_terrain.cpp \
	code/renderer/tr_world.cpp \
	code/server/exe_headers.cpp \
	code/server/sv_ccmds.cpp \
	code/server/sv_client.cpp \
	code/server/sv_game.cpp \
	code/server/sv_init.cpp \
	code/server/sv_main.cpp \
	code/server/sv_savegame.cpp \
	code/server/sv_snapshot.cpp \
	code/server/sv_world.cpp \
	code/ui/ui_atoms.cpp \
	code/ui/ui_connect.cpp \
	code/ui/ui_main.cpp \
	code/ui/ui_saber.cpp \
	code/ui/ui_shared.cpp \
	code/ui/ui_syscalls.cpp \


LOCAL_SRC_FILES += $(JK2_SRC) 


include $(BUILD_SHARED_LIBRARY)








