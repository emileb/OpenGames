LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := ftedroid

LOCAL_CFLAGS :=  -DANDROID -DANDROID_NDK -DGLQUAKE -Wno-write-strings 
LOCAL_CPPFLAGS :=  $(LOCAL_CFLAGS)  -fpermissive

LOCAL_C_INCLUDES := $(TOP_DIR) $(TOP_DIR)/MobileTouchControls  $(LOCAL_PATH)/client $(LOCAL_PATH)/qclib $(LOCAL_PATH)/botlib $(LOCAL_PATH)/d3d  $(LOCAL_PATH)/server  $(LOCAL_PATH)/sw  $(LOCAL_PATH)/libs/freetype2/include $(LOCAL_PATH)/common  $(LOCAL_PATH)/gl

ifeq ($(ANTI_HACK),1)
	LOCAL_CFLAGS += -DANTI_HACK
endif


LOCAL_SRC_FILES:= \
	client/in_android.c	\
	client/sys_droid.cpp \
	client/snd_droid_opensl.c \
	gl/gl_viddroid.c \
	qclib/comprout.c		\
	qclib/hash.c		\
	qclib/initlib.c		\
	common/pr_bgcmd.c		\
	client/pr_skelobj.c		\
	qclib/pr_edict.c		\
	qclib/pr_exec.c		\
	qclib/pr_multi.c		\
	qclib/pr_x86.c		\
	qclib/qcc_cmdlib.c	\
	qclib/qccmain.c		\
	qclib/qcc_pr_comp.c	\
	qclib/qcc_pr_lex.c	\
	qclib/qcdecomp.c		\
	qclib/qcd_main.c \
	common/com_mesh.c	\
	common/com_phys_ode.c	\
	common/common.c 		\
	common/cvar.c 		\
	common/cmd.c 		\
	common/crc.c 		\
	common/fs.c			\
	common/fs_stdio.c		\
	common/fs_pak.c		\
	common/fs_zip.c		\
	common/mathlib.c 		\
	common/huff.c		\
	common/md4.c 		\
	common/sha1.c		\
	common/log.c 		\
	common/net_chan.c 		\
	common/net_wins.c 		\
	common/zone.c 		\
	common/qvm.c	\
	client/r_d3.c	\
	common/gl_q2bsp.c		\
	gl/glmod_doom.c 	\
	common/q3common.c	\
	server/world.c 		\
	server/sv_phys.c 		\
	server/sv_move.c 		\
	common/pmove.c		\
	common/pmovetst.c		\
	http/iwebiface.c		\
	common/translate.c		\
	common/plugin.c		\
	common/q1bsp.c		\
	common/q2pmove.c \
	gl/gl_font.c \
 	gl/gl_ngraph.c		\
	gl/gl_model.c	\
	gl/gl_shader.c	\
	gl/gl_shadow.c	\
	gl/gl_rlight.c	\
	gl/gl_hlmdl.c	\
	gl/gl_warp.c	\
	gl/ltface.c	\
	gl/gl_draw.c		\
	gl/gl_backend.c		\
	gl/gl_rmain.c		\
	gl/gl_rmisc.c		\
	gl/gl_rsurf.c		\
	gl/gl_screen.c		\
	gl/gl_bloom.c		\
	gl/gl_vidcommon.c \
	client/r_surf.c	\
	client/r_2d.c \
	client/textedit.c	\
	client/fragstats.c	\
	client/teamplay.c	\
	client/zqtp.c	\
	client/cl_demo.c	\
	client/cl_ents.c	\
	client/clq2_ents.c	\
	client/cl_input.c	\
	client/cl_main.c	\
	client/cl_parse.c	\
	client/cl_pred.c	\
	client/cl_tent.c	\
	client/cl_cam.c	\
	client/cl_screen.c	\
	client/pr_clcmd.c	\
	client/cl_ui.c	\
	client/cl_ignore.c \
	client/cl_cg.c \
	client/clq3_parse.c	\
	client/pr_csqc.c	\
	client/console.c	\
	client/image.c	\
	client/keys.c	\
	client/menu.c	\
	client/m_download.c	\
	client/m_master.c	\
	client/m_multi.c	\
	client/m_items.c	\
	client/m_options.c	\
	client/m_single.c	\
	client/m_script.c	\
	client/m_mp3.c	\
	client/roq_read.c	\
	client/clq2_cin.c	\
	client/net_master.c	\
	client/r_part.c	\
	client/p_script.c	\
	client/p_null.c	\
	client/p_classic.c	\
	client/r_partset.c	\
	client/renderer.c	\
	client/renderque.c	\
	client/sbar.c	\
	client/skin.c	\
	client/snd_al.c	\
	client/snd_dma.c	\
	client/snd_mem.c	\
	client/snd_mix.c	\
	client/snd_mp3.c	\
	client/snd_ov.c	\
	client/valid.c	\
	client/view.c	\
	client/wad.c			\
	http/ftpclient.c		\
	http/httpclient.c 	\
	client/pr_menu.c \
 	client/cd_droid.c \
 	gl/gl_alias.c		\
	gl/gl_heightmap.c		\
	server/pr_cmds.c 		\
	server/pr_q1qvm.c	\
	server/sv_master.c 	\
	server/sv_init.c 		\
	server/sv_main.c 		\
	server/sv_nchan.c 		\
	server/sv_ents.c 		\
	server/sv_send.c 		\
	server/sv_user.c		\
	server/sv_sql.c		\
	server/sv_mvd.c		\
	server/sv_ccmds.c 		\
	server/sv_rankin.c 	\
	server/sv_chat.c 		\
	server/sv_demo.c		\
	server/net_preparse.c 	\
	server/savegame.c		\
	server/svq2_ents.c 	\
	server/svq2_game.c 	\
	server/svq3_game.c	\
	http/webgen.c		\
	http/ftpserver.c		\
	http/httpserver.c
	

LOCAL_LDLIBS := -lGLESv1_CM -ldl -llog -lOpenSLES -lz
LOCAL_STATIC_LIBRARIES := beloko_tools sigc libzip libpng
LOCAL_SHARED_LIBRARIES := touchcontrols

include $(BUILD_SHARED_LIBRARY)



include 
