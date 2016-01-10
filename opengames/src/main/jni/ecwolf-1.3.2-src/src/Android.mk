LOCAL_PATH := $(call my-dir)



include $(CLEAR_VARS)


LOCAL_MODULE    := ecwolf

LOCAL_CFLAGS = -O2 -Dstricmp=strcasecmp -Dstrnicmp=strncasecmp -Dmalloc_usable_size=dlmalloc_usable_size\
	-DUSE_GPL -DUSE_GPL_DB\
	-DNO_GTK=1 \
	-fexceptions \
	-Wno-format  \
	-fpermissive


LOCAL_C_INCLUDES = \
 	$(LOCAL_PATH)/code/jpeg-8c \
	$(LOCAL_PATH)/g_shared  \
	$(LOCAL_PATH)/../bzip2 \
	$(LOCAL_PATH)/../lzma/C \
	$(LOCAL_PATH)/../gdtoa \
	$(TOP_DIR)/jpeg8d \
	

	
#also include my SDL for SDL_android_extra.h 
LOCAL_C_INCLUDES += $(TOP_DIR)/SDL/include $(TOP_DIR)/SDL_mixer $(TOP_DIR)/MobileTouchControls

LOCAL_C_INCLUDES +=	$(EC_WOLF_LOCAL_PATH)/src/android/extra_files

SRC = \
android/android-jni.cpp \
android/in_android.cpp \
android/wl_iwad_picker_android.cpp \
	dosbox/dbopl.cpp \
	g_shared/a_deathcam.cpp \
	g_shared/a_inventory.cpp \
	g_shared/a_keys.cpp \
	g_shared/a_patrolpoint.cpp \
	g_shared/a_playerpawn.cpp \
	g_shared/a_randomspawner.cpp \
	g_wolf/a_spearofdestiny.cpp \
	g_wolf/wolf_sbar.cpp \
	r_2d/r_draw.cpp \
	r_2d/r_drawt.cpp \
	r_2d/r_main.cpp \
	r_2d/r_things.cpp \
	r_data/colormaps.cpp \
	r_data/r_translate.cpp \
	r_data/renderstyle.cpp \
	resourcefiles/resourcefile.cpp \
	resourcefiles/ancientzip.cpp \
	resourcefiles/file_7z.cpp \
	resourcefiles/file_audiot.cpp \
	resourcefiles/file_directory.cpp \
	resourcefiles/file_gamemaps.cpp \
	resourcefiles/file_grp.cpp \
	resourcefiles/file_pak.cpp \
	resourcefiles/file_rff.cpp \
	resourcefiles/file_rtl.cpp \
	resourcefiles/file_lump.cpp \
	resourcefiles/file_macbin.cpp \
	resourcefiles/file_vgagraph.cpp \
	resourcefiles/file_vswap.cpp \
	resourcefiles/file_wad.cpp \
	resourcefiles/file_zip.cpp \
	resourcefiles/wolfmapcommon.cpp \
	sfmt/SFMT.cpp \
	textures/anim_switches.cpp \
	textures/animations.cpp \
	textures/automaptexture.cpp \
	textures/bitmap.cpp \
	textures/ddstexture.cpp \
	textures/emptytexture.cpp \
	textures/flattexture.cpp \
	textures/imgztexture.cpp \
	textures/jpegtexture.cpp \
	textures/multipatchtexture.cpp \
	textures/patchtexture.cpp \
	textures/pcxtexture.cpp \
	textures/pngtexture.cpp \
	textures/rawpagetexture.cpp \
	textures/rottflattexture.cpp \
	textures/solidtexture.cpp \
	textures/texture.cpp \
	textures/texturemanager.cpp \
	textures/tgatexture.cpp \
	textures/warptexture.cpp \
	textures/wolfrawtexture.cpp \
	textures/wolfshapetexture.cpp \
	thingdef/thingdef.cpp \
	thingdef/thingdef_codeptr.cpp \
	thingdef/thingdef_expression.cpp \
	thingdef/thingdef_properties.cpp \
	thingdef/thingdef_type.cpp \
	actor.cpp \
	am_map.cpp \
	colormatcher.cpp \
	config.cpp \
	c_cvars.cpp \
	dobject.cpp \
	dobjgc.cpp \
	farchive.cpp \
	files.cpp \
	filesys.cpp \
	filesys_steam.cpp \
	g_conversation.cpp \
	g_intermission.cpp \
	g_mapinfo.cpp \
	gamemap.cpp \
	gamemap_planes.cpp \
	gamemap_uwmf.cpp \
	id_ca.cpp \
	id_in.cpp \
	id_sd.cpp \
	id_us_1.cpp \
	id_vh.cpp \
	id_vl.cpp \
	language.cpp \
	lnspec.cpp \
	lumpremap.cpp \
	m_alloc.cpp \
	m_classes.cpp \
	m_random.cpp \
	m_png.cpp \
	name.cpp \
	p_switch.cpp \
	r_sprites.cpp \
	scanner.cpp \
	sdlvideo.cpp \
	sndinfo.cpp \
	sndseq.cpp \
	thinker.cpp \
	v_draw.cpp \
	v_font.cpp \
	v_palette.cpp \
	v_pfx.cpp \
	v_text.cpp \
	v_video.cpp \
	w_wad.cpp \
	weaponslots.cpp \
	wl_act2.cpp \
	wl_agent.cpp \
	wl_atmos.cpp \
	wl_cloudsky.cpp \
	wl_debug.cpp \
	wl_draw.cpp \
	wl_floorceiling.cpp \
	wl_game.cpp \
	wl_inter.cpp \
	wl_iwad.cpp \
	wl_loadsave.cpp \
	wl_main.cpp \
	wl_menu.cpp \
	wl_parallax.cpp \
	wl_play.cpp \
	wl_state.cpp \
	wl_text.cpp \
	zstrformat.cpp \
	zstring.cpp \
	
	
 
LOCAL_SRC_FILES = $(SRC)

LOCAL_LDLIBS := -lGLESv1_CM  -lEGL -ldl -llog -lz -lm -lc
LOCAL_STATIC_LIBRARIES := libjpeg libpng lzma gdtoa bzip2
LOCAL_SHARED_LIBRARIES := touchcontrols SDL SDL_mixer

include $(BUILD_SHARED_LIBRARY)






